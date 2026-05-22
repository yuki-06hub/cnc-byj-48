

#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <AccelStepper.h>

float x_endstop_value = 0;
float y_endstop_value = 0;
float home = 0;
float xmax = 0;
float ymax = 0;
float ciblex = 0;
float cibley = 0;
float pos_x = 0;
float pos_y = 0;

void _loop();
void _delay(float seconds);

void rgb_y(){
  analogWrite(2,255);
  analogWrite(3,120);
  analogWrite(4,0);
}

void rgb_w(){
  analogWrite(2,255);
  analogWrite(3,255);
  analogWrite(4,255);
}

void rgb_off(){
  analogWrite(2,0);
  analogWrite(3,0);
  analogWrite(4,0);
}

void rgb_r(){
  analogWrite(2,255);
  analogWrite(3,0);
  analogWrite(4,0);
}

void rgb_g(){
  analogWrite(2,0);
  analogWrite(3,255);
  analogWrite(4,0);
}

Servo servo_22;

void pendown1(){
  servo_22.write(80);
  _delay(1);
}

void penup(){
  servo_22.write(120);
  _delay(1);
}

void crit_errror(){
  while(1) {
    rgb_off();
    _delay(0.5);

    tone(11,82,250);
    delay(250);

    rgb_r();
    _delay(0.5);

    _loop();
  }
}

// IN1 IN3 IN2 IN4
AccelStepper stepMotor_1(
  AccelStepper::HALF4WIRE,
  36,
  32,
  34,
  30
);

AccelStepper stepMotor_2(
  AccelStepper::HALF4WIRE,
  46,
  42,
  44,
  40
);

void goto_(){

  rgb_y();

  stepMotor_1.moveTo(ciblex);
  stepMotor_2.moveTo(cibley);

  while(stepMotor_1.distanceToGo() != 0 ||
        stepMotor_2.distanceToGo() != 0)
  {
    stepMotor_1.run();
    stepMotor_2.run();

    pos_x = stepMotor_1.currentPosition();
    pos_y = stepMotor_2.currentPosition();

    _loop();
  }
}

void _delay(float seconds) {
  long endTime = millis() + seconds * 1000;

  while(millis() < endTime) {
    _loop();
  }
}

void setup() {

  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);

  servo_22.attach(22);

  pinMode(11,OUTPUT);

  // vitesse
  stepMotor_1.setMaxSpeed(800);
  stepMotor_2.setMaxSpeed(800);

  // accélération
  stepMotor_1.setAcceleration(400);
  stepMotor_2.setAcceleration(400);

  pinMode(50,INPUT_PULLUP);
  pinMode(51,INPUT_PULLUP);

  penup();

  rgb_w();

  xmax = 1430;
  ymax = 1095;

  y_endstop_value = -10;
  x_endstop_value = 10;

  home = 0;

  tone(11,1760,100);
  delay(100);

  // petit recul avant homing
  stepMotor_1.move(-100);
  stepMotor_2.move(100);

  while(stepMotor_1.distanceToGo() != 0 ||
        stepMotor_2.distanceToGo() != 0)
  {
    stepMotor_1.run();
    stepMotor_2.run();
  }

  _delay(1);

  // HOME X
  while(digitalRead(50) != LOW)
  {
    stepMotor_1.move(x_endstop_value);
    stepMotor_1.runToPosition();

    _loop();
  }

  home += 0.5;

  _delay(1);

  // HOME Y
  while(digitalRead(51) != LOW)
  {
    stepMotor_2.move(y_endstop_value);
    stepMotor_2.runToPosition();

    _loop();
  }

  home += 0.5;

  if(home < 1.1){

    rgb_g();

    stepMotor_1.setCurrentPosition(0);
    stepMotor_2.setCurrentPosition(0);

    pos_x = 0;
    pos_y = 0;

    tone(11,1976,250);
    delay(250);
  }

  _delay(1);
}

void _loop() {
}

void loop() {
  _loop();
}
