#include <Mecanum.h>

#define defaultSpeed 230

#define PWMA_1 2
#define PWMB_1 3
#define INA1_1 50
#define INA2_1 51
#define INB1_1 25
#define INB2_1 26
#define STBY_1 21

#define PWMA_2 4
#define PWMB_2 5
#define INA1_2 52
#define INA2_2 53
#define INB1_2 27
#define INB2_2 28
#define STBY_2 22

//                                            left                                                                right                                      mapping               //
//                       _________________________________________________                   _________________________________________________         __________________          //
//                       front                     back                stby                  front                     back               stby         from            to          //
//              _______________________    _______________________    _______      _______________________    _______________________    _______     ________    _______________   //
Mecanum mecanum(INA1_1, INA2_1, PWMB_1,    INB2_1, INB1_1, PWMA_1,    STBY_1,      INA2_2, INA1_2, PWMA_2,    INB1_2, INB2_2, PWMB_2,    STBY_2,     0, 1023,    0, defaultSpeed); //

#include <Mecaside.h>
Mecaside left(Left);
Mecaside right(Right);

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("forward");
  mecanum.forward(1023);
  delay(3000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(500);
  
  Serial.println("backward");
  mecanum.backward(1023);
  delay(3000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(500);
  
  /*Serial.println("sideway left");
  mecanum.sideway(1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("sideway right");
  mecanum.sideway(-1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("left");
  left.move(1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("right");
  right.move(1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("diagonal left forward");
  mecanum.diagonal(1023, 1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("diagonal right forward");
  mecanum.diagonal(-1023, 1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("diagonal left backward");
  mecanum.diagonal(1023, -1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);
  
  Serial.println("diagonal right backward");
  mecanum.diagonal(-1023, -1023);
  delay(1000);
  
  Serial.println("stop");
  mecanum.stop();
  delay(1000);*/
}
