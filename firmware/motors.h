#ifndef motors_h
#define motors_h

#include <Arduino.h>
#include <Wire.h>



//?to daniel: tem algum motivo de nao comecar tudo setado em zero ??
int motorsDefaultSpeed=195;
int motorRActive=0;
int motorRspeed=motorsDefaultSpeed;//setado a 195
int motorRTurn=0;
int motorLActive=0;
int motorLspeed=motorsDefaultSpeed;//setado a 195
int motorLTurn=0;






class Motors
{
  private:
    int defaultSpeed;//= 195;
    int LimitSpeedHIGH;//= 200;
    int LimitPIDSpeedLOW;//= 190;
    volatile int16_t PIDSpeedL;
    volatile int16_t PIDSpeedR;
    double tempo;//=0;
    double controletempo;//=0;
    int erro;
    int P;
    int D;
    int I;                                      
    int ultimoL;//=0
    int ultimoR;//=0
  public:

    
    int MOTOR_R;
    int MOTOR_L;
    volatile int counterL;//=0
    volatile int counterR;//=0
    
    void init_Motors(int defSpeed, int SPEEDHIGH, int SPEEDLOW);
    void setSpeed(int SpeedL, int SpeedR, int SPEEDHIGH, int SPEEDLOW);
    void turn_right(int PIDSpeedL, int PIDSpeedR, int SPEEDHIGH, int SPEEDLOW,int angle);
    void turn_left(int PIDSpeedL, int PIDSpeedR, int SPEEDHIGH, int SPEEDLOW,int angle);
    void move_backward(int PIDSpeedL, int PIDSpeedR, int SPEEDHIGH, int SPEEDLOW);
    void move_forward(int SpeedLeft, int SpeedRight, int SPEEDHIGH, int SPEEDLOW);
    void stop();
    void moveRightWheel(int motor, int speed, int direction);
    void moveLeftWheel(int motor, int speed, int direction);
    void control_movement();
    int16_t getSpeedR();
    int16_t getSpeedL();
};
/*
A method to initialize the motor control variables. The parameters are:

7- defSpeed : a default speed to the wheels
14- SPEEDHIGH : how much the speed can vary upwards.
15- SPEEDLOW : how much the speed can vary downwards.
*/
void Motors::init_Motors(int defSpeed, int SPEEDHIGH, int SPEEDLOW)
{

  MOTOR_R=1;
  MOTOR_L=1;
  counterL = 0;
  counterR = 0;
  defaultSpeed = defSpeed;
  PIDSpeedL= defSpeed;
  PIDSpeedR =defSpeed;
  LimitSpeedHIGH =defSpeed + SPEEDHIGH; //200;
  LimitPIDSpeedLOW = defSpeed - SPEEDLOW;//190;
  tempo = 0;
  controletempo = 0;
  P = 0;
  D = 0; 
  I = 0;
  erro=0;                                      
  ultimoL = 0;
  ultimoR = 0;
  pinMode(LBIN1,OUTPUT);
  pinMode(LBIN2,OUTPUT);
  pinMode(LPWMB,OUTPUT);
  pinMode(RAIN1,OUTPUT);
  pinMode(RAIN2,OUTPUT);
  pinMode(RPWMA,OUTPUT);
  //pinMode(pololuAL, INPUT);
  pinMode(pololuBL, INPUT);
  //pinMode(pololuAR, INPUT);
  pinMode(pololuBR, INPUT);
}
/*
This method will set the speed of the wheels and defines what are their max and min values. The parameters are:
1- PIDSpeedL : the speed of the left wheel.
2- PIDSpeedR : the speed of the right wheel.
3- SPEEDHIGH : how much the speed can vary upwards.
4- SPEEDLOW : how much the speed can vary downwards.
*/
void Motors::setSpeed(int SpeedLeft, int SpeedRight, int SPEEDHIGH, int SPEEDLOW)
{
  PIDSpeedL = SpeedLeft;
  PIDSpeedR = SpeedRight;
  LimitSpeedHIGH =defaultSpeed + SPEEDHIGH; //200;
  LimitPIDSpeedLOW = defaultSpeed - SPEEDLOW;//190;
}
/*
This method is used to spin the robot around it's own axis. How much the wheels need to turn is defined by how many tics the pololu encoders have to count. 
As we only have this control, the error of the spin will be big, because we map 360 degrees with aproximally 23 tics. 
The parameters of this methods are:
1- PIDSpeedL : the speed of the left wheel.
2- PIDSpeedR : the speed of the right wheel.
3- SPEEDHIGH : how much the speed can vary upwards.
4- SPEEDLOW : how much the speed can vary downwards.
5- angle : the final angle from the current position.
 */
