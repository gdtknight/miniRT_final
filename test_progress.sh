#!/bin/bash
cd /Users/yoshin/dev/working/miniRT
make clean >/dev/null 2>&1
make 2>&1 | tail -3
echo "---"
python3 -m norminette src/ includes/ 2>&1 | grep "Error:" | wc -l
python3 -m norminette src/ includes/ 2>&1 | grep "Error:" | awk '{print $2}' | sort | uniq -c | sort -rn
