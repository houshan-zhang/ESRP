#!/bin/bash
dir=${1}   #<a,b,c,d>
path=$(dirname "$(readlink -f "${0}")")
log_file=${path}/${dir}/${dir}.log

if [ -z "${dir}" ]; then
  echo "Error: 'dir' is not defined."
  echo "Usage: ${0} <dir>"
  exit 1
fi

if [ ! -f "mpi-${dir}.sh" ]; then
  echo "Error: File 'mpi-${dir}.sh' does not exist in the current directory."
  exit 1
fi


#rm -rf ${path}/${dir}
#mkdir ${path}/${dir}

#mpirun -n $(cat /proc/cpuinfo|grep "cpu cores"|uniq | awk '{print $4}') ../check/scripts/mpi/mpiexecline mpi-${dir}.sh

for prob1 in 1 2 3 4 5 6; do
  for mode in 1 2 3; do
    file_name="${dir}/a-${mode}-${prob1}"
    cat "${file_name}" >> "${log_file}"
  done
done

for prob2 in 1 2 3 4 5 6; do
  for mode in 1 2 3; do
    file_name="${dir}/b-${mode}-${prob2}"
    cat "${file_name}" >> "${log_file}"
  done
done

echo "All outputs have been combined into ${log_file}."
