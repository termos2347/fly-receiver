#pragma once
#include <cstdint>

// Структура данных для управления
struct ControlData {
    int16_t xAxis;      // Ось X джойстика (-512 до +512)
    int16_t yAxis;      // Ось Y джойстика (-512 до +512)
    bool buttonPressed; // Кнопка джойстика
    uint16_t crc;       // Контрольная сумма
};

// Конфигурация пинов
struct HardwareConfig {
    static const uint8_t JOYSTICK_X_PIN = 32;
    static const uint8_t JOYSTICK_Y_PIN = 33;
    static const uint8_t JOYSTICK_BUTTON_PIN = 4;
    static const uint8_t LED_PIN = 2;
};