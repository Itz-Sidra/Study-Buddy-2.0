#!/usr/bin/env python3
import http.server
import socketserver
import json
import os
import subprocess
import cgi
import tempfile
import urllib.parse
import sqlite3
import time
from pathlib import Path

# Constants
PORT = 8000
DATABASE_FILE = 'study_buddy.db'
BACKEND_EXECUTABLE = './backend'  # Compiled backend.c executable

# Create database tables if they don't exist
def init_db():
    conn = sqlite3.connect(DATABASE_FILE)
    cursor = conn.cursor()
    
    # Tasks table
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS tasks (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        date TEXT,
        completed INTEGER DEFAULT 0,
        user_id TEXT DEFAULT 'default',
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    ''')
    
    # Flashcards table
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS flashcards (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        front TEXT NOT NULL,
        back TEXT NOT NULL,
        user_id TEXT DEFAULT 'default',
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    ''')
    
    conn.commit()
    conn.close()

# Handler for incoming HTTP requests
class StudyBuddyHandler(http.server.SimpleHTTPRequestHandler):
    
    def _set_headers(self, content_type='application/json'):
        self.send_response(200)
        self.send_header('Content-type', content_type)
        self.send_header('Access-Control-Allow-Origin', '*')  # CORS header
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
    
    def do_OPTIONS(self):
        self._set_headers()
    
    def do_GET(self):
        # Parse the URL path and query string
        path_parts = self.path.split('?')
        path = path_parts[0]
        query_string = path_parts[1] if len(path_parts) > 1 else ""
        
        # Handle different endpoints
        if path == '/api/tasks':
            self.handle_tasks_get()
        elif path == '/api/flashcards':
            self.handle_flashcards_get()
        elif path.startswith('/api/physics'):
            self.handle_physics(query_string)
        elif path.startswith('/api/convert'):
            self.handle_convert(query_string)
        elif path == '/':
            # Serve the main index.html file
            self.path = '/index.html'
            return http.server.SimpleHTTPRequestHandler.do_GET(self)
        else:
            # Serve static files
            return http.server.SimpleHTTPRequestHandler.do_GET(self)
    
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')
        
        if self.path == '/api/tasks':
            self.handle_tasks_post(post_data)
        elif self.path == '/api/flashcards':
            self.handle_flashcards_post(post_data)
        elif self.path.startswith('/api/physics'):
            self.handle_physics(post_data)
        elif self.path.startswith('/api/convert'):
            self.handle_convert(post_data)
        else:
            self._set_headers()
            response = {'error': 'Unknown endpoint'}
            self.wfile.write(json.dumps(response).encode())
    
    def do_PUT(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')
        
        if self.path.startswith('/api/tasks/'):
            task_id = self.path.split('/')[-1]
            self.handle_task_update(task_id, post_data)
        else:
            self._set_headers()
            response = {'error': 'Unknown endpoint'}
            self.wfile.write(json.dumps(response).encode())
    
    def do_DELETE(self):
        if self.path.startswith('/api/tasks/'):
            task_id = self.path.split('/')[-1]
            self.handle_task_delete(task_id)
        elif self.path.startswith('/api/flashcards/'):
            flashcard_id = self.path.split('/')[-1]
            self.handle_flashcard_delete(flashcard_id)
        else:
            self._set_headers()
            response = {'error': 'Unknown endpoint'}
            self.wfile.write(json.dumps(response).encode())
    
    # Task handlers
    def handle_tasks_get(self):
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute('SELECT id, title, date, completed FROM tasks ORDER BY date ASC')
        tasks = []
        for row in cursor.fetchall():
            tasks.append({
                'id': row[0],
                'title': row[1],
                'date': row[2],
                'completed': bool(row[3])
            })
        conn.close()
        
        self._set_headers()
        self.wfile.write(json.dumps(tasks).encode())
    
    def handle_tasks_post(self, post_data):
        task_data = json.loads(post_data)
        
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute(
            'INSERT INTO tasks (title, date, completed) VALUES (?, ?, ?)',
            (task_data.get('title', ''), task_data.get('date', ''), int(task_data.get('completed', False)))
        )
        task_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        self._set_headers()
        response = {'id': task_id, 'success': True}
        self.wfile.write(json.dumps(response).encode())
    
    def handle_task_update(self, task_id, post_data):
        task_data = json.loads(post_data)
        
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute(
            'UPDATE tasks SET title = ?, date = ?, completed = ? WHERE id = ?',
            (task_data.get('title', ''), task_data.get('date', ''), 
             int(task_data.get('completed', False)), task_id)
        )
        conn.commit()
        conn.close()
        
        self._set_headers()
        response = {'success': True}
        self.wfile.write(json.dumps(response).encode())
    
    def handle_task_delete(self, task_id):
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute('DELETE FROM tasks WHERE id = ?', (task_id,))
        conn.commit()
        conn.close()
        
        self._set_headers()
        response = {'success': True}
        self.wfile.write(json.dumps(response).encode())
    
    # Flashcard handlers
    def handle_flashcards_get(self):
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute('SELECT id, front, back FROM flashcards')
        flashcards = []
        for row in cursor.fetchall():
            flashcards.append({
                'id': row[0],
                'front': row[1],
                'back': row[2]
            })
        conn.close()
        
        self._set_headers()
        self.wfile.write(json.dumps(flashcards).encode())
    
    def handle_flashcards_post(self, post_data):
        flashcard_data = json.loads(post_data)
        
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute(
            'INSERT INTO flashcards (front, back) VALUES (?, ?)',
            (flashcard_data.get('front', ''), flashcard_data.get('back', ''))
        )
        flashcard_id = cursor.lastrowid
        conn.commit()
        conn.close()
        
        self._set_headers()
        response = {'id': flashcard_id, 'success': True}
        self.wfile.write(json.dumps(response).encode())
    
    def handle_flashcard_delete(self, flashcard_id):
        conn = sqlite3.connect(DATABASE_FILE)
        cursor = conn.cursor()
        cursor.execute('DELETE FROM flashcards WHERE id = ?', (flashcard_id,))
        conn.commit()
        conn.close()
        
        self._set_headers()
        response = {'success': True}
        self.wfile.write(json.dumps(response).encode())
    
    # Physics solver handler - FIXED VERSION
    def handle_physics(self, query_string):
        try:
            # Call the C backend with the query string
            env = os.environ.copy()
            env["QUERY_STRING"] = query_string
            env["REQUEST_METHOD"] = self.command  # Use the actual request method
            env["PATH_INFO"] = "/physics"
            
            print(f"DEBUG: Calling backend with PATH_INFO=/physics, QUERY_STRING={query_string}")
            
            result = subprocess.run(
                [BACKEND_EXECUTABLE],
                env=env,
                capture_output=True,
                text=True,
                check=False
            )
            
            # Debug output if needed
            if result.returncode != 0:
                print(f"DEBUG: Backend exited with code {result.returncode}")
                print(f"DEBUG: Backend stderr: {result.stderr}")
            
            # Fix: Properly extract JSON from the output
            output = result.stdout.strip()
            
            # Find the actual JSON content after the headers
            json_content = None
            if '\r\n\r\n' in output:
                # Split at the double newline that separates headers from body
                headers, body = output.split('\r\n\r\n', 1)
                json_content = body
            else:
                # If there's no header separator, check if it's just JSON
                try:
                    json.loads(output)
                    json_content = output
                except:
                    # If parsing fails, look for the first { character
                    start_idx = output.find('{')
                    if start_idx >= 0:
                        json_content = output[start_idx:]
            
            if json_content:
                try:
                    json_output = json.loads(json_content)
                    self._set_headers()
                    self.wfile.write(json.dumps(json_output).encode())
                except json.JSONDecodeError as je:
                    print(f"DEBUG: JSON decode error: {je}, Content: {json_content[:100]}")
                    self._set_headers()
                    response = {'error': f'Backend returned malformed JSON. Content appears to be: {json_content[:100]}'}
                    self.wfile.write(json.dumps(response).encode())
            else:
                print(f"DEBUG: No JSON content found in: {output[:100]}")
                self._set_headers()
                response = {'error': 'Backend response contained no valid JSON data'}
                self.wfile.write(json.dumps(response).encode())
                
        except Exception as e:
            print(f"DEBUG: Exception: {str(e)}")
            self._set_headers()
            response = {'error': f'Backend execution failed: {str(e)}'}
            self.wfile.write(json.dumps(response).encode())
    
    # Unit converter handler - FIXED VERSION
    def handle_convert(self, query_string):
        try:
            # Call the C backend with the query string
            env = os.environ.copy()
            env["QUERY_STRING"] = query_string
            env["REQUEST_METHOD"] = self.command  # Use the actual request method
            env["PATH_INFO"] = "/convert"
            
            print(f"DEBUG: Calling backend with PATH_INFO=/convert, QUERY_STRING={query_string}")
            
            result = subprocess.run(
                [BACKEND_EXECUTABLE],
                env=env,
                capture_output=True,
                text=True,
                check=False
            )
            
            # Debug output if needed
            if result.returncode != 0:
                print(f"DEBUG: Backend exited with code {result.returncode}")
                print(f"DEBUG: Backend stderr: {result.stderr}")
            
            # Fix: Properly extract JSON from the output
            output = result.stdout.strip()
            
            # Find the actual JSON content after the headers
            json_content = None
            if '\r\n\r\n' in output:
                # Split at the double newline that separates headers from body
                headers, body = output.split('\r\n\r\n', 1)
                json_content = body
            else:
                # If there's no header separator, check if it's just JSON
                try:
                    json.loads(output)
                    json_content = output
                except:
                    # If parsing fails, look for the first { character
                    start_idx = output.find('{')
                    if start_idx >= 0:
                        json_content = output[start_idx:]
            
            if json_content:
                try:
                    json_output = json.loads(json_content)
                    self._set_headers()
                    self.wfile.write(json.dumps(json_output).encode())
                except json.JSONDecodeError as je:
                    print(f"DEBUG: JSON decode error: {je}, Content: {json_content[:100]}")
                    self._set_headers()
                    response = {'error': f'Backend returned malformed JSON. Content appears to be: {json_content[:100]}'}
                    self.wfile.write(json.dumps(response).encode())
            else:
                print(f"DEBUG: No JSON content found in: {output[:100]}")
                self._set_headers()
                response = {'error': 'Backend response contained no valid JSON data'}
                self.wfile.write(json.dumps(response).encode())
                
        except Exception as e:
            print(f"DEBUG: Exception: {str(e)}")
            self._set_headers()
            response = {'error': f'Backend execution failed: {str(e)}'}
            self.wfile.write(json.dumps(response).encode())

def main():
    # Initialize database
    init_db()
    
    # Compile the C backend
    try:
        subprocess.run(['gcc', '-o', BACKEND_EXECUTABLE, 'backend.c', '-lm'], check=True)
        print(f"Successfully compiled backend.c to {BACKEND_EXECUTABLE}")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling backend.c: {e}")
        return
    
    # Start the web server
    handler = StudyBuddyHandler
    with socketserver.TCPServer(("", PORT), handler) as httpd:
        print(f"Server started at http://localhost:{PORT}")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("Server stopped.")

if __name__ == "__main__":
    main()