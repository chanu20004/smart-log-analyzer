# Smart Log Analyzer (C++)

Smart Log Analyzer is a configurable C++ program that analyzes log files,
validates their format, performs time-based ERROR analysis, detects critical
errors using external configuration, and generates structured summary reports.

This project demonstrates practical use of C++ STL, file handling, parsing,
validation, and modular program design.

---

## Features

- Validates log format and time (HH:MM:SS)
- Skips malformed or empty log lines
- Counts ERROR occurrences per hour
- Detects peak ERROR hour
- Generates log level summary (INFO / WARN / ERROR)
- Detects critical ERRORs using configurable keywords
- Uses external configuration file (`critical.conf`)
- Command-line configurable input and output files

---

## Project Structure

SmartLogAnalyzer/
│
├── main.cpp # Core log analyzer implementation
├── critical.conf # Critical error keywords (configurable)
├── testfile.log # Sample log file for testing
├── .gitignore # Ignored files (executables, outputs)
├── README.md # Project documentation


---

## Input Log Format

Each log line should follow this format:

YYYY-MM-DD HH:MM:SS LEVEL message


Example:
2024-01-12 10:15:45 ERROR Database connection failed


---

## Critical Keywords Configuration

The file `critical.conf` contains keywords used to detect critical ERRORs.
Each line represents one keyword.

Example:
failed
timeout
exception
crash


Matching is case-insensitive.

---

## How to Compile and Run

### Compile
```bash
g++ main.cpp -o loganalyzer
Run
./loganalyzer testfile.log output.txt
(On Windows PowerShell use .\loganalyzer)

Output Report Includes
ERROR count per hour

Peak ERROR hour

Log level summary

Critical ERROR summary

Number of skipped (invalid) log lines

Concepts Used
C++ STL (map, vector, string)

File handling (ifstream, ofstream)

String parsing (istringstream)

Modular programming

Command-line arguments

Input validation

Author
Chanukya (chanu20004)
GitHub: https://github.com/chanu20004
