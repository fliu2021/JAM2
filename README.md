# JAM2
The JAM2 Monte Corla event generator is compiled with PYTHIA8.315.(Please check this on the website of JAM2: https://gitlab.com/transportmodel/jam2 ). To download and install, please make sure you already setup PYTHIA8.315. If not follow the procedures below:

# PYTHIA8.315
If you already have PYTHIA8.315, find the lib directory of it and go the "JAM2 install" section below. 
1. Make a directocry where you would like to put the PYTHIA8, for example:
   ```
   mkdir ~/Event_Genarator
   cd ~/Event_Generator
   ```
2. Now you are in the ~/Event_Generator, download the source code by :
   ```
   curl -O https://pythia.org/download/pythia83/pythia8315.tar.bz2
   tar -xvjf pythia8315.tar.bz2
   rm pythia8315.tar.bz2
   ```
3. Enter the directory of pythia8315
   ```
   cd pythia8315
   ```
4. (This step is not necessarily required, if you don't want to do this step, see step 5 )If you would like to mkdir a install directory for pythia8:
   ```
   ./configure --prefix=~/Event_Generator/pythia8315_install
   make -j16
   make install
   ```
   By doing this you can get a clean directory to put bin, include and libiary.
   
5. If you don't want to make a install directory , directly type:
   ```
   make -j16
   ```
6. You can alway find the bin, include, and lib directory in the
   ```
   ~/Event_Generator/pythia8315.
   ```
   If you do "make install", you can ALSO find bin, include, lib in the
   ```
   ~/Event_Generator/pythia8315_install
   ```

Now you are ready to go to install JAM2 

# JAM2 install 
1. Download source code of JAM2 by:
   ```
   cd ~/Event_Generator
   git clone https://gitlab.com/transportmodel/jam2.git
   cd jam2
   ```
2. Generate configure file by:
   ```
   autoreconf -i
   ```
3. Compile jam2 with pythia8315:
   ```
   ./configure PYTHIA8=~/Event_Generator/pythia8315 CXXFLAGS="-march=native -O3 -pipe -std=c++14"
   make -j16
   ```
   You can also make an install directory for jam2, but often this is not necessary.
4. Goto :
   ```
   cd jam2
   ```
   Find the execuable program "jam".

# RUN JAM2 (for test)
To run JAM2, you need a "jam.inp" as an input file. For a purpose of test, type:

    mv ./input/rqmdv.lambda.inp ./jam.inp
    ./jam 
    
    
You can get 2 events within 5 second.

# RUN JAM2(on a cluster)
1. To simulate JAM2 with lambda potential with a cluster, download the the main.cxx and run.sh in this repo:
   ```
   cd ~/Event_Genarator/jam2/jam2/
   git clone https://github.com/fliu2021/JAM2.git
   mv JAM2/main.cxx ./
   mv JAM2/run.sh ./
   rm -r JAM2
   ```
2. Open the main.cxx and have a look at line
   ```
   string outputFname="/Users/feng/Documents/physics_project/test_EG/JAM2_test_storage/JAM2_sqrt_SNN_7p7_";
   ```
   This is the directory to put the output file, modify this and type:
   ```
   make
   ```
3. Now you get a new execuable program "jam". Make sure the "run.sh" in the same directory as the execuable program "jam", type:
   ```
   chmod u+x run.sh
   ```
4. open the run.sh,have a look at:
   ```
   Main:numberOfEvents = 10 ! number of events to generate
   Beams:eCM =  7.7 ! the center of mass energy
   ```
   For the purpose of test, set the numberOfEvents to be 10, wich takes about 12 seconds. (To run on a cluster, a suggestion for the numberOfEvents per job is 5000 )
 5.If those two parameters are correct, run run.sh by
   ```
   ./run.sh 1
   ```
   Here 1 is the jobnumber. (Now need to input the random seed, because this is done in the run.sh file)
 6. Now you can find the outputfile "JAM2_sqrt_SNN_7p7_1.dat".
 
 7. To run on a cluster, use "run.sh" as the execuable:
    ```
    chmod u+x run.sh
    ./run.sh $(job_number)
    ``` 
     
   
   
















