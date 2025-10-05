from django.contrib import admin
from .models import Subject, Recruitment, Plan, Room, Tag, RoomTag, Meeting


@admin.register(Subject)
class SubjectAdmin(admin.ModelAdmin):
    list_display = ('subject_id', 'subject_name')
    search_fields = ('subject_name',)


@admin.register(Recruitment)
class RecruitmentAdmin(admin.ModelAdmin):
    list_display = ('recruitment_id', 'recruitment_name', 'year', 'semester')
    list_filter = ('year', 'semester')
    search_fields = ('recruitment_name',)


@admin.register(Plan)
class PlanAdmin(admin.ModelAdmin):
    list_display = ('plan_id', 'name', 'recruitment', 'status', 'created_by', 'created_at')
    list_filter = ('status', 'created_at')
    search_fields = ('name', 'description')
    readonly_fields = ('created_at',)


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
    list_display = ('meeting_id', 'subject', 'group', 'teacher', 'room', 'day_of_week', 'start_hour', 'end_hour', 'cycle_type')
    list_filter = ('cycle_type', 'day_of_week', 'recruitment')
    search_fields = ('subject__subject_name', 'group__group_name', 'teacher__first_name', 'teacher__last_name')
    raw_id_fields = ('plan', 'recruitment', 'subject', 'room', 'group', 'teacher')

