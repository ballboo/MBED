#ifndef Sensor_h
#define Sensor_h

class Sensor
{
    public:

        Sensor(int pin);
        int GetSensor();
        void CheckStatus(int num);

    private:
        int _pin ; 

};
#endif
