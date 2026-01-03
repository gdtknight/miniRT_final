#!/bin/bash
# Fix line continuation indentation issues

files=(
"src/ray/intersect_cylinder.c"
"src/lighting/lighting.c"
"src/lighting/shadow_attenuation.c"
"src/lighting/shadow_calc.c"
)

for file in "${files[@]}"; do
if [ -f "$file" ]; then
echo "Processing: $file"
# This is complex - will do manually
fi
done
