#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

// HTTP response helper functions
void send_response_header(bool is_json) {
    printf("Content-Type: %s\r\n\r\n", is_json ? "application/json" : "text/plain");
}

void parse_json_body(char* post_data, char* params[][2], int* num_params);
void sanitize_input(const char* input, char* output, size_t output_size);
bool is_valid_unit(const char* type, const char* unit);

// Parse form data from query string
void parse_query_string(char* query, char* params[][2], int* num_params) {
    *num_params = 0;
    if (!query || strlen(query) == 0) {
        return;
    }
    
    // Make a copy of the query string since strtok modifies it
    char* query_copy = strdup(query);
    if (!query_copy) {
        return; // Memory allocation failed
    }
    
    char* token = strtok(query_copy, "&");
    
    while (token != NULL && *num_params < 10) {
        char* equals = strchr(token, '=');
        if (equals) {
            *equals = '\0';
            params[*num_params][0] = strdup(token);
            params[*num_params][1] = strdup(equals + 1);
            if (params[*num_params][0] && params[*num_params][1]) {
                (*num_params)++;
            }
        }
        token = strtok(NULL, "&");
    }
    
    free(query_copy);
}

// Get parameter value by name
const char* get_param(char* params[][2], int num_params, const char* name) {
    for (int i = 0; i < num_params; i++) {
        if (params[i][0] && strcmp(params[i][0], name) == 0) {
            return params[i][1];
        }
    }
    return NULL;
}

// Helper function to URL decode a string
void url_decode(char* dst, const char* src) {
    char a, b;
    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && 
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a')
                a -= 'a'-'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a'-'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = 16*a+b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

// Todo List Functionality
#define MAX_TASKS 100
#define MAX_TITLE_LENGTH 100

typedef struct {
    int id;
    char title[MAX_TITLE_LENGTH];
    char date[20]; // YYYY-MM-DD format
    bool completed;
} Task;

// In-memory storage for tasks (in a real application, this would be in a database)
Task tasks[MAX_TASKS];
int next_task_id = 1;
int task_count = 0;

// Function to add a new task
void add_task(const char* title, const char* date, bool completed) {
    if (task_count < MAX_TASKS) {
        Task new_task;
        new_task.id = next_task_id++;
        
        // Copy title with length limit to prevent buffer overflow
        strncpy(new_task.title, title, MAX_TITLE_LENGTH - 1);
        new_task.title[MAX_TITLE_LENGTH - 1] = '\0'; // Ensure null termination
        
        // Copy date if provided
        if (date && strlen(date) > 0) {
            strncpy(new_task.date, date, sizeof(new_task.date) - 1);
            new_task.date[sizeof(new_task.date) - 1] = '\0'; // Ensure null termination
        } else {
            new_task.date[0] = '\0'; // Empty string
        }
        
        new_task.completed = completed;
        
        tasks[task_count++] = new_task;
    }
}

// Function to get a task by ID
Task* get_task_by_id(int id) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            return &tasks[i];
        }
    }
    return NULL;
}

// Function to update a task
bool update_task(int id, const char* title, const char* date, bool completed) {
    Task* task = get_task_by_id(id);
    
    if (task) {
        if (title) {
            strncpy(task->title, title, MAX_TITLE_LENGTH - 1);
            task->title[MAX_TITLE_LENGTH - 1] = '\0';
        }
        
        if (date) {
            strncpy(task->date, date, sizeof(task->date) - 1);
            task->date[sizeof(task->date) - 1] = '\0';
        }
        
        task->completed = completed;
        return true;
    }
    
    return false;
}

// Function to delete a task
bool delete_task(int id) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            // Move all tasks after this one forward
            for (int j = i; j < task_count - 1; j++) {
                tasks[j] = tasks[j + 1];
            }
            task_count--;
            return true;
        }
    }
    return false;
}

// Handle getting all tasks
void handle_get_tasks() {
    send_response_header(true);
    
    printf("[");
    for (int i = 0; i < task_count; i++) {
        printf("{\"id\": %d, \"title\": \"%s\", \"date\": \"%s\", \"completed\": %s}",
               tasks[i].id,
               tasks[i].title,
               tasks[i].date,
               tasks[i].completed ? "true" : "false");
        
        if (i < task_count - 1) {
            printf(",");
        }
    }
    printf("]");
}

