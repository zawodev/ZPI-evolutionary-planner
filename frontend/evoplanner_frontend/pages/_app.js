import Navbar from "@/components/Navbar";
import styles from "@/styles/light_theme.module.css"
export default function App({ Component, pageProps }) {
  return (
    <>
      <Navbar />
      <Component {...pageProps} />;
    </>
  );
}
