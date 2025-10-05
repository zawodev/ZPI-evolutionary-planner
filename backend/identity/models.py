import uuid

from django.contrib.auth.models import AbstractUser
from django.db import models


class User(AbstractUser):
    ROLE_CHOICES = [
        ('admin', 'Admin'),
        ('office', 'Office'),
        ('leader', 'Student'),
        ('participant', 'Participant')
    ]
    role = models.CharField(max_length=20, choices=ROLE_CHOICES, default='participant', null=False)

    class Meta:
        db_table = 'core_users'

    def __str__(self):
        return f"{self.first_name} {self.last_name} ({self.username})"


class Organization(models.Model):
    organization_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    organization_name = models.CharField(max_length=255)

    class Meta:
        db_table = 'core_organizations'

    def __str__(self):
        return self.organization_name


class Group(models.Model):
    group_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    group_name = models.CharField(max_length=255)
    category = models.CharField(max_length=100)
    organization = models.ForeignKey(
        Organization,
        on_delete=models.CASCADE,
        db_column='organizationid',
        related_name='groups'
    )

    class Meta:
        db_table = 'core_groups'

    def __str__(self):
        return self.group_name


class UserGroup(models.Model):
    user = models.ForeignKey(
        User,
        on_delete=models.CASCADE,
        db_column='userid',
        related_name='user_groups'
    )
    group = models.ForeignKey(
        Group,
        on_delete=models.CASCADE,
        db_column='groupid',
        related_name='group_users'
    )

    class Meta:
        db_table = 'core_usergroups'
        unique_together = ('user', 'group')

    def __str__(self):
        return f"{self.user} -> {self.group}"
