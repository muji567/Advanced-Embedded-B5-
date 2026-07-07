#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <LiquidCrystal.h>

#include <WiFi.h>
#include <PubSubClient.h>

// --- WiFi ---
const char* ssid = "Ip";
const char* password = "123456ac";

// --- MQTT (the Pi's bulletin board) ---
const char* mqtt_server = "172.20.10.3";  // Pi's IP address
WiFiClient espClient;
PubSubClient mqttClient(espClient);

#define buttonPin 12

// RFID
#define SS_PIN 5
#define RST_PIN 27
MFRC522 rfid(SS_PIN, RST_PIN);

// Gate servo
#define gate1Pin 14
Servo gate1;

// LEDs
#define greenPin 15
#define redPin 25

// Buzzer
#define buzzerPin 4

// Ultrasonic
#define trigPin 32
#define echoPin 33

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(21, 22, 16, 17, 26, 13);

// Parking spots
int parkingSpots = 7;

// Your two approved cards
byte authorizedUIDs[2][4] = {
  {0x6A, 0x61, 0xF5, 0x00},
  {0xA3, 0x6E, 0x84, 0x30}
};

bool checkAccess() {
  for (int i = 0; i < 2; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (rfid.uid.uidByte[j] != authorizedUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}

// Sends the current parking count to the Pi
void publishSpots() {
  String msg = "{\"available\":" + String(parkingSpots) + "}";
  mqttClient.publish("gate/status", msg.c_str());
}

void beep(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
    delay(150);
  }
}

void showReady() {
  lcd.clear();
  lcd.print("Scan card...");
  lcd.setCursor(0, 1);
  lcd.print("Spots: ");
  lcd.print(parkingSpots);
}

void openGate() {
  digitalWrite(greenPin, LOW);   // green on
  beep(1, 200);

  if (parkingSpots > 0) {
    gate1.write(90);
    parkingSpots = parkingSpots - 1;
    publishSpots();   // tell the dashboard

    lcd.clear();
    lcd.print("Gate Open");
    lcd.setCursor(0, 1);
    lcd.print("Spots left: ");
    lcd.print(parkingSpots);

    unsigned long start = millis();
    while (getDistance() > 15 && millis() - start < 5000) delay(100);
    start = millis();
    while (getDistance() < 15 && millis() - start < 8000) delay(100);

    gate1.write(0);
  } else {
    lcd.clear();
    lcd.print("Parking FULL");
    delay(2000);
  }

  digitalWrite(greenPin, HIGH);  // green off
  showReady();
}

void denyAccess() {
  Serial.println("Access denied");
  lcd.clear();
  lcd.print("Access Denied");
  digitalWrite(redPin, LOW);     // red on
  beep(2, 150);
  delay(1500);
  digitalWrite(redPin, HIGH);    // red off
  showReady();
}

// Runs whenever the dashboard sends a command
void onMessage(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Command received: ");
  Serial.println(message);

  if (message == "OPEN") {
    openGate();   // dashboard button opens the gate
  }
}

// Keeps the ESP32 connected to the bulletin board
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Gate")) {
      Serial.println("connected");
      mqttClient.subscribe("gate/entry/command");  // listen for commands
      publishSpots();  // send starting count
    } else {
      Serial.print("failed, retrying in 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(greenPin, HIGH);  // start off
  digitalWrite(redPin, HIGH);    // start off

  gate1.attach(gate1Pin);
  gate1.write(0);

  lcd.begin(16, 2);
  showReady();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(onMessage);

  Serial.println("Ready. Scan a card...");
}

void loop() {
  // Keep MQTT connection alive
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // "Car leaving" button
  if (digitalRead(buttonPin) == LOW) {
    if (parkingSpots < 7) {
      parkingSpots = parkingSpots + 1;
      publishSpots();   // tell the dashboard
      showReady();
      delay(300);
    }
  }

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  if (checkAccess()) {
    Serial.println("Valid card - waiting for car");
    lcd.clear();
    lcd.print("Card OK");
    lcd.setCursor(0, 1);
    lcd.print("Pull up to gate");

    unsigned long start = millis();
    bool carArrived = false;

    while (millis() - start < 10000) {
      if (getDistance() < 50) {
        carArrived = true;
        break;
      }
      delay(100);
    }

    if (carArrived) {
      openGate();
    } else {
      lcd.clear();
      lcd.print("Timeout");
      delay(1500);
      showReady();
    }
  } else {
    denyAccess();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}