#!/bin/bash
#first argument is the particle
#second argument is the charge
#third argument is number of events

folder="./data/${1}"
mkdir -p $folder

particle=${1}
energy=${2}
number=${3}

energyline="\/gun\/energy"
numberline="\/run\/beamOn"
particleline="\/gun\/particle"
seedline="\/random\/setSeeds"

timestamp=$(date '+%Y_%m_%d_%H-%M-%S')
energyfolder="${folder}/${energy}GeV/"
mkdir -p $energyfolder
seed1_0=$(date +%s)
seed2_0=$(date +%s)
seed1=${seed1_0: -4}
seed2=${seed2_0: -5}
   

newGlobalSetupFile=${energyfolder}/GlobalSetup_${timestamp}.mac

cp GlobalSetup.mac ${newGlobalSetupFile}

sed -i -E "s/([0-9]+ GeV)/${energy} GeV/" $newGlobalSetupFile
sed -i -E "s/(${numberline} [0-9]+)/${numberline} ${number}/" $newGlobalSetupFile
sed -i -E "s/(${particleline} [A-Za-z+-]+)/${particleline} ${particle}/" $newGlobalSetupFile
sed -i -E "s/(${seedline} [0-9]+ [0-9]+)/${seedline} ${seed1} ${seed2}/" $newGlobalSetupFile

sleep 1

./FoCal_Main ${particle} ${energy} ${timestamp}

echo $timestamp
    #cp hits.root ${energyfolder}/hits_${e}GeV.root
    #cp diagnostic.root ${energyfolder}/diagnostic_${e}GeV.root