void Motors::turn_right(int PIDSpeedL, int PIDSpeedR, int SPEEDHIGH, int SPEEDLOW, int angle)// 1,-1 para direita, -1,1 para esquerda
{
   int number_of_ticks =  map(angle, 0,360,0,23);
   if((motorRActive==1) && (motorLActive==1))
   {
      if((1*counterL<number_of_ticks) || (-1*counterR<number_of_ticks))
      {
            moveRightWheel(MOTOR_R, PIDSpeedR, 1);
            moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
      }
      else
      {
            moveRightWheel(MOTOR_R, 0, 0);
            moveLeftWheel(MOTOR_L, 0, 0); 
      }
   }  
}
/*
This method is used to spin the robot around it's own axis. How much the wheels need to turn is defined by how many tics the pololu encoders have to count. 
As we only have this control, the error of the spin will be big, because we map 360 degrees with aproximally 23 tics. 
The parameters of this methods are:
1- PIDSpeedL : the speed of the left wheel.
2- PIDSpeedR : the speed of the right wheel.
3- SPEEDHIGH : how much the speed can vary upwards.
4- SPEEDLOW : how much the speed can vary downwards.
5- angle : the final angle from the current position.
 */
void Motors::turn_left(int PIDSpeedL, int PIDSpeedR, int SPEEDHIGH, int SPEEDLOW,int angle)// 1,-1 para direita, -1,1 para esquerda
{

   int number_of_ticks =  map(angle, 0,360,0,23);
   if((motorRActive==1) && (motorLActive==1))
   {
      if((-1*counterL<number_of_ticks) || (1*counterR<number_of_ticks))
      {
            moveRightWheel(MOTOR_R, PIDSpeedR, 0);
            moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
      }
      else
      {
            moveRightWheel(MOTOR_R, 0, 0);
            moveLeftWheel(MOTOR_L, 0, 0); 
      }
   }  
}
/*
 This method is used to move the robot backward. It has a PD (without an integral) controller that change the speed of the wheels, varying between the defined limits. It's parameters are:
1- PIDSpeedL : the speed of the left wheel.
2- PIDSpeedR : the speed of the right wheel.
3- SPEEDHIGH : how much the speed can vary upwards.
4- SPEEDLOW : how much the speed can vary downwards.
 */
