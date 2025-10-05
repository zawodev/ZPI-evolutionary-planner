from django.contrib import admin
from .models import User, Organization, Group, UserGroup

# Register your models here.
admin.site.register(User)
admin.site.register(Organization)
admin.site.register(Group)
admin.site.register(UserGroup)