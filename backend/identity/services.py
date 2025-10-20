from typing import Union
from django.db.models import QuerySet, Q
from scheduling.models import Meeting
from .models import User


def get_active_meetings_for_user(user_or_id: Union[User, int, str]) -> QuerySet:
    """
    Returns a QuerySet of all Meeting objects where the given user (instance or PK)
    is either the host_user OR belongs to the Group assigned to the Meeting,
    and recruitment.plan_status == 'active'.

    The function accepts a User instance or a user PK (UUID/string) and returns
    meetings ordered by day_of_week and start_hour.
    """
    user_id = user_or_id.pk if hasattr(user_or_id, 'pk') else user_or_id

    qs = (
        Meeting.objects
        .filter(
            Q(host_user_id=user_id) |
            Q(group__group_users__user_id=user_id),
            recruitment__plan_status='active'
        )
        .select_related('recruitment', 'subject', 'room', 'group', 'host_user')
        .order_by('day_of_week', 'start_hour')
        .distinct()
    )
    return qs