void Motors::move_backward(int PIDSpeedL, int PIDSpeedR, int SPEEDHIGH, int SPEEDLOW)
{
   setSpeed(PIDSpeedL, PIDSpeedR, SPEEDHIGH, SPEEDLOW);
   moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
   moveRightWheel(MOTOR_R, PIDSpeedR, 1);

        if(counterL < counterR)//when the left wheel it's faster then the right..
        {
          if(PIDSpeedL > LimitPIDSpeedLOW) //if the left speed can decrease speed...
          {
            P=(counterL-counterR)*kP;
            //tempo=(millis()-controletempo);
            //D =(counterL-ultimoL)*kD/tempo;
            //I = I + ((counterL-ultimoL) * kI) * tempo;
            PIDSpeedL=PIDSpeedL+P+D+I;
            moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
          }
          else if(PIDSpeedR<LimitSpeedHIGH)//if the right speed can increse speed
          {
            P=(counterR-counterL)*kP;
            //tempo=(millis()-controletempo);
            //D =(ultimoR-counterR)*kD/tempo;
            //I = I + ((ultimoR-counterR) * kI) * tempo;
            PIDSpeedR=PIDSpeedR+P+D+I;
            moveRightWheel(MOTOR_R, PIDSpeedR, 1);
          }
          else// else change both speeds
          {            
            PIDSpeedL=PIDSpeedL+3;//PIDSpeedL+3;
            moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
            PIDSpeedR=PIDSpeedR-3;
            moveRightWheel(MOTOR_R, PIDSpeedR, 1);
          }
        }
        else if(counterR < counterL)//when the right wheel it's faster then the left..
        {
           if(PIDSpeedR > LimitPIDSpeedLOW)//if the right speed can decrease speed...
          {            
            P=(counterR-counterL)*kP;
            //tempo=(millis()-controletempo);
            //D =(counterR-ultimoR)*kD/tempo;
            //I = I + ((counterR-ultimoR) * kI) * tempo;
            PIDSpeedR=PIDSpeedR+P+D+I;
            moveRightWheel(MOTOR_R, PIDSpeedR, 1);
          }
          else if(PIDSpeedL<LimitSpeedHIGH)//if the left speed can increse speed
          {            
            P=(counterL-counterR)*kP;
            //tempo=(millis()-controletempo);
            //D =(ultimoL-counterL)*kD/tempo;
            //I = I + ((ultimoL-counterL) * kI) * tempo;
            PIDSpeedL=PIDSpeedL+P+D+I;
            moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
          }
           else// else change both speeds
          {            
            PIDSpeedL=PIDSpeedL-3;
            moveLeftWheel(MOTOR_L, PIDSpeedL, 0);
            PIDSpeedR=PIDSpeedR+3;
            moveRightWheel(MOTOR_R, PIDSpeedR, 1);
          }
        }
        //controletempo=millis();
        //ultimoR=counterR;
        //ultimoL=counterL;
   
   //I=0;
   //moveRightWheel(MOTOR_R, 0, 1);
   //moveLeftWheel(MOTOR_L, 0, 0); 
}
/*
 This method is used to move the robot forward. It has a PD (without an integral) controller that change the speed of the wheels, varying between the defined limits. It's parameters are:
1- PIDSpeedL : the speed of the left wheel.
2- PIDSpeedR : the speed of the right wheel.
3- SPEEDHIGH : how much the speed can vary upwards.
4- SPEEDLOW : how much the speed can vary downwards.
 */
void Motors::move_forward(int SpeedL, int SpeedR, int SPEEDHIGH, int SPEEDLOW)
{
  /*systemMsg("motorRActive");
  systemMsg(String(motorRActive));
  systemMsg("motorLActive");
  systemMsg(String(motorLActive));*/
        if(counterL > counterR)//when the left wheel is faster than the right..
        {
          if(PIDSpeedL > LimitPIDSpeedLOW) //if the left speed can decrease speed...
          {
            P=(counterR-counterL)*kP;
            //tempo=(millis()-controletempo);
            //D =(ultimoL-counterL)*kD/tempo;
            //I = I + ((ultimoL-counterL) * kI) * tempo;
            PIDSpeedL=PIDSpeedL+P+D+I;
           // moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
          }
          else if(PIDSpeedR < LimitSpeedHIGH)//if the right speed can increase speed...
          {
            P=(counterL-counterR)*kP;
            //tempo=(millis()-controletempo);
            //D =(counterR-ultimoR)*kD/tempo;
            //I = I + ((counterR-ultimoR) * kI) * tempo;
            PIDSpeedR=PIDSpeedR+P+D+I;
            //moveRightWheel(MOTOR_R, PIDSpeedR, 0);
      
          }
          else// else change both speeds
          {
            PIDSpeedL=195;//PIDSpeedL+3;
           // moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
            PIDSpeedR=195;//PIDSpeedR-3;
           // moveRightWheel(MOTOR_R, PIDSpeedR, 0);
          }
        }
        else if(counterR > counterL)//when the right wheel is faster than the left..
        {
           if(PIDSpeedR > LimitPIDSpeedLOW)//if the right speed can decrease speed...
          {
            P=(counterL-counterR)*kP;
            //tempo=(millis()-controletempo);
            //D =(ultimoR-counterR)*kD/tempo;
            //I = I + ((ultimoR-counterR) * kI) * tempo;
            PIDSpeedR=PIDSpeedR+P+D+I;
           // moveRightWheel(MOTOR_R, PIDSpeedR, 0);
          }
          else if(PIDSpeedL<LimitSpeedHIGH)//if the left speed can increase speed...
          {
            P=(counterR-counterL)*kP;
            //tempo=(millis()-controletempo);
            //D =(counterL-ultimoL)*kD/tempo;
            PIDSpeedL=PIDSpeedL+P+D+I;
            //moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
          }
          else// else change both speeds
          {
            PIDSpeedL=195;//PIDSpeedL-3;
            //moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
            PIDSpeedR=195;//PIDSpeedR+3;
            //moveRightWheel(MOTOR_R, PIDSpeedR, 0);
          }
        }
        moveLeftWheel(MOTOR_L, PIDSpeedL, 1);
        moveRightWheel(MOTOR_R, PIDSpeedR, 0);
        //controletempo=millis();
        //ultimoR=counterR;
        //ultimoL=counterL;
   //I=0;
   //moveRightWheel(MOTOR_R, 0, 0);
   //moveLeftWheel(MOTOR_L, 0, 1); 
}

