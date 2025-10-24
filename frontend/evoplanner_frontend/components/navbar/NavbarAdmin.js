// components/navbar/Navbar.js
import Link from 'next/link';

export default function NavbarAdmin() {
    return (
        <header className="header">
            <div className="logo logo--header">Rekruter+</div>

            <nav>
                <Link href="/admin/users">
                    <button className="header-nav nav-home">Użytkownicy</button>
                </Link>
                <Link href="/admin/rooms">
                    <button className="header-nav nav-func">Pokoje</button>
                </Link>
                <Link href="/admin/createrec">
                    <button className="header-nav nav-contact">Stwórz rejestrację</button>
                </Link>
            </nav>

            <div>
                <div className="header-icon header-icon--main"></div>
                <div className="login-btn-wrapper">
                    <Link href="/login">
                        <button className="btn btn--primary btn--login">Wyloguj się</button>
                    </Link>
                </div>
            </div>
        </header>
    );
}