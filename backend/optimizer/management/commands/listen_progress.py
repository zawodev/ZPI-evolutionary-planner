from django.core.management.base import BaseCommand
from optimizer.services import ProgressListener
from optimizer.logger import get_logger
import logging
import signal
import sys

logger = get_logger(__name__)

# this background task needs to be run alongside the Django server
class Command(BaseCommand):
    help = 'Listen for progress updates from the optimizer service via Redis Pub/Sub'
    
    def __init__(self):
        super().__init__()
        self.listener = None
        self.running = True
    
    def add_arguments(self, parser):
        parser.add_argument(
            '--verbose',
            action='store_true',
            help='Enable verbose logging',
        )
    
    def handle(self, *args, **options):
        if options['verbose']:
            logging.basicConfig(level=logging.DEBUG)
        else:
            logging.basicConfig(level=logging.INFO)
        
        # setup signal handlers for graceful shutdown
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)
        
        self.stdout.write(
            self.style.SUCCESS('Starting Redis progress listener...')
        )
        
        try:
            self.listener = ProgressListener()
            self.listener.start_listening()
            
            # Keep the main thread alive while the listener runs in background
            while self.running:
                import time
                time.sleep(1)
                
        except KeyboardInterrupt:
            self.stdout.write(
                self.style.WARNING('Received interrupt signal, shutting down...')
            )
        except Exception as e:
            self.stdout.write(
                self.style.ERROR(f'Error in progress listener: {e}')
            )
            logger.exception("Progress listener error")
        finally:
            if self.listener:
                self.listener.stop_listening()
                self.stdout.write(
                    self.style.SUCCESS('Progress listener stopped.')
                )
    
    def signal_handler(self, sig, frame):
        """Handle shutdown signals"""
        self.stdout.write(
            self.style.WARNING(f'Received signal {sig}, shutting down...')
        )
        self.running = False
        if self.listener:
            self.listener.stop_listening()
        sys.exit(0)
