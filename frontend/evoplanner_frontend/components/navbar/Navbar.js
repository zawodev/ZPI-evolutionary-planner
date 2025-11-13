/* frontend/evoplanner_frontend/components/navbar/Navbar.js */
import Link from 'next/link';
import { useState, useEffect } from 'react';
import { useRouter } from 'next/router';

export default function Navbar() {
  const [user, setUser] = useState(null);
  const router = useRouter();

  const checkUser = () => {
    const storedUser = localStorage.getItem('user');
    if (storedUser) {
      try {
        setUser(JSON.parse(storedUser));
      } catch (e) {
        console.error("Failed to parse user from localStorage", e);
        localStorage.removeItem('user');
        setUser(null);
      }
    } else {
      setUser(null);
    }
  };

  useEffect(() => {
    checkUser();
  }, []);

  useEffect(() => {
    const handleRouteChange = () => {
      checkUser();
    };

    router.events.on('routeChangeComplete', handleRouteChange);

    return () => {
      router.events.off('routeChangeComplete', handleRouteChange);
    };
  }, [router.events]);

  const handleLogout = () => {
    localStorage.removeItem('access_token');
    localStorage.removeItem('refresh_token');
    localStorage.removeItem('user');
    
    setUser(null);
    
     router.push('/login');
  };

  return (
    <header className="header">
      <div className="logo logo--header">OptiSlots</div>
      <nav>
        {user ? (
          <>
            <Link href="/entries">
              <button className="header-nav nav-entries">Zgłoszenia</button>
            </Link>
            <Link href="/plans">
              <button className="header-nav nav-plans">Plany</button>
            </Link>
          </>
        ) : (
          <>
            <Link href="/">
              <button className="header-nav nav-home">Strona główna</button>
            </Link>
            <Link href="/features">
              <button className="header-nav nav-func">Funkcjonalności</button>
            </Link>
            <Link href="/contact">
              <button className="header-nav nav-contact">Kontakt</button>
            </Link>
          </>
        )}
      </nav>
      <div>
        <div className="header-icon header-icon--main"></div>
        <div className="login-btn-wrapper">
          {user ? (
            <button 
              onClick={handleLogout} 
              className="btn btn--primary btn--neutral"
            >
              Wyloguj
            </button>
          ) : (
            <Link href="/login">
              <button className="btn btn--primary btn--login">Zaloguj się!</button>
            </Link>
          )}
        </div>        
      </div>
    </header>
  );
}