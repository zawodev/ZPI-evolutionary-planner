from rest_framework import generics, permissions, status
from rest_framework.response import Response
from rest_framework.views import APIView
from django.contrib.auth import get_user_model
from django.shortcuts import get_object_or_404
from .models import Organization, Group, UserGroup
from rest_framework_simplejwt.tokens import RefreshToken
from .serializers import (
    RegisterSerializer, UserSerializer, OrganizationSerializer, GroupSerializer, UserGroupSerializer
)

User = get_user_model()


class RegisterView(generics.CreateAPIView):
    queryset = User.objects.all()
    serializer_class = RegisterSerializer
    permission_classes = [permissions.AllowAny]


class LoginView(APIView):
    permission_classes = [permissions.AllowAny]

    def post(self, request):
        from django.contrib.auth import authenticate

        username = request.data.get('username')
        password = request.data.get('password')
        user = authenticate(request, username=username, password=password)

        if not user:
            return Response({"detail": "Invalid logging credentials 😨"}, status=400)

        refresh = RefreshToken.for_user(user)
        return Response({
            'refresh': str(refresh),
            'access': str(refresh.access_token),
            'user': UserSerializer(user).data
        })


class LogoutView(APIView):
    def post(self, request):
        try:
            refresh_token = request.data["refresh"]
            token = RefreshToken(refresh_token)
            return Response({"detail": "Logged out 😎"})
        except Exception:
            return Response({"detail": "Error logging out 🤔"}, status=400)


class UserProfileView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def get(self, request):
        serializer = UserSerializer(request.user)
        return Response(serializer.data)


class OrganizationAddView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        serializer = OrganizationSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


class OrganizationDeleteView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def delete(self, request, organization_id):
        organization = get_object_or_404(Organization, pk=organization_id)
        organization.delete()
        return Response({"detail": "Organization deleted 🐉"}, status=status.HTTP_204_NO_CONTENT)


class GroupAddView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        serializer = GroupSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


class GroupDeleteView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def delete(self, request, group_id):
        group = get_object_or_404(Group, pk=group_id)
        group.delete()
        return Response({"detail": "Group deleted 🙁"}, status=status.HTTP_204_NO_CONTENT)


class UserGroupAddView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        """
        Dodaj użytkownika do grupy.
        """
        serializer = UserGroupSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


class UserGroupDeleteView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def delete(self, request):
        """
        Usuń użytkownika z grupy (wymaga user i group w body).
        """
        user_id = request.data.get('user')
        group_id = request.data.get('group')

        if not user_id or not group_id:
            return Response(
                {"detail": "Required fields: user, group 😨"},
                status=status.HTTP_400_BAD_REQUEST
            )

        try:
            user_group = UserGroup.objects.get(user_id=user_id, group_id=group_id)
            user_group.delete()
            return Response({"detail": "User removed from group 💥"}, status=status.HTTP_204_NO_CONTENT)
        except UserGroup.DoesNotExist:
            return Response(
                {"detail": "Relation user–group does not exist 🚨"},
                status=status.HTTP_404_NOT_FOUND
            )