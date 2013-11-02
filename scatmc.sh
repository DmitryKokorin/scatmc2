#!/bin/bash


PHOTONS=10000
SCATTERINGS=10000
SEED=42
H='0.2'
LABEL="starkexp"


WORK_DIR=output-$(hostname)-${LABEL}-${H}T-e-pi_2-$(date +%Y%m%d)
LOG_FILE=$WORK_DIR/"scatmc.log"


OEPARTITION_FILE="oepartition_${H}T_${LABEL}.txt"
EOPARTITION_FILE="eopartition_${H}T_${LABEL}.txt"
EEPARTITION_FILE="eepartition_${H}T_${LABEL}.txt"
OFREEPATH_FILE="ofreepath_${H}T_${LABEL}.txt"
EFREEPATH_FILE="efreepath_${H}T_${LABEL}.txt"
ECHANNELPROB_FILE="echannel_${H}T_${LABEL}.txt"
OESCFUNCTION_FILE="oescfunction_${H}T_${LABEL}.txt"
EESCFUNCTION_FILE="eescfunction_${H}T_${LABEL}.txt"
ONORM_FILE="onorm_${H}T_${LABEL}.txt"
ENORM_FILE="enorm_${H}T_${LABEL}.txt"

mkdir -p $WORK_DIR

make clean

OLD="const Float H = ;"
NEW="const Float H = ${H}e+4;"

sed -e 's/'"$OLD"'/'"$NEW"'/g' optics.cpp.template.H > optics.cpp
make


#gdb -ex run --args  \
./scatmc2 \
        --workdir $WORK_DIR \
        --loadoeprofile $OEPARTITION_FILE \
        --loadeoprofile $EOPARTITION_FILE \
        --loadeeprofile $EEPARTITION_FILE \
        --loadofreepath $OFREEPATH_FILE \
        --loadefreepath $EFREEPATH_FILE \
        --loadeeprobability $ECHANNELPROB_FILE \
        --loadoescfunction ${OESCFUNCTION_FILE} \
        --loadeescfunction ${EESCFUNCTION_FILE} \
        --loadonorm ${ONORM_FILE} \
        --loadenorm ${ENORM_FILE} \
        --photons $PHOTONS \
        --scatterings $SCATTERINGS \
        --seed $SEED \
        2> $LOG_FILE &

tail -f $LOG_FILE
