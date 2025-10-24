import styles from '@/styles/components/_admin.module.css';
import NavbarAdmin from '@/components/navbar/NavbarAdmin';
import { useState } from "react";
export default function Users() {
    const [selectedCategory, setSelectedCategory] = useState("Users");
    const [attendees, setAttendees] = useState([]);
    const [attendants, setAttendants] = useState([]);
    const [groups, setGroups] = useState([]);
    const [searchTerm, setSearchTerm] = useState("");

    // Form states
    const [firstName, setFName] = useState("");
    const [surName, setSName] = useState("");
    const [userEmail, setUserEmail] = useState("");
    const [userRole, setUserRole] = useState("participant");
    const [groupName, setGroupName] = useState("");
    const [selectedGroup, setSelectedGroup] = useState("");
    const addUser = () => {
        if (!firstName|| !surName || !userEmail) return;
        if (userRole === 'participant') {
            const newUser = {
            first_name: firstName,
            last_name: surName,
            email: userEmail,
            role: userRole,
            group: selectedGroup ? selectedGroup : null,
            };
            setAttendees([...attendees, newUser]);
        } else if (role === 'host') {
            const newUser = {
            first_name: firstName,
            last_name: surName,
            email: userEmail,
            role: userRole,
            };
            setAttendants([...attendants, newUser]);
        }

        setFName("");
        setFName("");
        setUserEmail("");
        setSelectedGroup("");
    };

    const addGroup = () => {
        if (!groupName) return;
        setGroups([...groups, { name: groupName }]);
        setGroupName("");
    };

    const filteredAttendees = attendees.filter(
        (u) =>
            (u.first_name.toLowerCase().includes(searchTerm.toLowerCase()) || u.last_name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                u.email.toLowerCase().includes(searchTerm.toLowerCase()))
    );

    return (
        <div className={`${styles.background} ${styles.grid}`}>
            <NavbarAdmin></NavbarAdmin>
            <div className={styles.left}>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("Users")}
                        className="btn btn--secondary btn--form"
                    >
                        Użytkownicy
                    </button>
                </div>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("Attendees")}
                        className="btn btn--secondary btn--form"
                    >
                        Uczestniczący
                    </button>
                </div>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("Attendants")}
                        className="btn btn--secondary btn--form"
                    >
                        Prowadzący
                    </button>
                </div>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("Groups")}
                        className="btn btn--secondary btn--form"
                    >
                        Grupy
                    </button>
                </div>
            </div>

            <div className={styles.right}>
                {selectedCategory === "Users" && (
                    <div>
                        <h2>Dodaj użytkownika</h2>
                        <h3> Tutaj możesz dodawać użytkowników. Po wypełnieniu formularza nasz system automatycznie wyśle email zawierający login i hasło na podany adres. Jeśli chcesz dodać uczestników do grup udaj sie na podstronę "Grupy"</h3>
                        <div >
                            <div className={styles.namegrid}>
                                <div className="login-input-wrapper">
                                    <input
                                        type="text"
                                        placeholder="Imię"
                                        className="input input--login"
                                        value={firstName}
                                        onChange={(e) => setFName(e.target.value)}
                                    />
                                </div>
                                <div className="login-input-wrapper">
                                    <input
                                        type="text"
                                        placeholder="Nazwisko"
                                        className="input input--login"
                                        value={surName}
                                        onChange={(e) => setSName(e.target.value)}
                                    />
                                </div>
                            </div>
                            <div className="login-input-wrapper">
                                <input
                                    type="email"
                                    placeholder="Adres email"
                                    className="input input--login"
                                    value={userEmail}
                                    onChange={(e) => setUserEmail(e.target.value)}
                                />
                            </div>
                            <div className="login-button-wrapper" style={{ display: "flex", justifyContent: "space-between", width: "100%" }}>
                                <button
                                    onClick={() => setUserRole("participant")}
                                    className={`btn btn--form ${userRole === "participant"
                                        ? styles.btnselect
                                        : styles.btn
                                        }`}
                                >
                                    Uczestniczący
                                </button>
                                <button
                                    onClick={() => setUserRole("host")}
                                    className={`btn btn--form ${userRole === "host"
                                        ? styles.btnselect
                                        : styles.btn
                                        }`}
                                >
                                    Prowadzący
                                </button>
                                <button
                                    onClick={() => setUserRole("office")}
                                    className={`btn btn--form ${userRole === "office"
                                        ? styles.btnselect
                                        : styles.btn
                                        }`}
                                >
                                    Sekretariat
                                </button>
                            </div>
                            {userRole === "attendee" && groups.length > 0 && (
                                <div>
                                    <label className="block text-sm font-medium mb-1">
                                        Assign to Group:
                                    </label>
                                    <select
                                        className="w-full border p-2 rounded"
                                        value={selectedGroup}
                                        onChange={(e) => setSelectedGroup(e.target.value)}
                                    >
                                        <option value="">Select a group</option>
                                        {groups.map((g, i) => (
                                            <option key={i} value={g.name}>
                                                {g.name}
                                            </option>
                                        ))}
                                    </select>
                                </div>
                            )}
                            <div style={{ width: '100%',display:'flex',justifyContent:"center", padding:"10vh" }}>
                                <button
                                    onClick={addUser}
                                    className={`btn btn--form ${styles.btnadd}`}
                                >
                                    Dodaj użytkownika
                                </button>
                            </div>
                        </div>
                    </div>
                )}

                {selectedCategory === "Attendees" && (
                    <div className="bg-white rounded-lg shadow p-6">
                        <h2 className="text-xl font-semibold mb-4">Uczestniczący</h2>
                        <input
                            type="text"
                            placeholder="Search attendees..."
                            className="w-full border p-2 rounded mb-3"
                            value={searchTerm}
                            onChange={(e) => setSearchTerm(e.target.value)}
                        />
                        <ul className="space-y-1">
                            {filteredAttendees.map((u, i) => (
                                <li key={i} className="border-b py-1 text-sm">
                                    {u.first_name} {u.last_name} ({u.email})
                                </li>
                            ))}
                            {filteredAttendees.length === 0 && (
                                <p className="text-gray-500 text-sm">No attendees found.</p>
                            )}
                        </ul>
                    </div>
                )}

                {selectedCategory === "Attendants" && (
                    <div className="bg-white rounded-lg shadow p-6">
                        <h2 className="text-xl font-semibold mb-4">Prowadzący</h2>
                        <ul className="space-y-1">
                            {attendants
                                .map((u, i) => (
                                    <li key={i} className="border-b py-1 text-sm">
                                        {u.first_name} {u.last_name} ({u.email})
                                    </li>
                                ))}
                            {attendants.length === 0 && (
                                <p className="text-gray-500 text-sm">No attendants yet.</p>
                            )}
                        </ul>
                    </div>
                )}

                {selectedCategory === "Groups" && (
                    <div className="bg-white rounded-lg shadow p-6">
                        <h2 className="text-xl font-semibold mb-4">Dodaj grupę</h2>
                        <div className="space-y-3">
                            <div className="login-input-wrapper">
                                <input
                                    type="text"
                                    placeholder="Nazwa grupy"
                                    className="input input--login"
                                    value={groupName}
                                    onChange={(e) => setGroupName(e.target.value)}
                                />
                            </div>
                            <div style={{ width: '100%',display:'flex',justifyContent:"center" }}>
                                <button
                                    onClick={addGroup}
                                    className={`btn btn--form ${styles.btnadd}`}
                                >
                                    Dodaj grupę
                                </button>
                            </div>
                        </div>

                        {/* Groups list */}
                        <div className="mt-6">
                            <h3 className="font-semibold mb-2">All Groups</h3>
                            <ul className="space-y-1">
                                {groups.map((g, i) => (
                                    <li key={i} className="border-b py-1 text-sm">
                                        {g.name}
                                    </li>
                                ))}
                            </ul>
                        </div>
                    </div>
                )}
            </div>
        </div>
    );
}