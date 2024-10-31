#!/bin/bash

process_counts=(1 2 4 8 16)
strip_values=(8 64 512 4096 32768 262114)
output_file="execution_data.txt"
echo "Execution Data" > "$output_file"
echo "===================" >> "$output_file"
echo "Processes, Strips, Result" >> "$output_file"

for processes in "${process_counts[@]}"; do
    for strips in "${strip_values[@]}"; do
        export TRAPEZOID_STRIPS=$strips
        echo "Running ./cc-lab2 with ${processes} processes and ${strips} strips..."
        result=$(mpirun --oversubscribe -np $processes ./bin/cc-lab2)
        echo "${processes}, ${strips}, ${result}" >> "$output_file"
    done
done

echo "Execution data has been saved to $output_file."

