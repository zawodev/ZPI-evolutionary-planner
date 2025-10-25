# Generated manually

import django.db.models.deletion
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('optimizer', '0001_initial'),
        ('scheduling', '0002_rename_end_date_recruitment_expiration_date_and_more'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='optimizationjob',
            name='user',
        ),
        migrations.AddField(
            model_name='optimizationjob',
            name='recruitment',
            field=models.ForeignKey(
                default=None,
                on_delete=django.db.models.deletion.CASCADE,
                related_name='optimization_jobs',
                to='scheduling.recruitment'
            ),
            preserve_default=False,
        ),
    ]
