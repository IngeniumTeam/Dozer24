// MultiStepper.pde
// -*- mode: C++ -*-
//
// Shows how to multiple simultaneous steppers
// Runs one stepper forwards and backwards, accelerating and decelerating
// at the limits. Runs other steppers at the same time
//
// Copyright (C) 2009 Mike McCauley
// $Id: MultiStepper.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

AccelStepper stepper1(AccelStepper::DRIVER, 2, 3);
//AccelStepper stepper2(AccelStepper::FULL4WIRE, 6, 7, 8, 9);

const double speed = 500.0;
const int accelTime = 2;

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
  stepper1.setMaxSpeed(speed);
  stepper1.setAcceleration(speed / accelTime);
  stepper1.move(-10000);
  while (digitalRead(A0) == HIGH) {
    stepper1.run();
  }
  Serial.println("stop");
  stepper1.stop();
  stepper1.setCurrentPosition(stepper1.currentPosition());
  stepper1.moveTo(2000);
}

void loop()
{
  //if (stepper1.distanceToGo() == 0)
  //  stepper1.moveTo(10000);
  stepper1.run();
  //stepper2.run();
}
