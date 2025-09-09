# Testowanie Backend z Postmanem

## 1. Uruchomienie środowiska

```bash
# Terminal 1: Uruchom docker-compose (Redis + Backend + Progress Listener)
docker-compose up

# Terminal 2: Uruchom optimizer C++
cd optimizer_service
# skompiluj i uruchom optimizer
```

## 2. Testowanie w Postmanie

### Endpoint: Utworzenie Job
- **Metoda**: POST
- **URL**: `http://localhost:8000/api/v1/jobs/`
- **Headers**: 
  - `Content-Type: application/json`
- **Body** (raw JSON): Użyj zawartości z `test_example.json`

### Endpoint: Status Job
- **Metoda**: GET
- **URL**: `http://localhost:8000/api/v1/jobs/{job_id}/status/`
- Gdzie `{job_id}` to UUID zwrócone z utworzenia job

### Endpoint: Lista Jobs
- **Metoda**: GET
- **URL**: `http://localhost:8000/api/v1/jobs/`

### Endpoint: Anulowanie Job
- **Metoda**: POST
- **URL**: `http://localhost:8000/api/v1/jobs/{job_id}/cancel/`
- **Headers**: `Content-Type: application/json`
- **Body**: `{"reason": "Testing cancellation"}`

### Endpoint: Health Check
- **Metoda**: GET
- **URL**: `http://localhost:8000/api/v1/health/`

## 3. Co obserwować

**W konsoli Django** (backend):
```
[DJANGO] Submitted job {uuid} with max_execution_time: 30s
[REDIS] Published job to queue: {uuid}
[REDIS] Received progress update for job {uuid}, iteration 0
[REDIS] Updated database for job {uuid}, iteration 0
[WEBSOCKET] Sent progress_update for job {uuid}
```

**W konsoli C++ optimizer**:
```
[REDIS] Published job to queue: {uuid}
Job {uuid} - ProblemData with X students, Y groups...
Iteration 0, best fitness: 0.XXX
[REDIS] Set cancel flag for job: {uuid} (przy anulowaniu)
```

## 4. Struktura odpowiedzi

**Utworzenie job**:
```json
{
  "id": "uuid-string",
  "status": "queued",
  "max_execution_time": 30,
  "created_at": "2025-09-09T...",
  "current_iteration": 0,
  "latest_progress": null
}
```

**Progress updates** (przez WebSocket lub status endpoint):
```json
{
  "job_id": "uuid",
  "iteration": 0,
  "best_solution": {
    "fitness": 0.77,
    "genotype": [...],
    "by_student": [...],
    "by_group": [...]
  }
}
```
