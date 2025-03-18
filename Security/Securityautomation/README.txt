Security Automation Project - Log Analysis Tool
=================================================

Overview:
---------
This project demonstrates a simple security automation tool that analyzes system logs
to detect suspicious login attempts. The Bash script (log_monitor.sh) scans your system's
authentication log for failed password attempts and authentication failures.

Project Structure:
------------------
SecurityAutomation/
  ├── log_monitor.sh      - Bash script that performs log analysis.
  └── README.txt          - This documentation file.

Features:
---------
- Detects and displays the last 20 entries of "failed password" attempts.
- Detects and displays the last 20 entries of "authentication failure" events.
- Automatically selects the appropriate log file:
     - /var/log/auth.log for Debian/Ubuntu systems.
     - /var/log/secure for RedHat/CentOS systems.

Usage:
------
1. Make the script executable:
     chmod +x log_monitor.sh

2. Run the script without arguments to use the default log file:
     ./log_monitor.sh

   Alternatively, specify a custom log file:
     ./log_monitor.sh /path/to/logfile

Requirements:
-------------
- Bash shell (common on Linux systems)
- Access to a system authentication log (/var/log/auth.log or /var/log/secure)

Notes:
------
- This tool is intended  to demonstrate basic security automation.
- Depending on your system, you might need root privileges to read the log files.


