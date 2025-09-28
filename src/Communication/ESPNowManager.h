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
    
private:
    uint8_t receiverMac[6] = {0}; // Будет устанавливаться при спаривании
    bool paired = false;
    unsigned long lastSendTime = 0;
    
    static void onDataSent(const uint8_t* mac, esp_now_send_status_t status);
    bool addPeer(const uint8_t* mac);
};