import pika
import json
import logging
import redis
from typing import Dict, Any, Optional
from django.conf import settings
from django.utils import timezone
from channels.layers import get_channel_layer
from asgiref.sync import async_to_sync
from .models import OptimizationJob, OptimizationProgress
from .logger import get_logger

logger = get_logger(__name__)


class RabbitMQService:
    """Service for RabbitMQ communication"""
    
    def __init__(self):
        self.connection = None
        self.channel = None
        self.connect()
    
    def connect(self):
        """Establish connection to RabbitMQ"""
        try:
            credentials = pika.PlainCredentials(
                settings.RABBITMQ_USERNAME, 
                settings.RABBITMQ_PASSWORD
            )
            parameters = pika.ConnectionParameters(
                host=settings.RABBITMQ_HOST,
                port=settings.RABBITMQ_PORT,
                virtual_host=settings.RABBITMQ_VHOST,
                credentials=credentials
            )
            self.connection = pika.BlockingConnection(parameters)
            self.channel = self.connection.channel()
            
            # declare all queues
            self.channel.queue_declare(queue=settings.OPTIMIZER_QUEUE, durable=True) # to start optimization jobs
            self.channel.queue_declare(queue=settings.PROGRESS_QUEUE, durable=True) # to receive progress updates
            self.channel.queue_declare(queue=settings.CONTROL_QUEUE, durable=True) # to send control messages (like cancel)
            
            logger.info("Connected to RabbitMQ successfully")
        except Exception as e:
            logger.error(f"Failed to connect to RabbitMQ: {e}")
            raise
    
    def publish_job(self, job_id: str, problem_data: Dict[str, Any]):
        """Publish optimization job to optimizer queue"""
        try:
            message = {
                'job_id': job_id,
                'problem_data': problem_data,
                'timestamp': timezone.now().isoformat()
            }
            
            self.channel.basic_publish(
                exchange='',
                routing_key=settings.OPTIMIZER_QUEUE,
                body=json.dumps(message),
                properties=pika.BasicProperties(
                    delivery_mode=2,  # make message persistent (important apparently)
                    message_id=job_id
                )
            )
            logger.info(f"Published job {job_id} to optimizer queue")
        except Exception as e:
            logger.error(f"Failed to publish job {job_id}: {e}")
            raise
    
    def publish_control_message(self, job_id: str, action: str, data: Optional[Dict] = None):
        """Publish control message (like cancel) to control queue"""
        try:
            message = {
                'job_id': job_id,
                'action': action,
                'data': data or {},
                'timestamp': timezone.now().isoformat()
            }
            
            self.channel.basic_publish(
                exchange='',
                routing_key=settings.CONTROL_QUEUE,
                body=json.dumps(message),
                properties=pika.BasicProperties(
                    delivery_mode=2,
                    message_id=f"{job_id}_{action}"
                )
            )
            logger.info(f"Published control message {action} for job {job_id}")
        except Exception as e:
            logger.error(f"Failed to publish control message for job {job_id}: {e}")
            raise
    
    def close(self):
        """Close RabbitMQ connection"""
        if self.connection and not self.connection.is_closed:
            self.connection.close()


class ProgressListener:
    """Service for listening to progress updates from optimizer"""
    
    def __init__(self):
        self.rabbitmq = RabbitMQService()
        self.channel_layer = get_channel_layer()
    
    def start_listening(self):
        """Start listening for progress updates"""
        self.rabbitmq.channel.basic_consume(
            queue=settings.PROGRESS_QUEUE,
            on_message_callback=self.handle_progress_update,
            auto_ack=True
        )
        
        logger.info("Started listening for progress updates")
        self.rabbitmq.channel.start_consuming()
    
    def handle_progress_update(self, channel, method, properties, body):
        """Handle progress update from optimizer"""
        try:
            data = json.loads(body)
            job_id = data.get('job_id')
            iteration = data.get('iteration_num')
            results = data.get('results', {})
            
            if not job_id or iteration is None:
                logger.warning(f"Invalid progress update data: {data}")
                return
            
            # update job in database
            try:
                job = OptimizationJob.objects.get(id=job_id)
                job.current_iteration = iteration
                job.updated_at = timezone.now()
                
                # update final_solution with latest best solution
                best_solution = results.get('best_solution', {})
                job.final_solution = best_solution
                
                if job.status == 'queued':
                    job.status = 'running'
                    job.started_at = timezone.now()
                
                job.save()
                
                # create progress record
                progress = OptimizationProgress.objects.create(
                    job=job,
                    iteration=iteration,
                    best_solution=best_solution
                )
                
                # send websocket update
                self.send_websocket_update(job_id, 'progress_update', {
                    'job_id': job_id,
                    'iteration': iteration,
                    'best_solution': progress.best_solution,
                    'timestamp': progress.timestamp.isoformat()
                })
                
                logger.info(f"Updated progress for job {job_id}, iteration {iteration}")
                
            except OptimizationJob.DoesNotExist:
                logger.warning(f"Job {job_id} not found for progress update")
                
        except Exception as e:
            logger.error(f"Error handling progress update: {e}")
    
    def send_websocket_update(self, job_id: str, message_type: str, data: Dict[str, Any]):
        """Send update to WebSocket clients"""
        try:
            async_to_sync(self.channel_layer.group_send)(
                f'job_progress_{job_id}',
                {
                    'type': message_type,
                    'data': data
                }
            )
        except Exception as e:
            logger.error(f"Failed to send WebSocket update: {e}")


