from django.db import models
from django.conf import settings
from identity.models import User, Group


class Subject(models.Model):
    subject_id = models.AutoField(primary_key=True)
    subject_name = models.CharField(max_length=255)

    class Meta:
        db_table = 'scheduling_subjects'

    def __str__(self):
        return self.subject_name


class Recruitment(models.Model):
    SEMESTER_CHOICES = [
        ('winter', 'Winter'),
        ('summer', 'Summer'),
    ]
    
    recruitment_id = models.AutoField(primary_key=True)
    recruitment_name = models.CharField(max_length=255)
    year = models.IntegerField()
    semester = models.CharField(max_length=20, choices=SEMESTER_CHOICES)

    class Meta:
        db_table = 'scheduling_recruitments'

    def __str__(self):
        return f"{self.recruitment_name} ({self.year} - {self.semester})"


class Plan(models.Model):
    STATUS_CHOICES = [
        ('draft', 'Draft'),
        ('ready', 'Ready'),
        ('archived', 'Archived'),
    ]
    
    plan_id = models.AutoField(primary_key=True)
    recruitment = models.ForeignKey(
        Recruitment,
        on_delete=models.CASCADE,
        db_column='recruitmentid',
        related_name='plans'
    )
    name = models.CharField(max_length=255)
    description = models.TextField(blank=True, null=True)
    created_at = models.DateTimeField(auto_now_add=True)
    created_by = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.SET_NULL,
        null=True,
        db_column='createdby',
        related_name='created_plans'
    )
    status = models.CharField(max_length=20, choices=STATUS_CHOICES, default='draft')

    class Meta:
        db_table = 'scheduling_plans'

    def __str__(self):
        return f"{self.name} ({self.status})"


class Room(models.Model):
    room_id = models.AutoField(primary_key=True)
    building_name = models.CharField(max_length=255)
    room_number = models.CharField(max_length=50)
    capacity = models.IntegerField()

    class Meta:
        db_table = 'scheduling_rooms'

    def __str__(self):
        return f"{self.building_name} - {self.room_number}"


class Tag(models.Model):
    tag_id = models.AutoField(primary_key=True)
    tag_name = models.CharField(max_length=100, unique=True)

    class Meta:
        db_table = 'scheduling_tags'

    def __str__(self):
        return self.tag_name


class RoomTag(models.Model):
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
    CYCLE_TYPE_CHOICES = [
        ('weekly', 'Weekly'),
        ('biweekly', 'Biweekly'),
        ('monthly', 'Monthly'),
    ]
    
    meeting_id = models.AutoField(primary_key=True)
    plan = models.ForeignKey(
        Plan,
        on_delete=models.CASCADE,
        db_column='planid',
        related_name='meetings'
    )
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
    teacher = models.ForeignKey(
        settings.AUTH_USER_MODEL,
        on_delete=models.CASCADE,
        db_column='teacherid',
        related_name='taught_meetings'
    )
    start_hour = models.IntegerField(help_text="Start hour (0-23)")
    end_hour = models.IntegerField(help_text="End hour (0-23)")
    day_of_week = models.IntegerField(help_text="Day of week (0=Monday, 6=Sunday)")
    day_of_cycle = models.IntegerField(help_text="Day in cycle: weekly 0-6, biweekly 0-13, monthly 0-30")
    cycle_type = models.CharField(max_length=20, choices=CYCLE_TYPE_CHOICES, default='weekly')

    class Meta:
        db_table = 'scheduling_meetings'

    def __str__(self):
        return f"Meeting: {self.subject} - {self.group} ({self.day_of_week})"
