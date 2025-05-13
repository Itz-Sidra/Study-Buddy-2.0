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
            modules[0].classList.add('active');
            featureCards[0].classList.add('active');
            document.querySelector('.content-area').scrollIntoView({ behavior: 'smooth' });
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
                
                // Create confetti effect
                createConfetti();
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
        
        // Initialize tasks from localStorage
        let tasks = JSON.parse(localStorage.getItem('studyBuddyTasks')) || [];
        
        // Display tasks
        function renderTasks() {
            taskList.innerHTML = '';
            
            tasks.forEach((task, index) => {
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
                    tasks[index].completed = checkbox.checked;
                    if (checkbox.checked) {
                        li.classList.add('completed');
                    } else {
                        li.classList.remove('completed');
                    }
                    saveTasks();
                });
                
                const deleteBtn = li.querySelector('.delete-btn');
                deleteBtn.addEventListener('click', () => {
                    tasks.splice(index, 1);
                    renderTasks();
                    saveTasks();
                });
                
                const editBtn = li.querySelector('.edit-btn');
                editBtn.addEventListener('click', () => {
                    taskTitleInput.value = task.title;
                    taskDateInput.value = task.date ? task.date : '';
                    tasks.splice(index, 1);
                    renderTasks();
                    saveTasks();
                });
                
                taskList.appendChild(li);
            });
        }
        
        // Save tasks to localStorage
        function saveTasks() {
            localStorage.setItem('studyBuddyTasks', JSON.stringify(tasks));
        }
        
        // Add task event
        addTaskBtn.addEventListener('click', () => {
            const title = taskTitleInput.value.trim();
            const date = taskDateInput.value;
            
            if (title) {
                tasks.push({
                    title,
                    date,
                    completed: false
                });
                
                taskTitleInput.value = '';
                taskDateInput.value = '';
                
                renderTasks();
                saveTasks();
            }
        });
        
        // Initialize task list
        renderTasks();
        
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
            let totalGrade = 0;
            let validGrades = 0;
            
            grades.forEach(grade => {
                if (grade.value && !isNaN(parseFloat(grade.value))) {
                    const value = parseFloat(grade.value);
                    if (value >= 0 && value <= 100) {
                        totalGrade += value;
                        validGrades++;
                    }
                }
            });
            
            if (validGrades > 0) {
                const percentage = totalGrade / validGrades;
                averageGrade.textContent = percentage.toFixed(2) + '%';
                
                let letter = '';
                if (percentage >= 90) letter = 'A';
                else if (percentage >= 80) letter = 'B';
                else if (percentage >= 70) letter = 'C';
                else if (percentage >= 60) letter = 'D';
                else letter = 'F';
                
                gradeLetter.textContent = `Grade: ${letter}`;
                gradeResult.style.display = 'block';
            }
        });
        
        // SGPA Calculator Functionality
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
            let totalGradePoints = 0;
            let totalCredits = 0;

            for (let i = 1; i <= numSubjects; i++) {
                const creditPoints = parseFloat(document.getElementById(`creditPoints_${i}`).value);
                const gradeCharacter = document.getElementById(`gradeCharacter_${i}`).value;
                const gradePoints = convertGradeToPoints(gradeCharacter);
                totalGradePoints += creditPoints * gradePoints;
                totalCredits += creditPoints;
            }

            const sgpa = totalGradePoints / totalCredits;
            sgpaValue.textContent = sgpa.toFixed(3);
            sgpaResult.style.display = 'block';
        });

        function convertGradeToPoints(grade) {
            switch(grade) {
                case 'O': return 10;
                case 'A+': return 9;
                case 'A': return 8;
                case 'B+': return 7;
                case 'B': return 6;
                case 'C': return 5;
                case 'U': return 0;
                default: return 0;
            }
        }

        
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
                solve: (values) => {
                    return values.u + values.a * values.t;
                },
                unit: 'm/s',
                equation: 'v = u + at'
            },
            'distance': {
                variables: [
                    { name: 'u', label: 'Initial velocity (u)', unit: 'm/s' },
                    { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' },
                    { name: 't', label: 'Time (t)', unit: 's' }
                ],
                solve: (values) => {
                    return values.u * values.t + 0.5 * values.a * Math.pow(values.t, 2);
                },
                unit: 'm',
                equation: 's = ut + (1/2)at²'
            },
            'velocity-squared': {
                variables: [
                    { name: 'u', label: 'Initial velocity (u)', unit: 'm/s' },
                    { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' },
                    { name: 's', label: 'Distance (s)', unit: 'm' }
                ],
                solve: (values) => {
                    const result = Math.pow(values.u, 2) + 2 * values.a * values.s;
                    return result >= 0 ? Math.sqrt(result) : "Complex value";
                },
                unit: 'm/s',
                equation: 'v = √(u² + 2as)'
            },
            'force': {
                variables: [
                    { name: 'm', label: 'Mass (m)', unit: 'kg' },
                    { name: 'a', label: 'Acceleration (a)', unit: 'm/s²' }
                ],
                solve: (values) => {
                    return values.m * values.a;
                },
                unit: 'N',
                equation: 'F = ma'
            },
            'kinetic-energy': {
                variables: [
                    { name: 'm', label: 'Mass (m)', unit: 'kg' },
                    { name: 'v', label: 'Velocity (v)', unit: 'm/s' }
                ],
                solve: (values) => {
                    return 0.5 * values.m * Math.pow(values.v, 2);
                },
                unit: 'J',
                equation: 'KE = (1/2)mv²'
            },
            'potential-energy': {
                variables: [
                    { name: 'm', label: 'Mass (m)', unit: 'kg' },
                    { name: 'g', label: 'Gravity (g)', unit: 'm/s²', default: 9.8 },
                    { name: 'h', label: 'Height (h)', unit: 'm' }
                ],
                solve: (values) => {
                    return values.m * values.g * values.h;
                },
                unit: 'J',
                equation: 'PE = mgh'
            },
            'ohms-law': {
                variables: [
                    { name: 'i', label: 'Current (I)', unit: 'A' },
                    { name: 'r', label: 'Resistance (R)', unit: 'Ω' }
                ],
                solve: (values) => {
                    return values.i * values.r;
                },
                unit: 'V',
                equation: 'V = IR'
            },
            'power': {
                variables: [
                    { name: 'v', label: 'Voltage (V)', unit: 'V' },
                    { name: 'i', label: 'Current (I)', unit: 'A' }
                ],
                solve: (values) => {
                    return values.v * values.i;
                },
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
            
            // Get input values, handling negative signs
            let allInputsFilled = true;
            formula.variables.forEach(variable => {
                const input = document.getElementById(variable.name);
                
                // Check if the input value is not empty
                if (input.value.trim() !== "") {
                    // Parse the value and handle negative signs
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
            
            // Only solve if all inputs are filled and valid
            if (allInputsFilled) {
                const result = formula.solve(values);
                physicsAnswer.textContent = result.toFixed(2);
                physicsUnit.textContent = formula.unit;
                physicsResult.style.display = 'block';
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
                units: ['meter', 'centimeter', 'kilometer', 'inch', 'foot', 'yard', 'mile'],
                convertTo: (value, from, to) => {
                    // Convert to meters first
                    let meters;
                    switch(from) {
                        case 'meter': meters = value; break;
                        case 'centimeter': meters = value / 100; break;
                        case 'kilometer': meters = value * 1000; break;
                        case 'inch': meters = value * 0.0254; break;
                        case 'foot': meters = value * 0.3048; break;
                        case 'yard': meters = value * 0.9144; break;
                        case 'mile': meters = value * 1609.34; break;
                    }
                    
                    // Convert from meters to target unit
                    switch(to) {
                        case 'meter': return meters;
                        case 'centimeter': return meters * 100;
                        case 'kilometer': return meters / 1000;
                        case 'inch': return meters / 0.0254;
                        case 'foot': return meters / 0.3048;
                        case 'yard': return meters / 0.9144;
                        case 'mile': return meters / 1609.34;
                    }
                }
            },
            'mass': {
                units: ['kilogram', 'gram', 'milligram', 'pound', 'ounce', 'ton'],
                convertTo: (value, from, to) => {
                    // Convert to grams first
                    let grams;
                    switch(from) {
                        case 'kilogram': grams = value * 1000; break;
                        case 'gram': grams = value; break;
                        case 'milligram': grams = value / 1000; break;
                        case 'pound': grams = value * 453.592; break;
                        case 'ounce': grams = value * 28.3495; break;
                        case 'ton': grams = value * 1000000; break;
                    }
                    
                    // Convert from grams to target unit
                    switch(to) {
                        case 'kilogram': return grams / 1000;
                        case 'gram': return grams;
                        case 'milligram': return grams * 1000;
                        case 'pound': return grams / 453.592;
                        case 'ounce': return grams / 28.3495;
                        case 'ton': return grams / 1000000;
                    }
                }
            },
            'temperature': {
                units: ['celsius', 'fahrenheit', 'kelvin'],
                convertTo: (value, from, to) => {
                    // Convert to celsius first
                    let celsius;
                    switch(from) {
                        case 'celsius': celsius = value; break;
                        case 'fahrenheit': celsius = (value - 32) * (5/9); break;
                        case 'kelvin': celsius = value - 273.15; break;
                    }
                    
                    // Convert from celsius to target unit
                    switch(to) {
                        case 'celsius': return celsius;
                        case 'fahrenheit': return celsius * (9/5) + 32;
                        case 'kelvin': return celsius + 273.15;
                    }
                }
            },
            'time': {
                units: ['second', 'minute', 'hour', 'day', 'week', 'month', 'year'],
                convertTo: (value, from, to) => {
                    // Convert to seconds first
                    let seconds;
                    switch(from) {
                        case 'second': seconds = value; break;
                        case 'minute': seconds = value * 60; break;
                        case 'hour': seconds = value * 3600; break;
                        case 'day': seconds = value * 86400; break;
                        case 'week': seconds = value * 604800; break;
                        case 'month': seconds = value * 2592000; break;
                        case 'year': seconds = value * 31536000; break;
                    }
                    
                    // Convert from seconds to target unit
                    switch(to) {
                        case 'second': return seconds;
                        case 'minute': return seconds / 60;
                        case 'hour': return seconds / 3600;
                        case 'day': return seconds / 86400;
                        case 'week': return seconds / 604800;
                        case 'month': return seconds / 2592000;
                        case 'year': return seconds / 31536000;
                    }
                }
            },
            'volume': {
                units: ['liter', 'milliliter', 'cubic_meter', 'gallon', 'quart', 'pint', 'cup'],
                convertTo: (value, from, to) => {
                    // Convert to milliliters first
                    let ml;
                    switch(from) {
                        case 'liter': ml = value * 1000; break;
                        case 'milliliter': ml = value; break;
                        case 'cubic_meter': ml = value * 1000000; break;
                        case 'gallon': ml = value * 3785.41; break;
                        case 'quart': ml = value * 946.353; break;
                        case 'pint': ml = value * 473.176; break;
                        case 'cup': ml = value * 236.588; break;
                    }
                    
                    // Convert from milliliters to target unit
                    switch(to) {
                        case 'liter': return ml / 1000;
                        case 'milliliter': return ml;
                        case 'cubic_meter': return ml / 1000000;
                        case 'gallon': return ml / 3785.41;
                        case 'quart': return ml / 946.353;
                        case 'pint': return ml / 473.176;
                        case 'cup': return ml / 236.588;
                    }
                }
            }
        };

        // Conversion logic
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
        
        convertBtn.addEventListener('click', () => {
            const type = conversionType.value;
            const from = fromUnit.value;
            const to = toUnit.value;
            const value = parseFloat(valueInput.value);
            
            if (type && from && to && !isNaN(value)) {
                const result = conversions[type].convertTo(value, from, to);
                convertedValue.textContent = result.toFixed(4);
                conversionEquation.textContent = `${value} ${from} = ${result.toFixed(4)} ${to}`;
                conversionResult.style.display = 'block';
            }
        });

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
                
                // Set default values
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
                const result = conversions[type].convertTo(value, from, to);
                convertedValue.textContent = result.toFixed(4);
                conversionEquation.textContent = `${value} ${from} = ${result.toFixed(4)} ${to}`;
                conversionResult.style.display = 'block';
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
        
        // Initialize flashcards from localStorage
        let flashcards = JSON.parse(localStorage.getItem('studyBuddyFlashcards')) || [];
        let currentFlashcardIndex = 0;
        
        // Display flashcards
        function renderFlashcards() {
            flashcardsContainer.innerHTML = '';
            
            if (flashcards.length > 0) {
                const flashcard = document.createElement('div');
                flashcard.classList.add('flashcard');
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
                flashcardsControls.style.display = 'flex';
                
                // Add click event to flip the card
                flashcard.addEventListener('click', () => {
                    flashcard.classList.toggle('flipped');
                });
            } else {
                flashcardsControls.style.display = 'none';
            }
        }
        
        // Save flashcards to localStorage
        function saveFlashcards() {
            localStorage.setItem('studyBuddyFlashcards', JSON.stringify(flashcards));
        }
        
        // Add flashcard event
        addFlashcardBtn.addEventListener('click', () => {
            const front = flashcardFront.value.trim();
            const back = flashcardBack.value.trim();
            
            if (front && back) {
                flashcards.push({ front, back });
                
                flashcardFront.value = '';
                flashcardBack.value = '';
                
                currentFlashcardIndex = flashcards.length - 1;
                renderFlashcards();
                saveFlashcards();
            }
        });
        
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
        renderFlashcards();