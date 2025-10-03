// Login page with the form for a login. Register sends to a diffrent page
import React, { useState } from "react";

export default function LoginPage() {
  const [login, setLogin] = useState("");
  const [password, setPassword] = useState("");

  const handleLogin = (e) => { //handles api call - method should be under api/login.js
    e.preventDefault();
    // TODO: Api call
    navigate("/check")
    console.log("Logging in with:", login, password);
  };

  const goToRegister = () => { //just sends to register page
     window.location.href = "/check";
  };

  return (
    //main page TODO: these styles should be in styles/login
    <div className="flex items-center justify-center h-screen">
      <div className="bg-red-100 shadow-lg rounded-2xl p-8 w-96 text-center">
        <div className="mb-6">
          <img
            src="/globe.svg"
            alt="App Logo"
            className="mx-auto w-20 h-20"
          />
        </div>

        <form onSubmit={handleLogin} className="space-y-4">
          <input
            type="text"
            placeholder="Login"
            value={login}
            onChange={(e) => setLogin(e.target.value)}
            className="w-full p-3 border rounded-lg focus:outline-none focus:ring-2 focus:ring-red-200"
            required
          />

          <input
            type="password"
            placeholder="Password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            className="w-full p-3 border rounded-lg focus:outline-none focus:ring-2 focus:ring-red-200"
            required
          />

          
          <div className="space-y-2">
            <button
              type="submit"
              className="w-full bg-red-600 text-white py-3 rounded-lg hover:bg-red-700 transition"
            >
              Login
            </button>
            <button
              type="button"
              onClick={goToRegister}
              className="w-full bg-gray-200 text-gray-700 py-3 rounded-lg hover:bg-gray-300 transition"
            >
              Register
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}