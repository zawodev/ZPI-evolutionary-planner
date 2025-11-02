/* pages/entries.js */

import React, { useState, useEffect, useRef } from "react";

const calculateSlotPosition = (startTime, endTime) => {
  const startHour = parseInt(startTime.split(':')[0]);
  const endHour = parseInt(endTime.split(':')[0]);
  const startMinutes = parseInt(startTime.split(':')[1]) || 0;
  const endMinutes = parseInt(endTime.split(':')[1]) || 0;
  
  const gridStart = 7;
  const hourHeight = 50;
  
  const top = ((startHour - gridStart) + (startMinutes / 60)) * hourHeight + 5;
  const height = ((endHour - startHour) + ((endMinutes - startMinutes) / 60)) * hourHeight;
  
  return { top, height };
};

const timeToMinutes = (time) => {
  const [hours, minutes] = time.split(':').map(Number);
  return hours * 60 + minutes;
};

const minutesToTime = (minutes) => {
  const hours = Math.floor(minutes / 60);
  const mins = minutes % 60;
  return `${hours}:${mins.toString().padStart(2, '0')}`;
};

export default function EntriesPage() {
  const [scheduleData, setScheduleData] = useState({
    monday: [],
    tuesday: [],
    wednesday: [],
    thursday: [],
    friday: []
  });

  const [fileError, setFileError] = useState(null);
  const [isLoading, setIsLoading] = useState(true);
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

  const JSON_FILE_PATH = '/json/report-schedule.json';

  const calculateUsedPriority = () => {
    let total = 0;
    days.forEach(day => {
      if (scheduleData[day]) {
        scheduleData[day].forEach(slot => {
          total += slot.priority || 0;
        });
      }
    });
    return total;
  };

  const saveScheduleToFile = async () => {
    try {
      const response = await fetch('/api/save-schedule', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(scheduleData)
      });

      if (response.ok) {
        alert('Harmonogram został zapisany pomyślnie!');
      } else {
        throw new Error('Błąd podczas zapisywania');
      }
    } catch (error) {
      console.error('Error saving schedule:', error);
      alert('Nie udało się zapisać harmonogramu. Używam zapisu lokalnego...');
      
      const dataStr = JSON.stringify(scheduleData, null, 2);
      const dataBlob = new Blob([dataStr], { type: 'application/json' });
      const url = URL.createObjectURL(dataBlob);
      const link = document.createElement('a');
      link.href = url;
      link.download = 'schedule-data.json';
      link.click();
      URL.revokeObjectURL(url);
    }
  };

  const clearAllPreferences = () => {
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

  useEffect(() => {
    const loadScheduleData = async () => {
      try {
        const response = await fetch(JSON_FILE_PATH);
        
        if (!response.ok) {
          throw new Error(`Failed to load file: ${response.statusText}`);
        }

        const jsonData = await response.json();
        
        if (typeof jsonData !== 'object') {
          throw new Error('Invalid JSON structure: expected an object');
        }

        const validDays = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday'];
        const newScheduleData = {};

        validDays.forEach(day => {
          if (jsonData[day] && Array.isArray(jsonData[day])) {
            newScheduleData[day] = jsonData[day].map(slot => {
              if (!slot.start || !slot.end || !slot.type || !slot.label) {
                throw new Error(`Invalid slot structure in ${day}`);
              }
              return {
                start: slot.start,
                end: slot.end,
                type: slot.type,
                label: slot.label,
                priority: slot.priority || 1
              };
            });
          } else {
            newScheduleData[day] = [];
          }
        });

        setScheduleData(newScheduleData);
        setIsLoading(false);
      } catch (error) {
        setFileError(`Error loading schedule: ${error.message}`);
        setIsLoading(false);
        console.error('File loading error:', error);
      }
    };

    loadScheduleData();
  }, []);

  const getPositionInfo = (e, columnElement) => {
    const rect = columnElement.getBoundingClientRect();
    const y = e.clientY - rect.top;
    const hourHeight = 50;
    const gridStart = 7;
    const gridEnd = 18;
    
    const clampedY = Math.max(0, Math.min(y, (gridEnd - gridStart) * hourHeight));
    
    const totalMinutes = Math.floor((clampedY / hourHeight) * 60);
    const hour = gridStart + Math.floor(totalMinutes / 60);
    const minutes = Math.floor((totalMinutes % 60) / 15) * 15;
    
    // Dodatkowe zabezpieczenie
    const finalHour = Math.max(gridStart, Math.min(hour, gridEnd));
    
    return {
      time: `${finalHour}:${minutes.toString().padStart(2, '0')}`,
      minutes: finalHour * 60 + minutes
    };
  };

  const handleMouseDown = (e, day, columnIndex) => {
    if (e.button !== 0) return;
    
    const column = e.currentTarget;
    const posInfo = getPositionInfo(e, column);
    
    setIsDragging(true);
    setDragDay(day);
    setDragStart(posInfo);
    setDragEnd(posInfo);
  };

  const handleMouseMove = (e) => {
    if (!isDragging || !dragDay) return;
    
    const columns = document.querySelectorAll('.schedule-column');
    const dayIndex = days.indexOf(dragDay);
    const column = columns[dayIndex];
    
    if (column) {
      const posInfo = getPositionInfo(e, column);
      setDragEnd(posInfo);
    }
  };

  const handleMouseUp = () => {
    if (!isDragging || !dragStart || !dragEnd || !dragDay) {
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
    e.stopPropagation();
    const slot = scheduleData[day][slotIndex];
    setEditingSlot({
      ...slot,
      day: day,
      index: slotIndex
    });
    setModalMode('edit');
    setShowModal(true);
  };

  const handleAddSlot = () => {
    if (!pendingSlot) return;

    const label = pendingSlot.type === 'prefer' ? 'Chce mieć zajęcia' : 'Brak zajęć';
    const newSlot = {
      start: pendingSlot.start,
      end: pendingSlot.end,
      type: pendingSlot.type,
      label: label,
      priority: pendingSlot.priority
    };

    setScheduleData(prev => {
      const daySlots = [...prev[pendingSlot.day]];
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
    if (!editingSlot) return;

    const label = editingSlot.type === 'prefer' ? 'Chce mieć zajęcia' : 'Brak zajęć';
    
    setScheduleData(prev => {
      const daySlots = [...prev[editingSlot.day]];
      daySlots[editingSlot.index] = {
        start: editingSlot.start,
        end: editingSlot.end,
        type: editingSlot.type,
        label: label,
        priority: editingSlot.priority
      };
      
      return {
        ...prev,
        [editingSlot.day]: daySlots
      };
    });

    handleCloseModal();
  };

  const handleDeleteSlot = () => {
    if (!editingSlot) return;

    setScheduleData(prev => {
      const daySlots = [...prev[editingSlot.day]];
      daySlots.splice(editingSlot.index, 1);
      
      return {
        ...prev,
        [editingSlot.day]: daySlots
      };
    });

    handleCloseModal();
  };

  const handleCloseModal = () => {
    setShowModal(false);
    setPendingSlot(null);
    setEditingSlot(null);
    setDragStart(null);
    setDragEnd(null);
    setDragDay(null);
  };

  useEffect(() => {
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
    if (!isDragging || !dragStart || !dragEnd || !dragDay) return null;

    const startMinutes = Math.min(dragStart.minutes, dragEnd.minutes);
    const endMinutes = Math.max(dragStart.minutes, dragEnd.minutes);
    
    const startTime = minutesToTime(startMinutes);
    const endTime = minutesToTime(endMinutes);
    
    const { top, height } = calculateSlotPosition(startTime, endTime);

    return {
      top,
      height,
      startTime,
      endTime
    };
  };

  return (
    <div className="entries-container">
      <div className="entries-background">
        <div className="entries-content">
          <div className="entries-main">
            <aside className="entries-sidebar">
              <div className="entries-section">
                <h3 className="entries-section-title">Nadchodzące zgłoszenia:</h3>
                <div className="entries-item active">
                  <span>IST - Lato, 2024/25</span>
                </div>
                <div className="entries-item">
                  <span>IKW - Zima, 2024/25</span>
                </div>
              </div>
              <div className="entries-section">
                <h3 className="entries-section-title">Zakończone zgłoszenia:</h3>
                <div className="entries-item">
                  <span>IST - Lato, 2024/25</span>
                </div>
                <div className="entries-item">
                  <span>IKW - Zima, 2024/25</span>
                </div>
                {fileError && (
                  <div className="entries-error-message">
                    {fileError}
                  </div>
                )}
              </div>
              <div className="entries-section">
                <h3 className="entries-section-title">Akcje:</h3>
                <button onClick={saveScheduleToFile} className="btn btn--primary btn--filler">
                  Zachowaj zmiany
                </button>
                <div className="pt-md"></div>
                <button onClick={clearAllPreferences} className="btn btn--delete btn--filler">
                  Wyczyść Preferencje
                </button>
              </div>
            </aside>
            <main className="entries-schedule">
              {isLoading ? (
                <div className="entries-loading-indicator">
                  <p>Ładowanie harmonogramu...</p>
                </div>
              ) : (
                <React.Fragment>
                  <div className="entries-header">
                    <h2 className="entries-title">Wybrane Zgłoszenia: IST - Lato 2024/25</h2>
                    <div className="entries-stats">
                      <div className="label soft-blue">
                        Punkty Priorytetu: {calculateUsedPriority()}/{maxPriority}
                      </div>
                      <div className="label soft-yellow">
                        Zamknięcie za: 3d 7h
                      </div>
                    </div>
                  </div>
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
                        {days.map((day, index) => {
                          const dragPreview = (isDragging && dragDay === day) ? getDragPreview() : null;
                          
                          return (
                            <div 
                              key={day} 
                              className="schedule-column"
                              onMouseDown={(e) => handleMouseDown(e, day, index)}
                            >
                              {scheduleData[day] && scheduleData[day].map((slot, slotIndex) => {
                                const { top, height } = calculateSlotPosition(slot.start, slot.end);
                                return (
                                  <div
                                    key={`${day}-${slotIndex}`}
                                    className={`schedule-slot ${slot.type}`}
                                    style={{
                                      top: `${top}px`,
                                      height: `${height}px`
                                    }}
                                    onClick={(e) => handleSlotClick(e, day, slotIndex)}
                                    onMouseDown={(e) => e.stopPropagation()}
                                  >
                                    <span className="slot-label">Preferencja: {slot.label}</span>
                                    <div className="slot-details">
                                      <span className="slot-time">{slot.start}-{slot.end}</span>
                                      <span className="slot--label-points">
                                        {slot.priority}pt
                                      </span>
                                    </div>
                                  </div>
                                );
                              })}
                              
                              {dragPreview && (
                                <div
                                  className="schedule-slot schedule-slot-creating"
                                  style={{
                                    top: `${dragPreview.top}px`,
                                    height: `${dragPreview.height}px`,
                                  }}
                                >
                                  <span className="slot-time">{dragPreview.startTime}-{dragPreview.endTime}</span>
                                </div>
                              )}
                            </div>
                          );
                        })}
                      </div>
                    </div>
                  </div>
                </React.Fragment>
              )}
            </main>
          </div>
        </div>
      </div>

      {showModal && (
        <div className="modal-backdrop">
          <div className="modal-content">
            <h3 className="modal-title">
              {modalMode === 'create' ? 'Nowa preferencja' : 'Edytuj preferencję'}
            </h3>
            <p className="modal-subtitle">
              {modalMode === 'create' 
                ? `Zaznaczony czas: ${pendingSlot?.start} - ${pendingSlot?.end}`
                : `Edytujesz: ${editingSlot?.start} - ${editingSlot?.end}`
              }
            </p>

            {modalMode === 'create' ? (
              <div className="modal-body">
                <div>
                  <label className="modal-label">
                    Typ preferencji
                  </label>
                  <div className="modal-button-group">
                    <button
                      onClick={() => setPendingSlot({...pendingSlot, type: 'prefer'})}
                      className={`btn modal-choice-btn ${pendingSlot?.type === 'prefer' ? 'modal-choice-btn--prefer-active' : ''}`}
                    >
                      Chcę zajęcia
                    </button>
                    <button
                      onClick={() => setPendingSlot({...pendingSlot, type: 'avoid'})}
                      className={`btn modal-choice-btn ${pendingSlot?.type === 'avoid' ? 'modal-choice-btn--avoid-active' : ''}`}
                    >
                      Chcę wolne
                    </button>
                  </div>
                </div>

                <div>
                  <label className="modal-label">
                    Punkty priorytetu: {pendingSlot?.priority || 1}
                  </label>
                  <input
                    type="range"
                    min="1"
                    max="10"
                    value={pendingSlot?.priority || 1}
                    onChange={(e) => setPendingSlot({...pendingSlot, priority: parseInt(e.target.value)})}
                    className="scrollbar--priority"
                  />
                  <div className="modal-range-labels">
                    <span>1</span>
                    <span>10</span>
                  </div>
                </div>

                <div className="modal-actions">
                  <button
                    onClick={handleAddSlot}
                    className="btn btn--primary btn--filler"
                  >
                    Dodaj
                  </button>
                  <button
                    onClick={handleCloseModal}
                    className="btn btn--neutral btn--filler"
                  >
                    Anuluj
                  </button>
                </div>
              </div>
            ) : (
              <div className="modal-body">
                <div className="modal-input-grid">
                  <div>
                    <label className="modal-label modal-label--small">
                      Start
                    </label>
                    <input
                      type="time"
                      value={editingSlot?.start || ''}
                      onChange={(e) => setEditingSlot({...editingSlot, start: e.target.value})}
                      className="modal-input"
                      step="900"
                    />
                  </div>
                  <div>
                    <label className="modal-label modal-label--small">
                      Koniec
                    </label>
                    <input
                      type="time"
                      value={editingSlot?.end || ''}
                      onChange={(e) => setEditingSlot({...editingSlot, end: e.target.value})}
                      className="modal-input"
                      step="900"
                    />
                  </div>
                </div>

                <div>
                  <label className="modal-label">
                    Typ preferencji
                  </label>
                  <div className="modal-button-group">
                    <button
                      onClick={() => setEditingSlot({...editingSlot, type: 'prefer'})}
                      className={`btn modal-choice-btn ${editingSlot?.type === 'prefer' ? 'modal-choice-btn--prefer-active' : ''}`}
                    >
                      Chcę zajęcia
                    </button>
                    <button
                      onClick={() => setEditingSlot({...editingSlot, type: 'avoid'})}
                      className={`btn modal-choice-btn ${editingSlot?.type === 'avoid' ? 'modal-choice-btn--avoid-active' : ''}`}
                    >
                      Chcę wolne
                    </button>
                  </div>
                </div>

                <div>
                  <label className="modal-label">
                    Punkty priorytetu: {editingSlot?.priority}
                  </label>
                  <input
                    type="range"
                    min="1"
                    max="10"
                    value={editingSlot?.priority || 1}
                    onChange={(e) => setEditingSlot({...editingSlot, priority: parseInt(e.target.value)})}
                    className="scrollbar--priority"
                  />
                  <div className="modal-range-labels">
                    <span>1</span>
                    <span>10</span>
                  </div>
                </div>

                <div className="modal-actions">
                  <button onClick={handleUpdateSlot} className="btn btn--primary btn--filler">
                    Zapisz
                  </button>
                  <button onClick={handleDeleteSlot} className="btn btn--delete">
                    Usuń
                  </button>
                  <button onClick={handleCloseModal} className="btn btn--neutral">
                    Anuluj
                  </button>
                </div>
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
}