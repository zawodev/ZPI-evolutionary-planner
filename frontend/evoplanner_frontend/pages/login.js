/* frontend/evoplanner_frontend/pages/login.js */
import React, { useState } from "react";
import Link from "next/link";
import Navbar from "@/components/navbar/Navbar";
import Image from "next/image";
import { useRouter } from "next/router";

export default function LoginPage() {
  const [login, setLogin] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const [isLoading, setIsLoading] = useState(false);
  const router = useRouter();

  const handleLogin = async (e) => {
    e.preventDefault();
    setError("");
    setIsLoading(true);
    
    try {
      const response = await fetch('http://127.0.0.1:8000/api/v1/identity/login/', {
        method: 'POST',
        headers: { 
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ 
          username: login, 
          password: password 
        })
      });

      const data = await response.json();
      
      if (response.ok) {
        if (data.access) {
          localStorage.setItem('access_token', data.access);
        }
        if (data.refresh) {
          localStorage.setItem('refresh_token', data.refresh);
        }
        
        if (data.user) {
          localStorage.setItem('user', JSON.stringify(data.user));
        }
        
        router.push("/entries");
      } else {
        setError(data.message || data.detail || "Nieprawidłowy login lub hasło");
      }
    } catch (error) {
      console.error("Login failed:", error);
      setError("Błąd połączenia z serwerem. Spróbuj ponownie.");
    } finally {
      setIsLoading(false);
    }
  };

  const goToRegister = () => {
    router.push("/register");
  };

  return (
    <div className="background">
      <Navbar />
      
      <div className="login-container">
        <div className="login-frame">
          <div className="card card--login">
            <strong className="logo logo--login">
              OptiSlots
            </strong>
            
            <form onSubmit={handleLogin}>
              {error && (
                <div className="error-message">
                  {error}
                </div>
              )}

              <div className="login-input-wrapper">
                <p className="info-text">Adres e-mail lub login</p>
                <input
                  type="text"
                  placeholder="imienazwisko@email.com"
                  value={login}
                  onChange={(e) => setLogin(e.target.value)}
                  className="input input--login"
                  required
                  disabled={isLoading}
                />
              </div>

              <div className="login-input-wrapper">
                <p className="info-text">Hasło</p>
                <input
                  type="password"
                  placeholder="*********"
                  value={password}
                  onChange={(e) => setPassword(e.target.value)}
                  className="input input--login"
                  required
                  disabled={isLoading}
                />
              </div>

              <div className="login-button-wrapper">
                <button 
                  type="submit" 
                  className="btn btn--primary btn--form"
                  disabled={isLoading}
                >
                  {isLoading ? 'Logowanie...' : 'Zaloguj'}
                </button>
              </div>

              <div className="login-divider">
                <p className="info-text">
                  Lub kontynuuj przez
                </p>
              </div>

              <div className="login-button-wrapper">
                <button
                  type="button"
                  onClick={goToRegister}
                  className="btn btn--secondary btn--form btn--with-icon"
                  disabled={isLoading}
                >
                  <div className="icon-wrapper">
                    <Image 
                      src="/images/googleicon.svg" 
                      alt="Google Logo" 
                      width={20} 
                      height={20}
                    />
                  </div>
                  <span>Google</span>
                </button>
              </div>

              <div className="login-button-wrapper">
                <button
                  type="button"
                  onClick={goToRegister}
                  className="btn btn--secondary btn--form"
                  disabled={isLoading}
                >
                  USOS
                </button>
              </div>
            </form>
          </div>
        </div>
      </div>
    </div>
  );
}