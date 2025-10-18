from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status, permissions
from django.shortcuts import get_object_or_404
from datetime import datetime

from .models import Subject, Recruitment, Plan, Room, Tag, RoomTag, Meeting
from .serializers import (
    SubjectSerializer,
    RecruitmentSerializer,
    PlanSerializer,
    RoomSerializer,
    TagSerializer,
    RoomTagSerializer,
    MeetingSerializer
)
from .services import is_room_available


class BaseCrudView(APIView):
    permission_classes = [permissions.IsAuthenticated]
    model = None
    serializer_class = None
    lookup_field = None

    def get(self, request, pk=None):
        if pk:
            instance = get_object_or_404(self.model, **{self.lookup_field: pk})
            serializer = self.serializer_class(instance)
            return Response(serializer.data)
        instances = self.model.objects.all()
        serializer = self.serializer_class(instances, many=True)
        return Response(serializer.data)

    def post(self, request):
        serializer = self.serializer_class(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

    def put(self, request, pk):
        instance = get_object_or_404(self.model, **{self.lookup_field: pk})
        serializer = self.serializer_class(instance, data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

    def patch(self, request, pk):
        instance = get_object_or_404(self.model, **{self.lookup_field: pk})
        serializer = self.serializer_class(instance, data=request.data, partial=True)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

    def delete(self, request, pk):
        instance = get_object_or_404(self.model, **{self.lookup_field: pk})
        instance.delete()
        return Response(status=status.HTTP_204_NO_CONTENT)


class SubjectView(BaseCrudView):
    model = Subject
    serializer_class = SubjectSerializer
    lookup_field = 'subject_id'


class RecruitmentView(BaseCrudView):
    model = Recruitment
    serializer_class = RecruitmentSerializer
    lookup_field = 'recruitment_id'


class PlanView(BaseCrudView):
    model = Plan
    serializer_class = PlanSerializer
    lookup_field = 'plan_id'


class RoomView(BaseCrudView):
    model = Room
    serializer_class = RoomSerializer
    lookup_field = 'room_id'


class TagView(BaseCrudView):
    model = Tag
    serializer_class = TagSerializer
    lookup_field = 'tag_id'


class RoomTagView(BaseCrudView):
    model = RoomTag
    serializer_class = RoomTagSerializer
    lookup_field = 'id'


class MeetingView(BaseCrudView):
    model = Meeting
    serializer_class = MeetingSerializer
    lookup_field = 'meeting_id'


class RoomAvailabilityAPIView(APIView):
    def get(self, request, room_id):
        # oczekujemy query params: ?start=2025-10-17T09:00&end=2025-10-17T11:00
        start = request.query_params.get('start')
        end = request.query_params.get('end')
        if not start or not end:
            return Response({'detail': 'Missing start/end'}, status=status.HTTP_400_BAD_REQUEST)
        try:
            start_dt = datetime.fromisoformat(start)
            end_dt = datetime.fromisoformat(end)
        except ValueError:
            return Response({'detail': 'Bad datetime format'}, status=status.HTTP_400_BAD_REQUEST)

        try:
            room = Room.objects.get(pk=room_id)
        except Room.DoesNotExist:
            return Response({'detail': 'Room not found'}, status=status.HTTP_404_NOT_FOUND)

        available = is_room_available(room, start_dt, end_dt)
        return Response({'available': available})
