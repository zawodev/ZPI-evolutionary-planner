from django.contrib import admin
from .models import UserPreferences, Constraints


@admin.register(UserPreferences)
class UserPreferencesAdmin(admin.ModelAdmin):
    list_display = [field.name for field in UserPreferences._meta.fields]
    list_filter = ['recruitment']
    search_fields = ['user__username', 'recruitment__recruitment_name']


@admin.register(Constraints)
class ConstraintsAdmin(admin.ModelAdmin):
    list_display = [field.name for field in Constraints._meta.fields]
    list_filter = ['recruitment']
    search_fields = ['recruitment__recruitment_name']
