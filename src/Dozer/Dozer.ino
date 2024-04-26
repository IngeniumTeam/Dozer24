#include <Bluetooth.h>
#include <Mecanum.h>
#include <Report.h>
#include <BlackLineSensor.h>
#include <Led.h>
#include <Cherry.h>
#include <Digit.h>
#include <Timino.h>
#include <StepperMotor.h>

#define MIN_SPEED 90
#define DEFAULT_SPEED 180
#define MAX_SPEED 255

#define DIAGONAL_THRESHOLD 30

#define DEBUG false
#define TIME_DEBUG false

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
Mecanum mecanum(INA2_1, INA1_1, PWMA_1,    INB2_1, INB1_1, PWMB_1,    STBY_1,      INA1_2, INA2_2, PWMA_2,    INB2_2, INB1_2, PWMB_2,    STBY_2,     0, 255,    0, DEFAULT_SPEED); //

#include <Mecaside.h>
Mecaside left(Left);
Mecaside right(Right);

int sizes[NUM_VALUES] = { 1, 4, 8, 9, 9, 1, 9, 9, 1 };
Bluetooth bluetooth(&Serial1, sizes, NUM_VALUES, '.');
Report report(&Serial, DEBUG, 100);

BlackLineSensor blackLine(A0, A1, A2);

LedRGB bluetoothLed(RGBA_1, RGBB_1, RGBC_1, true);
LedRGB led2(RGBA_2, RGBB_2, RGBC_2, true);
Digit digit(DIGITB, DIGITA, 7);

SingleServo rackServo(SERVO_3, 8, 50);
SingleServo solarServo(SERVO_4, 30, 110);

StepperMotor rackStepper(STEP, DIR, LMTS_1, false, false, 3000, 2); /*, LMTS_2 */

int estimation = 60;
int speedStatus = 0;
int key = 0;

void setup() {
  // Serial setup //
  {
#if DEBUG
    Serial.println("Debug mode is on.");
    Serial.println("Serial communication is on.");
    Serial.println("Bluetooth communication is on.");
#endif
#if DEBUG
    Serial.begin(9600);
    Serial.println("Serial communication is on.");
#endif
    Serial1.begin(9600);
#if DEBUG
    Serial.println("Bluetooth communication is on.");
#endif
  }
  // Setup and stop the robot //
  {
    digit.display(estimation);
#if DEBUG
    Serial.println("Estimation is on.");
#endif
    rackServo.setup();
    rackServo.close();
    solarServo.setup();
    solarServo.close();
#if DEBUG
    Serial.println("Servo is on and open.");
#endif
    rackStepper.setup();
#if DEBUG
    Serial.println("Stepper motor is on.");
#endif
    stop();
#if DEBUG
    Serial.println("All systems are running.");
#endif
  }
}

void loop() {
#if TIME_DEBUG
  int start = millis();
#endif
  rackStepper.loop();
  //report.print();
  switch (bluetooth.receive()) {
    case 0:
      report.ok++;
      report.prob = 0;
      bluetoothLed.on(0, 0, 255);
      // Estimation //
      {
#if DEBUG
        Serial.print("estimation: "); Serial.print(bluetooth.message.get(ESTIMATION)); Serial.print(" | ");
#endif
        if (bluetooth.message.get(ESTIMATION) != 0 && bluetooth.message.get(ESTIMATION) != estimation) {
          estimation = bluetooth.message.get(ESTIMATION);
          digit.display(estimation);
        }
      }
      // Switch //
      {
#if DEBUG
        Serial.print("switch: "); Serial.print(bluetooth.message.get(SWITCH) != 0); Serial.print(" | ");
#endif
        rackServo.move(bluetooth.message.get(SWITCH) != 0);
      }
      // Speed change //
      {
        const int joystickY = bluetooth.message.get(JOYSTICK_LEFT_Y);
        if (joystickY > 255 && speedStatus != 1) {
#if DEBUG
          Serial.print("boost | ");
#endif
          mecanum.setMaxSpeed(MAX_SPEED);
          speedStatus = 1;
        }
        if (joystickY == 255 && speedStatus != 0) {
#if DEBUG
          Serial.print("default | ");
#endif
          mecanum.setMaxSpeed(DEFAULT_SPEED);
          speedStatus = 0;
        }
        if (joystickY < 255 && speedStatus != 2) {
#if DEBUG
          Serial.print("slow | ");
#endif
          mecanum.setMaxSpeed(MIN_SPEED);
          speedStatus = 2;
        }
      }
      // Motors //
      {
#if DEBUG
        Serial.print("left: "); Serial.print(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) + (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)), -255, 255)); Serial.print(" | ");
        Serial.print("right: "); Serial.print(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) - (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)), -255, 255)); Serial.print(" | ");
        Serial.print("sideway: "); Serial.print(constrain(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255, -255, 255)); Serial.print(" | ");
#endif
        left.move(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) + (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)), -255, 255));
        right.move(constrain((-(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) - (bluetooth.message.get(JOYSTICK_LEFT_X) - 255)), -255, 255));
        mecanum.sideway(constrain(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255, -255, 255));
        if (abs(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255) > DIAGONAL_THRESHOLD && abs(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255) > DIAGONAL_THRESHOLD) {
#if DEBUG
          Serial.print("diagonal: "); Serial.print(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255); Serial.print(", "); Serial.print(bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255); Serial.print(" | ");
#endif
          mecanum.diagonal(bluetooth.message.get(JOYSTICK_RIGHT_X) - 255, bluetooth.message.get(JOYSTICK_RIGHT_Y) - 255);
        }
      }
      // Brake //
      {
        if (bluetooth.message.get(JOYSTICK_LEFT_CLCK)) {
#if DEBUG
          Serial.print("brake |");
#endif
          mecanum.brake();
        }
      }
      // Keypad //
      {
        if (bluetooth.message.get(KEYPAD) != key && bluetooth.message.get(KEYPAD) != 0) {
          key = bluetooth.message.get(KEYPAD);
#if DEBUG
          Serial.print("key: "); Serial.print(key); Serial.print(" | ");
#endif
          switch (key) {
            case 1:
              stop();
              rackStepper.moveTo(0);
              break;
            case 2:
              break;
            case 3:
              stop();
              rackStepper.moveTo(600);
              break;
            case 4:
              stop();
              rackStepper.moveTo(230);
              break;
            case 5:
              stop();
              rackStepper.moveTo(370);
              break;
            case 6:
              stop();
              solarServo.close();
              rackStepper.moveTo(550);
              break;
            case 7:
              stop();
              rackStepper.moveTo(600);
              solarServo.open();
              break;
            case 8:
              solarServo.open();
              break;
            case 9:
              solarServo.close();
              break;
            case 10:
              rackStepper.setup();
              break;
            case 11:
              stop();
              break;
          }
        }
      }
      break;
    case 1:
      report.inv++;
      report.prob = 0;
      bluetoothLed.off();
      break;
    case 2:
      report.ntr++;
      report.prob++;
      bluetoothLed.off();
      break;
  }
  if (report.prob >= 20) {
    stop();
  }
#if TIME_DEBUG
  Serial.print("time: "); Serial.print(millis() - start);
#endif
#if DEBUG
  Serial.println();
#endif
}

void stop() {
#if DEBUG
  Serial.print("stop | ");
#endif
  mecanum.stop();
}
