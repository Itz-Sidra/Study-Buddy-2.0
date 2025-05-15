// Mobile Navigation Toggle
const mobileNavToggle = document.querySelector('.mobile-nav-toggle');
const closeNavBtn = document.querySelector('.close-nav');
const mobileNav = document.querySelector('.mobile-nav');
const overlay = document.querySelector('.overlay');

mobileNavToggle.addEventListener('click', () => {
    mobileNav.classList.add('active');
    overlay.classList.add('active');
});

closeNavBtn.addEventListener('click', () => {
    mobileNav.classList.remove('active');
    overlay.classList.remove('active');
});

overlay.addEventListener('click', () => {
    mobileNav.classList.remove('active');
    overlay.classList.remove('active');
});

// Feature Card Selection
const featureCards = document.querySelectorAll('.feature-card');
const modules = document.querySelectorAll('.module');
const getStartedBtn = document.getElementById('get-started-btn');

// Show initial module when "Get Started" is clicked
getStartedBtn.addEventListener('click', () => {
    document.querySelector('.initial-content-area').scrollIntoView({ behavior: 'smooth' });

    // Create confetti effect
    createConfetti();
});

featureCards.forEach(card => {
    card.addEventListener('click', () => {
        const moduleId = card.getAttribute('data-module');
        
        // Remove active class from all cards and modules
        featureCards.forEach(c => c.classList.remove('active'));
        modules.forEach(m => m.classList.remove('active'));
        
        // Add active class to clicked card and corresponding module
        card.classList.add('active');
        document.getElementById(moduleId).classList.add('active');
        
        // Scroll to content area
        document.querySelector('.content-area').scrollIntoView({ behavior: 'smooth' });
    });
});

// Ripple Effect on Buttons
document.addEventListener('click', function(e) {
    if (e.target.tagName === 'BUTTON') {
        const button = e.target;
        const ripple = document.createElement('span');
        ripple.classList.add('ripple');
        button.appendChild(ripple);
        
        const rect = button.getBoundingClientRect();
        const size = Math.max(rect.width, rect.height);
        const x = e.clientX - rect.left - size / 2;
        const y = e.clientY - rect.top - size / 2;
        
        ripple.style.width = ripple.style.height = `${size}px`;
        ripple.style.left = `${x}px`;
        ripple.style.top = `${y}px`;
        
        setTimeout(() => ripple.remove(), 600);
    }
});

// Confetti Effect
function createConfetti() {
    const colors = ['#8C52FF', '#FF6B6B', '#5CE1E6', '#FFDE59', '#72EFB1'];
    
    for (let i = 0; i < 50; i++) {
        const confetti = document.createElement('div');
        confetti.classList.add('confetti');
        confetti.style.left = `${Math.random() * 100}vw`;
        confetti.style.backgroundColor = colors[Math.floor(Math.random() * colors.length)];
        confetti.style.width = `${Math.random() * 10 + 5}px`;
        confetti.style.height = `${Math.random() * 10 + 5}px`;
        confetti.style.animationDuration = `${Math.random() * 2 + 2}s`;
        
        document.body.appendChild(confetti);
        
        setTimeout(() => {
            confetti.remove();
        }, 3000);
    }
}

// Set minimum date for task date input to today's date
function setMinDate() {
    const today = new Date();
    let month = today.getMonth() + 1;
    let day = today.getDate();
    
    month = month < 10 ? '0' + month : month;
    day = day < 10 ? '0' + day : day;
    
    const formattedDate = today.getFullYear() + '-' + month + '-' + day;
    document.getElementById('task-date').min = formattedDate;
}

// Call this function when page loads
setMinDate();

// To-Do List Functionality
const taskTitleInput = document.getElementById('task-title');
const taskDateInput = document.getElementById('task-date');
const addTaskBtn = document.getElementById('add-task-btn');
const taskList = document.getElementById('task-list');

// Initialize tasks array
let tasks = [];

// Load tasks from the server
function loadTasks() {
    fetch('/api/tasks')
        .then(response => response.json())
        .then(data => {
            tasks = data;
            renderTasks();
        })
        .catch(error => {
            console.error('Error loading tasks:', error);
        });
}

