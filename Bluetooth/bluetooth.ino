#include <SoftwareSerial.h>

// HC-06 Bluetooth module connection
// For Arduino Mega 2560, use different pins than Arduino Uno
// recommended to use pins 10-11 for SoftwareSerial
SoftwareSerial bluetooth(10, 11); // RX (pin 10), TX (pin 11)

// FSR pins on Mega 2560
const int FSR1_PIN = A1;
const int FSR2_PIN = A2;
const int FSR3_PIN = A3;
const int FSR4_PIN = A4;
const int FSR5_PIN = A5;
const int FSR6_PIN = A6;

// Variables to store FSR readings
int fsrReadings[6];

// Constants for data processing
const int NUM_SAMPLES = 5;        // Number of samples for smoothing
const int SEND_INTERVAL = 100;    // Send data every 100ms
const char DELIMITER = ',';       // Delimiter for data
const char START_MARKER = '<';    // Start marker for data packet
const char END_MARKER = '>';      // End marker for data packet

unsigned long lastSendTime = 0;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  
  // Initialize Bluetooth communication
  // HC-06 typically uses 9600 baud rate by default
  bluetooth.begin(9600);
  
  // Optional: Add a startup delay to ensure stable initialization
  delay(1000);
  
  Serial.println("FSR Posture Detection System Started");
}

// Function to read and smooth FSR values
int smoothFSRReading(int pin) {
  long sum = 0;
  
  // Take multiple samples and average them
  for(int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(2);  // Short delay between readings
  }
  
  return sum / NUM_SAMPLES;
}

// Function to map FSR readings to a 0-100 scale
int normalizeFSRReading(int reading) {
  // Map the FSR reading (typically 0-1023) to 0-100
  // Adjust the input range based on your FSR characteristics
  int normalizedValue = map(reading, 0, 1023, 0, 100);
  
  // Constrain the value to ensure it stays within 0-100
  return constrain(normalizedValue, 0, 100);
}

void loop() {
  // Check if it's time to send new data
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    // Read all FSR sensors
    fsrReadings[0] = smoothFSRReading(FSR1_PIN);
    fsrReadings[1] = smoothFSRReading(FSR2_PIN);
    fsrReadings[2] = smoothFSRReading(FSR3_PIN);
    fsrReadings[3] = smoothFSRReading(FSR4_PIN);
    fsrReadings[4] = smoothFSRReading(FSR5_PIN);
    fsrReadings[5] = smoothFSRReading(FSR6_PIN);
    
    // Start the data packet
    bluetooth.print(START_MARKER);
    
    // Send normalized readings
    for(int i = 0; i < 6; i++) {
      // Normalize the reading
      // int normalizedReading = normalizeFSRReading();
      
      // Send the normalized value
      bluetooth.print(fsrReadings[i]);
      
      // Add delimiter if not the last value
      if(i < 5) {
        bluetooth.print(DELIMITER);
      }
    }
    
    // End the data packet
    bluetooth.println(END_MARKER);
    
    // Debug output to Serial Monitor
    Serial.print("Raw Values: ");
    for(int i = 0; i < 6; i++) {
      Serial.print(fsrReadings[i]);
      Serial.print(DELIMITER);
    }
    Serial.println();
    
    // Serial.print("Normalized Values: ");
    // for(int i = 0; i < 6; i++) {
    //   Serial.print(normalizeFSRReading(fsrReadings[i]));
    //   Serial.print(DELIMITER);
    // }
    // Serial.println();
    
    // Update last send time
    lastSendTime = millis();
  }
}