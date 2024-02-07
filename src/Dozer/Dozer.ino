#include <Bluetooth.h>
#include <Mecanum.h>
#include <Report.h>
#include <BlackLineSensor.h>
#include <Led.h>
#include <Cherry.h>
#include <Digit.h>
#include <Timino.h>
#include <StepperMotor.h>

#define LOOP_TIME 10
#define DEFAULT_SPEED 200
#define DIAGONAL_THRESHOLD 75
#define DEBUG false

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

#define SWITCH 0
#define KEYPAD 1
#define ESTIMATION 2
#define JOYSTICK_LEFT_X 3
#define JOYSTICK_LEFT_Y 4
#define JOYSTICK_LEFT_CLCK 5
#define JOYSTICK_RIGHT_X 6
#define JOYSTICK_RIGHT_Y 7
#define JOYSTICK_RIGHT_CLCK 8

#define NUM_VALUES 9

//                                            left                                                                right                                     mapping                //
//                       _________________________________________________                   _________________________________________________        ___________________          //
//                       top                       bottom              stby                  top                       bottom             stby        from             to          //
//              _______________________    _______________________    _______      _______________________    _______________________    _______     _______    ________________   //
Mecanum mecanum(INA2_1, INA1_1, PWMA_1,    INB1_1, INB2_1, PWMB_1,    STBY_1,      INA1_2, INA2_2, PWMA_2,    INB2_2, INB1_2, PWMB_2,    STBY_2,     0, 511,    0, DEFAULT_SPEED); //

#include <Mecaside.h>
Mecaside left(Left, 255);
Mecaside right(Right, 255);

int sizes[NUM_VALUES] = { 1, 4, 8, 9, 9, 1, 9, 9, 1 };
Bluetooth bluetooth(&Serial1, sizes, NUM_VALUES, '.');
Report report(&Serial, DEBUG, 100);

BlackLineSensor blackLine(A0, A1, A2);

LedRGB bluetoothLed(RGBA_1, RGBB_1, RGBC_1, true);
LedRGB led2(RGBA_2, RGBB_2, RGBC_2, true);
Digit digit(DIGITB, DIGITA, 7);

SingleServo servo(SERVO_1, 90, 0);

StepperMotor stepper1(STEP, DIR, LMTS_1, false, false, 3000, 2); /*, LMTS_2 */

#include "AutoPilot.h"

int estimation = 60;
int speedStatus = 0;

void setup() {
  // Serial setup //
  {
    Serial1.begin(9600);
    Serial.begin(9600);
    Serial.println("test");
#if DEBUG
    Serial.println("Debug mode is on.");
    Serial.println("Serial communication is on...");
    Serial.println("Bluetooth communication is on...");
#endif
  }
  // Setup and stop the robot  //
  {
    digit.display(estimation);
    servo.setup();
    servo.open();
    stepper1.setup();
    stop();
#if DEBUG
    Serial.println("All systems are running.");
#endif
  }
}

void loop() {
  stepper1.loop();
  report.print();
  if (bluetooth.receive()) {
    report.ok++;
    report.prob = 0;
    bluetoothLed.on(0, 0, 255);
    {
#if DEBUG
      Serial.print("estimation: ");
      Serial.println(bluetooth.message.get(ESTIMATION));
      Serial.println();
#endif
      if (bluetooth.message.get(ESTIMATION) != -1 && bluetooth.message.get(ESTIMATION) != estimation) {
        estimation = bluetooth.message.get(ESTIMATION);
        digit.display(estimation);
      }
    }
    // Switch //
    {
#if DEBUG
      Serial.print("switch: ");
      Serial.println(bluetooth.message.get(SWITCH) != 0);
      Serial.println();
#endif
      servo.move(bluetooth.message.get(SWITCH) != 0);
    }
    // Keypad //
    {
#if DEBUG
      Serial.print("key: ");
      Serial.println(bluetooth.message.get(KEYPAD));
      Serial.println();
#endif
      switch (bluetooth.message.get(KEYPAD)) {
        case 1:
          stepper1.moveTo(500);
          doubleExample.open();
          break;
        case 2:
          servo.open();
          stepper1.moveTo(0);
          break;
        case 3:
          stepper1.moveTo(0);
          break;
        case 4:
          servo.toggle();
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
#if DEBUG
      Serial.print("y.l: ");
      Serial.println(bluetooth.message.get(JOYSTICK_LEFT_Y));
      Serial.print("y.r: ");
      Serial.println(bluetooth.message.get(JOYSTICK_RIGHT_Y));
      Serial.println();
      Serial.print("x.l: ");
      Serial.println(bluetooth.message.get(JOYSTICK_LEFT_X));
      Serial.print("x.r: ");
      Serial.println(bluetooth.message.get(JOYSTICK_RIGHT_X));
      Serial.println();
      Serial.println();

      Serial.print("left: ");
      Serial.println(constrain(bluetooth.message.get(JOYSTICK_RIGHT_Y) + bluetooth.message.get(JOYSTICK_LEFT_X), 0, 512));
      Serial.print("right: ");
      Serial.println(constrain(bluetooth.message.get(JOYSTICK_RIGHT_Y) - bluetooth.message.get(JOYSTICK_LEFT_X), 0, 512));
      Serial.println();
      Serial.print("sideway: ");
      Serial.println(bluetooth.message.get(JOYSTICK_LEFT_X));
      Serial.print("diagonal: ");
      Serial.println(bluetooth.message.get(JOYSTICK_RIGHT_X), bluetooth.message.get(JOYSTICK_RIGHT_Y));
      Serial.println();
      Serial.println();
#endif
      // Simple
      {
        if (bluetooth.message.get(JOYSTICK_LEFT_Y) > 255) {
          if (speedStatus != 1) {
            Serial.println("boost");
            mecanum.changeSpeed(255);
          }
          speedStatus = 1;
        }
        if (bluetooth.message.get(JOYSTICK_LEFT_Y) == 255) {
          if (speedStatus != 0) {
            Serial.println("default");
            mecanum.changeSpeed(DEFAULT_SPEED);
          }
          speedStatus = 0;

        }
        if (bluetooth.message.get(JOYSTICK_LEFT_Y) < 255) {
          if (speedStatus != 2) {
            Serial.println("slow");
            mecanum.changeSpeed(100);
          }
          speedStatus = 2;
        }
        left.move(constrain(((bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255) + (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)) + 255, 0, 511));
        right.move(constrain(((bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255) - (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)) + 255, 0, 511));
      }
      // Others
      {
        /*mecanum.sideway(bluetooth.message.get(JOYSTICK_RIGHT_X));
          if (abs(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) > DIAGONAL_THRESHOLD && abs(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255) > DIAGONAL_THRESHOLD) {
          mecanum.diagonal(bluetooth.message.get(JOYSTICK_RIGHT_X), bluetooth.message.get(JOYSTICK_RIGHT_Y));
          }*/
      }
    }
  } else {
    report.ntr++;
    report.prob++;
    bluetooth.empty();
    bluetoothLed.off();
  }
  if (report.prob >= 10) {
    stop();
  }
}

void stop() {
#if DEBUG
  // Serial.println("stop");
#endif
  mecanum.stop();
}