// Handle creating a new task
void handle_create_task(char* post_data) {
    char* params[10][2];
    int num_params = 0;
    
    // Parse JSON body
    parse_json_body(post_data, params, &num_params);
    
    const char* title = get_param(params, num_params, "title");
    const char* date = get_param(params, num_params, "date");
    const char* completed_str = get_param(params, num_params, "completed");
    
    if (!title || strlen(title) == 0) {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Title is required\"}");
        return;
    }
    
    // Validate date format if provided (YYYY-MM-DD)
    if (date && strlen(date) > 0) {
        if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
            send_response_header(true);
            printf("{\"success\": false, \"error\": \"Invalid date format. Use YYYY-MM-DD\"}");
            return;
        }
        
        // Check if digits are in the correct positions
        for (int i = 0; i < 10; i++) {
            if ((i == 4 || i == 7) && date[i] != '-') {
                send_response_header(true);
                printf("{\"success\": false, \"error\": \"Invalid date format. Use YYYY-MM-DD\"}");
                return;
            } else if (i != 4 && i != 7 && !isdigit(date[i])) {
                send_response_header(true);
                printf("{\"success\": false, \"error\": \"Invalid date format. Use YYYY-MM-DD\"}");
                return;
            }
        }
        
        // Validate month and day values
        int year = (date[0] - '0') * 1000 + (date[1] - '0') * 100 + (date[2] - '0') * 10 + (date[3] - '0');
        int month = (date[5] - '0') * 10 + (date[6] - '0');
        int day = (date[8] - '0') * 10 + (date[9] - '0');
        
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            send_response_header(true);
            printf("{\"success\": false, \"error\": \"Invalid date values\"}");
            return;
        }
        
        // Additional validation for months with fewer than 31 days
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
            send_response_header(true);
            printf("{\"success\": false, \"error\": \"Invalid day for selected month\"}");
            return;
        }
        
        // February validation including leap years
        if (month == 2) {
            bool is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            int max_days = is_leap_year ? 29 : 28;
            
            if (day > max_days) {
                send_response_header(true);
                printf("{\"success\": false, \"error\": \"Invalid day for February\"}");
                return;
            }
        }
    }
    
    bool completed = completed_str && strcmp(completed_str, "true") == 0;
    
    // Decode URL encoded data
    char decoded_title[MAX_TITLE_LENGTH];
    url_decode(decoded_title, title);
    
    // Sanitize title input to prevent XSS or other injection issues
    // Simple sanitization - remove HTML tags (a more robust solution would use a library)
    char sanitized_title[MAX_TITLE_LENGTH];
    sanitize_input(decoded_title, sanitized_title, MAX_TITLE_LENGTH);
    
    add_task(sanitized_title, date, completed);
    
    send_response_header(true);
    printf("{\"success\": true, \"id\": %d}", next_task_id - 1);
    
    // Free allocated memory for parameters
    for (int i = 0; i < num_params; i++) {
        free(params[i][0]);
        free(params[i][1]);
    }
}

// Helper function to sanitize input 
void sanitize_input(const char* input, char* output, size_t output_size) {
    size_t i = 0, j = 0;
    bool in_tag = false;
    
    // Simple sanitization removing HTML tags
    while (input[i] != '\0' && j < output_size - 1) {
        if (input[i] == '<') {
            in_tag = true;
        } else if (input[i] == '>') {
            in_tag = false;
        } else if (!in_tag) {
            output[j++] = input[i];
        }
        i++;
    }
    output[j] = '\0';
}

// Handle updating a task
void handle_update_task(char* path_info, char* post_data) {
    // Extract task ID from path
    int task_id = 0;
    sscanf(path_info, "/tasks/%d", &task_id);
    
    if (task_id <= 0) {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Invalid task ID\"}");
        return;
    }
    
    char* params[10][2];
    int num_params = 0;
    
    // Parse JSON body
    parse_json_body(post_data, params, &num_params);
    
    const char* title = get_param(params, num_params, "title");
    const char* date = get_param(params, num_params, "date");
    const char* completed_str = get_param(params, num_params, "completed");
    
    bool completed = completed_str && strcmp(completed_str, "true") == 0;
    
    char decoded_title[MAX_TITLE_LENGTH];
    if (title) {
        url_decode(decoded_title, title);
    }
    
    bool success = update_task(task_id, title ? decoded_title : NULL, date, completed);
    
    send_response_header(true);
    if (success) {
        printf("{\"success\": true}");
    } else {
        printf("{\"success\": false, \"error\": \"Task not found\"}");
    }
    
    // Free allocated memory for parameters
    for (int i = 0; i < num_params; i++) {
        free(params[i][0]);
        free(params[i][1]);
    }
}

// Handle deleting a task
void handle_delete_task(char* path_info) {
    // Extract task ID from path
    int task_id = 0;
    sscanf(path_info, "/tasks/%d", &task_id);
    
    if (task_id <= 0) {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Invalid task ID\"}");
        return;
    }
    
    bool success = delete_task(task_id);
    
    send_response_header(true);
    if (success) {
        printf("{\"success\": true}");
    } else {
        printf("{\"success\": false, \"error\": \"Task not found\"}");
    }
}

// Main handler for todo list API
void handle_tasks(char* request_method, char* path_info, char* query_string, char* post_data) {
    // GET all tasks
    if (strcmp(request_method, "GET") == 0 && (strcmp(path_info, "/tasks") == 0)) {
        handle_get_tasks();
    }
    // POST - create new task
    else if (strcmp(request_method, "POST") == 0 && (strcmp(path_info, "/tasks") == 0)) {
        handle_create_task(post_data);
    }
    // PUT - update task
    else if (strcmp(request_method, "PUT") == 0 && strncmp(path_info, "/tasks/", 7) == 0) {
        handle_update_task(path_info, post_data);
    }
    // DELETE - delete task
    else if (strcmp(request_method, "DELETE") == 0 && strncmp(path_info, "/tasks/", 7) == 0) {
        handle_delete_task(path_info);
    }
    else {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Invalid tasks endpoint or method\"}");
    }
}

