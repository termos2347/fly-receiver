#include "Core/Types.h"
#include "Input/Joystick.h"
#include "Communication/ESPNowManager.h"

Joystick joystick;
ESPNowManager espNow;

// MAC адрес приемника
uint8_t receiverMac[] = {0x14, 0x2B, 0x2F, 0xC9, 0x46, 0x88};

void printDeviceInfo() {
  Serial.println("🎮 ===== ИНФОРМАЦИЯ ПЕРЕДАТЧИКА =====");
  Serial.print("MAC адрес: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getEfuseMac(), HEX);
  Serial.print("Частота CPU: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("Flash размер: ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
  Serial.println("=====================================");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("🎮 Запуск передатчика...");
  
  // Вывод информации об устройстве
  printDeviceInfo();
  
  joystick.begin();
  espNow.begin();
  
  // Добавляем приемник как пиар
  Serial.println("⏳ Добавление приемника...");
  
  if (espNow.addPeer(receiverMac)) {
    Serial.print("✅ Приемник добавлен: ");
    for(int i = 0; i < 6; i++) {
      Serial.print(receiverMac[i], HEX);
      if(i < 5) Serial.print(":");
    }
    Serial.println();
  } else {
    Serial.println("❌ Не удалось добавить приемник");
    Serial.println("⚠️  Проверьте MAC-адрес и перезагрузите устройство");
  }
  
  // Индикация готовности
  pinMode(2, OUTPUT);
  for(int i = 0; i < 3; i++) {
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
    delay(100);
  }
  
  Serial.println("🚀 Передатчик готов к работе");
  if (espNow.isConnected()) {
    Serial.println("📡 Связь с приемником установлена");
  } else {
    Serial.println("⚠️  Связь с приемником НЕ установлена");
  }
  Serial.println("📊 Ожидание данных джойстика...");
}

void loop() {
  joystick.update();
  ControlData data = joystick.getData();
  
  // Проверка CRC перед отправкой
  static uint16_t lastCRC = 0;
  uint16_t currentCRC = joystick.calculateCRC(data);
  
  if (currentCRC == data.crc && currentCRC != lastCRC) {
    espNow.sendData(data);
    lastCRC = currentCRC;
  }
  
  // Вывод отладочной информации каждые 500 мс
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.printf("📤 X: %4d, Y: %4d, BTN: %d, CRC: %04X\n", 
                 data.xAxis, data.yAxis, data.buttonPressed, data.crc);
    lastPrint = millis();
  }
  
  // Индикация работы (медленное мигание)
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    digitalWrite(2, !digitalRead(2));
    lastBlink = millis();
  }
  
  delay(10);
}