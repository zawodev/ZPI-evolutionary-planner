import styles from "@/styles/light_theme.module.css"

export default function Navbar() {
    return (
        <nav className={styles.header}>
            <div>
                <div>
                    <strong className={styles.logo}>Rekruter+</strong>

                    <button className={styles["nav-home"]}>Home</button>
                    <button className={styles["nav-func"]}>Funkcjonalności</button>
                    <button className={styles["nav-contact"]}>Kontakt</button>
                    <button className={styles["login-btn"]}>Zaloguj się</button>

                </div>
            </div>
        </nav>
    );
}