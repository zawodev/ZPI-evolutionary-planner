from django.urls import re_path
from . import consumers

websocket_urlpatterns = [
    re_path(r'ws/jobs/(?P<job_id>[0-9a-f-]+)/$', consumers.JobProgressConsumer.as_asgi()),
]
