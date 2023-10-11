class AutoPilot
{
  public:
    AutoPilot() {}
    void drift ()
    {
      mecanum.forward(1023);
      delay(700);
      left.forward(128);
      delay(300);
      mecanum.sidewayRight(1023);
      delay(500);
      right.forward(1023);
      left.forward(128);
      delay(100);
      left.forward(1023);
      right.forward(128);
      delay(100);
      mecanum.forward(1023);
      delay(500);
      mecanum.stop();
    }
    void winDance ()
    {
      left.forward(1023);
      right.backward(1023);
      delay(1000);
      right.forward(1023);
      left.backward(1023);
      delay(1000);
      mecanum.forward(1023);
      delay(100);
      mecanum.backward(1023);
      delay(200);
      mecanum.forward(1023);
      delay(100);
      right.forward(1023);
      left.backward(1023);
      delay(1000);
    }
} autoPilot;
