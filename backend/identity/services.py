from typing import Union
from django.db.models import QuerySet, Q
from scheduling.models import Meeting, Recruitment
from .models import User


def get_active_meetings_for_user(user_or_id: Union[User, int, str]) -> QuerySet:
    """
    Returns a QuerySet of all Meeting objects where the given user (instance or PK)
    is either the host_user (via subject_group) OR belongs to the Group assigned to the Meeting,
    and recruitment.plan_status == 'active'.

    The function accepts a User instance or a user PK (UUID/string) and returns
    meetings ordered by day_of_week and start_hour.
    """
    user_id = user_or_id.pk if hasattr(user_or_id, 'pk') else user_or_id

    qs = (
        Meeting.objects
        .filter(
            Q(subject_group__host_user_id=user_id) |
            Q(subject_group__group__group_users__user_id=user_id),
            recruitment__plan_status='active'
        )
        .select_related('recruitment', 'subject_group__subject', 'subject_group__group', 'subject_group__host_user', 'room')
        .order_by('day_of_week', 'start_hour')
        .distinct()
    )
    return qs


def get_recruitments_for_user(user_or_id: Union[User, int, str], active_only: bool = False) -> QuerySet:
    """
    Return a QuerySet of Recruitment objects in which the given user is a participant.

    Uses the `UserRecruitment` table (reverse relation `recruitment_users`) to determine
    membership: a user is a participant if there exists a UserRecruitment row linking them
    to the Recruitment.

    Parameters:
    - user_or_id: User instance or primary key (UUID/string).
    - active_only: when True, filter only recruitments with plan_status == 'active'.

    Returns: QuerySet[Recruitment] containing unique Recruitment records.
    """
    user_id = user_or_id.pk if hasattr(user_or_id, 'pk') else user_or_id

    filters = {
        'recruitment_users__user_id': user_id
    }
    if active_only:
        filters['plan_status'] = 'active'

    qs = (
        Recruitment.objects
        .filter(**filters)
        .prefetch_related('meetings', 'recruitment_users', 'recruitment_users__user')
        .order_by('recruitment_name')
        .distinct()
    )
    return qs
