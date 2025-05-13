````markdown
# Study Buddy

Welcome to **Study Buddy**! 🌟 Your all-in-one study companion that makes learning fun! ✨

Study Buddy is a versatile web application designed to help you with various aspects of your academic journey. Whether you're managing your tasks, calculating your grades, or solving physics problems, Study Buddy has you covered! 

## Features

### To-Do List
- Keep track of your tasks and deadlines. Organize your study schedule and stay on top of your work.

### Grade Calculator
- Calculate your grades and monitor your academic progress in real-time.

### SGPA Calculator
- Track your academic performance and calculate your SGPA to understand where you stand.

### Physics Solver
- A collection of physics formulas to solve problems in an instant. Just choose your formula from a dropdown, and get the results!

### Unit Converter
- Easily convert between different units. Whether it's distance, mass, or time, this tool helps you get the right conversions.

### Flashcards
- Memorize your answers with flashcards! Perfect for preparing for exams, quizzes, and homework.

## Technologies Used
- **Frontend**: HTML, CSS, JavaScript
- **Backend**: C
- **Database**: Not applicable

## Installation

### Prerequisites
- **MinGW** (Minimalist GNU for Windows) installed on your system to compile C code.
- A modern web browser (Chrome, Firefox, etc.)

### Steps to Run the Project

1. **Clone the repository**:
    ```bash
    git clone https://github.com/yourusername/Study-Buddy-2.0.git
    ```

2. **Navigate to the project directory**:
    ```bash
    cd Study-Buddy-2.0
    ```

3. **Run the Makefile** to compile the C code:
    - If you're on Windows, use MinGW to run the following:
      ```bash
      mingw32-make
      ```
    - On Linux/macOS:
      ```bash
      make
      ```

4. **Access the webapp** by opening your browser and going to `http://localhost:8080`.

## Makefile Explanation

The Makefile used in this project automates the build process. It compiles the C code and starts the project in one go.

```makefile
CC = gcc
CFLAGS = -Wall -O2 -mconsole
TARGET = study_buddy

all: $(TARGET)

$(TARGET): study_buddy.c
	$(CC) $(CFLAGS) -o $(TARGET) study_buddy.c -lws2_32

clean:
	rm -f $(TARGET).exe
````

### Key Makefile Steps:

* **CC**: Specifies the compiler (`gcc` for C).
* **CFLAGS**: Compiler flags (`-Wall` for warnings, `-O2` for optimization, `-mconsole` for Windows console).
* **TARGET**: The output executable (`study_buddy`).
* **all**: Default target that builds the project by compiling `study_buddy.c`.
* **clean**: Removes the executable file.

## Usage

### 1. **To-Do List**:

* Add tasks with deadlines to stay organized.
* Mark tasks as complete once done.

### 2. **Grade Calculator**:

* Input your grades for different subjects and calculate your total score.

### 3. **SGPA Calculator**:

* Enter your grades and credits to calculate your SGPA.

### 4. **Physics Solver**:

* Choose a formula from the dropdown, input the known values, and let Study Buddy do the rest.

### 5. **Unit Converter**:

* Select the units you want to convert, enter the value, and get the converted result instantly.

### 6. **Flashcards**:

* Create flashcards for the concepts you want to memorize and review them regularly.

## Contributing

Feel free to fork the repository and contribute by submitting issues or pull requests. You can help us improve the webapp by adding new features, fixing bugs, or enhancing the UI.

### Steps to contribute:

1. Fork the repository
2. Create a new branch (`git checkout -b feature-name`)
3. Make your changes
4. Commit your changes (`git commit -am 'Add new feature'`)
5. Push to the branch (`git push origin feature-name`)
6. Open a pull request with a description of your changes.

---

Thank you for using **Study Buddy**! 🌟 Let's make studying fun and efficient together. ✨

```
