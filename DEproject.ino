// Libraries
#include <SoftwareSerial.h>

// Constants
const int pirPin = 2;                                      // PIR sensor pin
const int ledPins[] = { 3, 4, 5, 6 };                      // LED pins
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);  // Number of LEDs
const int bluetoothTx = 1;                                // Bluetooth TX pin
const int bluetoothRx = 0;                                // Bluetooth RX pin

// Variables
int pirState = LOW;                                  // PIR sensor state
bool motionDetected = false;                         // Motion detection flag
bool bluetoothControl = false;                       // Bluetooth control flag
unsigned long lastMotionTime = 0;                    // Time of last motion detection
const unsigned long motionTimeout = 10 * 1000;       // 10 seconds in milliseconds
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);  // RX, TX

void setup() {
  pinMode(pirPin, INPUT);
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Set up Bluetooth communication
  Serial.begin(9600);
  bluetooth.begin(9600);

  // Print initialization message
  Serial.println("Initialization complete. Waiting for motion or Bluetooth commands...");
}

void loop() {
  // Read PIR sensor
  pirState = digitalRead(pirPin);

  // If motion detected and not controlled by Bluetooth, set motion detection flag and turn on LEDs
  if (pirState == HIGH && !bluetoothControl) {
    if (!motionDetected) {
      Serial.println("Motion detected!");
      motionDetected = true;
      lastMotionTime = millis();  // Update last motion time
      for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], HIGH);
      }
    }
  } else {
    if (motionDetected && !bluetoothControl) {  // If motion detected and not controlled by Bluetooth
      unsigned long currentTime = millis();
      // Turn off LEDs after timeout
      if (currentTime - lastMotionTime >= motionTimeout) {
        Serial.println("No motion detected for 10 seconds. Turning off LEDs.");
        for (int i = 0; i < numLeds; i++) {
          digitalWrite(ledPins[i], LOW);
        }
        motionDetected = false;  // Reset motion detection flag
      }
    }
  }

  // Check for Bluetooth commands
  if (bluetooth.available() > 0) {
    char command = bluetooth.read();
    Serial.print("Received command: ");
    Serial.println(command);

    // Command to control LEDs
    if (command >= '1' && command <= '4') {
      int ledIndex = command - '1';
      digitalWrite(ledPins[ledIndex], HIGH);
      Serial.print("LED ");
      Serial.print(ledIndex + 1);
      Serial.println(" turned on.");
      bluetoothControl = true;  // Set flag to indicate LEDs are controlled by Bluetooth
    } else if (command == '0') {
      for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      Serial.println("All LEDs turned off.");
      bluetoothControl = false;  // Reset flag to indicate LEDs are not controlled by Bluetooth
      motionDetected = false;    // Reset motion detection flag when turning off LEDs via Bluetooth
    } else {
      Serial.println("Invalid command.");
    }
  }
}