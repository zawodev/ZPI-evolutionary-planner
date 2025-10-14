import React, { useState, useEffect } from "react";
import Link from "next/link";
import Navbar from "@/components/navbar/Navbar";

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
    monday: [
      { start: "7:00", end: "9:00", type: "avoid", label: "Brak zajęć" },
      { start: "10:00", end: "15:00", type: "prefer", label: "Chce mieć zajęcia" },
      { start: "16:00", end: "18:00", type: "avoid", label: "Brak zajęć" }    
    ],
    tuesday: [
      { start: "7:00", end: "18:00", type: "prefer", label: "Chce mieć zajęcia" }
    ],
    wednesday: [
      { start: "7:00", end: "11:00", type: "avoid", label: "Brak zajęć" }
    ],
    thursday: [
      { start: "7:00", end: "18:00", type: "prefer", label: "Chce mieć zajęcia" }
    ],
    friday: [
      { start: "12:00", end: "18:00", type: "avoid", label: "Brak zajęć" }
    ]
  });

  const days = ['monday', 'tuesday', 'wednesday', 'thursday', 'friday'];
  const dayLabels = ['Pon', 'Wt', 'Śr', 'Czw', 'Pt'];
  const hours = ["7:00", "8:00", "9:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00"];

  return (
    <div className="entries-container">
      <Navbar />
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
              </div>
            </aside>
            <main className="entries-schedule">
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
            </main>
          </div>
        </div>
      </div>
    </div>
  );
}