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
  const [fetchError, setFetchError] = useState(null);

  // Stan dla harmonogramu (preferencji)
  const [scheduleData, setScheduleData] = useState({
    monday: [],
    tuesday: [],
    wednesday: [],
    thursday: [],
    friday: []
  });

  const [isLoadingSchedule, setIsLoadingSchedule] = useState(false); // Oddzielny stan ładowania dla preferencji
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
      // TODO: Zaimplementować pobieranie i transformację danych preferencji
      // Na razie czyścimy dane i ustawiamy ładowanie
      console.log(`Wybrano rekrutację: ${selectedRecruitment.recruitment_id}. Gotowy do pobrania preferencji.`);
      
      // Tutaj w przyszłości będzie fetch do:
      // /api/v1/preferences/user-preferences/<recruitment_id>/<user_id>/
      // i transformacja danych z PreferredTimeslots na format scheduleData
      
      setIsLoadingSchedule(true); // Ustaw ładowanie harmonogramu
      
      // Symulacja ładowania
      setTimeout(() => {
        // Na razie ładujemy puste dane, docelowo tu będzie transformacja
        setScheduleData({
          monday: [],
          tuesday: [],
          wednesday: [],
          thursday: [],
          friday: []
        });
        setIsLoadingSchedule(false); // Zakończ ładowanie harmonogramu
      }, 500); // Usunąć timeout po implementacji fetch

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
// ... (bez zmian)
// ... (reszta funkcji bez zmian)
    let total = 0;
    days.forEach(day => {
      const validSlots = (scheduleData[day] || []).filter(Boolean);
      validSlots.forEach(slot => {
        total += slot.priority || 0;
      });
    });
    return total;
  };

  const saveScheduleToFile = async () => {
    // TODO: Zmienić na wysyłanie danych do backendu (PUT /api/v1/preferences/user-preferences/...)
    // Trzeba będzie transformować 'scheduleData' z powrotem na format 'preferences_data'
    alert("Funkcja zapisywania preferencji nie jest jeszcze połączona z backendem.");
    console.log("Zapisywanie danych (TODO):", scheduleData);
  };
// ... (reszta funkcji bez zmian)
  const clearAllPreferences = () => {
// ... (bez zmian)
    if (window.confirm('Czy na pewno chcesz usunąć wszystkie preferencje? Ta akcja jest nieodwracalna.')) {
      setScheduleData({
        monday: [],
        tuesday: [],
        wednesday: [],
        thursday: [],
        friday: []
      });
      alert('Wszystkie preferencje zostały usunięte.');
    }
  };

  const getPositionInfo = (e, columnElement) => {
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
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
// ... (bez zmian)
    setShowModal(false);
    setPendingSlot(null);
    setEditingSlot(null);
    setDragStart(null);
    setDragEnd(null);
    setDragDay(null);
  };

  useEffect(() => {
// ... (bez zmian)
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
// ... (bez zmian)
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
            fileError={fetchError}
            onSave={saveScheduleToFile}
            onClear={clearAllPreferences}
            recruitments={recruitments}
            isLoading={isLoadingRecruitments}
            selectedRecruitment={selectedRecruitment}
            onSelectRecruitment={setSelectedRecruitment}
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