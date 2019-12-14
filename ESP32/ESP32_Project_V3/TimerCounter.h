#ifndef TimerCounter_h
#define TimerCounter_h

class TimerCounter
{
    public:

        TimerCounter();
        void SetTimer(int SelectTimer,int TimeInterval);
        void Start();
        void Stop();
        void Reset();
        int  GetTime();

    private:

};
#endif