#include <Wire.h>
#include <EVShield.h>
#include <EVs_NXTLight.h>
#include <EVShieldAGS.h>


//EV shield variables
EVShield  evshield(0x34, 0x36);
EVs_NXTLight myLight;


//Motor variables
char dir;
int color = 0;
int deg = 100;

//Echo sensor variables
int trigPin = 11; // Trigger
int echoPin = 12; // Echo
long duration; //distance value

void sorter_end() {
  evshield.bank_b.motorRunDegrees(SH_Motor_2, SH_Direction_Reverse, SH_Speed_Full, deg + 80, SH_Completion_Wait_For, SH_Next_Action_Brake);
  delay(500);
  evshield.bank_b.motorRunDegrees(SH_Motor_2, SH_Direction_Forward, SH_Speed_Full, deg + 70, SH_Completion_Wait_For, SH_Next_Action_Brake);
}

int detect() {
  // Can detect the existance of an object, This function uses Pin 12 & 11 of the arduino
  //to read an echo sensor. 12>>encho, 11>>trigger
  //The sensor is triggered
  digitalWrite(trigPin, LOW);
  delayMicroseconds(50);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin, LOW);
  //Read pulse data
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  //Serial.println(duration);
  //Object check
  if (duration <= 500) return (1);
  else return (0);
}

int colorRead() {
  uint16_t light;
  Serial.println('Inside READ');
  while (1) {
    light = myLight.readRaw();
    //Serial.println(light);
    delay(30);
    if (light >= 680) {
      return 1;
    }
    else {
      return 0;
    }
  }
}
int grab_mov() {
  //Moves motor over to sensor
  evshield.bank_a.motorRunDegrees(SH_Motor_2, SH_Direction_Forward, SH_Speed_Full, deg, SH_Completion_Wait_For, SH_Next_Action_Brake);
  //Returns color of block blue = 1 red = 0
  color = colorRead();
  if (color == 1) {
    //Sets the motor back to the belt and releases the object to belt if blue.
    evshield.bank_a.motorRunDegrees(SH_Motor_2, SH_Direction_Reverse, SH_Speed_Full, deg, SH_Completion_Wait_For, SH_Next_Action_Brake);
    delay(200);
    grabber('O');
  }
  else if (color == 0) {
    //Releases the red block into cup, then comes back to belt in open position.
    evshield.bank_a.motorRunDegrees(SH_Motor_2, SH_Direction_Forward, SH_Speed_Full, deg + 80, SH_Completion_Wait_For, SH_Next_Action_Brake);
    delay(10);
    grabber('O');
    evshield.bank_a.motorRunDegrees(SH_Motor_2, SH_Direction_Reverse, SH_Speed_Full, deg + 40, SH_Completion_Wait_For, SH_Next_Action_Brake);
  }

}

void grabber(char Dir) {

  if (Dir == 'O') {
    //Opens gripper for grabing
    evshield.bank_a.motorRunDegrees(SH_Motor_1, SH_Direction_Reverse, SH_Speed_Full, deg, SH_Completion_Wait_For, SH_Next_Action_Brake);

  }

  else if (Dir == 'G') {
    //Close Gripper to catch
    evshield.bank_a.motorRunDegrees(SH_Motor_1, SH_Direction_Forward, SH_Speed_Full, deg, SH_Completion_Wait_For, SH_Next_Action_Brake);
    //Move motor to light sensor
    delay(200);
    grab_mov();

  }



}




void setup() {
  evshield.init(SH_HardwareI2C);
  //Needed for debugging
  Serial.begin(9600);
  //Light sensor setup
  myLight.init(&evshield, SH_BAS1);
  myLight.setReflected();
  //Distance Echo setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //initial opening
  grabber('O');
}

void loop() {
  //Check if block is in front of grabber
    if (1 == detect()) {
      //Open grabber delay 300 and then close.
      delay(300);
      grabber('G');
      Serial.println(detect());
  
    }
 
}


