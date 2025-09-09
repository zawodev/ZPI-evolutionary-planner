from rest_framework import serializers
from .models import OptimizationJob, OptimizationProgress
import jsonschema


class ProblemDataSerializer(serializers.Serializer):
    """Serializer for ProblemData structure"""
    constraints = serializers.DictField()
    preferences = serializers.DictField()
    max_execution_time = serializers.IntegerField(min_value=10, max_value=3600)
    
    def validate_constraints(self, value):
        """Validate constraints structure"""
        required_fields = [
            'timeslots_per_day', 'groups_per_subject', 'groups_soft_capacity',
            'students_subjects', 'teachers_groups', 'rooms_unavailability_timeslots'
        ]
        
        for field in required_fields:
            if field not in value:
                raise serializers.ValidationError(f"Missing required field: {field}")
        
        return value
    
    def validate_preferences(self, value):
        """Validate preferences structure"""
        required_sections = ['students', 'teachers', 'management']
        
        for section in required_sections:
            if section not in value:
                raise serializers.ValidationError(f"Missing required section: {section}")
        
        return value


class OptimizationJobCreateSerializer(serializers.Serializer):
    """Serializer for creating optimization jobs"""
    problem_data = ProblemDataSerializer()
    
    def create(self, validated_data):
        """Create a new optimization job"""
        from .models import OptimizationJob
        
        job = OptimizationJob.objects.create(
            problem_data=validated_data['problem_data'],
            max_execution_time=validated_data['problem_data']['max_execution_time'],
            user=self.context['request'].user if self.context['request'].user.is_authenticated else None
        )
        return job


class OptimizationProgressSerializer(serializers.ModelSerializer):
    """Serializer for optimization progress updates"""
    
    class Meta:
        model = OptimizationProgress
        fields = [
            'iteration', 'timestamp', 'best_solution'
        ]
        read_only_fields = ['timestamp']


class OptimizationJobSerializer(serializers.ModelSerializer):
    """Serializer for optimization job details"""
    latest_progress = serializers.SerializerMethodField()
    
    class Meta:
        model = OptimizationJob
        fields = [
            'id', 'status', 'max_execution_time', 'created_at', 'updated_at',
            'started_at', 'completed_at', 'error_message', 'final_solution',
            'current_iteration', 'latest_progress'
        ]
        read_only_fields = [
            'id', 'created_at', 'updated_at', 'started_at', 'completed_at',
            'current_iteration'
        ]
    
    def get_latest_progress(self, obj):
        """Get the latest progress update"""
        latest = obj.progress_updates.first()
        if latest:
            return OptimizationProgressSerializer(latest).data
        return None


class OptimizationJobListSerializer(serializers.ModelSerializer):
    """Serializer for job listings"""
    
    class Meta:
        model = OptimizationJob
        fields = [
            'id', 'status', 'created_at', 'updated_at', 'current_iteration'
        ]

class JobCancelSerializer(serializers.Serializer):
    """Serializer for job cancellation requests"""
    reason = serializers.CharField(max_length=500, required=False, allow_blank=True)
    
    def validate(self, data):
        """Validate that the job can be cancelled"""
        job = self.context.get('job')
        if job and job.status not in ['queued', 'running']:
            raise serializers.ValidationError(
                f"Cannot cancel job with status: {job.status}"
            )
        return data
