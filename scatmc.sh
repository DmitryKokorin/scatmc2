#!/bin/bash


PHOTONS=10000
SCATTERINGS=10000
SEED=42
H='0.2'


WORK_DIR=output-$(hostname)-starkexp-${H}T-e-pi_2-$(date +%Y%m%d)
LOG_FILE=$WORK_DIR/"diffmc.log"


OEPARTITION_FILE="oepartition_${H}T_starkexp.txt"
EOPARTITION_FILE="eopartition_${H}T_starkexp.txt"
EEPARTITION_FILE="eepartition_${H}T_starkexp.txt"
OFREEPATH_FILE="ofreepath_${H}T_starkexp.txt"
EFREEPATH_FILE="efreepath_${H}T_starkexp.txt"
ECHANNELPROB_FILE="echannel_${H}T_starkexp.txt"
OEXTFUNCTION_FILE="oextfunction_${H}T_starkexp.txt"
EEXTFUNCTION_FILE="eextfunction_${H}T_starkexp.txt"

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
        --photons $PHOTONS \
        --scatterings $SCATTERINGS \
        --seed $SEED \
        2> $LOG_FILE &

tail -f $WORK_DIR/$LOG_FILE
