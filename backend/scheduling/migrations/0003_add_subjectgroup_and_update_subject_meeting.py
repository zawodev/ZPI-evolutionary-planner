# Generated manually

import django.db.models.deletion
import uuid
from django.conf import settings
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('scheduling', '0002_rename_end_date_recruitment_expiration_date_and_more'),
        ('identity', '0002_user_organization'),
    ]

    operations = [
        # Add duration_blocks to Subject
        migrations.AddField(
            model_name='subject',
            name='duration_blocks',
            field=models.IntegerField(
                default=4,
                help_text='Duration in 15-minute blocks (e.g., 4 = 1 hour)'
            ),
        ),
        
        # Create SubjectGroup model
        migrations.CreateModel(
            name='SubjectGroup',
            fields=[
                ('subject_group_id', models.UUIDField(default=uuid.uuid4, editable=False, primary_key=True, serialize=False)),
                ('group', models.ForeignKey(db_column='groupid', on_delete=django.db.models.deletion.CASCADE, related_name='subject_groups', to='identity.group')),
                ('recruitment', models.ForeignKey(db_column='recruitmentid', on_delete=django.db.models.deletion.CASCADE, related_name='subject_groups', to='scheduling.recruitment')),
                ('subject', models.ForeignKey(db_column='subjectid', on_delete=django.db.models.deletion.CASCADE, related_name='subject_groups', to='scheduling.subject')),
            ],
            options={
                'db_table': 'scheduling_subjectgroups',
                'unique_together': {('subject', 'recruitment', 'group')},
            },
        ),
        
        # Remove old fields from Meeting
        migrations.RemoveField(
            model_name='meeting',
            name='subject',
        ),
        migrations.RemoveField(
            model_name='meeting',
            name='group',
        ),
        migrations.RemoveField(
            model_name='meeting',
            name='end_hour',
        ),
        
        # Add subject_group to Meeting
        migrations.AddField(
            model_name='meeting',
            name='subject_group',
            field=models.ForeignKey(
                db_column='subjectgroupid',
                on_delete=django.db.models.deletion.CASCADE,
                related_name='meetings',
                to='scheduling.subjectgroup',
                default=None
            ),
            preserve_default=False,
        ),
    ]
