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

AccelStepper stepper1(AccelStepper::DRIVER, 8, 10);
//AccelStepper stepper2(AccelStepper::FULL4WIRE, 6, 7, 8, 9);

void setup()
{
  stepper1.setMaxSpeed(200.0);
  stepper1.setAcceleration(100.0);
  stepper1.moveTo(24);

  /*stepper2.setMaxSpeed(300.0);
    stepper2.setAcceleration(100.0);
    stepper2.moveTo(1000000); */
}

void loop()
{
  if (stepper1.distanceToGo() == 0)
    stepper1.moveTo(-stepper1.currentPosition());
  stepper1.run();
  //stepper2.run();
}
