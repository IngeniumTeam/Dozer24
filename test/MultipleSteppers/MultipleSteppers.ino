// MultiStepper.pde
// -*- mode: C++ -*-
//
// Shows how to multiple simultaneous steppers
// Runs one stepper forwards and backwards, accelerating and decelerating
// at the limits. Runs other steppers at the same time
//
// Copyright (C) 2009 Mike McCauley
// $Id: MultiStepper.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

//#include <AccelStepper.h>
#include <StepperMotor.h>

#define STEP 6
#define DIR 23
#define EN 24

#define LMTS_1 36
#define LMTS_2 35

StepperMotor stepper1(STEP, DIR, LMTS_1, false, false, 200, 2);
//AccelStepper stepper1(AccelStepper::DRIVER, STEP, DIR);
//AccelStepper stepper2(AccelStepper::FULL4WIRE, 6, 7, 8, 9);

const double speed = 200.0;
const int accelTime = 2;

void setup()
{
  Serial.begin(9600);
  //stepper1.setMaxSpeed(speed);
  //stepper1.setAcceleration(speed / accelTime);
  stepper1.moveTo(-10000);
}

void loop()
{
  //if (stepper1.distanceToGo() == 0)
  //  stepper1.moveTo(10000);
  stepper1.loop();
  //stepper2.run();
}
