# JAM2
The JAM2 Monte Corla event generator is compiled with PYTHIA8.315.(Please check this on the website of JAM2: https://gitlab.com/transportmodel/jam2 ). To download and install, please make sure you already setup PYTHIA8.315. If not follow the procedures below:

#PYTHIA8.315
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
   make -j8
   ```
6. You can alway find the bin, include, and lib directory in the ~/Event_Generator/pythia8315.
   If you do "make install", you can also find bin, include, lib in the "~/Event_Generator/pythia8315_install"

   Anyway, you can always get the complile command by running the bin.

Now you are ready to go to install JAM2 

#JAM2 install 
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
   Find the execuable executable "jam".

#RUN JAM2 (for test)
To run JAM2, you need a "jam.inp" as an input file. For a purpose of test, type:
    ```
    mv ./input/jamtest.inp ./jam.inp
    ./jam 
    ```   
You can get 2 event2 within 5 second. 
#RUN JAM2(on a cluster)
