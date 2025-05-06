#!/bin/bash
file="${1}"

if [ ! -f "${file}/${file}.log" ]; then
  echo "Error: File '${file}/${file}.log' not found."
  exit 1
fi

echo " --- Time(s) ---"
grep "(e) CPLEX solving time:" "${file}/${file}.log" | awk '{print $5}'
echo " --- Gap(%) ---"
grep "(c) Relative Gap:" "${file}/${file}.log" | awk '{print $4}'