// Display tasks
function renderTasks() {
    taskList.innerHTML = '';
    
    tasks.forEach(task => {
        const li = document.createElement('li');
        li.classList.add('task-item');
        if (task.completed) {
            li.classList.add('completed');
        }
        
        li.innerHTML = `
            <input type="checkbox" class="task-check" ${task.completed ? 'checked' : ''}>
            <div class="task-content">
                <div class="task-title">${task.title}</div>
                <div class="task-date">${task.date ? new Date(task.date).toLocaleDateString() : 'No due date'}</div>
            </div>
            <div class="task-actions">
                <button class="edit-btn"><i class="fas fa-edit"></i></button>
                <button class="delete-btn"><i class="fas fa-trash"></i></button>
            </div>
        `;
        
        // Event listeners for task actions
        const checkbox = li.querySelector('.task-check');
        checkbox.addEventListener('change', () => {
            updateTaskStatus(task, checkbox, li);
        });
        
        const deleteBtn = li.querySelector('.delete-btn');
        deleteBtn.addEventListener('click', () => {
            deleteTask(task.id);
        });
        
        const editBtn = li.querySelector('.edit-btn');
        editBtn.addEventListener('click', () => {
            prepareTaskForEditing(task);
        });
        
        taskList.appendChild(li);
    });
}

// Update task completion status
function updateTaskStatus(task, checkbox, li) {
    const updatedTask = { ...task, completed: checkbox.checked };
    
    fetch(`/api/tasks/${task.id}`, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(updatedTask),
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            task.completed = checkbox.checked;
            if (checkbox.checked) {
                li.classList.add('completed');
            } else {
                li.classList.remove('completed');
            }
        }
    })
    .catch(error => {
        console.error('Error updating task:', error);
        checkbox.checked = !checkbox.checked; // Revert the checkbox state if there's an error
    });
}

// Delete a task
function deleteTask(taskId) {
    fetch(`/api/tasks/${taskId}`, {
        method: 'DELETE',
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            loadTasks(); // Reload tasks from server
        }
    })
    .catch(error => {
        console.error('Error deleting task:', error);
    });
}

// Prepare a task for editing
function prepareTaskForEditing(task) {
    taskTitleInput.value = task.title;
    taskDateInput.value = task.date ? task.date : '';
    
    // Delete the task and let the user create a new one with the edited values
    deleteTask(task.id);
}

// Add task event
addTaskBtn.addEventListener('click', () => {
    const title = taskTitleInput.value.trim();
    const date = taskDateInput.value;
    
    if (title) {
        const newTask = {
            title,
            date,
            completed: false
        };
        
        fetch('/api/tasks', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(newTask),
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                taskTitleInput.value = '';
                taskDateInput.value = '';
                loadTasks(); // Reload tasks from server
            }
        })
        .catch(error => {
            console.error('Error adding task:', error);
        });
    }
});

// Initialize task list
loadTasks();

// Function to validate grade input (0-100)
function validateGradeInput(input) {
    // Remove any non-numeric characters
    input.value = input.value.replace(/[^0-9]/g, '');
    
    const value = parseInt(input.value);
    
    // Enforce min and max constraints
    if (value < 0) {
        input.value = 0;
    } else if (value > 100) {
        input.value = 100;
    }
}

// Grade Calculator Functionality
const addSubjectBtn = document.getElementById('add-subject-btn');
const calculateGradeBtn = document.getElementById('calculate-grade-btn');
const gradeItems = document.getElementById('grade-items');
const gradeResult = document.getElementById('grade-result');
const averageGrade = document.getElementById('average-grade');
const gradeLetter = document.getElementById('grade-letter');

addSubjectBtn.addEventListener('click', () => {
    const gradeItem = document.createElement('div');
    gradeItem.classList.add('grade-item');
    gradeItem.innerHTML = `
        <input type="text" class="subject-name" placeholder="Subject Name">
        <input type="number" class="subject-grade" placeholder="Grade (0-100)" min="0" max="100" oninput="validateGradeInput(this)">
    `;
    gradeItems.appendChild(gradeItem);
});

