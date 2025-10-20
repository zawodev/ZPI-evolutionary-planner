from django.contrib import admin
from .models import Subject, Recruitment, Room, Tag, RoomTag, Meeting


@admin.register(Subject)
class SubjectAdmin(admin.ModelAdmin):
    list_display = ('subject_id', 'subject_name')
    search_fields = ('subject_name',)


@admin.register(Recruitment)
class RecruitmentAdmin(admin.ModelAdmin):
    list_display = (
        'recruitment_id',
        'recruitment_name',
        'start_date',
        'end_date',
        'cycle_type',
        'plan_status',
        'default_token_count',
        'round_count',
    )
    list_filter = ('cycle_type', 'plan_status')
    search_fields = ('recruitment_name',)


@admin.register(Room)
class RoomAdmin(admin.ModelAdmin):
    list_display = ('room_id', 'building_name', 'room_number', 'capacity')
    list_filter = ('building_name',)
    search_fields = ('building_name', 'room_number')


@admin.register(Tag)
class TagAdmin(admin.ModelAdmin):
    list_display = ('tag_id', 'tag_name')
    search_fields = ('tag_name',)


@admin.register(RoomTag)
class RoomTagAdmin(admin.ModelAdmin):
    list_display = ('room', 'tag')
    list_filter = ('tag',)


@admin.register(Meeting)
class MeetingAdmin(admin.ModelAdmin):
    list_display = (
        'meeting_id',
        'subject',
        'group',
        'host_user',
        'room',
        'recruitment',
        'day_of_week',
        'start_hour',
        'end_hour',
    )
    list_filter = ('day_of_week', 'recruitment')
    search_fields = (
        'subject__subject_name',
        'group__group_name',
        'host_user__first_name',
        'host_user__last_name',
    )
    raw_id_fields = ('recruitment', 'subject', 'room', 'group', 'host_user', 'required_tag')