class OptimizerService:
    """Main service for optimization operations"""
    
    def __init__(self):
        self.rabbitmq = RabbitMQService()
        self.redis_client = None
        if getattr(settings, 'USE_REDIS', True):
            try:
                self.redis_client = redis.Redis(
                    host='localhost',
                    port=6379,
                    db=0,
                    decode_responses=True
                )
                # test connection quickly
                self.redis_client.ping()
            except Exception as e:
                logger.warning(f"Redis not available, caching disabled: {e}")
                self.redis_client = None
    
    def submit_job(self, problem_data: Dict[str, Any]) -> OptimizationJob:
        """Submit a new optimization job"""
        try:
            # create job in database
            job = OptimizationJob.objects.create(
                problem_data=problem_data,
                max_execution_time=problem_data.get('max_execution_time', 300)
            )
            
            # publish to RabbitMQ
            self.rabbitmq.publish_job(str(job.id), problem_data)
            
            # optionally cache job info in Redis
            if self.redis_client:
                try:
                    self.redis_client.hset(
                        f"job:{job.id}",
                        mapping={
                            'status': job.status,
                            'created_at': job.created_at.isoformat(),
                            'max_execution_time': job.max_execution_time
                        }
                    )
                    self.redis_client.expire(f"job:{job.id}", 3600)  # ttl
                except Exception as e:
                    logger.warning(f"Redis cache update failed: {e}")
            
            logger.info(f"Submitted optimization job {job.id}")
            return job
            
        except Exception as e:
            logger.error(f"Failed to submit job: {e}")
            raise
    
    def cancel_job(self, job_id: str) -> bool:
        """Cancel an optimization job"""
        try:
            job = OptimizationJob.objects.get(id=job_id)
            
            if job.status not in ['queued', 'running']:
                return False
            
            # update job status
            job.status = 'cancelled'
            job.completed_at = timezone.now()
            job.save()
            
            # send cancel message to optimizer
            self.rabbitmq.publish_control_message(job_id, 'cancel')
            
            # optionally update redis cache
            if self.redis_client:
                try:
                    self.redis_client.hset(f"job:{job_id}", 'status', 'cancelled')
                except Exception as e:
                    logger.warning(f"Redis cache update failed: {e}")
            
            # send websocket notification
            channel_layer = get_channel_layer()
            async_to_sync(channel_layer.group_send)(
                f'job_progress_{job_id}',
                {
                    'type': 'job_status_change',
                    'data': {
                        'job_id': job_id,
                        'status': 'cancelled',
                        'timestamp': timezone.now().isoformat()
                    }
                }
            )
            
            logger.info(f"Cancelled job {job_id}")
            return True
            
        except OptimizationJob.DoesNotExist:
            logger.warning(f"Job {job_id} not found for cancellation")
            return False
        except Exception as e:
            logger.error(f"Failed to cancel job {job_id}: {e}")
            raise
    
    def get_job_status(self, job_id: str) -> Optional[Dict[str, Any]]:
        """Get job status from cache or database"""
        try:
            # try redis cache first if available
            if self.redis_client:
                try:
                    cached_data = self.redis_client.hgetall(f"job:{job_id}")
                    if cached_data:
                        return cached_data
                except Exception as e:
                    logger.warning(f"Redis cache read failed: {e}")
            
            # fallback to database
            job = OptimizationJob.objects.get(id=job_id)
            status_data = {
                'status': job.status,
                'created_at': job.created_at.isoformat(),
                'updated_at': job.updated_at.isoformat(),
                'current_iteration': job.current_iteration
            }
            
            # optionally update cache
            if self.redis_client:
                try:
                    self.redis_client.hset(f"job:{job_id}", mapping=status_data)
                    self.redis_client.expire(f"job:{job_id}", 3600)
                except Exception as e:
                    logger.warning(f"Redis cache update failed: {e}")
            
            return status_data
            
        except OptimizationJob.DoesNotExist:
            return None
        except Exception as e:
            logger.error(f"Failed to get job status for {job_id}: {e}")
            return None
