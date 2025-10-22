from django.db import models
from django.conf import settings
from identity.models import User, Group
import uuid


class Subject(models.Model):
    subject_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    subject_name = models.CharField(max_length=255)

    class Meta:
        db_table = 'scheduling_subjects'

    def __str__(self):
        return self.subject_name


class Recruitment(models.Model):
    CYCLE_TYPE_CHOICES = [
        ('weekly', 'Weekly'),
        ('biweekly', 'Biweekly'),
        ('monthly', 'Monthly'),
    ]
    STATUS_CHOICES = [
        ('archived', 'Archived'),
        ('draft', 'Draft'),
        ('optimizing', 'Optimizing'),
        ('active', 'Active'),
    ]

    recruitment_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    recruitment_name = models.CharField(max_length=255)

    day_start_time = models.TimeField(blank=True, null=True)
    day_end_time = models.TimeField(blank=True, null=True)

    host_prefs_start_date = models.DateField(blank=True, null=True) # zbieranie preferencji hostów
    user_prefs_start_date = models.DateField(blank=True, null=True) # zbieranie preferencji użytkowników
    optimization_start_date = models.DateField(blank=True, null=True) # rozpoczęcie optymalizacji
    optimization_end_date = models.DateField(blank=True, null=True) # zakończenie optymalizacji
    expiration_date = models.DateField(blank=True, null=True) # data wygaśnięcia rekrutacji

    preference_threshold = models.FloatField(default=0.5)
    users_submitted_count = models.IntegerField(default=0)

    cycle_type = models.CharField(
        max_length=20,
        choices=CYCLE_TYPE_CHOICES,
        default='weekly',
        db_column='cycletype'
    )
    plan_status = models.CharField(max_length=20, choices=STATUS_CHOICES, default='draft')
    default_token_count = models.IntegerField(default=3)
    max_round_execution_time = models.IntegerField(default=300) # in seconds

    class Meta:
        db_table = 'scheduling_recruitments'

    def __str__(self):
        return f"{self.recruitment_name} ({self.cycle_type})"


class Room(models.Model):
    room_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    building_name = models.CharField(max_length=255)
    room_number = models.CharField(max_length=50)
    capacity = models.IntegerField()

    class Meta:
        db_table = 'scheduling_rooms'

    def __str__(self):
        return f"{self.building_name} - {self.room_number}"


class Tag(models.Model):
    tag_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    tag_name = models.CharField(max_length=100, unique=True)

    class Meta:
        db_table = 'scheduling_tags'

    def __str__(self):
        return self.tag_name


class RoomTag(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    room = models.ForeignKey(
        Room,
        on_delete=models.CASCADE,
        db_column='roomid',
        related_name='room_tags'
    )
    tag = models.ForeignKey(
        Tag,
        on_delete=models.CASCADE,
        db_column='tagid',
        related_name='tagged_rooms'
    )

    class Meta:
        db_table = 'scheduling_roomtags'
        unique_together = ('room', 'tag')

    def __str__(self):
        return f"{self.room} - {self.tag}"


class Meeting(models.Model):
    meeting_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    recruitment = models.ForeignKey(
        Recruitment,
        on_delete=models.CASCADE,
        db_column='recruitmentid',
        related_name='meetings'
    )
    subject = models.ForeignKey(
        Subject,
        on_delete=models.CASCADE,
        db_column='subjectid',
        related_name='meetings'
    )
    room = models.ForeignKey(
        Room,
        on_delete=models.CASCADE,
        db_column='roomid',
        related_name='meetings'
    )
    group = models.ForeignKey(
        Group,
        on_delete=models.CASCADE,
        db_column='groupid',
        related_name='meetings'
    )
    host_user = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.CASCADE,
        db_column='teacherid',
        related_name='taught_meetings'
    )
    required_tag = models.ForeignKey(
        Tag,
        on_delete=models.CASCADE,
        db_column='requiredtagid',
        blank=True,
        null=True
    )
    start_hour = models.IntegerField(help_text="Start hour (0-23)")
    end_hour = models.IntegerField(help_text="End hour (0-23)")
    day_of_week = models.IntegerField(help_text="Day of week (0=Monday, 6=Sunday)")
    day_of_cycle = models.IntegerField(help_text="Day in cycle: weekly 0-6, biweekly 0-13, monthly 0-30")

    class Meta:
        db_table = 'scheduling_meetings'

    def __str__(self):
        return f"Meeting: {self.subject} - {self.group} ({self.day_of_week})"
