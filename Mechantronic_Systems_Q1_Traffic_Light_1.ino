/*
 * ============================================================
 *
 * Name: Christopher Redfern
 * BEng Electrical and Electronics Engineering
 * University of Lincoln
 * ELE2003M - Mechatronics Systems
 * Question 1: Traffic Light Demonstrator
 *
 * ============================================================
 *
 * Description:
 *   Simulates a UK pedestrian crossing traffic light system.
 *   Car lights cycle automatically: Green > Amber > Red > Red+Amber > Green
 *   When pedestrian button is pressed, the system safely
 *   transitions to pedestrian crossing mode, then returns
 *   to normal car traffic flow.
 *
 * Hardware:
 *   - Arduino (Freenove)
 *   - Car Red LED    -> Pin 2
 *   - Car Amber LED  -> Pin 3
 *   - Car Green LED  -> Pin 4
 *   - Ped Red LED    -> Pin 5
 *   - Ped Green LED  -> Pin 6
 *   - Push Button    -> Pin 7 (with 10k pull-down resistor)
 *   - All LEDs via 220 ohm resistors to GND
 * ============================================================
 */

// --- Pin Definitions ---
const int CAR_RED    = 2;   // Car traffic light - Red LED
const int CAR_AMBER  = 3;   // Car traffic light - Amber LED
const int CAR_GREEN  = 4;   // Car traffic light - Green LED
const int PED_RED    = 5;   // Pedestrian light  - Red LED
const int PED_GREEN  = 6;   // Pedestrian light  - Green LED
const int BUTTON     = 7;   // Pedestrian push button

// --- Timing Constants (milliseconds) ---
const int TIME_CAR_GREEN   = 5000;  // Car green phase duration
const int TIME_CAR_AMBER   = 2000;  // Car amber warning duration
const int TIME_RED_PAUSE   = 1000;  // Pause on red before pedestrian goes
const int TIME_PED_CROSS   = 5000;  // Pedestrian crossing duration
const int TIME_RED_AMBER   = 2000;  // Red+Amber before car green resumes

// --- Global Variable ---
bool buttonPressed = false;  // Tracks if pedestrian button has been pressed


// ============================================================
// SETUP - Runs once when Arduino powers on
// ============================================================
void setup() {
  // Set all LED pins as outputs
  pinMode(CAR_RED,   OUTPUT);
  pinMode(CAR_AMBER, OUTPUT);
  pinMode(CAR_GREEN, OUTPUT);
  pinMode(PED_RED,   OUTPUT);
  pinMode(PED_GREEN, OUTPUT);

  // Set button pin as input
  pinMode(BUTTON, INPUT);

  // Open Serial Monitor for debugging (optional)
  Serial.begin(9600);
  Serial.println("Traffic Light System Started");

  // Start with car on Green, pedestrian on Red (safe default)
  allLightsOff();
  digitalWrite(CAR_GREEN, HIGH);
  digitalWrite(PED_RED,   HIGH);
}


// ============================================================
// LOOP - Runs repeatedly after setup()
// ============================================================
void loop() {

  // --- PHASE 1: Car GREEN ---
  Serial.println("Phase 1: Car GREEN");
  digitalWrite(CAR_GREEN, HIGH);
  digitalWrite(PED_RED,   HIGH);
  waitAndCheckButton(TIME_CAR_GREEN);

  // If button was pressed during green phase, start crossing sequence
  if (buttonPressed) {
    pedestrianCrossing();
    buttonPressed = false;
    return; // Restart loop from beginning
  }

  // --- PHASE 2: Car AMBER ---
  Serial.println("Phase 2: Car AMBER");
  allLightsOff();
  digitalWrite(CAR_AMBER, HIGH);
  digitalWrite(PED_RED,   HIGH);
  waitAndCheckButton(TIME_CAR_AMBER);

  // --- PHASE 3: Car RED (check for button press here too) ---
  Serial.println("Phase 3: Car RED");
  allLightsOff();
  digitalWrite(CAR_RED,  HIGH);
  digitalWrite(PED_RED,  HIGH);

  // If button was pressed at any point, trigger crossing now
  if (buttonPressed) {
    pedestrianCrossing();
    buttonPressed = false;
    return;
  }

  waitAndCheckButton(TIME_RED_PAUSE);

  // --- PHASE 4: Car RED + AMBER (preparing to go green) ---
  Serial.println("Phase 4: Car RED + AMBER");
  allLightsOff();
  digitalWrite(CAR_RED,   HIGH);
  digitalWrite(CAR_AMBER, HIGH);
  digitalWrite(PED_RED,   HIGH);
  delay(TIME_RED_AMBER);

  // Return to Phase 1 (Green) at top of loop
}


// ============================================================
// FUNCTION: pedestrianCrossing()
// Executes the full pedestrian crossing sequence
// ============================================================
void pedestrianCrossing() {
  Serial.println(">>> PEDESTRIAN CROSSING ACTIVATED <<<");

  // Step 1: Car goes to Amber as warning
  allLightsOff();
  digitalWrite(CAR_AMBER, HIGH);
  digitalWrite(PED_RED,   HIGH);
  delay(TIME_CAR_AMBER);

  // Step 2: Car goes to Red - pedestrian still Red momentarily
  allLightsOff();
  digitalWrite(CAR_RED, HIGH);
  digitalWrite(PED_RED, HIGH);
  delay(TIME_RED_PAUSE);

  // Step 3: Pedestrian goes Green - safe to cross!
  Serial.println("Pedestrian GREEN - safe to cross");
  allLightsOff();
  digitalWrite(CAR_RED,   HIGH);
  digitalWrite(PED_GREEN, HIGH);
  delay(TIME_PED_CROSS);

  // Step 4: Pedestrian back to Red
  allLightsOff();
  digitalWrite(CAR_RED, HIGH);
  digitalWrite(PED_RED, HIGH);
  delay(TIME_RED_PAUSE);

  // Step 5: Car Red + Amber - preparing to resume
  allLightsOff();
  digitalWrite(CAR_RED,   HIGH);
  digitalWrite(CAR_AMBER, HIGH);
  digitalWrite(PED_RED,   HIGH);
  delay(TIME_RED_AMBER);

  // Step 6: Car back to Green - normal flow resumes
  allLightsOff();
  digitalWrite(CAR_GREEN, HIGH);
  digitalWrite(PED_RED,   HIGH);
  Serial.println("Returning to normal car flow");
}


// ============================================================
// FUNCTION: waitAndCheckButton(duration)
// Waits for the specified duration in 50ms chunks,
// checking the button state during the wait.
// This prevents the button being missed during long delays.
// ============================================================
void waitAndCheckButton(int duration) {
  int elapsed = 0;
  while (elapsed < duration) {
    if (digitalRead(BUTTON) == HIGH) {
      buttonPressed = true;
      Serial.println("Button pressed! Queuing pedestrian crossing...");
    }
    delay(50);
    elapsed += 50;
  }
}


// ============================================================
// FUNCTION: allLightsOff()
// Turns off all LEDs - used before switching to a new phase
// ============================================================
void allLightsOff() {
  digitalWrite(CAR_RED,   LOW);
  digitalWrite(CAR_AMBER, LOW);
  digitalWrite(CAR_GREEN, LOW);
  digitalWrite(PED_RED,   LOW);
  digitalWrite(PED_GREEN, LOW);
}
