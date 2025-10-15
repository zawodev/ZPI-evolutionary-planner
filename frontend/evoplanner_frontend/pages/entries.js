import React, { useState, useEffect } from "react";

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

  const days = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday'];
  const dayLabels = ['Pon', 'Wt', 'Śr', 'Czw', 'Pt'];
  const hours = ["7:00", "8:00", "9:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00"];

  // Ścieżka do pliku JSON - zmień na swoją ścieżkę
  const JSON_FILE_PATH = '/json/report-schedule.json';

  useEffect(() => {
    const loadScheduleData = async () => {
      try {
        const response = await fetch(JSON_FILE_PATH);
        
        if (!response.ok) {
          throw new Error(`Failed to load file: ${response.statusText}`);
        }

        const jsonData = await response.json();
        
        // Validate the JSON structure
        if (typeof jsonData !== 'object') {
          throw new Error('Invalid JSON structure: expected an object');
        }

        // Validate that the days exist and have proper structure
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
                label: slot.label
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
                  <div style={{ padding: '10px', marginTop: '10px', fontSize: '12px', color: '#e74c3c', backgroundColor: '#ffe6e6', borderRadius: '4px' }}>
                    {fileError}
                  </div>
                )}
              </div>
            </aside>
            <main className="entries-schedule">
              {isLoading ? (
                <div style={{ padding: '20px', textAlign: 'center' }}>
                  <p>Ładowanie harmonogramu...</p>
                </div>
              ) : (
                <React.Fragment>
                  <div className="entries-header">
                    <h2 className="entries-title">Wybrane Zgłoszenia: IST - Lato 2024/25</h2>
                    <div className="entries-stats">
                      <div className="entries-badge priority">
                        Punkty Priorytetu: 10/40
                      </div>
                      <div className="entries-badge deadline">
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
                      <div className="schedule-calendar">
                        {days.map((day, index) => (
                          <div key={day} className="schedule-column">
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
                                >
                                  <span className="slot-label">Preferencja: {slot.label}</span>
                                  <span className="slot-time">{slot.start}-{slot.end}</span>
                                </div>
                              );
                            })}
                          </div>
                        ))}
                      </div>
                    </div>
                  </div>
                </React.Fragment>
              )}
            </main>
          </div>
        </div>
      </div>
    </div>
  );
}