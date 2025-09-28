#include "ESPNowManager.h"

void ESPNowManager::begin() {
    WiFi.mode(WIFI_STA);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ Ошибка инициализации ESP-NOW");
        return;
    }
    
    esp_now_register_send_cb(onDataSent);
    Serial.println("✅ ESP-NOW инициализирован");
}

bool ESPNowManager::sendData(const ControlData& data) {
    if (!paired) {
        Serial.println("⚠️  Приемник не спарен");
        return false;
    }
    
    if (millis() - lastSendTime < 20) { // 50 Hz
        return false;
    }
    
    esp_err_t result = esp_now_send(receiverMac, (uint8_t*)&data, sizeof(data));
    lastSendTime = millis();
    
    return result == ESP_OK;
}

void ESPNowManager::onDataSent(const uint8_t* mac, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        digitalWrite(2, HIGH);
        delay(10);
        digitalWrite(2, LOW);
    }
}

bool ESPNowManager::addPeer(const uint8_t* mac) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    return esp_now_add_peer(&peerInfo) == ESP_OK;
}

void ESPNowManager::pairWithReceiver() {
    // Режим спаривания - будем ждать широковещательный пакет от приемника
    Serial.println("🔍 Режим спаривания...");
    // Здесь можно добавить логику автоматического спаривания
}

bool ESPNowManager::isConnected() {
    return paired;
}