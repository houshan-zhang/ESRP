#!/bin/bash

CMDFILE=${1}

# record the parameter file first, and then set the other parameters separately
echo "${CHECKPATH}/bin/netrec \
   \"SETTING=${CHECKPATH}/settings/${SETTING}.set MODE=${MODE} TIME=${TIMELIMIT} SEED=${SEED} MINGAP=${MIPGAP} MEM=${MEMLIMIT}\"" \
   > ${CMDFILE}

${CHECKPATH}/bin/netrec \
   "SETTING=${CHECKPATH}/settings/${SETTING}.set MODE=${MODE} TIME=${TIMELIMIT} SEED=${SEED} MINGAP=${MIPGAP} MEM=${MEMLIMIT}"

retcode=$?
if [[ ${retcode} != "0" ]]
then
   exit ${retcode};
fi