calculateGradeBtn.addEventListener('click', () => {
    const grades = document.querySelectorAll('.subject-grade');
    const subjects = document.querySelectorAll('.subject-name');
    let gradeValues = [];
    
    // Collect grade data
    for (let i = 0; i < grades.length; i++) {
        if (grades[i].value && !isNaN(parseFloat(grades[i].value))) {
            const value = parseFloat(grades[i].value);
            if (value >= 0 && value <= 100) {
                gradeValues.push(value);
            }
        }
    }
    
    if (gradeValues.length > 0) {
        // Build query parameters - backend expects a comma-separated list
        const queryParams = `grades=${gradeValues.join(',')}`;
        
        // Make API call to the backend for grade calculation with corrected endpoint
        fetch(`/api/calculate-grade?${queryParams}`)
            .then(response => response.json())
            .then(data => {
                if (data.error) {
                    alert('Error: ' + data.error);
                } else {
                    averageGrade.textContent = data.average.toFixed(2) + '%';
                    gradeLetter.textContent = `Grade: ${data.letter}`;
                    gradeResult.style.display = 'block';
                }
            })
            .catch(error => {
                console.error('Error calculating grade:', error);
                alert('Failed to contact the server. Check your connection and try again.');
            });
    } else {
        alert('Please add at least one valid grade.');
    }
});

// SGPA Calculator to use C backend
const sgpaForm = document.getElementById("sgpaForm");
const numSubjectsInput = document.getElementById("numSubjects");
const subjectInputsContainer = document.getElementById("subjectInputs");
const sgpaResult = document.getElementById("sgpa-result");
const sgpaValue = document.getElementById("sgpa-value");

numSubjectsInput.addEventListener("change", function() {
    const numSubjects = parseInt(this.value);
    let subjectInputsHTML = '';

    for (let i = 1; i <= numSubjects; i++) {
        subjectInputsHTML += `
            <label for="creditPoints_${i}">Credit ${i}:</label>
            <input type="number" id="creditPoints_${i}" name="creditPoints_${i}" min="1" required>
            <label for="gradeCharacter_${i}">Grade ${i}:</label>
            <select id="gradeCharacter_${i}" name="gradeCharacter_${i}">
                <option value="O">O</option>
                <option value="A+">A+</option>
                <option value="A">A</option>
                <option value="B+">B+</option>
                <option value="B">B</option>
                <option value="C">C</option>
                <option value="U">U</option>
            </select><br>
        `;
    }

    subjectInputsContainer.innerHTML = subjectInputsHTML;
});

sgpaForm.addEventListener("submit", function(event) {
    event.preventDefault();
    const numSubjects = parseInt(numSubjectsInput.value);
    
    if (numSubjects > 0) {
        // Build query parameters
        let queryParams = `num_subjects=${numSubjects}`;
        
        for (let i = 1; i <= numSubjects; i++) {
            const creditPoints = document.getElementById(`creditPoints_${i}`).value;
            const gradeCharacter = document.getElementById(`gradeCharacter_${i}`).value;
            
            if (!creditPoints || isNaN(parseFloat(creditPoints))) {
                alert(`Please enter a valid credit value for subject ${i}`);
                return;
            }
            
            // Match the parameter names that the C backend is expecting
            queryParams += `&creditPoints_${i}=${creditPoints}&gradeCharacter_${i}=${gradeCharacter}`;
        }
        
        // Make API call to the backend for SGPA calculation
        fetch(`/api/calculate-sgpa?${queryParams}`)
            .then(response => response.json())
            .then(data => {
                if (data.error) {
                    alert('Error: ' + data.error);
                } else {
                    sgpaValue.textContent = data.sgpa.toFixed(3);
                    sgpaResult.style.display = 'block';
                }
            })
            .catch(error => {
                console.error('Error calculating SGPA:', error);
                alert('Failed to contact the server. Check your connection and try again.');
            });
    } else {
        alert('Please specify a valid number of subjects.');
    }
});

// Physics Solver Functionality
const formulaSelect = document.getElementById('formula-select');
const formulaInputs = document.getElementById('formula-inputs');
const solvePhysicsBtn = document.getElementById('solve-physics-btn');
const physicsResult = document.getElementById('physics-result');
const physicsAnswer = document.getElementById('physics-answer');
const physicsUnit = document.getElementById('physics-unit');

