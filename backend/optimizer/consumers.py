import json
from channels.generic.websocket import AsyncWebsocketConsumer
from channels.db import database_sync_to_async
from django.contrib.auth.models import AnonymousUser
from .models import OptimizationJob


class JobProgressConsumer(AsyncWebsocketConsumer):
    """WebSocket consumer for real-time job progress updates"""
    
    async def connect(self):
        """Handle WebSocket connection"""
        self.job_id = self.scope['url_route']['kwargs']['job_id']
        self.job_group_name = f'job_progress_{self.job_id}'
        
        # check if job exists and user has permission
        job_exists = await self.check_job_exists()
        if not job_exists:
            await self.close(code=4004)  # not found
            return
        
        # join job group
        await self.channel_layer.group_add(
            self.job_group_name,
            self.channel_name
        )
        
        await self.accept()
        
        # send current job status
        await self.send_current_status()
    
    async def disconnect(self, close_code):
        """Handle WebSocket disconnection"""
        # leave job group
        await self.channel_layer.group_discard(
            self.job_group_name,
            self.channel_name
        )
    
    async def receive(self, text_data):
        """Handle messages from WebSocket"""
        try:
            data = json.loads(text_data)
            message_type = data.get('type')
            
            if message_type == 'get_status':
                await self.send_current_status()
            elif message_type == 'cancel_job':
                await self.handle_job_cancellation()
            else:
                await self.send(text_data=json.dumps({
                    'type': 'error',
                    'message': f'Unknown message type: {message_type}'
                }))
        except json.JSONDecodeError:
            await self.send(text_data=json.dumps({
                'type': 'error',
                'message': 'Invalid JSON format'
            }))
    
    async def job_progress_update(self, event):
        """Handle progress update from group"""
        await self.send(text_data=json.dumps({
            'type': 'progress_update',
            'data': event['data']
        }))
    
    async def job_status_change(self, event):
        """Handle status change from group"""
        await self.send(text_data=json.dumps({
            'type': 'status_change',
            'data': event['data']
        }))
    
    async def job_completed(self, event):
        """Handle job completion from group"""
        await self.send(text_data=json.dumps({
            'type': 'job_completed',
            'data': event['data']
        }))
    
    async def job_error(self, event):
        """Handle job error from group"""
        await self.send(text_data=json.dumps({
            'type': 'job_error',
            'data': event['data']
        }))
    
    @database_sync_to_async
    def check_job_exists(self):
        """Check if the job exists and user has permission"""
        try:
            job = OptimizationJob.objects.get(id=self.job_id)
            # todo:
            # - remember to add permission check here
            # - for now, access allowed to all jobs
            return True
        except OptimizationJob.DoesNotExist:
            return False
    
    @database_sync_to_async
    def get_job_status(self):
        """Get current job status and progress"""
        try:
            job = OptimizationJob.objects.get(id=self.job_id)
            latest_progress = job.progress_updates.first()
            
            data = {
                'job_id': str(job.id),
                'status': job.status,
                'current_iteration': job.current_iteration,
                'created_at': job.created_at.isoformat(),
                'updated_at': job.updated_at.isoformat(),
            }
            
            if job.started_at:
                data['started_at'] = job.started_at.isoformat()
            if job.completed_at:
                data['completed_at'] = job.completed_at.isoformat()
            if job.error_message:
                data['error_message'] = job.error_message
            if job.final_solution:
                data['final_solution'] = job.final_solution
            
            if latest_progress:
                data['latest_progress'] = {
                    'iteration': latest_progress.iteration,
                    'best_solution': latest_progress.best_solution,
                    'timestamp': latest_progress.timestamp.isoformat(),
                }
            
            return data
        except OptimizationJob.DoesNotExist:
            return None
    
    async def send_current_status(self):
        """Send current job status to client"""
        status_data = await self.get_job_status()
        if status_data:
            await self.send(text_data=json.dumps({
                'type': 'current_status',
                'data': status_data
            }))
        else:
            await self.send(text_data=json.dumps({
                'type': 'error',
                'message': 'Job not found'
            }))
    
    async def handle_job_cancellation(self):
        """Handle job cancellation request"""
        # import here to avoid circular imports
        from .services import OptimizerService
        
        try:
            service = OptimizerService()
            await database_sync_to_async(service.cancel_job)(self.job_id)
            
            await self.send(text_data=json.dumps({
                'type': 'cancellation_requested',
                'message': 'Job cancellation requested'
            }))
        except Exception as e:
            await self.send(text_data=json.dumps({
                'type': 'error',
                'message': f'Failed to cancel job: {str(e)}'
            }))
