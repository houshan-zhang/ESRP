#!/bin/bash

CMDFILE=${1}

# post system information
uname -a
cat /proc/cpuinfo | grep 'model name' | uniq | sed 's/model name/CPU/g'

echo "@01 SETTING: ${SETTING}, SEED: ${SEED}"
echo "@02 START TIME: $(date "+%Y-%m-%d %H:%M:%S")"
echo ""
${CHECKPATH}/scripts/run_netrec.sh ${CMDFILE}
retcode=$?
if [[ ${retcode} != "0" ]]
then
   echo ""
   echo "ERROR! run_netrec.sh exit code ${retcode}"
   echo ""
   exit ${retcode}
fi
echo ""
echo "@03 END TIME: $(date "+%Y-%m-%d %H:%M:%S")"
TIMEHOUR=$(echo "scale=1; ${TIMELIMIT}/3600" | bc)
TIMEHOUR=$(printf "%0.1f" $TIMEHOUR)
echo "@04 TIMELIMIT: ${TIMELIMIT} s [${TIMEHOUR} h]"
MEMGB=$(echo "scale=1; ${MEMLIMIT}/1024" | bc)
echo "@05 MEMLIMIT: ${MEMLIMIT} MB [${MEMGB} GB]"
if [[ ${MIPGAP} == "0" ]]
then
   echo "@06 GAPLIMIT: ${MIPGAP} [0.0 %]"
else
   GAPPERCENTAGE=$(echo "scale=1; ${MIPGAP}*100.0" | bc)
   echo "@06 GAPLIMIT: ${MIPGAP} [${GAPPERCENTAGE} %]"
fi
echo "= over ="
