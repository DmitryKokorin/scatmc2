#!/bin/bash


PHOTONS=10000
SCATTERINGS=10000
SEED=42
H='0.2'
LABEL="starkexp"


WORK_DIR=output-$(hostname)-${LABEL}-${H}T-e-pi_2-$(date +%Y%m%d)
LOG_FILE=$WORK_DIR/"diffmc.log"


OEPARTITION_FILE="oepartition_${H}T_${LABEL}.txt"
EOPARTITION_FILE="eopartition_${H}T_${LABEL}.txt"
EEPARTITION_FILE="eepartition_${H}T_${LABEL}.txt"
OFREEPATH_FILE="ofreepath_${H}T_${LABEL}.txt"
EFREEPATH_FILE="efreepath_${H}T_${LABEL}.txt"
ECHANNELPROB_FILE="echannel_${H}T_${LABEL}.txt"
OEXTFUNCTION_FILE="oextfunction_${H}T_${LABEL}.txt"
EEXTFUNCTION_FILE="eextfunction_${H}T_${LABEL}.txt"
ONORM_FILE="onorm_${H}T_${LABEL}.txt"
ENORM_FILE="enorm_${H}T_${LABEL}.txt"

mkdir -p $WORK_DIR

make clean

OLD="const Float H = ;"
NEW="const Float H = ${H}e+4;"

sed -e 's/'"$OLD"'/'"$NEW"'/g' optics.cpp.template.H > optics.cpp
make


#gdb -ex run --args  \
./diffmc \
        --workdir $WORK_DIR \
        --saveoepartition $OEPARTITION_FILE \
        --saveeopartition $EOPARTITION_FILE \
        --saveeepartition $EEPARTITION_FILE \
        --saveofreepath $OFREEPATH_FILE \
        --saveefreepath $EFREEPATH_FILE \
        --saveechannelprob $ECHANNELPROB_FILE \
        --saveoextfunction ${OEXTFUNCTION_FILE} \
        --saveeextfunction ${EEXTFUNCTION_FILE} \
        --saveonorm ${ONORM_FILE} \
        --saveenorm ${ENORM_FILE} \
        --photons $PHOTONS \
        --scatterings $SCATTERINGS \
        --seed $SEED \
        2> $LOG_FILE &

tail -f $WORK_DIR/$LOG_FILE
