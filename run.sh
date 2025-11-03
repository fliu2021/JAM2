#!/bin/bash
 
INPUTFILE="jam_$1.inp"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)" 
cd $SCRIPT_DIR


echo "make directory: playground"
mkdir playground

cd playground

echo "Main:numberOfEvents = 10 ! number of events to generate

Random:setSeed = on
Random:seed = "$(( $(printf %d 0x$(xxd -l 3 -ps -c 10 /dev/urandom)) + $1 ))"

Beams:beamA = 197Au
Beams:beamB = 197Au
Beams:eCM =  7.7
Beams:bmin = 0.0
Beams:bmax = 14.0


Cascade:TimeStepSize = 0.2
Cascade:TimeStep = 200

# Lambda potential type
#MeanField:optLambdaPotential=0  # same as nucleons x 2/3
MeanField:optLambdaPotential=5  # GWK3
#MeanField:optLambdaPotential=5  # GWK2
#MeanField:optLambdaPotential=7  # GWK3+MD2
#MeanField:optLambdaPotential=19  # GWK3+MD3

# set mean-field mode
MeanField:mode=14
MeanField:EoS= 12  # nucleon EoS: MS2
MeanField:twoBodyDistance = 3
MeanField:twoBodyDistanceMD = 3
MeanField:gaussWidth = 2.0

# factor for lambda potential in case MeanField:optLambdaPotential=0
MeanField:factorAlphaPotentialL=0.666666
MeanField:factorBetaPotentialL=0.666666
MeanField:factorGammaPotentialL=0.666666
MeanField:factorAttMomPotentialL=0.666666
MeanField:factorRepMomPotentialL=0.666666


# option for final decay of hadrons
111:mayDecay = off    ! pi0
221:mayDecay = off    ! eta
311:mayDecay = off    ! K0
321:mayDecay = off    ! K+
#333:mayDecay = off    ! phi
3122:mayDecay = off   ! Lambda0
3212:mayDecay = off   ! Sigma0
3112:mayDecay = off   ! Sigma-
3222:mayDecay = off   ! Sigma+
3322:mayDecay = off   ! Xi0
3312:mayDecay = off   ! Xi-
3334:mayDecay = off   ! Omega-

" > $INPUTFILE

cd ../

./jam -j $1