void Motors::moveRightWheel(int motor, int speed, int direction) 
{ 
  boolean inPinR1;
  boolean inPinR2; 
  if(direction == 1) 
  { 
    inPinR1 = HIGH; 
    inPinR2 = LOW; 
  }
  else
  {
    inPinR1 = LOW;
    inPinR2 = HIGH; 
  }

      digitalWrite(RAIN1, inPinR1); 
      digitalWrite(RAIN2, inPinR2);
      analogWrite(RPWMA, speed); 
      digitalWrite(STBY, HIGH); //disable standby
      
}
void Motors::moveLeftWheel(int motor, int speed, int direction) 
{ 
  boolean inPinL1;
  boolean inPinL2; 
  if(direction == 0) 
  { 
    inPinL1 = HIGH; 
    inPinL2 = LOW; 
  }
  else
  {
    inPinL1 = LOW;
    inPinL2 = HIGH; 
  }

  digitalWrite(LBIN1, inPinL1); 
  digitalWrite(LBIN2, inPinL2); 
  analogWrite(LPWMB, speed); 

      
}

void Motors::stop(){
  //enable standby  
  digitalWrite(STBY, LOW); 
}

/*
This function controls how the robot moves. The turn of the wheel indicates if the robot is moving forward, backward,left or right*/
void Motors::control_movement()
{

  if((motorRActive==1) && (motorLActive==1)){
	  if(motorLTurn==1 && motorRTurn==0) //move forward
	  {
	    move_forward(motorLspeed,motorRspeed,10,10);
	    //counterL=0;
	    //counterR=0;
	  }
	  else if ((motorLTurn==0) && (motorRTurn==1))//mofe backward
	  {
	    move_backward(motorLspeed,motorRspeed,10,10);
	    //counterL=0;
	    //counterR=0;
	  }
	  else if(motorLTurn==1 && motorRTurn==1)//turn right
	  {
	    turn_right(150,150,10,10,92);
	    //counterL=0;
	    //counterR=0;
	  }
	  else if(motorLTurn==0 && motorRTurn==0)//turn left
	  {
	    turn_left(150,150,10,10,90);
	    //counterL=0;
	    //counterR=0;
	  }
	}
	else stop();
}

int16_t Motors::getSpeedR(){
	return PIDSpeedR;
}
int16_t Motors::getSpeedL(){
	return PIDSpeedL;
}



Motors m;


/*
 Pin interrupts for the two pololu encoders. When the wheels turns forward, the counter is incremented and when it turns backwards, is decremented.
 */
