#include "Arduino.h"
class Timealert
{
  public:
    Timealert(int Hour, int Minute, int Second);
    void showtime(int hour, int minute, int second);
    void alert();
    int getHour();
    int getMinute();
    int getSecond();
  private:
    int H;
    int M;
    int S;
};
