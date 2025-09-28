// передатчик
#include <esp_now.h>
#include <WiFi.h>

struct ControlData {
  int16_t throttle = 0;
  int16_t rudder = 0;
  int16_t elevator = 0;
  int16_t ailerons = 0;
  bool buttonPressed = false;
};

// MAC приемника (самолета)
uint8_t receiverMac[] = {0x14, 0x2B, 0x2F, 0xC9, 0x46, 0x88};

#define BUTTON_PIN 4
#define LED_PIN 2

ControlData controlData;
bool receiverConfigured = false;
unsigned long lastSendTime = 0;
int errorCount = 0;

void printMacAddress() {
  Serial.println("=== ДИАГНОСТИКА ПЕРЕДАТЧИКА ===");
  Serial.print("MAC передатчика: ");
  Serial.println(WiFi.macAddress());
  Serial.print("MAC приемника: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(receiverMac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  // Проверяем Wi-Fi режим
  Serial.print("Wi-Fi режим: ");
  Serial.println(WiFi.getMode());
  Serial.println("=================================");
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("✅ Успешная отправка");
    errorCount = 0;
  } else {
    errorCount++;
    Serial.print("❌ Ошибка отправки #");
    Serial.println(errorCount);
    digitalWrite(LED_PIN, HIGH);
  }
  delay(50);
  digitalWrite(LED_PIN, LOW);
}

bool setupReceiver() {
  Serial.println("Настраиваю приемник...");
  
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  // Пробуем добавить пира несколько раз
  for (int i = 0; i < 3; i++) {
    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
      receiverConfigured = true;
      Serial.println("✅ Приемник настроен!");
      return true;
    }
    delay(100);
  }
  
  Serial.println("❌ Не удалось настроить приемник");
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Даем время для инициализации Serial
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Инициализация передатчика...");
  
  // Настройка Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.println("Wi-Fi в режиме STA");
  
  printMacAddress();

  // Инициализация ESP-NOW
  Serial.println("Инициализация ESP-NOW...");
  esp_err_t result = esp_now_init();
  
  if (result != ESP_OK) {
    Serial.print("❌ Ошибка инициализации ESP-NOW: ");
    Serial.println(result);
    
    // Пробуем переинициализировать
    Serial.println("Пробую переинициализировать...");
    delay(1000);
    result = esp_now_init();
    
    if (result != ESP_OK) {
      Serial.println("❌ Критическая ошибка ESP-NOW");
      return;
    }
  }
  
  Serial.println("✅ ESP-NOW инициализирован");
  
  esp_now_register_send_cb(OnDataSent);
  
  // Настройка приемника
  if (setupReceiver()) {
    Serial.println("🔄 Передатчик готов к работе");
    
    // Быстро мигаем 3 раза для индикации готовности
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }
}

void loop() {
  if (receiverConfigured) {
    if (millis() - lastSendTime > 100) { // 10 Hz вместо 20 Hz для стабильности
      lastSendTime = millis();
      
      // Тестовые данные
      controlData.throttle = 0; // Начинаем с 0 для безопасности
      controlData.rudder = 0;
      controlData.elevator = 0;
      controlData.ailerons = 0;
      controlData.buttonPressed = !digitalRead(BUTTON_PIN);
      
      // Отправка данных
      esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&controlData, sizeof(controlData));
      
      if (result != ESP_OK) {
        Serial.print("❌ Ошибка вызова отправки: ");
        Serial.println(result);
      }
      
      // Вывод статуса каждые 10 отправок
      static int sendCount = 0;
      sendCount++;
      if (sendCount % 10 == 0) {
        Serial.print("Отправок: ");
        Serial.print(sendCount);
        Serial.print(", Ошибок: ");
        Serial.println(errorCount);
      }
    }
  } else {
    // Пробуем перенастроить приемник
    if (millis() % 5000 == 0) {
      Serial.println("Пробую переподключиться...");
      setupReceiver();
    }
  }
  
  delay(10);
}