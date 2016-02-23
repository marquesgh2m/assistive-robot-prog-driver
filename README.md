#### To Install this driver ####
.1. open the player drivers directory and clone the repository driver
```bash
$ cd ~/player-3.0.2/examples/plugins/
$ git clone https://github.com/marquesgh2m/assistive-robot-prog-driver.git donnie
```
.2. add the line ADD_SUBDIRECTORY (donnie) in the end of the file  "player-3.0.2/examples/plugins/CMakeLists.txt"
```bash
$ cd ~/player-3.0.2/examples/plugins/CMakeLists.txt
$ echo "ADD_SUBDIRECTORY (donnie)" >> test.txt
```

.3. Open the directory to "player-3.0.2/build/" in a terminal and compile Player:
```bash  
$ cd ~/player-3.0.2/build/
$ $make
$ $sudo make install
```
  Obs: if you don't have the build directory look for the tutorial "How to compile Player"

#### To run this example:####

.1. In one terminal (In the Raspberry):
```bash
$ cd ~/player-3.0.2/examples/plugins/donnie
$ player server_donnie.cfg
```

.2. In another terminal (In the PC):
 ```bash
$ cd ~/player-3.0.2/examples/plugins/donnie/client
$ make
$ make run 
```

#### If you are using another serial port: ####
.1. Change it editing the port in "server_donnie.cfg" file:
```bash
$ nano server_donnie.cfg
```
And change the port, default port is "/dev/ttyAMA0".
Ctrl+X to exit nano, Y to save and Enter to confirmate the name.


