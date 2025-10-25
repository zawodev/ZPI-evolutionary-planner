from django.urls import path
from .views import (
    RegisterView,
    LoginView,
    LogoutView,
    UserProfileView,
    OrganizationAddView,
    OrganizationDeleteView,
    GroupAddView,
    GroupDeleteView,
    UserGroupAddView,
    UserGroupDeleteView,
    UserRecruitmentAddView,
    UserRecruitmentDeleteView,
    ActiveMeetingsByUserView,
    OfficeUserCreateView,
    SetUserPasswordView,
    RecruitmentsByUserView,
)

urlpatterns = [
    path('register/', RegisterView.as_view(), name='register'),
    path('login/', LoginView.as_view(), name='login'),
    path('logout/', LogoutView.as_view(), name='logout'),

    path('users/', UserProfileView.as_view(), name='user'),
    path('users/<user_pk>/availability/', ActiveMeetingsByUserView.as_view(), name='user-availability'),
    path('users/<user_pk>/recruitments/', RecruitmentsByUserView.as_view(), name='user-recruitments'),
    path('users/create/', OfficeUserCreateView.as_view(), name='office-user-create'),
    path('users/<user_pk>/set_password/', SetUserPasswordView.as_view(), name='set-user-password'),

    path('organizations/add/', OrganizationAddView.as_view(), name='organization-add'),
    path('organizations/delete/<uuid:organization_id>/', OrganizationDeleteView.as_view(), name='organization-delete'),

    path('groups/add/', GroupAddView.as_view(), name='group-add'),
    path('groups/delete/<uuid:group_id>/', GroupDeleteView.as_view(), name='group-delete'),

    path('user-groups/add/', UserGroupAddView.as_view(), name='usergroup-add'),
    path('user-groups/delete/', UserGroupDeleteView.as_view(), name='usergroup-delete'),

    path('user-recruitments/add/', UserRecruitmentAddView.as_view(), name='user-recruitment-add'),
    path('user-recruitments/delete/', UserRecruitmentDeleteView.as_view(), name='user-recruitment-delete'),
]