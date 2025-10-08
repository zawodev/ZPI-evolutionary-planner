// components/navbar/Navbar.js

export default function Navbar() {
  return (
    <header className="header">
      <div className="logo logo--header">Rekruter+</div>
      
      <nav>
        <button className="header-nav nav-home">Strona główna</button>
        <button className="header-nav nav-func">Funkcjonalności</button>
        <button className="header-nav nav-contact">Kontakt</button>
      </nav>
      
      <div>
        <div className="header-icon header-icon--main"></div>
        <div className="login-btn-wrapper">
          <button className="btn btn--primary btn--login">Login</button>
        </div>
      </div>
    </header>
  );
}