// Parse JSON from POST data
void parse_json_body(char* post_data, char* params[][2], int* num_params) {
    *num_params = 0;
    if (!post_data || strlen(post_data) == 0) {
        return;
    }
    
    // Very simple JSON parser - assumes one level of nesting
    // A proper implementation would use a JSON library
    char* key_start = NULL;
    char* value_start = NULL;
    bool in_string = false;
    bool in_key = false;
    bool in_value = false;
    char current_key[128] = {0};
    char current_value[512] = {0};
    
    for (char* p = post_data; *p; p++) {
        if (*p == '"') {
            if (!in_string) {
                in_string = true;
                if (!in_key && !in_value) {
                    in_key = true;
                    key_start = p + 1;
                } else if (!in_value && in_key) {
                    // Finished parsing key
                    in_key = false;
                } else if (!in_value) {
                    in_value = true;
                    value_start = p + 1;
                } else {
                    // Finished parsing value
                    in_value = false;
                    in_string = false;
                    
                    // Store the key-value pair
                    if (*num_params < 10) {
                        params[*num_params][0] = strdup(current_key);
                        params[*num_params][1] = strdup(current_value);
                        if (params[*num_params][0] && params[*num_params][1]) {
                            (*num_params)++;
                        }
                        
                        // Reset for next pair
                        memset(current_key, 0, sizeof(current_key));
                        memset(current_value, 0, sizeof(current_value));
                    }
                }
            } else {
                in_string = false;
            }
        } else if (in_string) {
            if (in_key && key_start) {
                size_t len = p - key_start;
                if (len < sizeof(current_key) - 1) {
                    strncpy(current_key, key_start, len);
                    current_key[len] = '\0';
                }
            } else if (in_value && value_start) {
                size_t len = p - value_start;
                if (len < sizeof(current_value) - 1) {
                    strncpy(current_value, value_start, len);
                    current_value[len] = '\0';
                }
            }
        }
    }
}

// Grade Calculator Functions

// Get the letter grade from a percentage
const char* get_letter_grade(double percentage) {
    if (percentage >= 90) return "A";
    else if (percentage >= 80) return "B";
    else if (percentage >= 70) return "C";
    else if (percentage >= 60) return "D";
    else return "F";
}

// Handle grade calculation
void handle_grade_calculation(char* query) {
    char* params[20][2]; // Increased to handle multiple grades
    int num_params = 0;
    parse_query_string(query, params, &num_params);
    
    const char* grades_str = get_param(params, num_params, "grades");
    
    if (!grades_str) {
        send_response_header(true);
        printf("{\"error\": \"Missing grades parameter\"}");
        return;
    }
    
    // Parse the comma-separated list of grades
    char* grades_copy = strdup(grades_str);
    if (!grades_copy) {
        send_response_header(true);
        printf("{\"error\": \"Memory allocation failed\"}");
        return;
    }
    
    char* token = strtok(grades_copy, ",");
    double total_grade = 0.0;
    int valid_grades = 0;
    
    while (token != NULL) {
        double grade = atof(token);
        if (grade >= 0 && grade <= 100) {
            total_grade += grade;
            valid_grades++;
        }
        token = strtok(NULL, ",");
    }
    
    free(grades_copy);
    
    if (valid_grades == 0) {
        send_response_header(true);
        printf("{\"error\": \"No valid grades provided (values must be between 0 and 100)\"}");
        return;
    }
    
    double percentage = total_grade / valid_grades;
    const char* letter = get_letter_grade(percentage);
    
    send_response_header(true);
    printf("{\"average\": %.2f, \"letter\": \"%s\"}", percentage, letter);
}

// SGPA Calculator Functions

// Convert grade character to points based on the provided conversion table
double convert_grade_to_points(const char* grade) {
    if (strcmp(grade, "O") == 0) return 10.0;
    else if (strcmp(grade, "A+") == 0) return 9.0;
    else if (strcmp(grade, "A") == 0) return 8.0;
    else if (strcmp(grade, "B+") == 0) return 7.0;
    else if (strcmp(grade, "B") == 0) return 6.0;
    else if (strcmp(grade, "C") == 0) return 5.0;
    else if (strcmp(grade, "U") == 0) return 0.0;
    else return -1.0; // Invalid grade
}

// Calculate SGPA based on credits and grades
double calculate_sgpa(int num_subjects, double* credits, double* grade_points) {
    double total_grade_points = 0.0;
    double total_credits = 0.0;
    
    for (int i = 0; i < num_subjects; i++) {
        total_grade_points += credits[i] * grade_points[i];
        total_credits += credits[i];
    }
    
    return (total_credits > 0) ? (total_grade_points / total_credits) : 0.0;
}

