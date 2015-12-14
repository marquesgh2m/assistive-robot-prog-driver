##### To Install this driver #####
1. copy the driver folder  to "player-3.0.2/examples/plugins/"
2. open the directory to "player-3.0.2/build/" in a terminal     
Obs: if you don't have the build directory look for the tutorial "How to compile Player"
3. add the line ADD_SUBDIRECTORY (donnie) in the end of the file  "player-3.0.2/examples/plugins/CMakeLists.txt"
4. make
5. sudo make install

##### To run this example:####

In one terminal:
1. $player server_donnie.cfg

In another terminal:
1. cd client/
2. $make
3. $make run 

