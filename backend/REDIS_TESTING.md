# Instrukcje testowania Redis API w Postman

## Przygotowanie środowiska

1. **Uruchom Redis**:
   ```bash
   # Windows (jeśli masz Redis zainstalowany)
   redis-server
   
   # Lub za pomocą Docker
   docker run -d -p 6379:6379 redis:latest
   ```

2. **Uruchom Django backend**:
   ```bash
   cd backend
   python manage.py runserver
   ```

3. **Uruchom progress listener w osobnym terminalu**:
   ```bash
   cd backend
   python manage.py listen_progress --verbose
   ```

4. **Uruchom optimizer service (C++)** w osobnym terminalu:
   ```bash
   cd optimizer_service
   # kompiluj i uruchom optimizer
   ./your_optimizer_executable
   ```

## Testowanie w Postman

### 1. Sprawdź status serwisu
**GET** `http://localhost:8000/api/v1/health/`

Oczekiwany response:
```json
{
    "status": "healthy",
    "timestamp": "2025-01-09T...",
    "services": {
        "database": "ok",
        "redis": "ok"
    }
}
```

### 2. Wyślij zadanie optymalizacji
**POST** `http://localhost:8000/api/v1/jobs/`

Headers:
```
Content-Type: application/json
```

Body (raw JSON) - skopiuj zawartość z pliku `test_example.json`:
```json
{
  "problem_data": {
    "students": [
      {
        "id": 1,
        "name": "Student A",
        "enrolled_subjects": [1, 2]
      },
      ...
    ],
    ...
    "max_execution_time": 30
  }
}
```

Oczekiwany response (201 Created):
```json
{
    "id": "uuid-job-id",
    "status": "queued",
    "created_at": "...",
    "max_execution_time": 30,
    "problem_data": {...}
}
```

**SKOPIUJ job_id z response - będzie potrzebny do następnych requestów!**

### 3. Sprawdź status zadania
**GET** `http://localhost:8000/api/v1/jobs/{job_id}/status/`

Oczekiwany response:
```json
{
    "job_id": "uuid-job-id",
    "status": "running",
    "created_at": "...",
    "updated_at": "...",
    "current_iteration": 5,
    "max_execution_time": 30
}
```

### 4. Pobierz szczegóły zadania
**GET** `http://localhost:8000/api/v1/jobs/{job_id}/`

### 5. Pobierz historię postępu
**GET** `http://localhost:8000/api/v1/jobs/{job_id}/progress/`

### 6. Anuluj zadanie (opcjonalnie)
**POST** `http://localhost:8000/api/v1/jobs/{job_id}/cancel/`

Body (może być pusty):
```json
{}
```

## Co powinno się dziać w konsolach

### W konsoli Django (progress listener):
```
[REDIS] Started listening for progress updates
[REDIS] Received progress update for job uuid-job-id, iteration 0
[REDIS] Updated database for job uuid-job-id, iteration 0
[WEBSOCKET] Sent progress_update for job uuid-job-id
```

### W konsoli Django (runserver):
```
[DJANGO] Submitted job uuid-job-id with max_execution_time: 30s
[REDIS] Published job to queue: uuid-job-id
```

### W konsoli C++ Optimizer:
```
[REDIS] Received job message from Redis: {"job_id":"uuid-job-id",...}
[REDIS] Sending progress to Redis for job uuid-job-id, iteration 0
[REDIS] Successfully sent progress for job: uuid-job-id (notified 1 subscribers)
```

## Debugowanie

### Sprawdź Redis ręcznie
```bash
redis-cli

# Sprawdź kolejkę zadań
LLEN optimizer:jobs
LRANGE optimizer:jobs 0 -1

# Sprawdź progress konkretnego job
GET optimizer:progress:{job_id}

# Sprawdź cancel flag
GET optimizer:cancel:{job_id}

# Sprawdź subskrypcje
PUBSUB CHANNELS
```

### Sprawdź logi Django
- Serwer Django powinien pokazywać requesty
- Progress listener powinien pokazywać otrzymane updates
- Błędy Redis połączenia będą widoczne w logach

### Sprawdź WebSocket (opcjonalnie)
Możesz połączyć się WebSocketem na:
`ws://localhost:8000/ws/jobs/{job_id}/progress/`

## Oczekiwany przepływ
1. POST job → job zapisany w bazie, wysłany do Redis queue
2. C++ optimizer odbiera z queue → zaczyna przetwarzanie
3. C++ wysyła progress updates → Django progress listener je odbiera
4. Django aktualizuje bazę danych → wysyła WebSocket updates
5. Zadanie kończy się (-1 iteration) → status zmienia się na "completed"

Jeśli wszystko działa, zobaczysz progress updates w real-time przez API!
