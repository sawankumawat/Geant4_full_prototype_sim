#!/bin/bash
# script for submitting jobs at 587 alf cluster

for e in 20 40 60 80 100 120 150 200 250 300 350 500 750 1000
do

	sleep 1
	sbatch -N1 -n1 --partition long --job-name="focalstandalone" ./runParticle.sh e+ $e 10000
	sleep 1
	sbatch -N1 -n1 --partition long --job-name="focalstandalone" ./runParticle.sh pi+ $e 10000
	sleep 1
	sbatch -N1 -n1 --partition long --job-name="focalstandalone" ./runParticle.sh proton $e 10000
	sleep 1
	sbatch -N1 -n1 --partition long --job-name="focalstandalone" ./runParticle.sh mu+ $e 10000


done