// Handle SGPA calculation
void handle_sgpa_calculation(char* query) {
    char* params[50][2]; // Increased to handle multiple subjects with longer param names
    int num_params = 0;
    parse_query_string(query, params, &num_params);
    
    const char* num_subjects_str = get_param(params, num_params, "num_subjects");
    
    if (!num_subjects_str) {
        send_response_header(true);
        printf("{\"error\": \"Missing number of subjects\"}");
        return;
    }
    
    int num_subjects = atoi(num_subjects_str);
    
    if (num_subjects <= 0 || num_subjects > 10) {
        send_response_header(true);
        printf("{\"error\": \"Invalid number of subjects (must be between 1 and 10)\"}");
        return;
    }
    
    double credits[10] = {0};
    double grade_points[10] = {0};
    bool valid_inputs = true;
    
    for (int i = 1; i <= num_subjects; i++) {
        char credit_param[20];
        char grade_param[20];
        
        // Match the parameter names from the frontend
        sprintf(credit_param, "creditPoints_%d", i);
        sprintf(grade_param, "gradeCharacter_%d", i);
        
        const char* credit_str = get_param(params, num_params, credit_param);
        const char* grade_str = get_param(params, num_params, grade_param);
        
        if (!credit_str || !grade_str) {
            valid_inputs = false;
            break;
        }
        
        double credit = atof(credit_str);
        double grade_point = convert_grade_to_points(grade_str);
        
        if (credit <= 0 || grade_point < 0) {
            valid_inputs = false;
            break;
        }
        
        credits[i-1] = credit;
        grade_points[i-1] = grade_point;
    }
    
    if (!valid_inputs) {
        send_response_header(true);
        printf("{\"error\": \"Invalid or missing credit points or grades\"}");
        return;
    }
    
    double sgpa = calculate_sgpa(num_subjects, credits, grade_points);
    
    send_response_header(true);
    printf("{\"sgpa\": %.3f}", sgpa);
}

// Physics solver functions
double velocity_formula(double u, double a, double t) {
    return u + a * t;
}

double distance_formula(double u, double a, double t) {
    return u * t + 0.5 * a * pow(t, 2);
}

double velocity_squared_formula(double u, double a, double s) {
    double result = pow(u, 2) + 2 * a * s;
    return result >= 0 ? sqrt(result) : -1; // Return -1 for complex values
}

double force_formula(double m, double a) {
    return m * a;
}

double kinetic_energy_formula(double m, double v) {
    return 0.5 * m * pow(v, 2);
}

double potential_energy_formula(double m, double g, double h) {
    return m * g * h;
}

double ohms_law_formula(double i, double r) {
    return i * r;
}

double power_formula(double v, double i) {
    return v * i;
}

// Solve physics problem
void handle_physics_solver(char* query) {
    char* params[10][2];
    int num_params = 0;
    parse_query_string(query, params, &num_params);
    
    const char* formula = get_param(params, num_params, "formula");
    
    if (!formula) {
        send_response_header(true);
        printf("{\"error\": \"Missing formula parameter\"}");
        return;
    }
    
    // Validate formula type first
    if (!(
        strcmp(formula, "velocity") == 0 || 
        strcmp(formula, "distance") == 0 || 
        strcmp(formula, "velocity-squared") == 0 || 
        strcmp(formula, "force") == 0 || 
        strcmp(formula, "kinetic-energy") == 0 || 
        strcmp(formula, "potential-energy") == 0 || 
        strcmp(formula, "ohms-law") == 0 || 
        strcmp(formula, "power") == 0
    )) {
        send_response_header(true);
        printf("{\"error\": \"Invalid formula type\"}");
        return;
    }
    
    double result = -1;
    char* endptr;
    
    if (strcmp(formula, "velocity") == 0) {
        const char* u_str = get_param(params, num_params, "u");
        const char* a_str = get_param(params, num_params, "a");
        const char* t_str = get_param(params, num_params, "t");
        
        if (!u_str || !a_str || !t_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for velocity formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double u = strtod(u_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid initial velocity value\"}");
            return;
        }
        
        double a = strtod(a_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid acceleration value\"}");
            return;
        }
        
        double t = strtod(t_str, &endptr);
        if (*endptr != '\0' || t < 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid time value (must be non-negative)\"}");
            return;
        }
        
        result = velocity_formula(u, a, t);
    } 
    else if (strcmp(formula, "distance") == 0) {
        const char* u_str = get_param(params, num_params, "u");
        const char* a_str = get_param(params, num_params, "a");
        const char* t_str = get_param(params, num_params, "t");
        
        if (!u_str || !a_str || !t_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for distance formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double u = strtod(u_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid initial velocity value\"}");
            return;
        }
        
        double a = strtod(a_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid acceleration value\"}");
            return;
        }
        
        double t = strtod(t_str, &endptr);
        if (*endptr != '\0' || t < 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid time value (must be non-negative)\"}");
            return;
        }
        
        result = distance_formula(u, a, t);
    }
    else if (strcmp(formula, "velocity-squared") == 0) {
        const char* u_str = get_param(params, num_params, "u");
        const char* a_str = get_param(params, num_params, "a");
        const char* s_str = get_param(params, num_params, "s");
        
        if (!u_str || !a_str || !s_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for velocity-squared formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double u = strtod(u_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid initial velocity value\"}");
            return;
        }
        
        double a = strtod(a_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid acceleration value\"}");
            return;
        }
        
        double s = strtod(s_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid distance value\"}");
            return;
        }
        
        result = velocity_squared_formula(u, a, s);
    }
    else if (strcmp(formula, "force") == 0) {
        const char* m_str = get_param(params, num_params, "m");
        const char* a_str = get_param(params, num_params, "a");
        
        if (!m_str || !a_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for force formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double m = strtod(m_str, &endptr);
        if (*endptr != '\0' || m <= 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid mass value (must be positive)\"}");
            return;
        }
        
        double a = strtod(a_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid acceleration value\"}");
            return;
        }
        
        result = force_formula(m, a);
    }
    else if (strcmp(formula, "kinetic-energy") == 0) {
        const char* m_str = get_param(params, num_params, "m");
        const char* v_str = get_param(params, num_params, "v");
        
        if (!m_str || !v_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for kinetic-energy formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double m = strtod(m_str, &endptr);
        if (*endptr != '\0' || m <= 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid mass value (must be positive)\"}");
            return;
        }
        
        double v = strtod(v_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid velocity value\"}");
            return;
        }
        
        result = kinetic_energy_formula(m, v);
    }
    else if (strcmp(formula, "potential-energy") == 0) {
        const char* m_str = get_param(params, num_params, "m");
        const char* g_str = get_param(params, num_params, "g");
        const char* h_str = get_param(params, num_params, "h");
        
        if (!m_str || !h_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for potential-energy formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double m = strtod(m_str, &endptr);
        if (*endptr != '\0' || m <= 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid mass value (must be positive)\"}");
            return;
        }
        
        double g;
        if (g_str) {
            g = strtod(g_str, &endptr);
            if (*endptr != '\0' || g <= 0) {
                send_response_header(true);
                printf("{\"error\": \"Invalid gravitational acceleration value (must be positive)\"}");
                return;
            }
        } else {
            g = 9.8; // Default value
        }
        
        double h = strtod(h_str, &endptr);
        if (*endptr != '\0' || h < 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid height value (must be non-negative)\"}");
            return;
        }
        
        result = potential_energy_formula(m, g, h);
    }
    else if (strcmp(formula, "ohms-law") == 0) {
        const char* i_str = get_param(params, num_params, "i");
        const char* r_str = get_param(params, num_params, "r");
        
        if (!i_str || !r_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for ohms-law formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double i = strtod(i_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid current value\"}");
            return;
        }
        
        double r = strtod(r_str, &endptr);
        if (*endptr != '\0' || r <= 0) {
            send_response_header(true);
            printf("{\"error\": \"Invalid resistance value (must be positive)\"}");
            return;
        }
        
        result = ohms_law_formula(i, r);
    }
    else if (strcmp(formula, "power") == 0) {
        const char* v_str = get_param(params, num_params, "v");
        const char* i_str = get_param(params, num_params, "i");
        
        if (!v_str || !i_str) {
            send_response_header(true);
            printf("{\"error\": \"Missing required parameters for power formula\"}");
            return;
        }
        
        // Validate numeric inputs
        double v = strtod(v_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid voltage value\"}");
            return;
        }
        
        double i = strtod(i_str, &endptr);
        if (*endptr != '\0') {
            send_response_header(true);
            printf("{\"error\": \"Invalid current value\"}");
            return;
        }
        
        result = power_formula(v, i);
    }
    
    send_response_header(true);
    if (result != -1) {
        printf("{\"result\": %.4f}", result);
    } else {
        printf("{\"error\": \"Invalid parameters or formula\"}");
    }
}

