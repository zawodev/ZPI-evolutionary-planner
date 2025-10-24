from typing import Union
from django.db.models import QuerySet
from django.utils import timezone
from django.db import transaction
from .models import Meeting, Room, Recruitment
import logging

logger = logging.getLogger(__name__)


def get_active_meetings_for_room(room_or_id: Union[Room, str, int]) -> QuerySet:
    """
    Returns a QuerySet of all Meeting objects related to the given Room (instance or PK),
    where recruitment.plan_status == 'active'.
    """
    room_id = room_or_id.pk if hasattr(room_or_id, 'pk') else room_or_id

    qs = (
        Meeting.objects
        .filter(room_id=room_id, recruitment__plan_status='active')
        .select_related('recruitment', 'subject_group__subject', 'subject_group__group', 'room', 'host_user', 'required_tag')
        .order_by('day_of_week', 'start_hour')
    )
    return qs


def should_start_optimization(recruitment):
    """check if optimization should start based on conditions"""
    if recruitment.plan_status != 'draft':
        return False
    
    today = timezone.now().date()
    
    # condition 1: optimization_start_date reached
    if recruitment.optimization_start_date and today >= recruitment.optimization_start_date:
        return True
    
    # condition 2: threshold reached between user_prefs_start_date and optimization_start_date
    if (recruitment.user_prefs_start_date and 
        recruitment.optimization_start_date and
        recruitment.user_prefs_start_date <= today < recruitment.optimization_start_date):
        
        # TODO: get total users in recruitment:
        total_users = -1 
        
        if total_users > 0:
            threshold_count = int(total_users * recruitment.preference_threshold)
            if recruitment.users_submitted_count >= threshold_count:
                return True
    
    return False


def trigger_optimization(recruitment):
    """trigger optimization for recruitment"""
    from optimizer.services import OptimizerService, convert_preferences_to_problem_data
    
    with transaction.atomic():
        recruitment.plan_status = 'optimizing'
        recruitment.save()
        logger.info(f"started optimization for recruitment {recruitment.recruitment_id}")
        
        # Convert preferences to problem_data
        try:
            problem_data = convert_preferences_to_problem_data(str(recruitment.recruitment_id))
        except NotImplementedError:
            logger.warning(f"convert_preferences_to_problem_data not yet implemented, using empty problem_data")
            problem_data = {}
        except Exception as e:
            logger.error(f"failed to convert preferences to problem_data: {e}")
            recruitment.plan_status = 'draft'
            recruitment.save()
            raise
        
        try:
            optimizer_service = OptimizerService()
            job = optimizer_service.submit_job({
                'recruitment_id': str(recruitment.recruitment_id),
                'max_execution_time': recruitment.max_round_execution_time,
                'problem_data': problem_data
            })
            logger.info(f"created optimization job {job.id} for recruitment {recruitment.recruitment_id}")
        except Exception as e:
            logger.error(f"failed to create optimization job: {e}")
            recruitment.plan_status = 'draft'
            recruitment.save()
            raise


def check_and_trigger_optimizations():
    """check all draft recruitments and trigger optimization if needed"""
    recruitments = Recruitment.objects.filter(plan_status='draft')
    
    for recruitment in recruitments:
        if should_start_optimization(recruitment):
            trigger_optimization(recruitment)


def archive_expired_recruitments():
    """archive recruitments that passed expiration_date"""
    today = timezone.now().date()
    
    expired = Recruitment.objects.filter(
        plan_status='active',
        expiration_date__lt=today
    )
    
    for recruitment in expired:
        recruitment.plan_status = 'archived'
        recruitment.save()
        logger.info(f"archived recruitment {recruitment.recruitment_id}")
