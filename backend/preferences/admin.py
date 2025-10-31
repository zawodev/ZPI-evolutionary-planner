from django.contrib import admin
from .models import UserPreferences, Constraints, ManagementPreferences, HeatmapCache


@admin.register(UserPreferences)
class UserPreferencesAdmin(admin.ModelAdmin):
    list_display = ['id', 'user', 'recruitment']
    list_filter = ['recruitment']
    search_fields = ['user__username', 'recruitment__recruitment_name']


@admin.register(Constraints)
class ConstraintsAdmin(admin.ModelAdmin):
    list_display = ['id', 'recruitment']
    list_filter = ['recruitment']
    search_fields = ['recruitment__recruitment_name']


@admin.register(ManagementPreferences)
class ManagementPreferencesAdmin(admin.ModelAdmin):
    list_display = ['id', 'recruitment']
    list_filter = ['recruitment']
    search_fields = ['recruitment__recruitment_name']


@admin.register(HeatmapCache)
class HeatmapCacheAdmin(admin.ModelAdmin):
    list_display = ['id', 'recruitment', 'last_updated', 'cached_value']
    list_filter = ['recruitment']
    search_fields = ['recruitment__recruitment_name']