// Unit conversion functions

// Length conversions (base unit: meters)
double convert_length(double value, const char* from, const char* to) {
    // Convert to meters first
    double meters = 0;
    
    if (strcmp(from, "meter") == 0) meters = value;
    else if (strcmp(from, "centimeter") == 0) meters = value / 100;
    else if (strcmp(from, "kilometer") == 0) meters = value * 1000;
    else if (strcmp(from, "inch") == 0) meters = value * 0.0254;
    else if (strcmp(from, "foot") == 0) meters = value * 0.3048;
    else if (strcmp(from, "yard") == 0) meters = value * 0.9144;
    else if (strcmp(from, "mile") == 0) meters = value * 1609.34;
    
    // Convert from meters to target unit
    if (strcmp(to, "meter") == 0) return meters;
    else if (strcmp(to, "centimeter") == 0) return meters * 100;
    else if (strcmp(to, "kilometer") == 0) return meters / 1000;
    else if (strcmp(to, "inch") == 0) return meters / 0.0254;
    else if (strcmp(to, "foot") == 0) return meters / 0.3048;
    else if (strcmp(to, "yard") == 0) return meters / 0.9144;
    else if (strcmp(to, "mile") == 0) return meters / 1609.34;
    
    return -1; // Error case
}

// Mass conversions (base unit: grams)
double convert_mass(double value, const char* from, const char* to) {
    // Convert to grams first
    double grams = 0;
    
    if (strcmp(from, "kilogram") == 0) grams = value * 1000;
    else if (strcmp(from, "gram") == 0) grams = value;
    else if (strcmp(from, "milligram") == 0) grams = value / 1000;
    else if (strcmp(from, "pound") == 0) grams = value * 453.592;
    else if (strcmp(from, "ounce") == 0) grams = value * 28.3495;
    else if (strcmp(from, "ton") == 0) grams = value * 1000000;
    
    // Convert from grams to target unit
    if (strcmp(to, "kilogram") == 0) return grams / 1000;
    else if (strcmp(to, "gram") == 0) return grams;
    else if (strcmp(to, "milligram") == 0) return grams * 1000;
    else if (strcmp(to, "pound") == 0) return grams / 453.592;
    else if (strcmp(to, "ounce") == 0) return grams / 28.3495;
    else if (strcmp(to, "ton") == 0) return grams / 1000000;
    
    return -1; // Error case
}

