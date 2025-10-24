from typing import Union
from django.db.models import QuerySet, Q
from scheduling.models import Meeting, Recruitment
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


def get_recruitments_for_user(user_or_id: Union[User, int, str], active_only: bool = False) -> QuerySet:
    """
    Zwraca QuerySet obiektów Recruitment, w których dany użytkownik jest uczestnikiem.

    Definicja "uczestnik": użytkownik należy do co najmniej jednej Group, która ma
    Meeting w danym Recruitment.

    Parametry:
    - user_or_id: User instance lub PK (UUID/string).
    - active_only: gdy True, filtruje tylko recruitmenty z plan_status == 'active'.

    Zwraca: QuerySet[Recruitment] (unikatowe rekordy).
    """
    user_id = user_or_id.pk if hasattr(user_or_id, 'pk') else user_or_id

    filters = {
        'meetings__group__group_users__user_id': user_id
    }
    if active_only:
        filters['plan_status'] = 'active'

    qs = (
        Recruitment.objects
        .filter(**filters)
        .prefetch_related('meetings', 'meetings__group')
        .order_by('recruitment_name')
        .distinct()
    )
    return qs
