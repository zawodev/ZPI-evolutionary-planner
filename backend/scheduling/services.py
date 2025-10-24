from typing import Union
from django.db.models import QuerySet
from .models import Meeting, Room, Recruitment
from django.contrib.auth import get_user_model


User = get_user_model()


def get_active_meetings_for_room(room_or_id: Union[Room, str, int]) -> QuerySet:
    """
    Returns a QuerySet of all Meeting objects related to the given Room (instance or PK),
    where recruitment.plan_status == 'active'.
    """
    room_id = room_or_id.pk if hasattr(room_or_id, 'pk') else room_or_id

    qs = (
        Meeting.objects
        .filter(room_id=room_id, recruitment__plan_status='active')
        .select_related('recruitment', 'subject', 'room', 'group', 'host_user', 'required_tag')
        .order_by('day_of_week', 'start_hour')
    )
    return qs


def get_users_for_recruitment(recruitment_or_id: Union[Recruitment, str, int], active_only: bool = False) -> QuerySet:
    """
    Return a QuerySet of all users who belong to groups that have Meetings
    in the specified Recruitment.

    Parameters:
    - recruitment_or_id: a Recruitment instance or the recruitment's primary key (recruitment_id).
    - active_only: if True, include only meetings whose recruitment has plan_status == 'active'.

    Usage examples:
    get_users_for_recruitment(recruitment_instance)
    get_users_for_recruitment(recruitment_id, active_only=True)
    """
    recruitment_id = recruitment_or_id.recruitment_id if hasattr(recruitment_or_id, 'recruitment_id') else recruitment_or_id

    filters = {
        'user_groups__group__meetings__recruitment_id': recruitment_id
    }
    if active_only:
        filters['user_groups__group__meetings__recruitment__plan_status'] = 'active'

    qs = (
        User.objects
        .filter(**filters)
        .select_related('organization')
        .prefetch_related('user_groups', 'user_groups__group')
        .order_by('username')
        .distinct()
    )
    return qs