// Temperature conversions
double convert_temperature(double value, const char* from, const char* to) {
    // Convert to celsius first
    double celsius = 0;
    
    if (strcmp(from, "celsius") == 0) celsius = value;
    else if (strcmp(from, "fahrenheit") == 0) celsius = (value - 32) * (5.0/9.0);
    else if (strcmp(from, "kelvin") == 0) celsius = value - 273.15;
    
    // Convert from celsius to target unit
    if (strcmp(to, "celsius") == 0) return celsius;
    else if (strcmp(to, "fahrenheit") == 0) return celsius * (9.0/5.0) + 32;
    else if (strcmp(to, "kelvin") == 0) return celsius + 273.15;
    
    return -1; // Error case
}

// Time conversions (base unit: seconds)
double convert_time(double value, const char* from, const char* to) {
    // Convert to seconds first
    double seconds = 0;
    
    if (strcmp(from, "second") == 0) seconds = value;
    else if (strcmp(from, "minute") == 0) seconds = value * 60;
    else if (strcmp(from, "hour") == 0) seconds = value * 3600;
    else if (strcmp(from, "day") == 0) seconds = value * 86400;
    else if (strcmp(from, "week") == 0) seconds = value * 604800;
    else if (strcmp(from, "month") == 0) seconds = value * 2592000; // Using 30 days
    else if (strcmp(from, "year") == 0) seconds = value * 31536000; // Using 365 days
    
    // Convert from seconds to target unit
    if (strcmp(to, "second") == 0) return seconds;
    else if (strcmp(to, "minute") == 0) return seconds / 60;
    else if (strcmp(to, "hour") == 0) return seconds / 3600;
    else if (strcmp(to, "day") == 0) return seconds / 86400;
    else if (strcmp(to, "week") == 0) return seconds / 604800;
    else if (strcmp(to, "month") == 0) return seconds / 2592000;
    else if (strcmp(to, "year") == 0) return seconds / 31536000;
    
    return -1; // Error case
}

// Volume conversions (base unit: milliliters)
double convert_volume(double value, const char* from, const char* to) {
    // Convert to milliliters first
    double ml = 0;
    
    if (strcmp(from, "liter") == 0) ml = value * 1000;
    else if (strcmp(from, "milliliter") == 0) ml = value;
    else if (strcmp(from, "cubic_meter") == 0) ml = value * 1000000;
    else if (strcmp(from, "gallon") == 0) ml = value * 3785.41;
    else if (strcmp(from, "quart") == 0) ml = value * 946.353;
    else if (strcmp(from, "pint") == 0) ml = value * 473.176;
    else if (strcmp(from, "cup") == 0) ml = value * 236.588;
    
    // Convert from milliliters to target unit
    if (strcmp(to, "liter") == 0) return ml / 1000;
    else if (strcmp(to, "milliliter") == 0) return ml;
    else if (strcmp(to, "cubic_meter") == 0) return ml / 1000000;
    else if (strcmp(to, "gallon") == 0) return ml / 3785.41;
    else if (strcmp(to, "quart") == 0) return ml / 946.353;
    else if (strcmp(to, "pint") == 0) return ml / 473.176;
    else if (strcmp(to, "cup") == 0) return ml / 236.588;
    
    return -1; // Error case
}

// Handle unit conversion
void handle_unit_conversion(char* query) {
    char* params[10][2];
    int num_params = 0;
    parse_query_string(query, params, &num_params);
    
    const char* type = get_param(params, num_params, "type");
    const char* from = get_param(params, num_params, "from");
    const char* to = get_param(params, num_params, "to");
    const char* value_str = get_param(params, num_params, "value");
    
    if (!type || !from || !to || !value_str) {
        send_response_header(true);
        printf("{\"error\": \"Missing required parameters\"}");
        return;
    }
    
    // Validate that value is a valid number
    char* endptr;
    double value = strtod(value_str, &endptr);
    
    // Check if conversion was successful and the entire string was used
    if (*endptr != '\0' || value_str == endptr) {
        send_response_header(true);
        printf("{\"error\": \"Invalid numeric value\"}");
        return;
    }
    
    // Check for negative values where they don't make sense
    if (value < 0 && (
        strcmp(type, "length") == 0 || 
        strcmp(type, "mass") == 0 || 
        strcmp(type, "time") == 0 || 
        strcmp(type, "volume") == 0)) {
        send_response_header(true);
        printf("{\"error\": \"Negative values not allowed for this conversion type\"}");
        return;
    }
    
    // Validate unit types are valid for the selected conversion type
    if (!is_valid_unit(type, from) || !is_valid_unit(type, to)) {
        send_response_header(true);
        printf("{\"error\": \"Invalid unit for selected conversion type\"}");
        return;
    }
    
    double result = -1;
    
    if (strcmp(type, "length") == 0) {
        result = convert_length(value, from, to);
    }
    else if (strcmp(type, "mass") == 0) {
        result = convert_mass(value, from, to);
    }
    else if (strcmp(type, "temperature") == 0) {
        result = convert_temperature(value, from, to);
    }
    else if (strcmp(type, "time") == 0) {
        result = convert_time(value, from, to);
    }
    else if (strcmp(type, "volume") == 0) {
        result = convert_volume(value, from, to);
    }
    
    send_response_header(true);
    if (result != -1) {
        printf("{\"result\": %.4f}", result);
    } else {
        printf("{\"error\": \"Invalid conversion parameters\"}");
    }
}

