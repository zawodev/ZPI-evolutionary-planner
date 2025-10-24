# Generated manually

import django.db.models.deletion
import uuid
from django.conf import settings
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('identity', '0002_user_organization'),
        ('scheduling', '0003_add_subjectgroup_and_update_subject_meeting'),
    ]

    operations = [
        migrations.CreateModel(
            name='UserRecruitment',
            fields=[
                ('id', models.UUIDField(default=uuid.uuid4, editable=False, primary_key=True, serialize=False)),
                ('recruitment', models.ForeignKey(db_column='recruitmentid', on_delete=django.db.models.deletion.CASCADE, related_name='recruitment_users', to='scheduling.recruitment')),
                ('user', models.ForeignKey(db_column='userid', on_delete=django.db.models.deletion.CASCADE, related_name='user_recruitments', to=settings.AUTH_USER_MODEL)),
            ],
            options={
                'db_table': 'identity_userrecruitments',
                'unique_together': {('user', 'recruitment')},
            },
        ),
    ]
