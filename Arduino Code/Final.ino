#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 49

MFRC522 rfid(SS_PIN, RST_PIN);

int signal1[] = {28, 30, 32};
int signal2[] = {22, 24, 26};
int signal3[] = {34, 36, 38};
int signal4[] = {40, 42, 44};

const int triggerpin1 = 2;
const int echopin1 = 6;
const int triggerpin2 = 3;
const int echopin2 = 7;
const int triggerpin3 = 4;
const int echopin3 = 8;
const int triggerpin4 = 5;
const int echopin4 = 9;

int redDelay = 5000;
int yellowDelay = 2000; // Delay for transition to yellow
int greenDelay = 10000; // Delay for green light
int rfidDelay = 5000;   // Delay for RFID control
int ultrasonicDelay = 5000; // Delay for ultrasonic control

unsigned long previousMillis = 0;
int currentRoad = 1;

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  // Initialize signal pins as OUTPUT
  for (int i = 0; i < 3; i++)
  {
    pinMode(signal1[i], OUTPUT);
    pinMode(signal2[i], OUTPUT);
    pinMode(signal3[i], OUTPUT);
    pinMode(signal4[i], OUTPUT);
  }

  // Initialize ultrasonic sensor pins
  pinMode(triggerpin1, OUTPUT);
  pinMode(echopin1, INPUT);
  pinMode(triggerpin2, OUTPUT);
  pinMode(echopin2, INPUT);
  pinMode(triggerpin3, OUTPUT);
  pinMode(echopin3, INPUT);
  pinMode(triggerpin4, OUTPUT);
  pinMode(echopin4, INPUT);

  // Set initial road to green
  resetSignals();
  digitalWrite(signal1[currentRoad - 1], HIGH);
}

void loop() {
  unsigned long currentMillis = millis();
  bool rfidDetected = false;

  // Check for RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String cardID = getRFIDCardID();
    if (cardID == "C2:E6:3C:2E" || cardID == "1E:9C:00:4B") {
      // signalType = RFID;
      Serial.println("Valid RFID detected on Road 1. Stopping all other roads.");
      rfidDetected = true;
      stopAllRoads(rfidDelay);
    }
  }

  if (!rfidDetected) {
    // Check Ultrasonic for all roads
    if (currentRoad != 1 && checkUltrasonic(echopin1)) {
      // signalType = Ultrasonic;
      Serial.println("Ultrasonic detected on Road 1. Stopping all other roads.");
      currentRoad = 1;
      stopTwoFour();
      return;
    }
    else if (currentRoad != 2 && checkUltrasonic(echopin2)) {
      // signalType = Ultrasonic;
      Serial.println("Ultrasonic detected on Road 2. Stopping all other roads.");
      currentRoad = 2;
      stopOneThree();
      return;
    }
    else if (currentRoad != 3 && checkUltrasonic(echopin3)) {
      // signalType = Ultrasonic;
      Serial.println("Ultrasonic detected on Road 3. Stopping all other roads.");
      currentRoad = 3;
      stopTwoFour();
      return;
    }
    else if (currentRoad != 4 && checkUltrasonic(echopin4)) {
      // signalType = Ultrasonic;
      Serial.println("Ultrasonic detected on Road 4. Stopping all other roads.");
      currentRoad = 4;
      stopOneThree();
      return;
    }
  }

  // Normal operation, set signals based on time
  if (currentMillis - previousMillis >= greenDelay) {
    previousMillis = currentMillis;
    resetSignals();

    if (currentRoad == 1) {
      stopOneThree();
    }
    else if (currentRoad == 2 ) {
      stopTwoFour();
    }

    currentRoad++;
    if (currentRoad > 2) {
      currentRoad = 1;
    }
    // signalType = None;
    Serial.println("\n Currunt Road: ");
    Serial.println(currentRoad);
  }
}

void resetSignals()
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(signal1[i], LOW);
    digitalWrite(signal2[i], LOW);
    digitalWrite(signal3[i], LOW);
    digitalWrite(signal4[i], LOW);
  }
}

void stopAllRoads(int delayTime)
{
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(signal1[i], LOW);
    digitalWrite(signal2[i], LOW);
    digitalWrite(signal3[i], LOW);
    digitalWrite(signal4[i], LOW);
  }
  delay(delayTime);
  digitalWrite(signal2[0], HIGH);
  digitalWrite(signal3[0], HIGH);
  digitalWrite(signal4[0], HIGH);

  digitalWrite(signal1[2], HIGH);
  delay(greenDelay);
}

void stopOneThree(){
  // Turn everything off
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(signal1[i], LOW);
    digitalWrite(signal2[i], LOW);
    digitalWrite(signal3[i], LOW);
    digitalWrite(signal4[i], LOW);
  }
  // Turn Yellow
  digitalWrite(signal1[1], HIGH);
  digitalWrite(signal2[1], HIGH);
  digitalWrite(signal3[1], HIGH);
  digitalWrite(signal4[1], HIGH);
  delay(yellowDelay);// wait
  //Turn yellow off
  digitalWrite(signal1[1], LOW);
  digitalWrite(signal2[1], LOW);
  digitalWrite(signal3[1], LOW);
  digitalWrite(signal4[1], LOW);
// Turn Red Roads 1 and 3
  digitalWrite(signal1[0], HIGH);
  digitalWrite(signal3[0], HIGH);
// Turn Green Roads 2 and 4
  digitalWrite(signal2[2], HIGH);
  digitalWrite(signal4[2], HIGH);
  delay(greenDelay);
}

void stopTwoFour(){
  // Turn everything off
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(signal1[i], LOW);
    digitalWrite(signal2[i], LOW);
    digitalWrite(signal3[i], LOW);
    digitalWrite(signal4[i], LOW);
  }
  // Turn Yellow
  digitalWrite(signal1[1], HIGH);
  digitalWrite(signal2[1], HIGH);
  digitalWrite(signal3[1], HIGH);
  digitalWrite(signal4[1], HIGH);
  delay(yellowDelay);// wait
  //Turn yellow off
  digitalWrite(signal1[1], LOW);
  digitalWrite(signal2[1], LOW);
  digitalWrite(signal3[1], LOW);
  digitalWrite(signal4[1], LOW);
// Turn Red Roads 2 and 4
  digitalWrite(signal2[0], HIGH);
  digitalWrite(signal4[0], HIGH);
// Turn Green Roads 1 and 3
  digitalWrite(signal1[2], HIGH);
  digitalWrite(signal3[2], HIGH);
  delay(greenDelay);
}

bool checkUltrasonic(int echoPin)
{
  digitalWrite(triggerpin1, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerpin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerpin1, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance <= 10)
  {
    return true;
  }

  return false;
}

String getRFIDCardID()
{
  String cardID = "";
  for (byte i = 0; i < 4; i++)
  {
    cardID += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : "");
  }
  cardID.toUpperCase();
  return cardID;
}