// Helper function to check if a unit is valid for a specific conversion type
bool is_valid_unit(const char* type, const char* unit) {
    if (strcmp(type, "length") == 0) {
        return (
            strcmp(unit, "meter") == 0 ||
            strcmp(unit, "centimeter") == 0 ||
            strcmp(unit, "kilometer") == 0 ||
            strcmp(unit, "inch") == 0 ||
            strcmp(unit, "foot") == 0 ||
            strcmp(unit, "yard") == 0 ||
            strcmp(unit, "mile") == 0
        );
    }
    else if (strcmp(type, "mass") == 0) {
        return (
            strcmp(unit, "kilogram") == 0 ||
            strcmp(unit, "gram") == 0 ||
            strcmp(unit, "milligram") == 0 ||
            strcmp(unit, "pound") == 0 ||
            strcmp(unit, "ounce") == 0 ||
            strcmp(unit, "ton") == 0
        );
    }
    else if (strcmp(type, "temperature") == 0) {
        return (
            strcmp(unit, "celsius") == 0 ||
            strcmp(unit, "fahrenheit") == 0 ||
            strcmp(unit, "kelvin") == 0
        );
    }
    else if (strcmp(type, "time") == 0) {
        return (
            strcmp(unit, "second") == 0 ||
            strcmp(unit, "minute") == 0 ||
            strcmp(unit, "hour") == 0 ||
            strcmp(unit, "day") == 0 ||
            strcmp(unit, "week") == 0 ||
            strcmp(unit, "month") == 0 ||
            strcmp(unit, "year") == 0
        );
    }
    else if (strcmp(type, "volume") == 0) {
        return (
            strcmp(unit, "liter") == 0 ||
            strcmp(unit, "milliliter") == 0 ||
            strcmp(unit, "cubic_meter") == 0 ||
            strcmp(unit, "gallon") == 0 ||
            strcmp(unit, "quart") == 0 ||
            strcmp(unit, "pint") == 0 ||
            strcmp(unit, "cup") == 0
        );
    }
    
    return false;
}

// Flashcard Functionality
#define MAX_FLASHCARDS 100
#define MAX_FLASHCARD_TEXT_LENGTH 500

typedef struct {
    int id;
    char front[MAX_FLASHCARD_TEXT_LENGTH];
    char back[MAX_FLASHCARD_TEXT_LENGTH];
} Flashcard;

// In-memory storage for flashcards (in a real application, this would be in a database)
Flashcard flashcards[MAX_FLASHCARDS];
int next_flashcard_id = 1;
int flashcard_count = 0;

// Function to add a new flashcard
void add_flashcard(const char* front, const char* back) {
    if (flashcard_count < MAX_FLASHCARDS) {
        Flashcard new_flashcard;
        new_flashcard.id = next_flashcard_id++;
        
        // Copy front text with length limit to prevent buffer overflow
        strncpy(new_flashcard.front, front, MAX_FLASHCARD_TEXT_LENGTH - 1);
        new_flashcard.front[MAX_FLASHCARD_TEXT_LENGTH - 1] = '\0'; // Ensure null termination
        
        // Copy back text with length limit to prevent buffer overflow
        strncpy(new_flashcard.back, back, MAX_FLASHCARD_TEXT_LENGTH - 1);
        new_flashcard.back[MAX_FLASHCARD_TEXT_LENGTH - 1] = '\0'; // Ensure null termination
        
        flashcards[flashcard_count++] = new_flashcard;
    }
}

// Function to get a flashcard by ID
Flashcard* get_flashcard_by_id(int id) {
    for (int i = 0; i < flashcard_count; i++) {
        if (flashcards[i].id == id) {
            return &flashcards[i];
        }
    }
    return NULL;
}

// Function to delete a flashcard
bool delete_flashcard(int id) {
    for (int i = 0; i < flashcard_count; i++) {
        if (flashcards[i].id == id) {
            // Move all flashcards after this one forward
            for (int j = i; j < flashcard_count - 1; j++) {
                flashcards[j] = flashcards[j + 1];
            }
            flashcard_count--;
            return true;
        }
    }
    return false;
}

// Handle getting all flashcards
void handle_get_flashcards() {
    send_response_header(true);
    
    printf("[");
    for (int i = 0; i < flashcard_count; i++) {
        printf("{\"id\": %d, \"front\": \"%s\", \"back\": \"%s\"}",
               flashcards[i].id,
               flashcards[i].front,
               flashcards[i].back);
        
        if (i < flashcard_count - 1) {
            printf(",");
        }
    }
    printf("]");
}

// Handle creating a new flashcard
void handle_create_flashcard(char* post_data) {
    char* params[10][2];
    int num_params = 0;
    
    // Parse JSON body
    parse_json_body(post_data, params, &num_params);
    
    const char* front = get_param(params, num_params, "front");
    const char* back = get_param(params, num_params, "back");
    
    if (!front || strlen(front) == 0 || !back || strlen(back) == 0) {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Front and back are required\"}");
        return;
    }
    
    // Decode URL encoded data
    char decoded_front[MAX_FLASHCARD_TEXT_LENGTH];
    char decoded_back[MAX_FLASHCARD_TEXT_LENGTH];
    url_decode(decoded_front, front);
    url_decode(decoded_back, back);
    
    add_flashcard(decoded_front, decoded_back);
    
    send_response_header(true);
    printf("{\"success\": true, \"id\": %d}", next_flashcard_id - 1);
    
    // Free allocated memory for parameters
    for (int i = 0; i < num_params; i++) {
        free(params[i][0]);
        free(params[i][1]);
    }
}

