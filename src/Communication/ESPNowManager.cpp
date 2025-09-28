#include "ESPNowManager.h"
#include <Arduino.h>

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
    
    if (result != ESP_OK) {
        Serial.print("❌ Ошибка отправки: ");
        Serial.println(result);
        return false;
    }
    
    return true;
}

void ESPNowManager::onDataSent(const uint8_t* mac, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        digitalWrite(2, HIGH);
        delay(10);
        digitalWrite(2, LOW);
    } else {
        Serial.print("❌ Ошибка доставки: ");
        Serial.println(status);
    }
}

bool ESPNowManager::addPeer(const uint8_t* mac) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    esp_err_t result = esp_now_add_peer(&peerInfo);
    if (result == ESP_OK) {
        memcpy(receiverMac, mac, 6);
        paired = true;
        Serial.println("✅ Пиар успешно добавлен");
        return true;
    } else {
        Serial.print("❌ Ошибка добавления пиара: ");
        Serial.println(result);
        return false;
    }
}

bool ESPNowManager::isConnected() {
    return paired;
}

void ESPNowManager::pairWithReceiver() {
    // Режим спаривания можно реализовать позже
    Serial.println("🔍 Режим спаривания...");
}