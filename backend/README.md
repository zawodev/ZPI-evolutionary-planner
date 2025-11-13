OptiSlots: The Evolutionary Timetabling System

Optislots is a powerful, web-based timetabling solution designed for complex organizations like universities and large businesses. It uses a high-performance C++ evolutionary algorithm to automatically generate optimal schedules based on a wide range of constraints and user preferences.

Key Features

Evolutionary Optimization: Utilizes a C++ genetic algorithm backend for high-speed, efficient schedule computation.

Preference-Based Scheduling: Allows users (e.g., students, teachers) to submit their availability and preferences (e.g., "prefer morning classes," "avoid gaps") via a simple web interface.

Multi-Tenancy: Manages multiple organizations, users, and groups, each with its own set of rules and constraints.

Asynchronous Job Processing: Optimization tasks are handled as background jobs, allowing the system to remain responsive. Users can view the progress of optimization in real-time.

Modern Tech Stack: Built with a React/Next.js frontend, a Django backend, and a C++ optimization service, all orchestrated with Docker and Redis.

System Architecture

The project operates on a microservice-based architecture:

Frontend (/frontend/evoplanner_frontend): A Next.js (React) application that provides the user interface. Users log in, view their assigned "Recruitments" (e.g., "Fall Semester 2025"), and submit their scheduling preferences on a visual grid.

Backend (/backend): A Django application that serves as the main API and data hub. It manages user identity, organizations, rooms, subjects, and constraints.

Optimizer Service (/optimizer_service): A standalone C++ application that contains the core genetic algorithm. It's built for high performance to handle complex optimization problems.

Redis (via docker-compose.yml): Acts as the message broker between the Backend and the Optimizer Service.

How it Works: The Optimization Workflow

Job Submission: An administrator (or an automated scheduler) triggers an optimization for a specific "Recruitment."

The Django Backend compiles all relevant data (user preferences, constraints, room capacities, etc.) into a JSON problem definition.

This problem is placed as a job onto a Redis queue (optimizer:jobs).

The C++ Optimizer Service is continuously listening to this queue. It dequeues the job and starts the evolutionary algorithm.

Real-time Progress: As the algorithm runs, the C++ service publishes progress updates (e.g., current iteration, best fitness score) to a Redis pub/sub channel (optimizer:progress:updates).

A separate Django background worker (listen_progress) subscribes to this channel. It receives the updates and saves them to the database, forwarding them to the frontend via WebSockets.

Completion: Once the optimization is complete (or hits the time limit), the C++ service publishes the final, optimized schedule (the "fittest individual") as a JSON object. The listener saves this solution, and the final schedule becomes available to all users in the frontend.

Getting Started

Prerequisites

Docker and Docker Compose

Node.js (v18 or later) and npm

1. Running the Application (Docker)

The entire backend stack (Django API, C++ Optimizer, background workers, and Redis) is managed by Docker Compose. The frontend is run locally for development.

Step 1: Start the Backend Services

This single command will build and start the backend, optimizer, progress-listener, scheduler, and redis services in detached mode.

docker compose up -d


Step 2: Start the Frontend

In a separate terminal, navigate to the frontend directory, install dependencies, and run the development server.

cd frontend/evoplanner_frontend
npm install
npm run dev


The frontend will be available at http://localhost:3000.
The Django API will be available at http://localhost:8000.

2. Manual Backend Development (Without Docker)

<i>To be updated</i>

Utility Scripts

Full System Cleanup

Warning: This will stop and delete all Docker containers, images, volumes, and networks on your system. Use with caution.

This is useful for clearing out old data and ensuring a clean build.

# Stop all running containers
docker stop $(docker ps -q) 2>/dev/null || true

# Remove all containers
docker rm -f $(docker ps -aq) 2>/dev/null || true

# Remove all images
docker rmi -f $(docker images -aq) 2>/dev/null || true

# Remove all volumes
docker volume rm $(docker volume ls -q) 2>/dev/null || true

# Prune networks, builder cache, and system
docker network prune -f
docker builder prune -af
docker system prune -a --volumes -f
