#ifndef StopWatch_h
#define StopWatch_h

class StopWatch
{
    public:

        StopWatch();
        void SetPin(int Pin_Start_Stop);
        void Start();
        void Stop();
        int GetHour();
        int GetMin();
        int GetSec();
        int GetMiliSec();
        void SerialPrint();

    private:
       int _Pin1;
};
#endif