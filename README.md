# 📚 Study Buddy

**Study Buddy** is your all-in-one academic toolkit — a powerful, lightweight web app built to simplify your studies. Whether you're tracking tasks, calculating SGPA, solving physics formulas, or mastering concepts with flashcards, Study Buddy has you covered.

> ✨ Designed for students. Powered by C, Python, HTML/CSS/JS. Optimized for simplicity.

---

## Features

### Task Manager (To-Do List)

* Add, edit, delete, and check off tasks with due dates.
* Keeps you organized and productive.

### Grade Calculator

* Input multiple subject marks and get percentage + letter grade instantly.
* Uses a robust C backend to ensure performance and accuracy.

### SGPA Calculator

* Input credit and grade for each subject.
* Calculates SGPA based on VIT University-style grading.

### Physics Solver

* Solve equations like: `v = u + at`, `s = ut + ½at²`, `F = ma`, and more.
* Just pick a formula and enter values — it does the math.

### Unit Converter

* Converts between units for Length, Mass, Time, Temperature, Volume.
* Example: meter → inch, kg → pounds, etc.

### Flashcards

* Create and revise flashcards.
* Great for quick revision and memory-based subjects.

---

## Tech Stack

| Layer    | Technology                      |
| -------- | ------------------------------- |
| Frontend | HTML, CSS, JavaScript           |
| Backend  | C (compiled)                    |
| Server   | Python (`http.server`)          |
| Database | SQLite (for tasks & flashcards) |

---

## Installation Guide

### ⚙️ Prerequisites

* Python 3.10+
* C Compiler (GCC or MinGW)
* Git
* Web browser (Chrome, Firefox, etc.)

---

### Setup Steps

1. **Clone the Repository**

   ```bash
   git clone https://github.com/yourusername/study-buddy.git
   cd study-buddy
   ```

2. **Compile the C Backend**

   * On Linux/macOS:

     ```bash
     make
     ```
   * On Windows (MinGW):

     ```bash
     mingw32-make
     ```

3. **Run the Server**

   ```bash
   python3 connector.py
   ```

4. **Visit in Browser**

   ```
   http://localhost:8000
   ```

---

## Project Structure

```
study-buddy/
├── backend.c           # Core C logic: SGPA, physics, conversions
├── .gitignore          # Has the backend.exe and study_buddy.db files 
├── connector.py        # Python server (routes frontend ↔ backend)
├── script.js           # Frontend JS for UI + AJAX calls
├── index.html          # Main HTML entry point
├── feature.html        # Another HTML file explaining the features of the file
├── style.css           # UI styles
└── Makefile            # Build instructions for backend.c
```

---

## Makefile Overview

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2 -lm

all: backend

backend: backend.c
	$(CC) $(CFLAGS) -o backend backend.c -lm

clean:
	rm -f backend
```

* **`make`** → Compiles `backend.c` to executable `./backend`
* **`make clean`** → Deletes compiled files

---

## Usage Overview

### 1. Task List

* Add tasks with due dates.
* Mark them complete or edit/delete them.

### 2. Grade Calculator

* Input subject grades (0–100).
* Get percentage + letter grade (A–F).

### 3. SGPA Calculator

* Enter grade + credit per subject.
* Computes SGPA using backend C logic.

### 4. Physics Solver

* Pick a formula, enter values.
* Get answers instantly using real physics equations.

### 5. Unit Converter

* Select category, input units and value.
* Get instant conversions for length, mass, time, etc.

### 6. Flashcards

* Add flashcards (front/back).
* Use them for self-study and memorization.

---

## 🤝 Contributing

Contributions welcome! Whether it’s a bug fix, UI polish, or feature idea — we’d love to see your PR.

### How to Contribute:

```bash
git fork https://github.com/yourusername/study-buddy.git
git checkout -b feature-name
# Make your changes
git commit -am "Add new feature"
git push origin feature-name
# Then open a pull request!
```

---

## 🧑‍💻 Maintainers

This project was built with ❤️ by a student, for students — to make studying efficient, stress-free, and interactive.

---

## Acknowledgements

* Thanks to the open-source community.

---

## 🌟 Final Words

> "Study smarter, not harder — with Study Buddy by your side."
