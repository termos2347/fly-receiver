#include "Joystick.h"
#include <Arduino.h>

void Joystick::begin() {
    pinMode(config.JOYSTICK_X_PIN, INPUT);
    pinMode(config.JOYSTICK_Y_PIN, INPUT);
    pinMode(config.JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
    
    // Автокалибровка при запуске
    calibrate();
}

void Joystick::calibrate() {
    Serial.println("🔧 Калибровка джойстика...");
    delay(100);
    
    xCenter = 0;
    yCenter = 0;
    
    // Усредняем несколько измерений
    for(int i = 0; i < 10; i++) {
        xCenter += analogRead(config.JOYSTICK_X_PIN);
        yCenter += analogRead(config.JOYSTICK_Y_PIN);
        delay(10);
    }
    
    xCenter /= 10;
    yCenter /= 10;
    
    calibrated = true;
    Serial.printf("✅ Центр: X=%d, Y=%d\n", xCenter, yCenter);
}

void Joystick::update() {
    if (!calibrated) return;
    
    // Чтение и фильтрация осей
    int rawX = readFilteredAnalog(config.JOYSTICK_X_PIN);
    int rawY = readFilteredAnalog(config.JOYSTICK_Y_PIN);
    
    // Преобразование в диапазон -512 до +512
    currentData.xAxis = constrain(map(rawX, 0, 4095, -512, 512), -512, 512);
    currentData.yAxis = constrain(map(rawY, 0, 4095, -512, 512), -512, 512);
    
    // Чтение кнопки
    currentData.buttonPressed = !digitalRead(config.JOYSTICK_BUTTON_PIN);
    
    // Расчет CRC
    currentData.crc = calculateCRC(currentData);
}

int Joystick::readFilteredAnalog(uint8_t pin) {
    // Простой медианный фильтр 3x
    int readings[3];
    for(int i = 0; i < 3; i++) {
        readings[i] = analogRead(pin);
        delay(2);
    }
    
    // Сортировка пузырьком для медианы (замена swap)
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2 - i; j++) {
            if(readings[j] > readings[j+1]) {
                // Ручная замена значений (аналог swap)
                int temp = readings[j];
                readings[j] = readings[j+1];
                readings[j+1] = temp;
            }
        }
    }
    
    return readings[1]; // Медиана
}

ControlData Joystick::getData() {
    return currentData;
}

uint16_t Joystick::calculateCRC(const ControlData& data) {
    // Простая контрольная сумма
    uint16_t crc = 0;
    const uint8_t* bytes = (const uint8_t*)&data;
    
    for(size_t i = 0; i < sizeof(ControlData) - sizeof(uint16_t); i++) {
        crc += bytes[i];
    }
    return crc;
}

bool Joystick::isConnected() {
    return calibrated && 
           analogRead(config.JOYSTICK_X_PIN) > 0 && 
           analogRead(config.JOYSTICK_Y_PIN) > 0;
}