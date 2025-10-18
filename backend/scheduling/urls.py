from django.urls import path
from .views import (
    SubjectView,
    RecruitmentView,
    PlanView,
    RoomView,
    TagView,
    RoomTagView,
    MeetingView,
    RoomAvailabilityAPIView
)

urlpatterns = [
    path('subjects/', SubjectView.as_view(), name='subjects'),
    path('subjects/<uuid:pk>/', SubjectView.as_view(), name='subject-detail'),

    path('recruitments/', RecruitmentView.as_view(), name='recruitments'),
    path('recruitments/<uuid:pk>/', RecruitmentView.as_view(), name='recruitment-detail'),

    path('plans/', PlanView.as_view(), name='plans'),
    path('plans/<uuid:pk>/', PlanView.as_view(), name='plan-detail'),

    path('rooms/', RoomView.as_view(), name='rooms'),
    path('rooms/<uuid:pk>/', RoomView.as_view(), name='room-detail'),
    path('rooms/<uuid:room_id>/availability/', RoomAvailabilityAPIView.as_view(), name='room-availability'),

    path('tags/', TagView.as_view(), name='tags'),
    path('tags/<uuid:pk>/', TagView.as_view(), name='tag-detail'),

    path('room-tags/', RoomTagView.as_view(), name='roomtags'),
    path('room-tags/<uuid:pk>/', RoomTagView.as_view(), name='roomtag-detail'),

    path('meetings/', MeetingView.as_view(), name='meetings'),
    path('meetings/<uuid:pk>/', MeetingView.as_view(), name='meeting-detail'),
]