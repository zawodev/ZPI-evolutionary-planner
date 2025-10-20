from typing import Union
from django.db.models import QuerySet
from .models import Meeting, Room


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
