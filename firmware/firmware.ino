/*
PARA COMPILAR USAR IDE ARDUINO 1.2 OU SUPERIOR 
*/

#include <Arduino.h>
#include "config.h"
#include <Wire.h>
#include <string.h>
#include "alerts.h"
#include "messages.h"
#include "motors.h"
#include "Ranger.h"
#include "bumper.h"
#include <Servo.h>

Servo neckServo;



/*TODO perguntar para o amory se eh uma boa ideia implementar uma logica de bits de status
robot status:
	waiting config file
	connection lost
	critical baterry
*/

uint32_t counter = 0;
uint32_t millisLastTime = 0;
uint32_t millisTimeStamp;

//SENSORS DATA GLOBAL VARIABLES 
uint8_t systemDio_data = 0;


void processCommand(){  
	if(receivedSystemDioCmd()){  //TODO trocar toda infraextrutura para SYSTEM_DIO_CMD //change state of the digital output
		digitalWrite(PIN_LED_BATTERY, cmd.data[0] & 1);             //-------x
		digitalWrite(PIN_LED_CONNECTION, (cmd.data[0] >> 1) & 1); //------x-
		digitalWrite(PIN_VIBRATION_R, (cmd.data[0] >> 2) & 1);    //-----x--
		digitalWrite(PIN_VIBRATION_L, (cmd.data[0] >> 3) & 1);    //----x---
		digitalWrite(PIN_BUZZER, (cmd.data[0] >> 4) & 1);         //---x----
	}
	else if(receivedPingCmd()){
		lostConnectionFlag = 0;
		ping_flag = 0;
		//digitalWrite(PIN_LED_CONNECTION,HIGH);
		//systemMsg("Ping Arduino Received");
	}
	else if(receivedVelCmd()){
		//m.control_movement();
		motorsCommandProcess(cmd.data);
	}
	else if(receivedServoCmd()){
		//TODO --definir as configuracoes do servo--if(cmd.data[0]==SCAN//SETPOS//GOTO(POS,VEL))
		//setNeckServoPos(cmd.data);
		neckServo.write(cmd.data[1]);
	}

	
	
	clearCommand();
}

void updateActuators(){
	//motorsCommandProcess(B10011001);
	if(counter % UPDATE_MOTORS_FRQ == 0){
		motorsUpdate();   //update pid
	}
	/*if(counter % UPDATE_SERVOS_FRQ == 0){
		neckServo.write(pos);
	}*/
}

void updateSensors(){
	if(counter % UPDATE_DIO_FRQ == 0){
		systemDio_data = 0; //0000 0000
		systemDio_data = systemDio_data | digitalRead(PIN_LED_BATTERY); //0000 000X
		systemDio_data = systemDio_data | (digitalRead(PIN_LED_CONNECTION) << 1); //0000 00X0   desloca a esquerda o valor lido e faz or com o tx_data. Ex: Lido(01) << 1 = 10
		systemDio_data = systemDio_data | (digitalRead(PIN_VIBRATION_R) << 2);
		systemDio_data = systemDio_data | (digitalRead(PIN_VIBRATION_L) << 3);
		systemDio_data = systemDio_data | (digitalRead(PIN_BUZZER) << 4);
	}
	
	if(counter % UPDATE_RANGER_FRQ == 0){
		ranger_update();
	}

	if(counter % 200 == 0){
		battery_update();
	}

	bumper_update();
	//motorsUpdate();   //update pid
}


void sendData(){
	int i;

	if(counter % SEND_DIO_FRQ == 0){
		sendSystemDioMsg(systemDio_data,5); //value(ex: 00 1110 0011), bitfield(ex: 10)
	}
	if(counter % SEND_RANGER_FRQ == 0){
		sendRangerMsg(range,6);
	}
	if(counter % SEND_BUMPER_FRQ == 0){
		sendBumperMsg(bumper,6);
	}
	if(counter % SEND_BATTERY_FRQ == 0){
		sendPowerMsg(battery);
		int rawValue = analogRead(PIN_BATTERY); //read the analogic input
		float volts = map(rawValue,ANALOG_EQUIV_EMPTY,ANALOG_EQUIV_FULL,BATTERY_EMPTY*10,BATTERY_FULL*10)/10.0;
		//systemMsg("Analog Read:");
		//systemMsg(String(rawValue));
		//systemMsg(String(volts));
	}
	if(counter % SEND_PING_FRQ == 0){
		if(ping_flag){
			sendPing();
			lostConnectionFlag = 1;
		}
		else {
			sendPing();
			ping_flag = 1;
		}
	}
	if(counter % SEND_ENCODER_FRQ == 0){
		sendEncoderMsg(m.counterR,m.counterL,m.getSpeedR(),m.getSpeedL());
		//systemMsg(String("Enc_R:")+String(m.counterR));
		//systemMsg(String("Enc_L:")+String(m.counterL));
	}
	if(counter % SEND_SYSTEMMSG_FRQ == 0){
		//systemMsg("Mensagem default");
		//convertToUint8_t(battery,2);
	}
	//if(counter % SEND_RANGER_FRQ == 0) sendEncoderTicks();
}

void driver_config(){
	waitingConfigFlag = 1;
	//blink fast when no config received yet
	do{
		if(counter % SEND_REQUESTCONFIG_FRQ == 0) sendRequestConfig();
		readCommand();
		battery_update();
		updateAlerts();
		updateCounter(); 
	}while(!receivedConfigCmd());
	waitingConfigFlag = 0;
	//updateConfigParameters()
}


void setup(){
	protocol_config();
	status_config();
    driver_config(); //after status_config beacuse led connection
	ranger_config();
	bumper_config();
	motors_config();

	neckServo.attach(PIN_SERVO_NECK);

}

void updateCounter(){
  millisTimeStamp = millisTimeStamp + (micros() - millisLastTime); 
  if(millisTimeStamp >= 1000){
		++counter;  //each counter means 1ms
		millisTimeStamp = millisTimeStamp - 1000; //discont the microsseconds that overload timeStamp
		millisLastTime = micros();
  }
}


void loop(){
	//deal with the incoming data from driver
	readCommand();

	//Determines what to do acording the message type
	processCommand(); 
	
	//read each sensor and write in the driver with the correct message type.
	updateSensors();  

 	//update leds, vibs and buzzer indicators
	updateAlerts();

	//send internal data to driver
	sendData();

	//update the moviment of motors.
	updateActuators();

	//increment the counter each 1ms (1000us)
	updateCounter(); 
}
