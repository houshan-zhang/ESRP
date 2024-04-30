#!/bin/bash

# script parameter
MODE=${1}               # solving mode
TIMELIMIT=${2}          # (int > 0) in seconds
MEMLIMIT=${3}           # (int > 0) in MB
THREADS=${4}            # (int > 0) number of threads to be used by the solver
MIPGAP=${5}             # the MIP gap is not uniqely defined through all solvers
OUTDIR=results/${6}     # output dir
SETTING=${7}            # setting file
SEED=${8}               # random seed
SEEDFILE=${9}           # random seedfile
CLUSTER=${10}           # run on cluster
EXCLUSIVE=${11}         # exclusive cluster run
MPI=${12}               # mpi parallel execute commands
QUEUE=${13}             # cluster queue name
TC=${14}                # total number of cores can used
HC=${15}                # number of cores per host allocated
JC=${16}                # number of cores per job used

# additional parameter
LINTOL=1e-4       # absolut tolerance for checking linear constraints and objective value
INTTOL=1e-4       # absolut tolerance for checking integrality constraints

# import some useful functions that make this script less cluttered
. $(dirname "${BASH_SOURCE[0]}")/run_functions.sh

# construct paths
CHECKPATH=$(pwd)

# check if the setting file exists
if [[ -n ${SETTING} && ! -f ${CHECKPATH}/settings/${SETTING}.set ]]
then
   echo "ERROR: setting file '${SETTING}.set' does not exist in 'settings' folder"
   exit -1
fi

# check if the solver link (binary) exists
if [[ ! -e "${CHECKPATH}/bin/netrec" ]]
then
   echo "ERROR: solver link 'netrec' does not exist in 'bin' folder"
   exit -1
fi

# check if the test seedfile exists
if [[ -n ${SEEDFILE} && ! -f ${CHECKPATH}/seeds/${SEEDFILE}.seed ]]
then
   echo "ERROR: seedfile file/link '${SEEDFILE}.seed' does not exist in 'seeds' folder"
   exit -1
fi

# check if the result folder exist. if not create the result folder
if [[ ! -e ${CHECKPATH}/results ]]
then
   mkdir ${CHECKPATH}/results
fi

# check if the output folder exist. if not create the solution folder
if [[ ! -e ${CHECKPATH}/$OUTDIR ]]
then
   mkdir -p ${CHECKPATH}/${OUTDIR}
fi

# check if the error folder exist. if not create the error folder
if [[ ! -e ${CHECKPATH}/${OUTDIR}/error ]]
then
   mkdir ${CHECKPATH}/${OUTDIR}/error
fi

# check if the command folder exist. if not create the command folder
if [[ ! -e ${CHECKPATH}/${OUTDIR}/cmd ]]
then
   mkdir ${CHECKPATH}/${OUTDIR}/cmd
fi

rm -f ${OUTDIR}/netrec.file
touch ${OUTDIR}/netrec.file

# touch history file
rm -f ${OUTDIR}/netrec.history
touch ${OUTDIR}/netrec.history

echo "==========================" start "=========================="
export OUTDIR
export MEMLIMIT
export TIMELIMIT
export CHECKPATH
export MODE
export THREADS
export MIPGAP
export LINTOL
export INTTOL
export SETTING
export CLUSTER
export EXCLUSIVE
export MPI
export QUEUE
if [[ -z ${SEEDFILE} ]]
then
   export SEED
   echo "netrec.${SEED}.${THREADS}threads.${TIMELIMIT}s.out" >> ${OUTDIR}/netrec.file
   if [[ ${MPI} == on ]]
   then
      echo -e "export SEED=${SEED} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export OUTDIR=${OUTDIR} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export MEMLIMIT=${MEMLIMIT} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export TIMELIMIT=${TIMELIMIT} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export CHECKPATH=${CHECKPATH} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export MODE={MODE} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export THREADS=${THREADS} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export MIPGAP=${MIPGAP} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export LINTOL=${LINTOL} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export INTTOL=${INTTOL} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export SETTING=${SETTING} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export CLUSTER=${CLUSTER} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export EXCLUSIVE=${EXCLUSIVE} && \c" >> ${OUTDIR}/netrec.history
      echo -e "export MPI=${MPI} && \c" >> ${OUTDIR}/netrec.history
      echo -e "${CHECKPATH}/scripts/runjob.sh" >> ${OUTDIR}/netrec.history
   else
      scripts/runjob.sh
   fi
