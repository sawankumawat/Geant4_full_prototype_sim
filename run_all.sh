#!/bin/bash

# for layers in {4..9}; do
#     sed -i "s/constexpr G4int NumberOfLayers = [0-9]\+;/constexpr G4int NumberOfLayers = $layers;/g" source/include/constants.hh
#     sed -i "s/constexpr G4int LayerLayout\[NumberOfLayers\] = {/constexpr G4int LayerLayout[NumberOfLayers] = { 0, /g" source/include/constants.hh


# Skip the inner loop for layer=8
# if [ "$layers" -eq 8 ]; then
#     echo "Skipping energy loop for layer=8"
# else
make
for energy in {1..6}; do
    # Update the energy in the .mac file
    sed -i "s/\/gun\/energy .*/\/gun\/energy ${energy} GeV/" GlobalSetup.mac

    # Run the simulation and send the exit command
    echo exit | ./FoCal_Main

    # Rename hits.root file to hits_0_energy.root
    # mv hits.root simData/hits_${layers-1}_${energy}.root
    mv hits.root simData/ch16_focused/hits_${energy}.root
done
# fi
# done