void pololuL()//interrupcao pro encoder 1
{
  if(digitalRead(pololuBL)==1)
   m.counterL++;
  else
   m.counterL--;
}
void pololuR() //interrupcao pro encoder2
{ 
    if(digitalRead(pololuBR)==1)
     m.counterR--;//counter2++;
    else
     m.counterR++;//counter2--;
}


/*
 A function to setup the environmnet of the motors. This fucntion initialize the motor with the right values and the pin interrupts.
 */
void setup_Environment(int interrup1, int interrup2)
{
  //Serial.begin(9600);
  //AFMS.begin();
  m.init_Motors(motorsDefaultSpeed,10,10);
  attachInterrupt(interrup1, pololuL, FALLING);
  attachInterrupt(interrup2, pololuR, FALLING);
}



void motors_config()
{
  setup_Environment(0,1);
}

void motorsUpdate()
{
  m.control_movement();
}

/*
void loop()
{
  m.control_movement();
  
Serial.print("1-");
Serial.print(m.counterL);
Serial.print(" ");
Serial.println(m.counterR);

motorRActive=0;
motorRTurn=0;
motorLActive=0;
motorLTurn=0;
}*/


/*

void moveBackward(uint8_t speed){
  digitalWrite(STBY, HIGH); //disable standby
  //Left Motor
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);
  //Right Motor
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speed);
}

void moveFoward(uint8_t speed){
  digitalWrite(STBY, HIGH); //disable standby
  //Left Motor
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, speed);
  //Right Motor
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, speed);
}

void rotateLeft(uint8_t speed){
  digitalWrite(STBY, HIGH); //disable standby
  //Left Motor
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, speed);
  //Right Motor
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speed);
}

void rotateRight(uint8_t speed){
  digitalWrite(STBY, HIGH); //disable standby
  //Left Motor
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);
  //Right Motor
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, speed);
}

void stop(){
  //enable standby  
  digitalWrite(STBY, LOW); 
}



*/






void motorsCommandProcess(uint8_t *data);
void setMotorsTurn(uint8_t newMotorRTurn, uint8_t newMotorLTurn);
//void setMotorsSpeed(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed);
void setMotorsActive(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed);
//void motorsUpdate();
//void setNeckServoPos(uint8_t *data);

void motorsCommandProcess(uint8_t *data){ 
    setMotorsTurn(data[0],data[2]); //determina o sentido dos motores 
    if((motorRActive==0) && (motorLActive==0)) m.setSpeed(motorRspeed, motorLspeed, 10, 10); //setMotorsSpeed(data[1],data[3]); //determina a velocidade dos motores
    setMotorsActive(data[1],data[3]); //determina se os motores esta ligado ou nao
}


void setMotorsTurn(uint8_t newMotorRTurn, uint8_t newMotorLTurn){ 
	if(newMotorRTurn==0x0F) motorLTurn=1;
	else motorLTurn=0;

	if(newMotorLTurn==0x0F) motorRTurn=1;
	else motorRTurn=0;
}
/*
void setMotorsSpeed(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed){
	/*motorRspeed = newMotorRSpeed; 
    motorLspeed = newMotorLSpeed;
}*/

void setMotorsActive(uint8_t newMotorRSpeed, uint8_t newMotorLSpeed){ 
    if(newMotorRSpeed) motorRActive = 1;
    else motorRActive =0;
    if(newMotorLSpeed) motorLActive = 1;
    else motorLActive = 0;
}

/*
void motorsUpdate(){
	if(motorRActive){
		if(motorRTurn==1 && motorLTurn==0){
			moveFoward(255);
		}
		else if(motorRTurn==0 && motorLTurn==1){
			moveBackward(255);
		}
		else if(motorRTurn==1 && motorLTurn==1){
			rotateLeft(255);
		}
		else if(motorRTurn==0 && motorLTurn==0){
			rotateRight(255);
		}
	}
	else stop();
}
/*
void setNeckServoPos(uint8_t *data){
	neckServo.write(data[1]);
}*/

#endif