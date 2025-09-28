#pragma once
#include <esp_now.h>
#include <WiFi.h>
#include "../Core/Types.h"

class ESPNowManager {
public:
    void begin();
    bool sendData(const ControlData& data);
    bool isConnected();
    void pairWithReceiver();
    bool addPeer(const uint8_t* mac); // Сделаем публичным для доступа из main.cpp
    
private:
    uint8_t receiverMac[6] = {0};
    bool paired = false;
    unsigned long lastSendTime = 0;
    
    static void onDataSent(const uint8_t* mac, esp_now_send_status_t status);
};