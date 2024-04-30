#include <SPI.h>
#include <Arduino.h>
#include <RH_RF69.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// ************ Radio Setup ***************

// Frequency for RF69 radio module
#define RF69_FREQ 915.0
// Pin configuration for the RFM69 module
#define RFM69_CS    5  // Chip select pin
#define RFM69_INT   15 // Interrupt pin
#define RFM69_RST   16 // Reset pin

// Radio driver instance for RH_RF69
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// ************ BLE Setup ***************

// UUIDs for the BLE service and characteristics
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_TX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

// BLE server and characteristic instances
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristicRX = nullptr;
BLECharacteristic* pCharacteristicTX = nullptr;
bool deviceConnected = false;

// Class to handle BLE server callbacks
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("Device disconnected");
    }
};

// Class to handle BLE characteristic callbacks
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string value = pCharacteristic->getValue();

        // Print received value
        if (!value.empty()) {
            Serial.print("Received Value via BLE: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);
            Serial.println();
        }

        // Process command to launch if received "launch"
        if (value == "launch") {
            digitalWrite(4, HIGH); 
            Serial.println("Launch command received, D4 set HIGH");
            delay(3000); 
            digitalWrite(4, LOW);
            Serial.println("D4 set LOW");
        }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // Radio reset sequence
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  pinMode(4, OUTPUT); // Setup pin 4 for output

  // Initialize RF69 module
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }

  // Set RF69 frequency
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // Set transmission power and high power mode
  rf69.setTxPower(20, true);

  // Encryption key for RF69
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  // Initialize BLE
  BLEDevice::init("L-TAS");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create characteristics for receiving and transmitting data
  pCharacteristicRX = pService->createCharacteristic(
                                       CHARACTERISTIC_UUID_RX,
                                       BLECharacteristic::PROPERTY_READ |
                                       BLECharacteristic::PROPERTY_WRITE |
                                       BLECharacteristic::PROPERTY_NOTIFY
                                     );

  pCharacteristicTX = pService->createCharacteristic(
                          CHARACTERISTIC_UUID_TX,
                          BLECharacteristic::PROPERTY_WRITE
                        );

  pCharacteristicTX->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("BLE service advertising started");
}

// Array to hold received packet data
String receivedPackets[100]; // Adjust size based on expected number of packets
int totalExpectedPackets = -1;
int packetsReceived = 0;

void loop() {
  if (rf69.available()) {
    // Buffer for received data
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN + 1];
    uint8_t len = sizeof(buf);

    // Check if packet received successfully
    if (rf69.recv(buf, &len)) {
      buf[len] = 0; // Ensure null termination
      String message = String((char *)buf);

      // Extract packet number and total number of packets
      int packetNum = message.substring(1, message.indexOf("of")).toInt();
      int totalPackets = message.substring(message.indexOf("of") + 2, message.indexOf(":")).toInt();

      // Store received data by packet number
      receivedPackets[packetNum] = message.substring(message.indexOf(":") + 1);

      packetsReceived++;
      if (totalExpectedPackets == -1) {
        totalExpectedPackets = totalPackets;
      }

      // Check if all packets have been received
      if (packetsReceived == totalExpectedPackets) {
        // Reassemble the complete message
        String completeMessage = "";
        for (int i = 0; i < totalExpectedPackets; i++) {
          completeMessage += receivedPackets[i];
        }

        // Output the complete message
        Serial.println(completeMessage);

        // Forward message to BLE Central device if connected
        if (deviceConnected) {
          pCharacteristicRX->setValue(completeMessage.c_str());
          pCharacteristicRX->notify();
        }

        // Reset packet tracking for the next message
        packetsReceived = 0;
        totalExpectedPackets = -1;
        for (int i = 0; i < 100; i++) { // Clear the packet buffer
          receivedPackets[i] = "";
        }
      }
    } else {
      Serial.println("Receive failed");
    }
  }
}
