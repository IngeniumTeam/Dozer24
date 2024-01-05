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
#define debugMode true

// Servo
#define SERVO_1 7
#define SERVO_2 8
#define SERVO_3 9
#define SERVO_4 10

// Relais
#define RELAIS_1 29
#define RELAIS_1 30

// Motors
//// 1
#define PWMA_1 2
#define PWMB_1 3
#define INA1_1 50
#define INA2_1 51
#define INB1_1 25
#define INB2_1 26
#define STBY_1 21
//// 2
#define PWMA_2 4
#define PWMB_2 5
#define INA1_2 52
#define INA2_2 53
#define INB1_2 27
#define INB2_2 28
#define STBY_2 22

// Stepper
//// 1
#define STEP 6
#define DIR 23
#define EN 24

// LEDs RGB
//// 1
#define RGBA_1 42
#define RGBB_1 37
#define RGBC_1 38
//// 2
#define RGBA_2 45
#define RGBB_2 44
#define RGBC_2 41

// Limit switch
//// 1
#define LMTS_1 36
//// 2
#define LMTS_2 35

// 7 segments digit
#define DIGITA 40
#define DIGITB 39

//                                            left                                                                right                                      mapping               //
//                       _________________________________________________                   _________________________________________________         __________________          //
//                       top                       bottom              stby                  top                       bottom             stby         from            to          //
//              _______________________    _______________________    _______      _______________________    _______________________    _______     ________    _______________   //
Mecanum mecanum(INA2_1, INA1_1, PWMA_1,    INB1_1, INB2_1, PWMB_1,    STBY_1,      INA1_2, INA2_2, PWMA_2,    INB2_2, INB1_2, PWMB_2,    STBY_2,     0, 1023,    0, defaultSpeed); //

#include <Mecaside.h>
Mecaside left(Left);
Mecaside right(Right);

Bluetooth bluetooth(&Serial1);
Report report(&Serial, debugMode, 100);

//Button stepperLimitSwitch(
BlackLineSensor blackLine(A0, A1, A2);

LedRGB bluetoothLed(RGBA_1, RGBB_1, RGBC_1, true);
LedRGB led2(RGBA_2, RGBB_2, RGBC_2, true);
Digit digit(DIGITB, DIGITA, 7);

SingleServo singleExample(SERVO_1, 90, 0);
DoubleServo doubleExample(SERVO_2, SERVO_3, 90, 0, 0, 90);

//AccelStepper stepper1(AccelStepper::DRIVER, 0, 0);
StepperMotor stepper1(STEP, DIR, LMTS_1, false, false, 150, 2); /*, LMTS_2 */

#include "AutoPilot.h"

int estimation = 60;

void setup ()
{
  // Serial setup //
  {
    Serial1.begin(9600);
    Serial.begin(9600);
#if debugMode
    Serial.println("Debug mode is on.");
    Serial.println("Serial communication is on...");
    Serial.println("Bluetooth communication is on...");
#endif
  }
  // Setup and stop the robot  //
  {
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
