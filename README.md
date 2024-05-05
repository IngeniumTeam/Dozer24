# Dozer24
This is the robot code from the Ingenium team.

## Software
### Description
The [src code](./src/) is an arduino sketch: [`Dozer.ino`](#dozerino)

#### [`Dozer.ino`](./src/Dozer/Dozer.ino)
##### Receive values
* Read values received by `Bluetooth` on the `Serial1` and deserialize them with the [ArduinoJson](https://github.com/bblanchon/ArduinoJson) library

##### Robot control
* `switch...case` to handle the value of the keypad:

| Number | Key    | Action                 | Description                          |
| :----: | :----: | ---------------------- | ------------------------------------ |
| 1      | 1      | open the rack          | drop the rack to the bottom and open the servomotor |
| 2      | 2      | drop plants            | open the servomotor and drop the plants |
| 3      | 3      | take the plants        | close the servomotor and take the rack up |
| 4      | 4      | rack mid-height        | get the rack to the height of the jardinière |
| 5      | 5      | rack up                | get the rack all the way up          |
| 6      | 6      | drop the plants        | get the rack down, open the servomotor and get the rack up |
| 7      | 7      | solar panel flip (rack)| get the rack at the height of the solar panel |
| 8      | 8      |                        |                                      |
| 9      | 9      | solar panel flip (servo) | get the rack up and deploy the stick to flip the solar panels from the inside |
| 10     | *      | stepper setup          | setup the stepper motor to find the limits (e.g. after a belt jump) |
| 11     | 0      | stop                   | stop the robot (motor and actuators) |
| 12     | #      | estimation             | type it to enter the estimation, all keys lose their usual binds. After another click on this key or 3 numbers entered, it loses the estimation focus. |

* Control the motors according the values of the joysticks:
  * The left one turns the robot on its axis. The Y value is used to apply the fast or slow mode.
  * The right one handles the horizontal translation: forward, backward, sideway right and left.

### Dependecies
The INO file depends of the following list of libraries:
* [Bluetooth](https://github.com/IngeniumTeam/Bluetooth)
    * [Arduino/SoftwareSerial](https://docs.arduino.cc/learn/built-in-libraries/software-serial): This library is built into the Arduino IDE
    * [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [Mecanum](https://github.com/IngeniumTeam/Mecanum)
    * [Motor](https://github.com/IngeniumTeam/Motor)
* [Report](https://github.com/IngeniumTeam/Report)
* [Led](https://github.com/IngeniumTeam/Led)
* [Cherry](https://github.com/IngeniumTeam/Cherry)
* [StepperMotor](https://github.com/IngeniumTeam/StepperMotor)
* [Digit](https://github.com/IngeniumTeam/Digit)
* [Timino](https://github.com/SimonPucheu/Timino)

## Hardware
* Arduino Mega 2560
* 4 motors equipped with [mecanum wheels](https://en.wikipedia.org/wiki/Mecanum_wheel)
* HC-05 bluetooth module
* 2 servomotors MG996
* Stepper motor
