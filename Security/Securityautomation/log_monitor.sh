#!/bin/bash
# Determine which log file to use.
if [ -n "$1" ]; then
    LOGFILE="$1"
else
    if [ -f /var/log/auth.log ]; then
        LOGFILE="/var/log/auth.log"
    elif [ -f /var/log/secure ]; then
        LOGFILE="/var/log/secure"
    else
        echo "No suitable authentication log file found."
        exit 1
    fi
fi

echo "Using log file: $LOGFILE"
echo "Scanning for suspicious login attempts..."
echo "--------------------------------------------"

# Display the last 20 lines that indicate failed password attempts.
echo "Failed password attempts (last 20 entries):"
grep -i "failed password" "$LOGFILE" | tail -n 20
echo ""

# Display the last 20 lines that indicate authentication failures.
echo "Authentication failures (last 20 entries):"
grep -i "authentication failure" "$LOGFILE" | tail -n 20
echo ""

echo "Log analysis complete."
