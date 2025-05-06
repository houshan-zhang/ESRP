#!/bin/bash
dir=${1}   #<b>

if [ -z "${1}" ]; then
  echo "Error: No directory provided."
  echo "Usage: ${0} <directory| tee -a"
  exit 1
fi

if [ ! -d "${dir}" ]; then
  echo "Error: Directory '${dir}' does not exist."
  exit 1
fi

../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=1.0 TIME=1800" | tee -a ${dir}/a-1-1
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.8 TIME=1800" | tee -a ${dir}/a-1-2
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.6 TIME=1800" | tee -a ${dir}/a-1-3
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.4 TIME=1800" | tee -a ${dir}/a-1-4
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.2 TIME=1800" | tee -a ${dir}/a-1-5
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.0 TIME=1800" | tee -a ${dir}/a-1-6

../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=1.0 TIME=1800" | tee -a ${dir}/a-2-1
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.8 TIME=1800" | tee -a ${dir}/a-2-2
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.6 TIME=1800" | tee -a ${dir}/a-2-3
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.4 TIME=1800" | tee -a ${dir}/a-2-4
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.2 TIME=1800" | tee -a ${dir}/a-2-5
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.0 TIME=1800" | tee -a ${dir}/a-2-6

../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=1.0 TIME=1800" | tee -a ${dir}/a-3-1
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.8 TIME=1800" | tee -a ${dir}/a-3-2
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.6 TIME=1800" | tee -a ${dir}/a-3-3
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.4 TIME=1800" | tee -a ${dir}/a-3-4
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.2 TIME=1800" | tee -a ${dir}/a-3-5
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB1=0.0 TIME=1800" | tee -a ${dir}/a-3-6



../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=1.0 TIME=1800" | tee -a ${dir}/b-1-1
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.9 TIME=1800" | tee -a ${dir}/b-1-2
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.8 TIME=1800" | tee -a ${dir}/b-1-3
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.7 TIME=1800" | tee -a ${dir}/b-1-4
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.6 TIME=1800" | tee -a ${dir}/b-1-5
../check/bin/netrec "MODE=1 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.5 TIME=1800" | tee -a ${dir}/b-1-6

../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=1.0 TIME=1800" | tee -a ${dir}/b-2-1
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.9 TIME=1800" | tee -a ${dir}/b-2-2
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.8 TIME=1800" | tee -a ${dir}/b-2-3
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.7 TIME=1800" | tee -a ${dir}/b-2-4
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.6 TIME=1800" | tee -a ${dir}/b-2-5
../check/bin/netrec "MODE=2 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.5 TIME=1800" | tee -a ${dir}/b-2-6

../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=1.0 TIME=1800" | tee -a ${dir}/b-3-1
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.9 TIME=1800" | tee -a ${dir}/b-3-2
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.8 TIME=1800" | tee -a ${dir}/b-3-3
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.7 TIME=1800" | tee -a ${dir}/b-3-4
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.6 TIME=1800" | tee -a ${dir}/b-3-5
../check/bin/netrec "MODE=3 CUST=1000 SUPP=100 BUDGET=10 TYPE=2 SAMP=100 QOS=10 PROB2=0.5 TIME=1800" | tee -a ${dir}/b-3-6
