#pragma once
#include "../Core/Types.h"

class Joystick {
public:
    void begin();
    void update();
    ControlData getData();
    void calibrate();
    bool isConnected();
    
    // Сделать метод публичным для доступа из main.cpp
    uint16_t calculateCRC(const ControlData& data);
    
private:
    HardwareConfig config;
    ControlData currentData;
    int xCenter = 2048;
    int yCenter = 2048;
    bool calibrated = false;
    
    int readFilteredAnalog(uint8_t pin);
    // Убрали calculateCRC из приватных методов
};