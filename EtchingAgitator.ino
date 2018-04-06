/* Thankz to http://www.pellinglab.net */

#include <Stepper.h>

const int stepsPerRevolution  = 200;
const int PIN_START           = 2;            // Start/Stop button
const int PIN_LOCK            = 3;            // Lock/Unlock button pin
const int LED_START           = 4;            // Start/Stop button
const int LED_LOCK            = 5;            // Lock/Unlock button pin
int speedVal                  = 0;            // Stepper motor speed, controlled by pot on A0
int ampVal                    = 0;            // Stepper motor  amplitude, controlled by pot on A1
int shakerState               = LOW;
int lockState                 = LOW;          // Current reading from the input PIN_LOCK
int startState                = LOW;          // Current reading from the input PIN_START

Stepper agitateStepper(stepsPerRevolution, 8, 9, 10, 11); // Set PINS from L293D IC to Arduino (8,9,10,11) PINS

void setup() {
  pinMode(PIN_START, INPUT);
  pinMode(PIN_LOCK, INPUT);
  pinMode(LED_START, OUTPUT);
  pinMode(LED_LOCK, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  checkButtonState();               // Check 2 Buttons state (LOCK and START buttons)
  if (shakerState == HIGH) {
    for (int i = 0; i < 2; i++) {
      if (i == 0) {
        if (lockState == LOW) {     // If not LOCK then
          setttingsCheck();         // get Speed & Amplitude from Analog PINs
        }
        agitateStepper.step(ampVal);// Set amplitude value
      } else {
        if (lockState == LOW) {     // If not LOCK then
          setttingsCheck();         // get Speed & Amplitude from Analog PINs
        }
        agitateStepper.step(-ampVal);
      }
    }
  } else  {
    lockState = LOW;                // Reset lock state if STOP
    agitateStepper.setSpeed(0);
    agitateStepper.step(0);
  }

  printValues();
}

void checkButtonState() {
  lockState = digitalRead(PIN_LOCK);                  // Get state of LOCK button
  startState = digitalRead(PIN_START);                // Get state of START button
  if (startState == LOW && shakerState == 1) {        // If STOP
    for (int i = 1; i < 5; i++) {                     // To avoid an abrupt stop, the speed is slowly decreased
      agitateStepper.setSpeed(speedVal / i);
      agitateStepper.step(ampVal);
      agitateStepper.step(-ampVal);
    }
    shakerState = LOW;                                  // Shaker Off
  } else if (startState == HIGH && shakerState == 0) { // Startup Routine
    agitateStepper.setSpeed(40);                       // Initial default speed
    agitateStepper.step(-205);                         // Move backwards by more than 1 revolution in order to find the start position
    agitateStepper.step(100);                          // Move to middle position of the full range of motion
    shakerState = HIGH;                                   // Shaker On
  } else {
    // Do nothing.
  }
  digitalWrite(LED_START, shakerState);               // indicates the state by ON/OFF the LED for START
  digitalWrite(LED_LOCK, lockState);                  // indicates the state by ON/OFF the LED for LOCK
}

void setttingsCheck() {
  speedVal = map(analogRead(0), 1023, 0, 30, 140);    // Read analog value as SPEED
  ampVal = map(analogRead(1), 1023, 0, 100, 25);      // Read analog value as AMPLITUDE
  agitateStepper.setSpeed(speedVal);
}

void printValues() {
  Serial.print("SPEED:");
  Serial.println(speedVal);
  Serial.print("Amplitude:");
  Serial.println(ampVal);
  Serial.print("state:");
  Serial.println(shakerState);
}

