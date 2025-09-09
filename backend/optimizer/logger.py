import logging
import sys


class ColoredFormatter(logging.Formatter):
    """Custom colored formatter for logs"""
    
    # ANSI color codes
    COLORS = {
        'DEBUG': '\033[36m', # cyan
        'INFO': '\033[33m', # yellow
        'WARNING': '\033[93m', # bright yellow
        'ERROR': '\033[91m', # red
        'CRITICAL': '\033[95m', # magenta
    }
    RESET = '\033[0m'
    
    def format(self, record):
        color = self.COLORS.get(record.levelname, '')
        reset = self.RESET
        
        # [LEVEL] Message
        formatted_message = f"{color}[{record.levelname}]{reset} {record.getMessage()}"
        return formatted_message


def get_logger(name: str = None):
    """Get a configured logger with colored output"""
    logger_name = name or 'optimizer'
    logger = logging.getLogger(logger_name)
    
    # only configure if not already configured
    if not logger.handlers:
        handler = logging.StreamHandler(sys.stdout)
        handler.setFormatter(ColoredFormatter())
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
        logger.propagate = False  # prevent duplicate logs
    
    return logger


# default logger for quick import
logger = get_logger('optimizer')
