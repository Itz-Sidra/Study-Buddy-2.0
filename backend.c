#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// HTTP response helper functions
void send_response_header(bool is_json) {
    printf("Content-Type: %s\r\n\r\n", is_json ? "application/json" : "text/plain");
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
    
    double result = -1;
    
    if (strcmp(formula, "velocity") == 0) {
        const char* u_str = get_param(params, num_params, "u");
        const char* a_str = get_param(params, num_params, "a");
        const char* t_str = get_param(params, num_params, "t");
        
        if (u_str && a_str && t_str) {
            double u = atof(u_str);
            double a = atof(a_str);
            double t = atof(t_str);
            result = velocity_formula(u, a, t);
        }
    } 
    else if (strcmp(formula, "distance") == 0) {
        const char* u_str = get_param(params, num_params, "u");
        const char* a_str = get_param(params, num_params, "a");
        const char* t_str = get_param(params, num_params, "t");
        
        if (u_str && a_str && t_str) {
            double u = atof(u_str);
            double a = atof(a_str);
            double t = atof(t_str);
            result = distance_formula(u, a, t);
        }
    }
    else if (strcmp(formula, "velocity-squared") == 0) {
        const char* u_str = get_param(params, num_params, "u");
        const char* a_str = get_param(params, num_params, "a");
        const char* s_str = get_param(params, num_params, "s");
        
        if (u_str && a_str && s_str) {
            double u = atof(u_str);
            double a = atof(a_str);
            double s = atof(s_str);
            result = velocity_squared_formula(u, a, s);
        }
    }
    else if (strcmp(formula, "force") == 0) {
        const char* m_str = get_param(params, num_params, "m");
        const char* a_str = get_param(params, num_params, "a");
        
        if (m_str && a_str) {
            double m = atof(m_str);
            double a = atof(a_str);
            result = force_formula(m, a);
        }
    }
    else if (strcmp(formula, "kinetic-energy") == 0) {
        const char* m_str = get_param(params, num_params, "m");
        const char* v_str = get_param(params, num_params, "v");
        
        if (m_str && v_str) {
            double m = atof(m_str);
            double v = atof(v_str);
            result = kinetic_energy_formula(m, v);
        }
    }
    else if (strcmp(formula, "potential-energy") == 0) {
        const char* m_str = get_param(params, num_params, "m");
        const char* g_str = get_param(params, num_params, "g");
        const char* h_str = get_param(params, num_params, "h");
        
        if (m_str && h_str) {
            double m = atof(m_str);
            double g = g_str ? atof(g_str) : 9.8; // Use default if not provided
            double h = atof(h_str);
            result = potential_energy_formula(m, g, h);
        }
    }
    else if (strcmp(formula, "ohms-law") == 0) {
        const char* i_str = get_param(params, num_params, "i");
        const char* r_str = get_param(params, num_params, "r");
        
        if (i_str && r_str) {
            double i = atof(i_str);
            double r = atof(r_str);
            result = ohms_law_formula(i, r);
        }
    }
    else if (strcmp(formula, "power") == 0) {
        const char* v_str = get_param(params, num_params, "v");
        const char* i_str = get_param(params, num_params, "i");
        
        if (v_str && i_str) {
            double v = atof(v_str);
            double i = atof(i_str);
            result = power_formula(v, i);
        }
    }
    
    send_response_header(true);
    if (result != -1) {
        printf("{\"result\": %.4f}", result);
    } else {
        printf("{\"error\": \"Invalid parameters or formula\"}");
    }
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
    
    double value = atof(value_str);
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
        else {
            send_response_header(true);
            printf("{\"error\": \"Unknown endpoint: %s\"}", path_info);
        }
    }
    // Process POST requests
    else if (strcmp(request_method, "POST") == 0) {
        // Get content length
        char* content_length_str = getenv("CONTENT_LENGTH");
        int content_length = content_length_str ? atoi(content_length_str) : 0;
        
        if (content_length > 0) {
            char* post_data = malloc(content_length + 1);
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
            
            if (strcmp(path_info, "/physics") == 0) {
                handle_physics_solver(post_data);
            }
            else if (strcmp(path_info, "/convert") == 0) {
                handle_unit_conversion(post_data);
            }
            else {
                send_response_header(true);
                printf("{\"error\": \"Unknown endpoint for POST: %s\"}", path_info);
            }
            
            free(post_data);
        } else {
            // Even if there's no POST data, attempt to handle the request
            if (strcmp(path_info, "/physics") == 0) {
                handle_physics_solver("");
            }
            else if (strcmp(path_info, "/convert") == 0) {
                handle_unit_conversion("");
            }
            else {
                send_response_header(true);
                printf("{\"error\": \"Unknown endpoint for empty POST: %s\"}", path_info);
            }
        }
    }
    else {
        send_response_header(true);
        printf("{\"error\": \"Unsupported request method: %s\"}", request_method);
    }
    
    return 0;
}