else
   for SEED in $(cat ${CHECKPATH}/seeds/${SEEDFILE}.seed)
   do
      export SEED
      echo "netrec.${SEED}.${THREADS}threads.${TIMELIMIT}s.out" >> ${OUTDIR}/netrec.file
      if [[ ${MPI} == on ]]
      then
         echo -e "export SEED=${SEED} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export OUTDIR=${OUTDIR} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export MEMLIMIT=${MEMLIMIT} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export TIMELIMIT=${TIMELIMIT} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export CHECKPATH=${CHECKPATH} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export MODE={MODE} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export THREADS=${THREADS} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export MIPGAP=${MIPGAP} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export LINTOL=${LINTOL} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export INTTOL=${INTTOL} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export SETTING=${SETTING} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export CLUSTER=${CLUSTER} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export EXCLUSIVE=${EXCLUSIVE} && \c" >> ${OUTDIR}/netrec.history
         echo -e "export MPI=${MPI} && \c" >> ${OUTDIR}/netrec.history
         echo -e "${CHECKPATH}/scripts/runjob.sh" >> ${OUTDIR}/netrec.history
      else
         scripts/runjob.sh
      fi
   done
fi
echo "=========================="\ \ end\ \ "=========================="

# write statistic shell script
rm -f ${OUTDIR}/netrec.sh
echo "#!/bin/bash" >> ${OUTDIR}/netrec.sh
echo "" >> ${OUTDIR}/netrec.sh
echo "path=\$(cd \"\$(dirname \"\$0\")\"; pwd)" >> ${OUTDIR}/netrec.sh
echo "rm -f \${path}/netrec.out" >> ${OUTDIR}/netrec.sh
echo "rm -f \${path}/netrec.netrec.res" >> ${OUTDIR}/netrec.sh
echo "for i in \$(cat \${path}/netrec.file)" >> ${OUTDIR}/netrec.sh
echo "do" >> ${OUTDIR}/netrec.sh
echo "   outfile=\${path}/\${i}" >> ${OUTDIR}/netrec.sh
echo "   if [[ -e \${outfile} ]]" >> ${OUTDIR}/netrec.sh
echo "   then" >> ${OUTDIR}/netrec.sh
echo "      gzip -f \${outfile}" >> ${OUTDIR}/netrec.sh
echo "   fi" >> ${OUTDIR}/netrec.sh
echo "   gunzip -c \${outfile}.gz >> \${path}/netrec.out" >> ${OUTDIR}/netrec.sh
echo "done" >> ${OUTDIR}/netrec.sh
echo "results_index=\$(echo \"\${path}\" | awk -F '/results/' '{print length(\$1)}')" >> ${OUTDIR}/netrec.sh
echo "check_path=\${path:0:\${results_index}}" >> ${OUTDIR}/netrec.sh

echo "awk -f \${check_path}/scripts/parse.awk -f \${check_path}/scripts/parse_netrec.awk -v "LINTOL=${LINTOL}" -v "MIPGAP=${MIPGAP}" ${SOLUFILE} \${path}/netrec.out | tee \${path}/netrec.res" >> ${OUTDIR}/netrec.sh

echo "rm -f \${path}/netrec.out" >> ${OUTDIR}/netrec.sh
chmod +x ${OUTDIR}/netrec.sh

if [[ ${MPI} == on ]]
then
   if [[ ${CLUSTER} == on ]]
   then
      bsub -J netrec -q ${QUEUE} -R "span[ptile=${HC}]" -n ${TC} -e %J.err -o %J.out "mpirun ./scripts/mpi/mpiexecline ./${OUTDIR}/netrec.history ${HC} ${JC}"
   else
      # count computer cores
      mpirun -n $(cat /proc/cpuinfo|grep "cpu cores"|uniq | awk '{print $4}') ./scripts/mpi/mpiexecline ./${OUTDIR}/netrec.history ${HC} ${JC}
   fi
fi

if [[ ${CLUSTER} == off ]]
then
   ${CHECKPATH}/${OUTDIR}/netrec.sh
fi
