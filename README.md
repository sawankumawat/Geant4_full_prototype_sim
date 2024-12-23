# Program installation

These are needed to run the simulation program:
 * GEANT4
 * ROOT6

It's important that both have been compiled with the same compiler version, otherwise there will be a linking problem when the program tries to create the dictionary for the custom datatype in which some of the data is saved.

To build the program do the following:

```
cd TestbeamSimulation
cmake -DGeant4_DIR=$GEANT4_DIR source/
make
```

Environmental variable ```GEANT4_DIR``` needs to be set to the Geant4 installation directory. This variable is set in ```source/CMakeLists.txt```.

# GEANT4 Version

- We are currently **using GEANT4 version 10.7.3** to perform the FoCal Standalone simulation, and we should be consistent with the GEANT4 version we use for the test beam simulations.
- However, our code is now compatible with both versions 10 and 11.
    - To run the simulation using **GEANT4 version 10**, the line ```#define G4VERSION_NUMBER 1073``` in ```source/include/constants.hh``` should be **uncommented**
    - To run the simulation using **GEANT4 version 11**, the line ```#define G4VERSION_NUMBER 1073``` in ```source/include/constants.hh``` should be **commented out**


# Running & modifying the simulation

Running the simulation is done just by running

```
./FoCal_Main
```
in the main directory.

## Setting number of events, partcle gun, etc.:

In the main directory you'll find ```GlobalSetup.mac```. There you can change for example the event number or which particle is used.

## Changing geometry
There are lots of variables in source/include/constants.hh which change the simulation setup. These might be the most relevant ones:
 * ```UseFoCalE```: swith FoCal-E on/off
 * ```UseFoCalH```: swith FoCal-H on/off
 * ```UseModifiedSetup```
    * if __true__ : use 2021 test beam setup (set ```NumberPAD=1``` and ```NumberPIX=2```, ```NumberW``` is the number of W plates in front of pad and pixels)
    * if __false__: use default setup (set ```NumberOfLayers``` so that it is sum of the pad and pixel layers, also set ```LayerLayout``` correctly)

### Rotate FoCal-H

FoCal-H can be rotated by changing the variable ```constexpr G4double HCAL_Rot_Y``` in ```source/include/constants.hh```:
- In the September 2022 test beams at SPS and PS, FoCal-H was rotated 1 degree
- In the November 2022 test beam at SPS, FoCal-H was rotated 2.085 degrees 


## Visualisation of the setup

Do the following to visualise the setup:
1.  Set ```nameUISession = "qt"```
2.  Uncomment ```ui->SessionStart()```
3.  Uncomment ```/control/execute visSetup_Simplex.mac``` in ```GlobalSetup.mac```



# Other notes

The code is modified from https://github.com/novitzky/FoCalStandalone, https://github.com/hrytkone/FoCalStandalone and FoCal-H part is from https://github.com/nschmidtALICE/fun4all_eicdetectors/tree/focal_TB/simulation/g4simulation/g4focal.