// Define formulas and their variables
const formulas = {
    'velocity': {
        variables: [
            { name: 'u', label: 'Initial velocity (u)', unit: 'm/s' },
            { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' },
            { name: 't', label: 'Time (t)', unit: 's' }
        ],
        unit: 'm/s',
        equation: 'v = u + at'
    },
    'distance': {
        variables: [
            { name: 'u', label: 'Initial velocity (u)', unit: 'm/s' },
            { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' },
            { name: 't', label: 'Time (t)', unit: 's' }
        ],
        unit: 'm',
        equation: 's = ut + (1/2)at²'
    },
    'velocity-squared': {
        variables: [
            { name: 'u', label: 'Initial velocity (u)', unit: 'm/s' },
            { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' },
            { name: 's', label: 'Distance (s)', unit: 'm' }
        ],
        unit: 'm/s',
        equation: 'v = √(u² + 2as)'
    },
    'force': {
        variables: [
            { name: 'm', label: 'Mass (m)', unit: 'kg' },
            { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' }
        ],
        unit: 'N',
        equation: 'F = ma'
    },
    'kinetic-energy': {
        variables: [
            { name: 'm', label: 'Mass (m)', unit: 'kg' },
            { name: 'v', label: 'Velocity (v)', unit: 'm/s' }
        ],
        unit: 'J',
        equation: 'KE = (1/2)mv²'
    },
    'potential-energy': {
        variables: [
            { name: 'm', label: 'Mass (m)', unit: 'kg' },
            { name: 'g', label: 'Gravity (g)', unit: 'm/s²', default: 9.8 },
            { name: 'h', label: 'Height (h)', unit: 'm' }
        ],
        unit: 'J',
        equation: 'PE = mgh'
    },
    'ohms-law': {
        variables: [
            { name: 'i', label: 'Current (I)', unit: 'A' },
            { name: 'r', label: 'Resistance (R)', unit: 'Ω' }
        ],
        unit: 'V',
        equation: 'V = IR'
    },
    'power': {
        variables: [
            { name: 'v', label: 'Voltage (V)', unit: 'V' },
            { name: 'i', label: 'Current (I)', unit: 'A' }
        ],
        unit: 'W',
        equation: 'P = VI'
    }
};

formulaSelect.addEventListener('change', () => {
    const selectedFormula = formulaSelect.value;
    formulaInputs.innerHTML = '';
    
    if (selectedFormula && formulas[selectedFormula]) {
        const formula = formulas[selectedFormula];
        
        formula.variables.forEach(variable => {
            const inputGroup = document.createElement('div');
            inputGroup.classList.add('form-group');
            
            inputGroup.innerHTML = `
                <label for="${variable.name}">${variable.label}</label>
                <input type="number" id="${variable.name}" step="any" placeholder="Enter value..." ${variable.default ? `value="${variable.default}"` : ''}>
                <small>${variable.unit}</small>
            `;
            
            formulaInputs.appendChild(inputGroup);
        });
        
        solvePhysicsBtn.style.display = 'block';
    } else {
        solvePhysicsBtn.style.display = 'none';
    }
    
    physicsResult.style.display = 'none';
});

solvePhysicsBtn.addEventListener('click', () => {
    const selectedFormula = formulaSelect.value;
    
    if (selectedFormula && formulas[selectedFormula]) {
        const formula = formulas[selectedFormula];
        const values = {};
        
        // Get input values
        let allInputsFilled = true;
        formula.variables.forEach(variable => {
            const input = document.getElementById(variable.name);
            
            if (input.value.trim() !== "") {
                const value = parseFloat(input.value);
                
                if (!isNaN(value)) {
                    values[variable.name] = value;
                } else {
                    allInputsFilled = false;
                }
            } else {
                allInputsFilled = false;
            }
        });
        
        if (allInputsFilled) {
            // Build query string
            let queryParams = `formula=${selectedFormula}`;
            for (const [key, value] of Object.entries(values)) {
                queryParams += `&${key}=${value}`;
            }
            
            // Make AJAX request to backend
            fetch(`/api/physics?${queryParams}`)
                .then(response => response.json())
                .then(data => {
                    if (data.error) {
                        alert('Error: ' + data.error);
                    } else {
                        physicsAnswer.textContent = data.result.toFixed(2);
                        physicsUnit.textContent = formula.unit;
                        physicsResult.style.display = 'block';
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    alert('Failed to contact the server. Check your connection and try again.');
                });
        } else {
            alert('Please fill in all inputs correctly.');
        }
    }
});

// Unit Converter Functionality
const conversionType = document.getElementById('conversion-type');
const fromUnit = document.getElementById('from-unit');
const toUnit = document.getElementById('to-unit');
const valueInput = document.getElementById('value-to-convert');
const valueInputGroup = document.getElementById('value-input-group');
const convertBtn = document.getElementById('convert-btn');
const conversionResult = document.getElementById('conversion-result');
const convertedValue = document.getElementById('converted-value');
const conversionEquation = document.getElementById('conversion-equation');
const unitSelectors = document.querySelector('.unit-selectors');
const valueError = document.getElementById('value-error');

// Define conversion units
const conversions = {
    'length': {
        units: ['meter', 'centimeter', 'kilometer', 'inch', 'foot', 'yard', 'mile']
    },
    'mass': {
        units: ['kilogram', 'gram', 'milligram', 'pound', 'ounce', 'ton']
    },
    'temperature': {
        units: ['celsius', 'fahrenheit', 'kelvin']
    },
    'time': {
        units: ['second', 'minute', 'hour', 'day', 'week', 'month', 'year']
    },
    'volume': {
        units: ['liter', 'milliliter', 'cubic_meter', 'gallon', 'quart', 'pint', 'cup']
    }
};

// Event listener to restrict input to numeric values and prevent negative input
valueInput.addEventListener('keydown', (e) => {
    // Allow only numeric values and control keys (backspace, delete, etc.)
    if (!/^[0-9\.]$/.test(e.key) && !['Backspace', 'Delete', 'ArrowLeft', 'ArrowRight'].includes(e.key)) {
        e.preventDefault();
    }
});

// Event listener to check for negative values and invalid inputs
valueInput.addEventListener('input', () => {
    const value = parseFloat(valueInput.value);
    if (value < 0 || isNaN(value)) {
        valueError.style.display = 'inline';
        convertBtn.disabled = true;
    } else {
        valueError.style.display = 'none';
        convertBtn.disabled = false;
    }
});

// Handle unit selection change
conversionType.addEventListener('change', () => {
    const selectedType = conversionType.value;
    fromUnit.innerHTML = '';
    toUnit.innerHTML = '';
    
    if (selectedType && conversions[selectedType]) {
        const units = conversions[selectedType].units;
        
        units.forEach(unit => {
            fromUnit.innerHTML += `<option value="${unit}">${unit.charAt(0).toUpperCase() + unit.slice(1)}</option>`;
            toUnit.innerHTML += `<option value="${unit}">${unit.charAt(0).toUpperCase() + unit.slice(1)}</option>`;
        });
        
        // Set different default values
        if (units.length > 1) {
            toUnit.value = units[1];
        }
        
        unitSelectors.style.display = 'flex';
        valueInputGroup.style.display = 'block';
        convertBtn.style.display = 'block';
    } else {
        unitSelectors.style.display = 'none';
        valueInputGroup.style.display = 'none';
        convertBtn.style.display = 'none';
    }
    
    conversionResult.style.display = 'none';
});

// Handle conversion on button click
convertBtn.addEventListener('click', () => {
    const type = conversionType.value;
    const from = fromUnit.value;
    const to = toUnit.value;
    const value = parseFloat(valueInput.value);
    
    if (type && from && to && !isNaN(value) && value >= 0) {
        // Build query string
        const queryParams = `type=${type}&from=${from}&to=${to}&value=${value}`;
        
        // Make AJAX request to backend
        fetch(`/api/convert?${queryParams}`)
            .then(response => response.json())
            .then(data => {
                if (data.error) {
                    alert('Error: ' + data.error);
                } else {
                    convertedValue.textContent = data.result.toFixed(4);
                    conversionEquation.textContent = `${value} ${from} = ${data.result.toFixed(4)} ${to}`;
                    conversionResult.style.display = 'block';
                }
            })
            .catch(error => {
                console.error('Error:', error);
                alert('Failed to contact the server. Check your connection and try again.');
            });
    }
});

// Flashcards Functionality
const flashcardsContainer = document.getElementById('flashcards-container');
const flashcardFront = document.getElementById('flashcard-front');
const flashcardBack = document.getElementById('flashcard-back');
const addFlashcardBtn = document.getElementById('add-flashcard-btn');
const prevFlashcardBtn = document.getElementById('prev-flashcard-btn');
const flipFlashcardBtn = document.getElementById('flip-flashcard-btn');
const nextFlashcardBtn = document.getElementById('next-flashcard-btn');
const flashcardsControls = document.querySelector('.flashcards-controls');
const deleteFlashcardBtn = document.getElementById('delete-flashcard-btn');

// Initialize flashcards
let flashcards = [];
let currentFlashcardIndex = 0;

// Load flashcards from the server
function loadFlashcards() {
    fetch('/api/flashcards')
        .then(response => response.json())
        .then(data => {
            flashcards = data;
            currentFlashcardIndex = 0; // Reset to first card when loading
            renderFlashcards();
            updateFlashcardControls();
        })
        .catch(error => {
            console.error('Error loading flashcards:', error);
        });
}

// Display flashcards
function renderFlashcards() {
    flashcardsContainer.innerHTML = '';
    
    if (flashcards.length > 0) {
        const flashcard = document.createElement('div');
        flashcard.classList.add('flashcard');
        flashcard.setAttribute('data-id', flashcards[currentFlashcardIndex].id);
        
        flashcard.innerHTML = `
            <div class="flashcard-inner">
                <div class="flashcard-front">
                    <div class="flashcard-content">${flashcards[currentFlashcardIndex].front}</div>
                </div>
                <div class="flashcard-back">
                    <div class="flashcard-content">${flashcards[currentFlashcardIndex].back}</div>
                </div>
            </div>
        `;
        
        flashcardsContainer.appendChild(flashcard);
        
        // Add click event to flip the card
        flashcard.addEventListener('click', () => {
            flashcard.classList.toggle('flipped');
        });
    } else {
        flashcardsContainer.innerHTML = '<div class="no-flashcards">No flashcards yet. Add some!</div>';
    }
    
    updateFlashcardControls();
}

// Update flashcard controls visibility and counters
function updateFlashcardControls() {
    if (flashcards.length > 0) {
        flashcardsControls.style.display = 'flex';
        
        // Add card counter if needed
        const cardCounter = document.getElementById('card-counter') || document.createElement('div');
        cardCounter.id = 'card-counter';
        cardCounter.textContent = `Card ${currentFlashcardIndex + 1} of ${flashcards.length}`;
        
        if (!document.getElementById('card-counter')) {
            flashcardsControls.appendChild(cardCounter);
        }
        
        // Enable/disable navigation buttons based on position
        if (deleteFlashcardBtn) {
            deleteFlashcardBtn.style.display = 'block';
        }
    } else {
        flashcardsControls.style.display = 'none';
        if (deleteFlashcardBtn) {
            deleteFlashcardBtn.style.display = 'none';
        }
    }
}

// Add flashcard event
addFlashcardBtn.addEventListener('click', () => {
    const front = flashcardFront.value.trim();
    const back = flashcardBack.value.trim();
    
    if (front && back) {
        const newFlashcard = { front, back };
        
        fetch('/api/flashcards', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(newFlashcard),
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                flashcardFront.value = '';
                flashcardBack.value = '';
                loadFlashcards(); // Reload flashcards from server
            }
        })
        .catch(error => {
            console.error('Error adding flashcard:', error);
        });
    }
});

// Delete current flashcard
function deleteCurrentFlashcard() {
    if (flashcards.length > 0) {
        const flashcardId = flashcards[currentFlashcardIndex].id;
        
        fetch(`/api/flashcards/${flashcardId}`, {
            method: 'DELETE',
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                loadFlashcards(); // Reload flashcards from server
            }
        })
        .catch(error => {
            console.error('Error deleting flashcard:', error);
        });
    }
}

// Add delete button event listener if it exists
if (deleteFlashcardBtn) {
    deleteFlashcardBtn.addEventListener('click', deleteCurrentFlashcard);
}

// Navigation and flip controls
prevFlashcardBtn.addEventListener('click', () => {
    if (flashcards.length > 0) {
        currentFlashcardIndex = (currentFlashcardIndex - 1 + flashcards.length) % flashcards.length;
        renderFlashcards();
    }
});

nextFlashcardBtn.addEventListener('click', () => {
    if (flashcards.length > 0) {
        currentFlashcardIndex = (currentFlashcardIndex + 1) % flashcards.length;
        renderFlashcards();
    }
});

flipFlashcardBtn.addEventListener('click', () => {
    const flashcard = document.querySelector('.flashcard');
    if (flashcard) {
        flashcard.classList.toggle('flipped');
    }
});

// Initialize flashcards
loadFlashcards();