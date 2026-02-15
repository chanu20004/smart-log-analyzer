# Smart Log Analyzer 🚀  

A production-style log analysis system built using **C++ (core processing engine)** and **Flask (web interface)**.

It analyzes large log files efficiently and provides structured insights with visual dashboards and JSON API support.

---

## 📌 Problem Statement

Modern applications generate massive log files.

Manually identifying:

- Error spikes  
- Peak failure hours  
- Log level distributions  
- Root causes  

is time-consuming and inefficient.

This project automates log analysis and provides:

- ⚡ Real-time spike detection  
- 📊 Structured error categorization  
- 🔌 REST API for integration  
- 📈 Interactive dashboard with charts  

---

## 🏗 System Architecture

```
User
  │
  ▼
Flask Web Server
  │
  ▼
File Merge Module
  │
  ▼
C++ Analysis Engine
  │
  ▼
JSON Output
  │
  ▼
Dashboard (Charts + Tables)
```

### Flow Explanation

1. User uploads one or more log files.
2. Flask merges files into a single input stream.
3. The C++ engine processes logs.
4. Results are generated in structured JSON format.
5. Flask renders charts and analytics in the UI.

---

## 🧠 Spike Detection Algorithm

The system uses a **Sliding Window (Queue-Based) Algorithm**.

### Steps:

1. Convert log timestamp to total seconds.
2. Push timestamp into a queue.
3. Remove timestamps older than 5-minute window.
4. If queue size exceeds threshold → mark as spike.

### ⏱ Time Complexity

- **O(N)**  
Where `N` = number of log lines.

Each log is processed once.

---

## ⚙ Tech Stack

### Core Engine
- C++
- STL (`unordered_map`, `queue`, `vector`)

### Web Layer
- Python (Flask)
- HTML + CSS
- Chart.js

### API
- REST Endpoint: `/api/analyze`
- JSON structured output

---

## 📊 Features

- Multi-file upload  
- Automatic file merge before analysis  
- Error-per-hour visualization  
- Log level pie chart  
- Error category distribution  
- JSON mode support (`--json` flag`)  
- Downloadable report  
- REST API support  

---

## 📡 REST API Usage

### Endpoint

POST `/api/analyze`

### Request Type

`form-data`  
Key: `file` (log file)

### Sample Response

```json
{
  "errors_per_hour": {
    "14": 12,
    "15": 8
  },
  "log_levels": {
    "ERROR": 20,
    "INFO": 50,
    "WARNING": 10
  },
  "categories": {
    "Database": 8,
    "Timeout": 6,
    "Authentication": 3
  }
}
```

---

## 🖥 How To Run

### 1️⃣ Compile C++ Engine

```
g++ main.cpp -o main.exe
```

### 2️⃣ Run Flask Application

```
python app.py
```

### 3️⃣ Open in Browser

```
http://127.0.0.1:5000
```

---

## 📝 Sample Log Input

```
2024-01-12 14:01:10 ERROR Database failure
2024-01-12 14:02:05 ERROR Timeout occurred
```

---

## 💡 Why This Project Stands Out

- Demonstrates system design thinking
- Combines C++ performance with Python web integration
- Uses efficient O(N) algorithm
- Includes API + UI + Visualization
- Follows modular architecture

---

## 🔮 Future Improvements

- Docker deployment  
- Authentication system  
- Real-time streaming log support  
- Advanced anomaly detection  

---

### 👨‍💻 Author

Chanukya Latchubukta
