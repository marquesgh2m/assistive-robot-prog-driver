
/*
This program can send a diocmd to arduino and can receive some ports states from arduino.
*/

#include <iostream>
#include <string>
#include <libplayerc++/playerc++.h>
#include <libplayercore/playercore.h>
#include <time.h>
#include <sys/time.h>

using namespace PlayerCc;
using namespace std;


int motorActiveFlag = 0; //debug
int lastXPos = 0;

int main(int argc, char *argv[]){
	int i,n;
	unsigned char value;
	string option;
	string host;
	int port;
	//default arguments
	host = "localhost"; 
	port = 6665;


	//Arguments treatment
	for(i=1;i<argc;i++){      
		if(argv[i][0]=='-'&argv[i][1]=='h'){
			host = argv[i+1];  
			i=i+1;
		}
		else if(argv[i][0]=='-'&argv[i][1]=='p'){
			port = atoi(argv[i+1]);  //convert to int
			i=i+1;
		}
		else {
			cout << endl << "./main [options]" << endl;
			cout << "Where [options] can be:" << endl;
			cout << "  -h <ip>        : host ip where Player is running.Default: localhost" << endl;
			cout << "  -p <port>      : port where Player will listen. Default: 6665" << endl << endl;
			return -1;
		}
	}

	PlayerClient    robot(host,port);
	DioProxy systemDio(&robot,0);
	RangerProxy myranger(&robot,0);
	Position2dProxy mymotors(&robot, 1);
	Position2dProxy neckServo(&robot, 0);
	BumperProxy mybumper(&robot, 0);
	PowerProxy mypower(&robot, 0);

	cout << "Client starts! " << endl;

	for(;;){
		system("clear");
		cout << endl << "Insert an option: " << endl;
		cout << "t - togle power led" << endl;
		cout << "s - 3 pulses system dio" << endl;
		cout << "d - print dio data" << endl;
		cout << "r - print rangers data" << endl;
		cout << "l - rangers print loop" << endl;
		cout << "m - Enter motors speed" << endl;
		cout << "o - Make a square" << endl;
		cout << "a - all proxy pos2D information" << endl;
		cout << "pf x - para frente x passos" << endl;
		cout << "pt x - para tras x passos" << endl;
		cout << "pd x - rotaciona direita x angulos" << endl;
		cout << "pe x - rotaciona esquerda x angulos" << endl;
		//cout << "ss - set speed" << endl;
		cout << "b - bumpers print loop" << endl;
		cout << "v - power print loop" << endl;
		cout << "p - position 2D" << endl;
		cout << "n - neck position" << endl;
		cout << "g - getPos" << endl;
		cout << "q - quit" << endl;

		cin >> option;
		
		if(option=="t"){
			if(value)value = value & ~(1 << 1); //XXXXXX0X 
			else value = value | (1 << 1); //XXXXXX1X;
			systemDio.SetOutput(8,value); //length (do_count) and decimal value (value)

			for(i=7;i>=0;i--)cout << ((value >> i) & 1); //show value as binary
			cout << endl;

			robot.Read(); //update proxies
		}
		else if(option=="s"){
				value=0;
				for(n=0;n<5;n++){
					value = value | (1 << n); //XXXXXX1X;
					systemDio.SetOutput(8,value); //length (do_count) and decimal value (value)

					for(int j=7;j>=0;j--)cout << ((value >> j) & 1); //show value as binary
						cout << endl;

					robot.Read(); //update proxies
				}
				sleep(1);
				for(n=0;n<5;n++){
					value = value & ~(1 << n); //XXXXXX0X 

					systemDio.SetOutput(8,value); //length (do_count) and decimal value (value)

					for(int j=7;j>=0;j--)cout << ((value >> j) & 1); //show value as binary
						cout << endl;

					robot.Read(); //update proxies
				}
				sleep(1);

		}
		else if(option=="d"){
			robot.Read(); //update proxies
			cout << "Dio data: "; 
			value = systemDio.GetDigin(); //returns the decimal value stored in the systemDio (DioProxy).
			cout << value << endl;
		}
		else if(option=="r"){
			robot.Read(); //update proxies
			cout << "Range 0: "; 
			cout << myranger[0] << endl;
			cout << "Range 1: "; 
			cout << myranger[1] << endl;
		}
		else if(option=="l"){
			for(;;){
				robot.Read(); //update proxies
			   for(i=0;i<myranger.GetRangeCount();i++){  //print all lasers receiveds
				cout << myranger[i] << " "; 
			   }
			   cout << endl;
			   //cout << myranger << endl; //print all proxy information

				sleep(0.5);
			}
		}
		else if(option=="m"){
			unsigned char motorsSpeed = 0;
			char rawMotorsSpeed[8];
			for(;;){
				motorsSpeed = 0;
				robot.Read(); //update proxies
				cout << "Enter motors speed:" << endl << "TLLLTRRR" << endl; 
				cin >> rawMotorsSpeed;
				/*
				if(rawMotorsSpeed[0]=="s"){
					systemDio.SetOutput(8,0);
					robot.Read(); //update proxies
					break;
				}*/


				for(i=0;i<8;i++){
					motorsSpeed = motorsSpeed | (unsigned char)rawMotorsSpeed[7-i]-48 << i;
				}
				cout << "rawMotorsSpeed " << rawMotorsSpeed << endl;
				cout << "motorsSpeed " << (int)motorsSpeed << endl;
				cout << "motorsSpeed(hex) " << hex << (int)motorsSpeed << endl; 

				//systemDio.SetOutput(8,motorsSpeed); //length (do_count) and decimal value (value)
				systemDio.SetOutput(8,motorsSpeed);
				robot.Read(); //update proxies
			}
		}
		else if(option=="o"){
			for(i=0;i<4;i++){
				systemDio.SetOutput(8,0x77); //01110111 foward
				robot.Read(); //update proxies
				sleep(3);

				systemDio.SetOutput(8,0x7f); //01111111 turn
				robot.Read(); //update proxies
				sleep(1.1);
			}
			systemDio.SetOutput(8,0); //stop
			robot.Read(); //update proxies
		}
		/* teste para ver como funciona o go to e o request geometri (ainda nao funciona bem)
		else if(option=="p"){
			double forwardSpeed, turnSpeed;
			forwardSpeed = 42;
			turnSpeed = 57;

			//robot.Read(); //update proxies
			mymotors.SetSpeed(forwardSpeed, dtor(turnSpeed)); //dtor() convert to radians ex: 180º = 3,14 rad

			mymotors.GoTo(2.0,3.0,5.0);


			player_pose3d_t p3d;
			p3d = mymotors.GetOffset();

			cout << "X[m]" << mymotors.GetXPos()
					<< " Y[m]" << mymotors.GetYPos()
					<< " yaw[rad]" << mymotors.GetYaw() << endl;

			mymotors.RequestGeom();

			cout << "X[m]" << mymotors.GetXPos()
					<< " Y[m]" << mymotors.GetYPos()
					<< " yaw[rad]" << mymotors.GetYaw() << endl;

			p3d = mymotors.GetOffset();
			cout << "X[m]" << p3d.px 
					<< " Y[m]" << p3d.py 
					<< " Z[m]" << p3d.pz 
					<< " roll[rad]" << p3d.proll 
					<< " pitch[rad]" << p3d.ppitch 
					<< " yaw[rad]" << p3d.pyaw << endl;

		}*/
		else if(option=="o"){
			robot.Read();
			cout << mymotors << endl;
		}
		else if(option.at(0)=='p'){
			string cmdValue;
			cin >> cmdValue;

			float linear_default_vel = 0.04; //[m/s]

			if(option.at(1)=='f')
				mymotors.SetSpeed(linear_default_vel,0); //mymotors.SetSpeed(atoi(cmdValue.c_str()),0);
			else if (option.at(1)=='d')
				mymotors.SetSpeed(0,linear_default_vel);
			else if (option.at(1)=='e')
				mymotors.SetSpeed(0,-linear_default_vel);
			else if (option.at(1)=='t')
				mymotors.SetSpeed(-linear_default_vel,0);


			int aux = atoi(cmdValue.c_str());

			int atualXPos = mymotors.GetXPos();
			lastXPos = atualXPos;
			while(true){
				robot.Read();
				atualXPos = mymotors.GetXPos();
				if(option.at(1)=='f'       && (atualXPos-lastXPos)>=aux) break;
				else if (option.at(1)=='t' && (lastXPos-atualXPos)>=aux) break;
			}
			mymotors.SetSpeed(0,0); //stop motor
			cout << "X[m]" << mymotors.GetXPos() << endl;
			//sleep(atoi(cmdValue.c_str()));
		
		}
		else if(option.at(0)=='n'){
			string cmdValue;
			if(option.size()==1){ //evita comandos sem espaço ex: "n180" ao inves de "n 180"
				cin >> cmdValue;
				neckServo.SetSpeed(0,atoi(cmdValue.c_str()));		
			}
		}
		else if(option.at(0)=='g'){
			robot.Read(); //sem o read nao atualiza os proxies que fizeram publish
			cout << "X[m]" << mymotors.GetXPos()
					<< " Y[m]" << mymotors.GetYPos()
					<< " yaw[rad]" << mymotors.GetYaw() << endl;
		}
		else if(option=="b"){
			string bumperNames[] = {"NE","N ","NW","SW", "S ", "SE"};
			for(;;){
				robot.Read(); //update proxies
				cout << "Bumpers count:" << mybumper.GetCount() << endl;
				if(mybumper.IsAnyBumped()){
					for(i=0;i<mybumper.GetCount();i++){  //print all lasers receiveds
						cout << bumperNames[i] << "  " << (mybumper.IsBumped(i) ? '1' : '0') << endl;
					}
					cout << endl;
				}
				else cout << "None bumpers had been bumped." << endl << endl;

				sleep(0.5);
			}
		}
		else if(option=="v"){
			for(;;){
				robot.Read(); //update proxies
				
				cout << "Power charge:" << mypower.GetCharge() << endl;
				cout << "Power percent:" << mypower.GetPercent() << endl;
				cout << "Power valid:" << mypower.IsValid() << endl << endl;


				sleep(0.5);
			}
		}
		else if(option=="q") break;

		cout << "option: " << option << endl;
		cout << "motor: " << motorActiveFlag << endl;
	}

	printf("\n\n");
	return 0;
}
