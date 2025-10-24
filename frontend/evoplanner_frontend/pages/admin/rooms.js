import styles from '@/styles/components/_admin.module.css';
import NavbarAdmin from '@/components/navbar/NavbarAdmin';
import { useState } from "react";
export default function Rooms() {
    const [selectedCategory, setSelectedCategory] = useState("AddRoom");
    const [rooms, setRooms] = useState([]);
    const [tags, setTags] = useState([]);
    const [roomTags, setRoomTags] = useState([]);
    const [searchTerm, setSearchTerm] = useState("");

    // Form states
    const [roomName, setRoomName] = useState("");
    const [roomSubName, setRoomSubName] = useState("");
    const [capacity, setCapacity] = useState("");
    const [tagName, setTagName] = useState("");
    const [selectedTag, setSelectedTag] = useState("");
    const addRoom = () => {
        if (!roomName) return;

        const newRoom = {
            building_name: roomName,
            room_number: roomSubName,
            capacity: capacity,
            tags:roomTags
        };

        setRooms([...users, newRoom]);
        setRoomName("");
        setRoomSubName("");
        setSelectedTag("");
        setRoomTags([]);
    };
    const addRoomTag = (tag) => {
        if (roomTags.includes(tag)) return;
        setRoomTags([...roomTags, tag]);
        setSelectedTag("")
    }

    const addTag= () => {
        if (!tagName) return;
        setTags([...tags, { name: tagName }]);
        setTagName("");
    };

    const filteredRooms = rooms.filter(
        (u) =>
            (u.building_name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                u.room_number.toLowerCase().includes(searchTerm.toLowerCase()))
    );

    return (
        <div className={`${styles.background} ${styles.grid}`}>
            <NavbarAdmin></NavbarAdmin>
            <div className={styles.left}>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("AddRoom")}
                        className="btn btn--secondary btn--form"
                    >
                        Dodaj Pokój
                    </button>
                </div>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("Rooms")}
                        className="btn btn--secondary btn--form"
                    >
                        Pokoje
                    </button>
                </div>
                <div className="login-button-wrapper">
                    <button
                        type="button"
                        onClick={() => setSelectedCategory("Tags")}
                        className="btn btn--secondary btn--form"
                    >
                        Cechy pokoi
                    </button>
                </div>
            </div>

            <div className={styles.right}>
                {selectedCategory === "AddRoom" && (
                    <div className="bg-white rounded-lg shadow p-6">
                        <h2 className="text-xl font-semibold mb-4">Dodaj pokój</h2>
                        <h3 className="text-xl font-semibold mb-4"> Tutaj możesz dodawać pokoje. Jeśli chcesz dodać cechy do pokoi udaj sie na podstronę "Cechy pokoi"</h3>
                        <div className="space-y-3">
                            <div className={styles.namegrid}>
                                <div className="login-input-wrapper">
                                    <input
                                        type="text"
                                        placeholder="Nazwa pokoju np.: nr. budynku"
                                        className="input input--login"
                                        value={roomName}
                                        onChange={(e) => setRoomName(e.target.value)}
                                    />
                                </div>
                                <div className="login-input-wrapper">
                                    <input
                                        type="text"
                                        placeholder="Podnazwa pokoju np.: numer sali"
                                        className="input input--login"
                                        value={roomSubName}
                                        onChange={(e) => setRoomSubName(e.target.value)}
                                    />
                                </div>
                            </div>
                            <div className="login-input-wrapper">
                                <input
                                    type="number"
                                    placeholder="Pojemność pokoju"
                                    className="input input--login"
                                    value={capacity}
                                    onChange={(e) => setCapacity(e.target.value)}
                                />
                            </div>
                            {tags.length > 0 && (
                                <div>
                                    <label className="block text-sm font-medium mb-1">
                                        Dodaj cechy pokoju:
                                    </label>
                                    <select
                                        className="w-full border p-2 rounded"
                                        value={selectedTag}
                                        onChange={(e) => addRoomTag(e.target.value)}
                                    >
                                        <option value="">Dodaj cechę</option>
                                        {tags.map((g, i) => (
                                            <option key={i} value={g.name}>
                                                {g.name}
                                            </option>
                                        ))}
                                    </select>
                                </div>
                            )}
                            <div className="mt-6">
                            <h3 className="font-semibold mb-2">Cechy pokoju</h3>
                            <ul className="space-y-1">
                                {roomTags.map((g, i) => (
                                    <li key={i} className="border-b py-1 text-sm">
                                        {g}
                                    </li>
                                ))}
                            </ul>
                        </div>
                            <div style={{ width: '100%',display:'flex',justifyContent:"center", padding:"10vh" }}>
                                <button
                                    onClick={addRoom}
                                    className={`btn btn--form ${styles.btnadd}`}
                                >
                                    Dodaj pokój
                                </button>
                            </div>
                        </div>
                    </div>
                )}

                {selectedCategory === "Rooms" && (
                    <div className="bg-white rounded-lg shadow p-6">
                        <h2 className="text-xl font-semibold mb-4">Pokoje</h2>
                        <input
                            type="text"
                            placeholder="Search attendees..."
                            className="w-full border p-2 rounded mb-3"
                            value={searchTerm}
                            onChange={(e) => setSearchTerm(e.target.value)}
                        />
                        <ul className="space-y-1">
                            {filteredRooms.map((u, i) => (
                                <li key={i} className="border-b py-1 text-sm">
                                    {u.building_name} ({u.room_number})
                                </li>
                            ))}
                            {filteredRooms.length === 0 && (
                                <p className="text-gray-500 text-sm">Nie znaleziono pokoju.</p>
                            )}
                        </ul>
                    </div>
                )}

                {selectedCategory === "Tags" && (
                    <div className="bg-white rounded-lg shadow p-6">
                        <h2 className="text-xl font-semibold mb-4">Dodaj cechę</h2>
                        <div className="space-y-3">
                            <div className="login-input-wrapper">
                                <input
                                    type="text"
                                    placeholder="Nazwa grupy"
                                    className="input input--login"
                                    value={tagName}
                                    onChange={(e) => setTagName(e.target.value)}
                                />
                            </div>
                            <div style={{ width: '100%',display:'flex',justifyContent:"center" }}>
                                <button
                                    onClick={addTag}
                                    className={`btn btn--form ${styles.btnadd}`}
                                >
                                    Dodaj grupę
                                </button>
                            </div>
                        </div>

                        <div className="mt-6">
                            <h3 className="font-semibold mb-2">Dostępne cechy</h3>
                            <ul className="space-y-1">
                                {tags.map((g, i) => (
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