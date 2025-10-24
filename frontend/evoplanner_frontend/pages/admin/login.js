import React, { useState } from "react";
import Link from "next/link";
import Navbar from "@/components/navbar/Navbar";
import Image from "next/image";
import styles from "@/styles/components/_admin.module.css";

export default function LoginPage() {
  const [login, setLogin] = useState("");
  const [password, setPassword] = useState("");

  const handleLogin = async (e) => {
    e.preventDefault();
    
    // TODO: Replace with actual API call
    try {
      // const response = await fetch('/api/login', {
      //   method: 'POST',
      //   headers: { 'Content-Type': 'application/json' },
      //   body: JSON.stringify({ login, password })
      // });
      // if (response.ok) {
      //   window.location.href = "/dashboard";
      // }
      
      console.log("Logging in with:", login, password);
      window.location.href = "/check";
    } catch (error) {
      console.error("Login failed:", error);
    }
  };

  const goToRegister = () => {
    window.location.href = "/register";
  };

  return (
    <div className={styles.background}>
      <Navbar />
      
      <div className="login-frame">
        <div className="card card--login">
          <strong className="logo logo--login" style={{ 
            position: 'static', 
            display: 'block', 
            textAlign: 'center', 
            fontSize: 'clamp(32px, 4vh, 48px)',
            marginBottom: '24px'
          }}>
            Rekruter+
          </strong>
          
          <form onSubmit={handleLogin}>
            <div className="login-input-wrapper" style={{ marginTop: '60px' }}>
              <p className="info-text">Adres e-mail</p>
              <input
                type="email"
                placeholder="imienazwisko@email.com"
                value={login}
                onChange={(e) => setLogin(e.target.value)}
                className="input input--login"
                required
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
              />
            </div>

            <div className="login-button-wrapper">
              <button type="submit" className="btn btn--primary btn--form">
                Zaloguj
              </button>
            </div>

            <div style={{ textAlign: 'center', marginTop: '24px' }}>
              <p className="info-text">
                Firma nie ma konta?{' '}
                <Link href="/admin/register" style={{ color: '#2163ff', fontWeight: 500 }}>
                  Zarejestruj się
                </Link>
              </p>
            </div>
          </form>
        </div>
      </div>
    </div>
  );
}