
#include <SoftwareSerial.h>

// GSM connections: Arduino RX, TX
SoftwareSerial gsm(7, 8);

// Sensor pin definitions
const int FLAME_SENSOR = 2;
const int VIBRATION_SENSOR = 3;
const int SMOKE_SENSOR = A0;
const int BUZZER = 9;

// Calibrate this value for your MQ-2 sensor
const int SMOKE_THRESHOLD = 400;

// Replace with your mobile number
const char PHONE_NUMBER[] = "+91XXXXXXXXXX";

// Track alert states
bool fireAlertSent = false;
bool smokeAlertSent = false;
bool vibrationAlertSent = false;

// Sensor states
bool fireDetected;
bool smokeDetected;
bool vibrationDetected;


// Initialize the system
void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(FLAME_SENSOR, INPUT);
  pinMode(VIBRATION_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

  Serial.println("Disaster Management System");
  Serial.println("Initializing GSM module...");

  delay(3000);

  gsm.println("AT");
  delay(1000);

  gsm.println("AT+CMGF=1");
  delay(1000);

  Serial.println("System ready.");
}


// Main monitoring loop
void loop() {

  int flameValue = digitalRead(FLAME_SENSOR);
  int vibrationValue = digitalRead(VIBRATION_SENSOR);
  int smokeValue = analogRead(SMOKE_SENSOR);

  // Many flame sensors output LOW when a flame is detected
  fireDetected = (flameValue == LOW);

  // Assumes SW-420 output goes HIGH when triggered.
  // Reverse this condition if your module behaves differently.
  vibrationDetected = (vibrationValue == HIGH);

  // Detect smoke or combustible gas above the threshold
  smokeDetected = (smokeValue > SMOKE_THRESHOLD);

  // Display sensor readings
  Serial.print("Smoke: ");
  Serial.print(smokeValue);

  Serial.print(" | Flame: ");
  Serial.print(flameValue);

  Serial.print(" | Vibration: ");
  Serial.println(vibrationValue);


  // FIRE ALERT
  if (fireDetected) {
    if (!fireAlertSent) {
      sendSMS("EMERGENCY: Possible fire detected!");
      fireAlertSent = true;
    }
  } else {
    fireAlertSent = false;
  }


  // SMOKE ALERT
  if (smokeDetected) {
    if (!smokeAlertSent) {
      sendSMS("WARNING: Smoke or combustible gas detected!");
      smokeAlertSent = true;
    }
  } else {
    smokeAlertSent = false;
  }


  // VIBRATION ALERT
  if (vibrationDetected) {
    if (!vibrationAlertSent) {
      sendSMS(
        "WARNING: Strong vibration detected. "
        "Possible seismic activity. Verify immediately."
      );

      vibrationAlertSent = true;
    }
  } else {
    vibrationAlertSent = false;
  }


  // Activate buzzer if a hazard is detected
  if (fireDetected || smokeDetected || vibrationDetected) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  delay(500);
}


// Send an SMS using the GSM module
void sendSMS(const char *message) {

  Serial.println("Sending SMS...");

  gsm.println("AT+CMGF=1");
  delay(1000);

  gsm.print("AT+CMGS=\"");
  gsm.print(PHONE_NUMBER);
  gsm.println("\"");

  delay(1000);

  gsm.print(message);

  // CTRL+Z submits the SMS
  gsm.write(26);

  delay(5000);

  Serial.println("SMS command submitted.");
}