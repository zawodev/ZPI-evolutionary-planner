# python
from datetime import datetime, time, timedelta

from .models import Meeting


def _time_to_decimal_hours(t: time) -> float:
    return t.hour + t.minute / 60 + t.second / 3600


def _datetime_to_decimal_hours(dt: datetime) -> float:
    return dt.hour + dt.minute / 60 + dt.second / 3600


def _ranges_overlap(a_start: float, a_end: float, b_start: float, b_end: float) -> bool:
    return (a_start < b_end) and (b_start < a_end)


def is_room_available(room, start_dt: datetime, end_dt: datetime) -> bool:
    """
    Sprawdza dostępność `room` w przedziale [start_dt, end_dt).
    Zwraca True jeśli brak kolizji, False jeśli istnieje spotkanie kolidujące.

    Założenia:
    - `Meeting.start_hour` i `end_hour` to całkowite godziny (0-23).
    - `Meeting` jest aktywne dla danego dnia jeśli:
        * rekrutacja ma start_date/end_date i data mieści się w tym zakresie (jeśli brak start_date traktujemy jako zawsze aktywna),
        * oraz obliczony day_in_cycle zgodny z typem cyklu (weekly/biweekly/monthly) zgadza się z `meeting.day_of_cycle`.
    """

    if start_dt >= end_dt:
        return False

    meetings_qs = Meeting.objects.filter(room=room)

    start_date = start_dt.date()
    end_date = (end_dt - timedelta(seconds=1)).date()

    day = start_date
    while day <= end_date:
        # godzinowy zakres do sprawdzenia na tym dniu
        if day == start_date:
            day_start_hr = _datetime_to_decimal_hours(start_dt)
        else:
            day_start_hr = 0.0

        if day == end_date:
            day_end_hr = _datetime_to_decimal_hours(end_dt)
        else:
            day_end_hr = 24.0

        # jeśli zakres w tym dniu jest pusty, pomiń
        if day_start_hr >= day_end_hr:
            day += timedelta(days=1)
            continue

        # pobierz spotkania tego pokoju — dodatkowo można filtrować po planach/rekrutacjach jeśli chcesz zmniejszyć liczbę wyników
        for meeting in meetings_qs:
            # sprawdzenie czy rekrutacja obejmuje ten dzień (jeśli zdefiniowano)
            recruitment = meeting.recruitment
            if recruitment.start_date and day < recruitment.start_date:
                continue
            if recruitment.end_date and day > recruitment.end_date:
                continue

            # oblicz dzień w cyklu zależnie od typu cyklu
            if recruitment.start_date:
                days_since_start = (day - recruitment.start_date).days
            else:
                # brak start_date — traktujemy jako zawsze aktywne i dopasowujemy po day_of_week
                days_since_start = None

            cycle_type = recruitment.cycle_type or 'weekly'
            if cycle_type == 'weekly':
                expected_day_in_cycle = day.weekday()  # 0-6
            elif cycle_type == 'biweekly':
                if days_since_start is None:
                    # brak punktu odniesienia — porównanie po day_of_week jako zapas
                    expected_day_in_cycle = day.weekday()
                else:
                    expected_day_in_cycle = days_since_start % 14
            elif cycle_type == 'monthly':
                if days_since_start is None:
                    expected_day_in_cycle = day.day - 1  # stara hipoteza, jeśli brak start_date
                else:
                    expected_day_in_cycle = days_since_start % 31
            else:
                expected_day_in_cycle = day.weekday()

            # jeśli meeting przechowuje day_of_cycle zgodnie z opisem, porównujemy
            if meeting.day_of_cycle is not None:
                if meeting.day_of_cycle != expected_day_in_cycle:
                    continue
            else:
                # fallback: sprawdź dzień tygodnia
                if meeting.day_of_week != day.weekday():
                    continue

            # sprawdź nakładanie się godzin
            meeting_start = float(meeting.start_hour)
            meeting_end = float(meeting.end_hour)
            if _ranges_overlap(day_start_hr, day_end_hr, meeting_start, meeting_end):
                return False  # kolizja znaleziona

        day += timedelta(days=1)

    return True
