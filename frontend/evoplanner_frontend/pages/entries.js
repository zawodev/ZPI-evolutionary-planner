/* pages/entries.js */

import React, { useState, useEffect, useRef } from "react";
import EntriesSidebar from "../components/EntriesSidebar";
import ScheduleHeader from "../components/ScheduleHeader";
import ScheduleColumn from "../components/ScheduleColumn";
import PreferenceModal from "../components/PreferenceModal";
import { calculateSlotPosition, timeToMinutes, minutesToTime } from "../utils/schedule";
import { useAuth } from '../contexts/AuthContext.js'; // <--- DODANO IMPORT KONTEKSTU

export default function EntriesPage() {
  const { user } = useAuth(); // <--- POBRANIE DANYCH UŻYTKOWNIKA

  // Stan dla rekrutacji
  const [recruitments, setRecruitments] = useState([]);
  const [selectedRecruitment, setSelectedRecruitment] = useState(null);
  const [isLoadingRecruitments, setIsLoadingRecruitments] = useState(true);
  const [fetchError, setFetchError] = useState(null); // Błąd ładowania rekrutacji LUB preferencji

  // Stan dla harmonogramu (preferencji)
  const [scheduleData, setScheduleData] = useState({
    monday: [],
    tuesday: [],
    wednesday: [],
    thursday: [],
    friday: []
  });

  const [isLoadingSchedule, setIsLoadingSchedule] = useState(false); // Oddzielny stan ładowania dla preferencji
  const [isSaving, setIsSaving] = useState(false); // Stan zapisywania zmian
  const [saveError, setSaveError] = useState(null); // Błąd zapisywania zmian

  const [isDragging, setIsDragging] = useState(false);
  const [dragStart, setDragStart] = useState(null);
  const [dragEnd, setDragEnd] = useState(null);
  const [dragDay, setDragDay] = useState(null);
  const [showModal, setShowModal] = useState(false);
  const [modalMode, setModalMode] = useState('create');
  const [pendingSlot, setPendingSlot] = useState(null);
  const [editingSlot, setEditingSlot] = useState(null);
  const [maxPriority, setMaxPriority] = useState(40);

  const calendarRef = useRef(null);

  const days = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday'];
  const dayLabels = ['Pon', 'Wt', 'Śr', 'Czw', 'Pt'];
  const hours = ["7:00", "8:00", "9:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00"];

  // Pobieranie listy rekrutacji dla użytkownika
  useEffect(() => {
    if (user && user.id) {
      const fetchRecruitments = async () => {
        setIsLoadingRecruitments(true);
        setFetchError(null);
        const token = localStorage.getItem('access_token');

        if (!token) {
          setFetchError("Brak autoryzacji. Zaloguj się ponownie.");
          setIsLoadingRecruitments(false);
          return;
        }

        try {
          const response = await fetch(`http://127.0.0.1:8000/api/v1/identity/users/${user.id}/recruitments/`, {
            headers: {
              'Authorization': `Bearer ${token}`,
              'Content-Type': 'application/json'
            }
          });

          if (!response.ok) {
            throw new Error(`Błąd pobierania rekrutacji: ${response.statusText}`);
          }
          const data = await response.json();
          setRecruitments(data);
        } catch (error) {
          setFetchError(error.message);
          console.error('Błąd pobierania rekrutacji:', error);
        } finally {
          setIsLoadingRecruitments(false);
        }
      };
      fetchRecruitments();
    } else {
      // Jeśli nie ma użytkownika, nie ładuj
      setIsLoadingRecruitments(false);
    }
  }, [user]); // Uruchom, gdy zmieni się użytkownik

  // Efekt do ładowania preferencji, gdy zmieni się wybrana rekrutacja
  useEffect(() => {
    if (selectedRecruitment && user) {
      const fetchPreferences = async () => {
        setIsLoadingSchedule(true);
        setFetchError(null); // Czyścimy błędy ładowania
        setSaveError(null);  // Czyścimy błędy zapisu
        const token = localStorage.getItem('access_token');

        if (!token) {
          setFetchError("Brak autoryzacji.");
          setIsLoadingSchedule(false);
          return;
        }

        try {
          const response = await fetch(`http://127.0.0.1:8000/api/v1/preferences/user-preferences/${selectedRecruitment.recruitment_id}/${user.id}/`, {
            headers: {
              'Authorization': `Bearer ${token}`,
              'Content-Type': 'application/json'
            }
          });

          const emptySchedule = { monday: [], tuesday: [], wednesday: [], thursday: [], friday: [] };

          if (response.status === 404) {
            // Nie znaleziono preferencji, ustaw domyślny pusty stan
            setScheduleData(emptySchedule);
          } else if (response.ok) {
            const data = await response.json();
            // Sprawdź, czy preferences_data istnieje i jest obiektem
            if (data.preferences_data && typeof data.preferences_data === 'object' && !Array.isArray(data.preferences_data)) {
              setScheduleData(data.preferences_data);
            } else {
              // Dane są puste lub w złym formacie, ustaw domyślny stan
              setScheduleData(emptySchedule);
            }
          } else {
            throw new Error(`Błąd ładowania preferencji: ${response.statusText}`);
          }
        } catch (error) {
          setFetchError(error.message);
          console.error('Błąd ładowania preferencji:', error);
        } finally {
          setIsLoadingSchedule(false);
        }
      };
      fetchPreferences();
    } else {
      // Wyczyść dane, jeśli nie wybrano rekrutacji
      setScheduleData({
        monday: [],
        tuesday: [],
        wednesday: [],
        thursday: [],
        friday: []
      });
    }
  }, [selectedRecruitment, user]);


  const calculateUsedPriority = () => {
    let total = 0;
// ... (reszta funkcji bez zmian)
    days.forEach(day => {
      const validSlots = (scheduleData[day] || []).filter(Boolean);
      validSlots.forEach(slot => {
        total += slot.priority || 0;
      });
    });
    return total;
  };

  const savePreferences = async () => {
    if (!selectedRecruitment || !user) {
      setSaveError("Nie wybrano rekrutacji lub użytkownika.");
      return;
    }
    
    setIsSaving(true);
    setSaveError(null);
    const token = localStorage.getItem('access_token');

    if (!token) {
      setSaveError("Brak autoryzacji. Zaloguj się ponownie.");
      setIsSaving(false);
      return;
    }

    try {
      const response = await fetch(`http://127.0.0.1:8000/api/v1/preferences/user-preferences/${selectedRecruitment.recruitment_id}/${user.id}/`, {
        method: 'PUT',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        // Wysyłamy cały obiekt scheduleData jako 'preferences_data'
        // Backend (views.py) wykryje, że to nie jest {path, value} i zapisze cały obiekt
        body: JSON.stringify(scheduleData) 
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.detail || `Błąd zapisu: ${response.statusText}`);
      }

      // Sukces
      setSaveError(null); 
      // Prosty alert dla użytkownika, można zastąpić ładniejszym powiadomieniem
      alert('Zmiany zapisane pomyślnie!'); 

    } catch (error) {
      setSaveError(error.message);
      console.error('Błąd zapisywania preferencji:', error);
    } finally {
      setIsSaving(false);
    }
  };

  const clearAllPreferences = () => {
// ... (reszta funkcji bez zmian)
    if (window.confirm('Czy na pewno chcesz usunąć wszystkie preferencje? Ta akcja jest nieodwracalna.')) {
      setScheduleData({
        monday: [],
        tuesday: [],
        wednesday: [],
        thursday: [],
        friday: []
      });
      // TODO: Dodać wywołanie savePreferences() po wyczyszczeniu, aby zapisać pusty stan w bazie?
      // savePreferences(); // Odkomentuj, jeśli wyczyszczenie ma być od razu zapisane
      alert('Wszystkie preferencje zostały wyczyszczone. Kliknij "Zachowaj zmiany", aby zapisać.');
    }
  };

  const getPositionInfo = (e, columnElement) => {
// ... (reszta funkcji bez zmian)
    if (!columnElement) {
      return { time: "NaN:NaN", minutes: NaN };
    }
    
    const rect = columnElement.getBoundingClientRect();
    
    if (!rect || typeof rect.top !== 'number' || typeof e.clientY !== 'number') {
      console.error("getPositionInfo failed to read coordinates", rect, e);
      return { time: "NaN:NaN", minutes: NaN };
    }

    const y = e.clientY - rect.top;
    const hourHeight = 50;
    const gridStart = 7;
    const gridEnd = 18;
    
    const clampedY = Math.max(0, Math.min(y, (gridEnd - gridStart) * hourHeight));
    
    if (isNaN(clampedY)) {
      console.error("getPositionInfo calculated NaN for clampedY");
      return { time: "NaN:NaN", minutes: NaN };
    }

    const totalMinutes = Math.floor((clampedY / hourHeight) * 60);
    const hour = gridStart + Math.floor(totalMinutes / 60);
    const minutes = Math.floor((totalMinutes % 60) / 15) * 15;
    
    const finalHour = Math.max(gridStart, Math.min(hour, gridEnd));
    
    if (isNaN(finalHour) || isNaN(minutes)) {
      console.error("getPositionInfo calculated NaN for time");
      return { time: "NaN:NaN", minutes: NaN };
    }

    return {
      time: `${finalHour}:${minutes.toString().padStart(2, '0')}`,
      minutes: finalHour * 60 + minutes
    };
  };

  const handleMouseDown = (e, day, columnIndex) => {
// ... (reszta funkcji bez zmian)
    if (e.button !== 0) return;
    
    const column = e.currentTarget;
    const posInfo = getPositionInfo(e, column);

    if (isNaN(posInfo.minutes)) {
      setIsDragging(false);
      return;
    }
    
    setIsDragging(true);
    setDragDay(day);
    setDragStart(posInfo);
    setDragEnd(posInfo);
  };

  const handleMouseMove = (e) => {
// ... (reszta funkcji bez zmian)
    if (!isDragging || !dragDay) return;
    
    const columns = document.querySelectorAll('.schedule-column');
    const dayIndex = days.indexOf(dragDay);
    const column = columns[dayIndex];
    
    if (column) {
      const posInfo = getPositionInfo(e, column);
      if (!isNaN(posInfo.minutes)) {
        setDragEnd(posInfo);
      }
    }
  };

  const handleMouseUp = () => {
// ... (reszta funkcji bez zmian)
    if (!isDragging || !dragStart || !dragEnd || !dragDay || isNaN(dragStart.minutes) || isNaN(dragEnd.minutes)) {
      setIsDragging(false);
      setDragStart(null);
      setDragEnd(null);
      setDragDay(null);
      return;
    }

    const startMinutes = Math.min(dragStart.minutes, dragEnd.minutes);
    const endMinutes = Math.max(dragStart.minutes, dragEnd.minutes);
    
    if (endMinutes - startMinutes < 15) {
      setIsDragging(false);
      setDragStart(null);
      setDragEnd(null);
      setDragDay(null);
      return;
    }

    const startTime = minutesToTime(startMinutes);
    const endTime = minutesToTime(endMinutes);

    setPendingSlot({
      day: dragDay,
      start: startTime,
      end: endTime,
      type: 'prefer',
      priority: 1
    });
    
    setModalMode('create');
    setShowModal(true);
    setIsDragging(false);
  };

  const handleSlotClick = (e, day, slotIndex) => {
// ... (reszta funkcji bez zmian)
    e.stopPropagation();
    const slot = scheduleData[day][slotIndex];
    if (!slot) return;

    setEditingSlot({
      ...slot,
      priority: slot.priority || 1, 
      day: day,
      index: slotIndex
    });
    setModalMode('edit');
    setShowModal(true);
  };

  const handleAddSlot = () => {
// ... (reszta funkcji bez zmian)
    if (!pendingSlot) return;

    const label = pendingSlot.type === 'prefer' ? 'Chce mieć zajęcia' : 'Brak zajęć';
    const newSlot = {
      start: pendingSlot.start,
      end: pendingSlot.end,
      type: pendingSlot.type,
      label: label,
      priority: pendingSlot.priority || 1
    };

    setScheduleData(prev => {
      const daySlots = (prev[pendingSlot.day] || []).filter(Boolean); 
      const newSlotStart = timeToMinutes(newSlot.start);
      const newSlotEnd = timeToMinutes(newSlot.end);
      
      const filteredSlots = daySlots.filter(slot => {
        const slotStart = timeToMinutes(slot.start);
        const slotEnd = timeToMinutes(slot.end);
        
        return !(
          (newSlotStart <= slotStart && newSlotEnd >= slotEnd) ||
          (newSlotStart >= slotStart && newSlotEnd <= slotEnd) ||
          (newSlotStart < slotEnd && newSlotEnd > slotStart)
        );
      });
      
      filteredSlots.push(newSlot);
      filteredSlots.sort((a, b) => timeToMinutes(a.start) - timeToMinutes(b.start));
      
      return {
        ...prev,
        [pendingSlot.day]: filteredSlots
      };
    });

    handleCloseModal();
  };

  const handleUpdateSlot = () => {
// ... (reszta funkcji bez zmian)
    if (!editingSlot) return;

    const label = editingSlot.type === 'prefer' ? 'Chce mieć zajęcia' : 'Brak zajęć';
    
    setScheduleData(prev => {
      const daySlots = (prev[editingSlot.day] || []).filter(Boolean);
      
      daySlots[editingSlot.index] = {
        start: editingSlot.start,
        end: editingSlot.end,
        type: editingSlot.type,
        label: label,
        priority: editingSlot.priority || 1
      };
      
      return {
        ...prev,
        [editingSlot.day]: daySlots.filter(Boolean)
      };
    });

    handleCloseModal();
  };

  const handleDeleteSlot = () => {
// ... (reszta funkcji bez zmian)
    if (!editingSlot) return;

    setScheduleData(prev => {
      const daySlots = (prev[editingSlot.day] || []).filter(Boolean);
      const newDaySlots = daySlots.filter((_, index) => index !== editingSlot.index);
      
      return {
        ...prev,
        [editingSlot.day]: newDaySlots
      };
    });

    handleCloseModal();
  };

  const handleCloseModal = () => {
// ... (reszta funkcji bez zmian)
    setShowModal(false);
    setPendingSlot(null);
    setEditingSlot(null);
    setDragStart(null);
    setDragEnd(null);
    setDragDay(null);
  };

  useEffect(() => {
// ... (reszta funkcji bez zmian)
    if (isDragging) {
      document.addEventListener('mousemove', handleMouseMove);
      document.addEventListener('mouseup', handleMouseUp);
      
      return () => {
        document.removeEventListener('mousemove', handleMouseMove);
        document.removeEventListener('mouseup', handleMouseUp);
      };
    }
  }, [isDragging, dragStart, dragEnd, dragDay]);

  const getDragPreview = () => {
// ... (reszta funkcji bez zmian)
    if (!isDragging || !dragStart || !dragEnd || !dragDay || isNaN(dragStart.minutes) || isNaN(dragEnd.minutes)) {
      return null;
    }

    const startMinutes = Math.min(dragStart.minutes, dragEnd.minutes);
    const endMinutes = Math.max(dragStart.minutes, dragEnd.minutes);
    
    const startTime = minutesToTime(startMinutes);
    const endTimeString = minutesToTime(endMinutes);
    
    const { top, height } = calculateSlotPosition(startTime, endTimeString);

    if (height === 0) return null;

    return {
      top,
      height,
      startTime,
      endTime: endTimeString
    };
  };

  return (
    <div className="entries-container">
      <div className="entries-content">
        <div className="entries-main">
          <EntriesSidebar
            fileError={fetchError || saveError} // Wyświetlamy błąd ładowania lub zapisu
            onSave={savePreferences} // Zaktualizowana funkcja zapisu
            onClear={clearAllPreferences}
            recruitments={recruitments}
            isLoading={isLoadingRecruitments}
            selectedRecruitment={selectedRecruitment}
            onSelectRecruitment={setSelectedRecruitment}
            isSaving={isSaving} // Przekazanie stanu zapisywania
          />
          <main className="entries-schedule">
            {isLoadingRecruitments ? (
              <div className="entries-loading-indicator">
                <p>Ładowanie rekrutacji...</p>
              </div>
            ) : !selectedRecruitment ? (
              <div className="entries-loading-indicator">
                <p>Proszę wybrać rekrutację z listy po lewej stronie.</p>
              </div>
            ) : isLoadingSchedule ? (
              <div className="entries-loading-indicator">
                <p>Ładowanie preferencji dla {selectedRecruitment.recruitment_name}...</p>
              </div>
            ) : (
              <React.Fragment>
                <ScheduleHeader
                  selectedRecruitment={selectedRecruitment}
                  usedPriority={calculateUsedPriority()}
                  maxPriority={maxPriority}
                />
                <div className="schedule-grid">
                  <div className="schedule-times">
                    {hours.map(time => (
                      <div key={time} className="schedule-time">{time}</div>
                    ))}
                  </div>
                  <div className="schedule-week">
                    <div className="schedule-days">
                      {dayLabels.map(day => (
                        <span key={day} className="schedule-day">{day}</span>
                      ))}
                    </div>
                    <div className="schedule-calendar" ref={calendarRef}>
                      {days.map((day, index) => (
                        <ScheduleColumn
                          key={day}
                          day={day}
                          slots={(scheduleData[day] || []).filter(Boolean)}
                          dragPreview={(isDragging && dragDay === day) ? getDragPreview() : null}
                          onMouseDown={(e) => handleMouseDown(e, day, index)}
                          onSlotClick={handleSlotClick}
                        />
                      ))}
                    </div>
                  </div>
                </div>
              </React.Fragment>
            )}
          </main>
        </div>
      </div>

      {showModal && (
        <PreferenceModal
          mode={modalMode}
          pendingSlot={pendingSlot}
          editingSlot={editingSlot}
          setPendingSlot={setPendingSlot}
          setEditingSlot={setEditingSlot}
          onClose={handleCloseModal}
          onAdd={handleAddSlot}
          onUpdate={handleUpdateSlot}
          onDelete={handleDeleteSlot}
        />
      )}
    </div>
  );
}