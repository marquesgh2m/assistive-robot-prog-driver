/*
USAR IDE ARDUINO 1.2 OU SUPERIOR 

 http://forum.arduino.cc/index.php?topic=45742.0 - checksum
 LiquidCrystal Library - Serial Input
 The circuit:
 LCD RS pin to digital pin 12
 LCD Enable pin to digital pin 11
 LCD D4 pin to digital pin 5
 LCD D5 pin to digital pin 4
 LCD D6 pin to digital pin 3
 LCD D7 pin to digital pin 2
 LCD R/W pin to ground
 10K potenciometer to LCD VO 
*/

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "protocol.h"
#include "sensors.h"
#define BUFFER_SIZE 200

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialize the library with the numbers of the interface pins

unsigned char rx_data[BUFFER_SIZE], tx_data[BUFFER_SIZE];
unsigned int rx_data_count, tx_data_count;

Player *client = new Player();
Ranger *ranger_ne = new Ranger(9,10); //sonar/ranger ne(45) n(90) nw(135) sw(225) s(270) se(315)
Ranger *ranger_n = new Ranger(6,7);

unsigned long time, wait;
unsigned long lastTime = 0;


int processCommand(){  //Determines what to do acording the message type
  if(client->readData(rx_data,&rx_data_count)){
    if(rx_data[0]==DIOPACK){
      digitalWrite(13, !digitalRead(13));
    }


  }
  return 0;
}

void update_sensors(){
  time = millis();
  wait = time - lastTime;  

  if(wait >= DIOFRQ){
    lastTime = time;

    tx_data[0]=0x21;
    //tx_data[0] = tx_data[0] | digitalRead(7); //000X
    //tx_data[0] = tx_data[0] | (digitalRead(8) << 1); //00X0   desloca a esquerda o valor lido e faz or com o tx_data. Ex: Lido(01) << 1 = 10

    tx_data_count = 1; //in bytes

    client->writeData(DIOPACK,tx_data,tx_data_count);
  //}
  //if(wait>=RANGERFRQ){
  //  lastTime = time;
    ranger_ne->update();
    ranger_n->update();

    tx_data[0]=ranger_ne->getRange();
    tx_data[1]=ranger_n->getRange();

    tx_data_count = 2; //in bytes
    client->writeData(RANGERPACK,tx_data,tx_data_count);

  }

}

void toPrint(){
  int i;
  lcd.clear();
  lcd.setCursor(0,0);
  for(i=0;i<rx_data_count;i++){
    lcd.write(rx_data[i]);
  }
  lcd.setCursor(0,1);
  lcd.print(ranger_ne->getRange());
  lcd.setCursor(5,1);
  lcd.print(ranger_n->getRange());
}
 
void setup(){
  Serial.begin(115200,SERIAL_8N1); // initialize the serial communications:
  lcd.begin(16,2);  // set up the LCD's number of columns and rows:
  lcd.clear();
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);
}

void loop(){

  processCommand(); //deal with the incoming data from driver

  toPrint(); //print all necessary messages in the default output. (LCD in this case)

  update_sensors();  //read each sensor and write in the driver with the correct message type.

  delay(50); //just a little delay. Do not work very well without it.
}
