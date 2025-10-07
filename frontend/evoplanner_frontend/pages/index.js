import Navbar from "@/components/Navbar";
import styles from "@/styles/light_theme.module.css"
export default function Home() {
    return (
        <div className={styles.background}>
            <Navbar />
            <section>
                <div className={styles["header-strip"]}>
                    <p className={styles["out-now"]}>Out Now!</p>
                </div>
                <div>
                    <h1 className={styles["hero-title"]}>
                        Twoje wybory. <br />
                        Twoje plany. <br />
                        Twoja wygoda. <br />
                    </h1>
                    <p className={styles["hero-desc"]}>
                        To nowoczesna aplikacja, która automatycznie tworzy harmonogram zajęć,
                        dopasowany jednocześnie do potrzeb uczniów i nauczycieli. Wystarczy
                        wprowadzić dostępność oraz priorytety, a system sam znajdzie najlepsze
                        rozwiązanie, minimalizując konflikty i zapewniając maksymalną wygodę
                        wszystkim stronom.
                    </p>
                </div>
            </section>
        </div>
    );
}