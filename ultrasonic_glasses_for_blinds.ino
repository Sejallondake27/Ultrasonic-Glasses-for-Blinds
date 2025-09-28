/*
 * Visually Impaired Assistance System
 * Using HC-SR04 Ultrasonic Sensor and Buzzer
 * For Arduino Uno/Nano
 */

// Pin definitions
#define TRIG_PIN 9
#define ECHO_PIN 10
#define BUZZER_PIN 8

// Distance thresholds (in cm)
#define SAFE_DISTANCE 100      // No buzzing beyond this distance
#define WARNING_DISTANCE 50    // Slow buzzing starts
#define DANGER_DISTANCE 20     // Fast buzzing starts
#define CRITICAL_DISTANCE 10   // Continuous buzzing

// Buzzer timing variables
#define SLOW_BUZZ_INTERVAL 500  // ms
#define FAST_BUZZ_INTERVAL 200  // ms

// Global variables
unsigned long previousBuzzTime = 0;
int buzzInterval = 0;
bool buzzing = false;

void setup() {
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  
  digitalWrite(BUZZER_PIN, LOW);// Ensure buzzer is off initially
  
  Serial.println("Visually Impaired Assistance System Started");
 
}

void loop() {
  
  float distance = getDistance();// Get distance measurement
  
  if (distance < 0) {
    distance = SAFE_DISTANCE + 10; // Treat as safe distance
  }
  
  controlBuzzer(distance);
  
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(50); 
}

float getDistance() {
  long duration;
  float distance;
  
  // Clear trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send 10us pulse to trigger
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read echo pulse duration
  duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  
  // Calculate distance (in cm)
  // Speed of sound = 340 m/s = 0.034 cm/us
  // Distance = (duration * 0.034) / 2
  distance = duration * 0.034 / 2;
  
  // Return -1 for invalid readings (out of range or no echo)
  if (distance <= 0 || distance > 400) {
    return -1;
  }
  
  return distance;
}

void controlBuzzer(float distance) {
  unsigned long currentTime = millis();
  
  if (distance > SAFE_DISTANCE) {
    // No object detected - turn off buzzer
    digitalWrite(BUZZER_PIN, LOW);
    buzzing = false;
    return;
  }
  else if (distance > WARNING_DISTANCE) {
    // Object far away - slow intermittent buzzing
    buzzInterval = SLOW_BUZZ_INTERVAL;
  }
  else if (distance > DANGER_DISTANCE) {
    // Object getting closer - medium speed buzzing
    buzzInterval = SLOW_BUZZ_INTERVAL / 2;
  }
  else if (distance > CRITICAL_DISTANCE) {
    // Object very close - fast buzzing
    buzzInterval = FAST_BUZZ_INTERVAL;
  }
  else {
    // Object critically close - continuous buzzing
    digitalWrite(BUZZER_PIN, HIGH);
    buzzing = true;
    return;
  }
  
  // Handle intermittent buzzing
  if (currentTime - previousBuzzTime >= buzzInterval) {
    buzzing = !buzzing;
    digitalWrite(BUZZER_PIN, buzzing ? HIGH : LOW);
    previousBuzzTime = currentTime;
  }
}


