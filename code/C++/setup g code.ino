#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Stepper.h>

float x_endstop_value = 0;
float y_endstop_value = 0;
float home = 0;
float xmax = 0;
float ymax = 0;
float ciblex = 0;
float cibley = 0;
float pos_x = 0;
float pos_y = 0;

Servo servo_22;

// 28BYJ-48
Stepper stepper_1(2048, 30, 32, 34, 36);
Stepper stepper_2(2048, 40, 42, 44, 46);

void _loop() {
}

void _delay(float seconds) {
  long endTime = millis() + seconds * 1000;

  while (millis() < endTime) {
    _loop();
  }
}

// =========================
// FONCTIONS
// =========================

void pendown1() {
  servo_22.write(80);
  _delay(1);
}

void pendown0() {
  servo_22.write(120);
  _delay(1);
}

void goto_() {

  // AXE X
  if (pos_x < ciblex) {

    while (pos_x != ciblex) {

      stepper_1.step(-10);
      pos_x += 10;
    }
  }

  if (pos_x > ciblex) {

    while (pos_x != ciblex) {

      stepper_1.step(10);
      pos_x -= 10;
    }
  }

  // AXE Y
  if (pos_y < cibley) {

    while (pos_y != cibley) {

      stepper_2.step(10);
      pos_y += 10;
    }
  }

  if (pos_y > cibley) {

    while (pos_y != cibley) {

      stepper_2.step(-10);
      pos_y -= 10;
    }
  }
}

// =========================
// SETUP
// =========================

void setup() {

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  pinMode(11, OUTPUT);

  pinMode(50, INPUT_PULLUP);
  pinMode(51, INPUT_PULLUP);

  servo_22.attach(22);

  stepper_1.setSpeed(10);
  stepper_2.setSpeed(10);

  pendown0();

  xmax = 1430;
  ymax = 1095;

  y_endstop_value = -10;
  x_endstop_value = 10;

  home = 0;

  // Bip démarrage
  tone(11, 1760, 100);
  delay(100);

  // Décalage initial
  stepper_1.step(-120);
  stepper_2.step(120);

  _delay(1);

  // HOME X
  while (digitalRead(50) == HIGH) {

    stepper_1.step(x_endstop_value);
  }

  home += 0.5;

  _delay(1);

  // HOME Y
  while (digitalRead(51) == HIGH) {

    stepper_2.step(y_endstop_value);
  }

  home += 0.5;

  // Home OK
  if (home >= 1.0) {

    pos_x = 0;
    pos_y = 0;

    tone(11, 1976, 250);
    delay(250);
  }

  _delay(1);
}

// =========================
// LOOP
// =========================

void loop() {

}
