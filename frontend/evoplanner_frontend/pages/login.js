// Login page with the form for a login. Register sends to a diffrent page
import React, { useState } from "react";
import styles from '@/styles/login.module.css';
import Navbar from "@/components/Navbar";
import Image from "next/image";
export default function LoginPage() {
  const [login, setLogin] = useState("");
  const [password, setPassword] = useState("");

  const handleLogin = (e) => { //handles api call - method should be under api/login.js
    e.preventDefault();
    // TODO: Api call
    window.location.href = "/check";
    console.log("Logging in with:", login, password);
  };

  const goToRegister = () => { //just sends to register page
     window.location.href = "/check";
  };

  return (
    //main page TODO: these styles should be in styles/login
    <div className={styles.background}>
      <Navbar />
      <div className={styles.loginframe}>
        <strong className={styles.logo}>Rekruter+</strong>
        <form onSubmit={handleLogin} className="space-y-4">
          <p className={styles.infotext}>Adres e-mail</p>
          <input
            type="text"
            placeholder=" imienazwisko@email.com"
            value={login}
            onChange={(e) => setLogin(e.target.value)}
            className={styles.logininput}
            style={{ '--top': `${15}vh` }}
            required
          />
          <p className={styles.infotext} style={{ '--top': `${19}vh` }}>Hasło</p>
          <input
            type="password"
            placeholder="*********"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            className={styles.logininput}
            style={{ '--top': `${23}vh` }}
            required
          />

          
          <div>
            <button
              type="submit"
              className={styles.loginbutton}
              style={{ '--top': `${30}vh` }}
            >
              Zaloguj
            </button>
            <p className={styles.infotext} style={{ '--top': `${35}vh`, '--left': `${8}vw` }}>Lub kontynuuj przez</p>
            <button
              type="button"
              onClick={goToRegister}
              className={styles.loginbutton}
              style={{ '--top': `${40}vh` }}
            >
              <div className={styles.iconwrapper}>
                <Image src="/googleicon.svg" alt="Google Logo" fill style={{ objectFit: "contain" }} />
              </div>
              <span>Google</span>
            </button>
            <button
              type="button"
              onClick={goToRegister}
              className={styles.loginbutton}
              style={{ '--top': `${45}vh` }}
            >
              Usos
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}