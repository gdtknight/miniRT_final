#!/bin/bash
# Fix whitespace issues in source files

# Convert tabs to spaces in all .c and .h files
find src includes -type f \( -name "*.c" -o -name "*.h" \) -exec bash -c '
	for file; do
		# Check if file has issues
		if grep -q $'\''\t'\'' "$file"; then
			echo "Fixing tabs in: $file"
			# Use expand to convert tabs to spaces (tab stop = 4)
			expand -t 4 "$file" > "$file.tmp" && mv "$file.tmp" "$file"
		fi
	done
' bash {} +

echo "Whitespace fixes complete"