// Handle deleting a flashcard
void handle_delete_flashcard(char* path_info) {
    // Extract flashcard ID from path
    int flashcard_id = 0;
    sscanf(path_info, "/flashcards/%d", &flashcard_id);
    
    if (flashcard_id <= 0) {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Invalid flashcard ID\"}");
        return;
    }
    
    bool success = delete_flashcard(flashcard_id);
    
    send_response_header(true);
    if (success) {
        printf("{\"success\": true}");
    } else {
        printf("{\"success\": false, \"error\": \"Flashcard not found\"}");
    }
}

// Main handler for flashcards API
void handle_flashcards(char* request_method, char* path_info, char* query_string, char* post_data) {
    // GET all flashcards
    if (strcmp(request_method, "GET") == 0 && (strcmp(path_info, "/flashcards") == 0)) {
        handle_get_flashcards();
    }
    // POST - create new flashcard
    else if (strcmp(request_method, "POST") == 0 && (strcmp(path_info, "/flashcards") == 0)) {
        handle_create_flashcard(post_data);
    }
    // DELETE - delete flashcard
    else if (strcmp(request_method, "DELETE") == 0 && strncmp(path_info, "/flashcards/", 12) == 0) {
        handle_delete_flashcard(path_info);
    }
    else {
        send_response_header(true);
        printf("{\"success\": false, \"error\": \"Invalid flashcards endpoint or method\"}");
    }
}

int main(void) {
    // Get request method and query string from environment
    char* request_method = getenv("REQUEST_METHOD");
    char* query_string = getenv("QUERY_STRING");
    char* path_info = getenv("PATH_INFO");
    
    if (!request_method || !path_info) {
        send_response_header(true);
        printf("{\"error\": \"Invalid request - missing method or path\"}");
        return 1;
    }
    
    // Create a buffer to store parameters
    char* params[10][2];
    memset(params, 0, sizeof(params)); // Initialize to NULL
    
    // Process GET requests
    if (strcmp(request_method, "GET") == 0) {
        if (strcmp(path_info, "/physics") == 0) {
            handle_physics_solver(query_string ? query_string : "");
        }
        else if (strcmp(path_info, "/convert") == 0) {
            handle_unit_conversion(query_string ? query_string : "");
        }
        else if (strcmp(path_info, "/calculate-sgpa") == 0) {
            handle_sgpa_calculation(query_string ? query_string : "");
        }
        else if (strcmp(path_info, "/calculate-grade") == 0) {
            handle_grade_calculation(query_string ? query_string : "");
        }
        else if (strcmp(path_info, "/tasks") == 0 || strncmp(path_info, "/tasks/", 7) == 0) {
            handle_tasks(request_method, path_info, query_string, NULL);
        }
        else if (strcmp(path_info, "/flashcards") == 0 || strncmp(path_info, "/flashcards/", 12) == 0) {
            handle_flashcards(request_method, path_info, query_string, NULL);
        }
        else {
            send_response_header(true);
            printf("{\"error\": \"Unknown endpoint: %s\"}", path_info);
        }
    }
    // Process POST requests
    else if (strcmp(request_method, "POST") == 0 || 
             strcmp(request_method, "PUT") == 0 || 
             strcmp(request_method, "DELETE") == 0) {
        // Get content length
        char* content_length_str = getenv("CONTENT_LENGTH");
        int content_length = content_length_str ? atoi(content_length_str) : 0;
        
        char* post_data = NULL;
        
        if (content_length > 0) {
            post_data = malloc(content_length + 1);
            if (!post_data) {
                send_response_header(true);
                printf("{\"error\": \"Memory allocation failed\"}");
                return 1;
            }
            
            // Read POST data from stdin
            size_t bytes_read = fread(post_data, 1, content_length, stdin);
            if (bytes_read != content_length) {
                send_response_header(true);
                printf("{\"error\": \"Failed to read POST data completely (%zu/%d bytes)\"}",
                       bytes_read, content_length);
                free(post_data);
                return 1;
            }
            
            post_data[content_length] = '\0';
        }
        
        if (strcmp(path_info, "/physics") == 0) {
            handle_physics_solver(post_data ? post_data : "");
        }
        else if (strcmp(path_info, "/convert") == 0) {
            handle_unit_conversion(post_data ? post_data : "");
        }
        else if (strcmp(path_info, "/calculate-sgpa") == 0) {
            handle_sgpa_calculation(post_data ? post_data : "");
        }
        else if (strcmp(path_info, "/calculate-grade") == 0) {
            handle_grade_calculation(post_data ? post_data : "");
        }
        else if (strcmp(path_info, "/tasks") == 0 || strncmp(path_info, "/tasks/", 7) == 0) {
            handle_tasks(request_method, path_info, query_string, post_data);
        }
        else if (strcmp(path_info, "/flashcards") == 0 || strncmp(path_info, "/flashcards/", 12) == 0) {
            handle_flashcards(request_method, path_info, query_string, post_data);
        }
        else {
            send_response_header(true);
            printf("{\"error\": \"Unknown endpoint for %s: %s\"}", request_method, path_info);
        }
        
        if (post_data) {
            free(post_data);
        }
    }
    else {
        send_response_header(true);
        printf("{\"error\": \"Unsupported request method: %s\"}", request_method);
    }
    
    return 0;
}