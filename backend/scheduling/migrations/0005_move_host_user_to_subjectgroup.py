# Generated manually

import django.db.models.deletion
from django.conf import settings
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('scheduling', '0003_add_subjectgroup_and_update_subject_meeting'),
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        # Add host_user to SubjectGroup
        migrations.AddField(
            model_name='subjectgroup',
            name='host_user',
            field=models.ForeignKey(
                db_column='hostuserid',
                on_delete=django.db.models.deletion.CASCADE,
                related_name='hosted_subject_groups',
                to=settings.AUTH_USER_MODEL,
                default=None
            ),
            preserve_default=False,
        ),
        
        # Remove host_user from Meeting
        migrations.RemoveField(
            model_name='meeting',
            name='host_user',
        ),
    ]
