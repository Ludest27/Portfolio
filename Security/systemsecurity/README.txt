File Integrity Monitoring Tool - README
=========================================

Overview:
---------
This tool monitors file integrity by computing SHA-256 hashes for specified files.
It operates in two modes:
  1. init: Computes and saves baseline hashes to "baseline.txt".
  2. check: Recomputes hashes and compares them against the baseline to detect changes.

Project Structure:
------------------
SystemSecurity/
  ├── integrity_monitor.c   - Main source code for the file integrity tool.
  ├── baseline.txt          - Stores baseline hashes (created during "init" mode).
  ├── Makefile              - Build instructions for compiling the tool.
  └── README.txt            - This documentation file.

Build Instructions:
-------------------
To build the tool, run the following command:
   gcc integrity_monitor.c -o integrity_monitor -lcrypto -Wall -Wextra -O2
Alternatively, use the provided Makefile by running:
   make

Usage:
------
1. Initialize the Baseline:
   Run the tool in "init" mode to compute and store SHA-256 hashes for your files.
   For example:
      ./integrity_monitor init file1 file2
   This will create or update "baseline.txt" with the filename and its computed hash.

2. Check File Integrity:
   Run the tool in "check" mode to verify current file hashes against the baseline.
   For example:
      ./integrity_monitor check file1 file2
   The tool will report "OK" if the file is unchanged, or "CHANGED" if the file has been modified.

Clean Up:
---------
To remove the compiled executable, run:
   make clean

Requirements:
-------------
- A C compiler (e.g., GCC)
- OpenSSL development libraries for SHA-256 functionality

Notes:
------
- Make sure the files you wish to monitor (e.g., file1, file2) exist before running the tool.
- "baseline.txt" is automatically created during initialization and is used for later comparisons.
- This tool is not to be deployed  and demonstrates basic system security monitoring.


