#include <Bluetooth.h>
#include <Mecanum.h>
#include <Report.h>
#include <BlackLineSensor.h>
#include <Led.h>
#include <Cherry.h>
#include <Digit.h>
#include <Timino.h>
#include <StepperMotor.h>

#define loopTime 20
#define defaultSpeed 230
#define diagonalThreshold 75
#define debugMode false

#define SERVO_1 13
#define SERVO_2 12
#define SERVO_3 11
#define SERVO_4 10
#define SERVO_5 9
#define SERVO_6 8
#define SERVO_7 7
#define SERVO_8 6

//                           left                              right                    mapping                 //
//                 __________________________        __________________________       ____________              //
//                 top        bottom     stby        top        bottom    stby       from       to              //
//              _________    _________    __      _________    _________    _       _______   ______            //
Mecanum mecanum(35, 34, 4,   39, 38, 2,   25,     36, 37, 3,   32, 33, 5,   7,      0, 1023,  0, defaultSpeed); //
//             in1,in2,pwm  in1,in2,pwm          in1,in2,pwm, in1,in2,pwm           min,max   min,max           //

#include <Mecaside.h>
Mecaside left(Left);
Mecaside right(Right);

Bluetooth bluetooth(&Serial1);
Report report(&Serial, debugMode, 100);

//Button stepperLimitSwitch(
BlackLineSensor blackLine(A0, A1, A2);

LedRGB bluetoothLed(28, 27, 26, true);
LedRGB led2(31, 30, 29, true);
Digit digit(49, 48, 7);

SingleServo singleExample(SERVO_1, 90, 0);
DoubleServo doubleExample(SERVO_2, SERVO_3, 90, 0, 0, 90);

//AccelStepper stepper1(AccelStepper::DRIVER, 0, 0);
StepperMotor stepper1(0, 0, 0, true, true);

#include "AutoPilot.h"

int estimation = 60;

void setup ()
{
  // Serial setup //
  {
    Serial1.begin(9600);
#if debugMode
    Serial.begin(9600);
    Serial.println("Debug mode is on.");
    Serial.println("Serial communication is on...");
    Serial.println("Bluetooth communication is on...");
#endif
  }
  // Setup and stop the robot  //
  {
    pinMode(50, OUTPUT);
    digitalWrite(50, LOW); // The ground pin of the digit
    digit.display(estimation);
    singleExample.setup();
    singleExample.open();
    doubleExample.setup();
    stepper1.setup();
    stop();
#if debugMode
    Serial.println("All systems are running.");
#endif
  }
}

void loop ()
{
  stepper1.loop();
  report.print();
  if (bluetooth.receive())
  {
    if (bluetooth.lastError == DeserializationError::Ok)
    {
      report.ok++;
      report.prob = 0;
      bluetoothLed.on(0, 0, 255);
      {
#if debugMode
        Serial.print("estimation: "); Serial.println(bluetooth.json["estimation"].as<int>()); Serial.println();
#endif
        if (bluetooth.json["estimation"].as<int>() != -1 && bluetooth.json["estimation"].as<int>() != estimation) {
          estimation = bluetooth.json["estimation"].as<int>();
          digit.display(estimation);
        }
      }
      // Switch //
      {
#if debugMode
        Serial.print("switch: "); Serial.println(bluetooth.json["switch"].as<bool>()); Serial.println();
#endif
        singleExample.move(bluetooth.json["switch"].as<bool>());
      }
      // Keypad //
      {
#if debugMode
        Serial.print("key: "); Serial.println(bluetooth.json["keypad"].as<int>()); Serial.println();
#endif
        switch (bluetooth.json["keypad"].as<int>())
        {
          case 1:
            stepper1.moveTo(2000);
            //doubleExample.open();
            break;
          case 2:
            break;
          case 3:
            //singleExample.toggle();
            break;
          case 4:
            break;
          case 5:
            break;
          case 6:
            break;
          case 7:
            break;
          case 8:
            break;
          case 9:
            break;
          case 10:
            break;
          case 11:
            stop();
            break;
        }
      }
      // Motors //
      {
#if debugMode
        Serial.print("y.l: "); Serial.println(bluetooth.json["joysticks"]["left"]["y"].as<int>());
        Serial.print("y.r: "); Serial.println(bluetooth.json["joysticks"]["right"]["y"].as<int>()); Serial.println();
        Serial.print("x.l: "); Serial.println(bluetooth.json["joysticks"]["left"]["x"].as<int>());
        Serial.print("x.r: "); Serial.println(bluetooth.json["joysticks"]["right"]["x"].as<int>()); Serial.println(); Serial.println();
        
        Serial.print("left: "); Serial.println(constrain(bluetooth.json["joysticks"]["right"]["y"].as<int>() + bluetooth.json["joysticks"]["left"]["x"].as<int>(), -1023, 1023));
        Serial.print("right: "); Serial.println(constrain(bluetooth.json["joysticks"]["right"]["y"].as<int>() + -(bluetooth.json["joysticks"]["left"]["x"].as<int>()), -1023, 1023)); Serial.println();
        Serial.print("sideway: "); Serial.println(bluetooth.json["joysticks"]["left"]["x"].as<int>());
        Serial.print("diagonal: "); Serial.println(bluetooth.json["joysticks"]["right"]["x"].as<int>(), bluetooth.json["joysticks"]["right"]["y"].as<int>()); Serial.println(); Serial.println();
#endif
        // Simple
        {
          left.move(constrain(bluetooth.json["joysticks"]["right"]["y"].as<int>() + bluetooth.json["joysticks"]["left"]["x"].as<int>(), -1023, 1023));
          right.move(constrain(bluetooth.json["joysticks"]["right"]["y"].as<int>() + -(bluetooth.json["joysticks"]["left"]["x"].as<int>()), -1023, 1023));
        }
        // Others
        {
          mecanum.sideway(bluetooth.json["joysticks"]["right"]["x"].as<int>());
          if (abs(bluetooth.json["joysticks"]["right"]["x"].as<int>()) > diagonalThreshold && abs(bluetooth.json["joysticks"]["right"]["y"].as<int>()) > diagonalThreshold) {
            mecanum.diagonal(bluetooth.json["joysticks"]["right"]["x"].as<int>(), bluetooth.json["joysticks"]["right"]["y"].as<int>());
          }
        }
      }
      /*  // Response //
        {
        bluetooth.json.clear();
        bluetooth.json["blackLine"]["pattern"] = blackLine.getPattern();
        bluetooth.json["blackLine"]["onTheLine"] = blackLine.lastPattern == Position.Pattern.OnTheLine;
        bluetooth.send();
        }*/
    }
    else
    {
      report.inv++;
      report.prob++;
      bluetooth.empty();
      bluetoothLed.on(255, 0, 0);
    }
  }
  else
  {
    report.ntr++;
    report.prob++;
    bluetooth.empty();
    bluetoothLed.off();
  }
  if (report.prob >= 10)
  {
    stop();
  }
  delay(loopTime);
}

void stop ()
{
#if debugMode
  Serial.println("stop"); Serial.println();
#endif
  mecanum.stop();
}
