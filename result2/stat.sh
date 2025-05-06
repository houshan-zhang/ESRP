#!/bin/bash

# 检查是否提供了两个参数
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <filename> <type>"
  echo "Type 1: Extract '(a) Optimal objective value' (5th column)"
  echo "Type 2: Extract '(d) CPX solving time' (5th column)"
  exit 1
fi

# 获取传入的参数
file="$1"
type="$2"

# 检查文件是否存在
if [ ! -f "$file" ]; then
  echo "Error: File '$file' not found."
  exit 1
fi

# 根据 type 参数执行不同的操作
if [ "$type" -eq 1 ]; then
  grep "(a) Optimal objective value:" "$file" | awk '{print $5}'
elif [ "$type" -eq 2 ]; then
  grep "(e) CPX solving time:" "$file" | awk '{print $5}'
else
  echo "Error: Invalid type. Please use 1 or 2."
  exit 1
fi
