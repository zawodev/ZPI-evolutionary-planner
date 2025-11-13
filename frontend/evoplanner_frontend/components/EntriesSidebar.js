import React from "react";

export default function EntriesSidebar({
  fileError,
  onSave,
  onClear,
  recruitments,
  isLoading,
  selectedRecruitment,
  onSelectRecruitment
}) {
  
  // Filtrowanie rekrutacji zgodnie z prośbą
  const upcomingRecruitments = recruitments.filter(rec => 
    rec.plan_status === 'draft'
  );
  const completedRecruitments = recruitments.filter(rec => 
    rec.plan_status !== 'draft' // Cała reszta ('active', 'optimizing', 'archived')
  );

  return (
    <aside className="entries-sidebar">
      <div className="entries-section">
        <h3 className="entries-section-title">Nadchodzące zgłoszenia:</h3>
        {isLoading && <div className="entries-item">Ładowanie...</div>}
        {fileError && <div className="entries-error-message">{fileError}</div>}
        
        {/* Pętla dla rekrutacji "nadchodzących" (draft) */}
        {!isLoading && !fileError && upcomingRecruitments.length > 0 ? (
          upcomingRecruitments.map(rec => (
            <div
              key={rec.recruitment_id}
              className={`entries-item ${selectedRecruitment?.recruitment_id === rec.recruitment_id ? 'active' : ''}`}
              onClick={() => onSelectRecruitment(rec)}
            >
              <span>{rec.recruitment_name}</span>
              {/* Usunięto wyświetlanie statusu */}
            </div>
          ))
        ) : (
          !isLoading && !fileError && <div className="entries-item">Brak nadchodzących rekrutacji</div>
        )}
      </div>
      <div className="entries-section">
        <h3 className="entries-section-title">Zakończone zgłoszenia:</h3>
        
        {/* Pętla dla rekrutacji "zakończonych" (reszta) */}
        {isLoading && <div className="entries-item">Ładowanie...</div>}
        {!isLoading && !fileError && completedRecruitments.length > 0 ? (
          completedRecruitments.map(rec => (
            <div
              key={rec.recruitment_id}
              className={`entries-item ${selectedRecruitment?.recruitment_id === rec.recruitment_id ? 'active' : ''}`}
              onClick={() => onSelectRecruitment(rec)}
            >
              <span>{rec.recruitment_name}</span>
              {/* Usunięto wyświetlanie statusu */}
            </div>
          ))
        ) : (
          !isLoading && !fileError && <div className="entries-item">Brak zakończonych rekrutacji</div>
        )}
      </div>
      <div className="entries-section">
        <h3 className="entries-section-title">Akcje:</h3>
        <button
          onClick={onSave}
          className="btn btn--primary btn--filler"
          disabled={!selectedRecruitment} // Wyłącz, jeśli nic nie wybrano
        >
          Zachowaj zmiany
        </button>
        <div className="pt-md"></div>
        <button
          onClick={onClear}
          className="btn btn--delete btn--filler"
          disabled={!selectedRecruitment} // Wyłącz, jeśli nic nie wybrano
        >
          Wyczyść Preferencje
        </button>
      </div>
    </aside>
  );
}