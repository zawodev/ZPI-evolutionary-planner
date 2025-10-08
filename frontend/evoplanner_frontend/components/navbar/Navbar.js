// Navbar.js

export default function Navbar() {
  return (
    <header className="header">
      {/* Logo */}
      <div className="logo logo--header">
        Rekruter+
      </div>
      
      {/* Navigation Links */}
      <button className="header-nav nav-home">
        Home
      </button>
      <button className="header-nav nav-func">
        Funkcjonalności
      </button>
      <button className="header-nav nav-contact">
        Kontakt
      </button>
      
      {/* Header Icon */}
      <div className="header-icon header-icon--main"></div>
      
      {/* Login Button */}
      <div className="login-btn-wrapper">
        <button className="btn btn--primary btn--login">
          Login
        </button>
      </div>
    </header>
  );
}