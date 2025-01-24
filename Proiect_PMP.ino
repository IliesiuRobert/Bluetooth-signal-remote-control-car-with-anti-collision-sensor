#include <Servo.h>

Servo motion;     // Motion servo
Servo directie;    // Direction servo

String command = "";         // Buffer for reading full commands
String currentCommand = "";  // Currently active command
bool stop = false;           // Stop flag

// Constants for HC-SR04
const int TRIG_PIN = 7;  // Ultrasonic sensor TRIG pin
const int ECHO_PIN = 6;  // Ultrasonic sensor ECHO pin

// LEDs
const int ledA4 = A4;
const int ledA5 = A5;
const int ledA6 = A6;

// Variables for distance measurement
float distance = 0;

void setup() {
  // Initialize Serial Communication
  Serial.begin(9600);

  // Initialize Bluetooth Communication (Serial1 for HC-05 on Mega)
  Serial1.begin(9600);

  // Configure ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configure LEDs
  pinMode(ledA4, OUTPUT);
  pinMode(ledA5, OUTPUT);
  pinMode(ledA6, OUTPUT);
  digitalWrite(ledA4, LOW);
  digitalWrite(ledA5, LOW);
  digitalWrite(ledA6, LOW);

  // Attach servos
  motion.attach(9);
  directie.attach(10);

  // Set initial servo positions
  motion.writeMicroseconds(1500);  
  directie.write(90);               
  delay(7000);                      
}

void loop() {
  // Measure distance continuously
  distance = measureDistance();

  // Update the stop condition based on distance threshold
  stop = (distance <= 20.0);

  // If a stop condition is met, halt all motion
  if (stop || currentCommand == "S") {
    idle();
    currentCommand = "";  // Clear the active command when stopping
  } else {
    // Execute the current active command if no stop condition
    executeCurrentCommand();
  }

  // Process new commands from Bluetooth
  processCommands();
}

void processCommands() {
  while (Serial1.available() > 0) {
    char incomingChar = Serial1.read(); // Read one character from Bluetooth

    if (incomingChar == '\n') {        // Command delimiter
      command.trim();                  // Remove extra spaces
      if (command == "S") {            // Stop command
        currentCommand = "S";
      } else {
        currentCommand = command;      // Update the current command
      }
      command = "";                    // Clear the command buffer
    } else {
      command += incomingChar;         // Append character to buffer
    }
  }
}

void executeCurrentCommand() {
  if (currentCommand == "FL" || currentCommand == "LF") {
    forwardLeft();
  } else if (currentCommand == "FR" || currentCommand == "RF") {
    forwardRight();
  } else if (currentCommand == "BL" || currentCommand == "LB") {
    backwardLeft();
  } else if (currentCommand == "BR" || currentCommand == "RB") {
    backwardRight();
  } else if (currentCommand == "F") {
    forward();
  } else if (currentCommand == "B") {
    backward();
  } else if (currentCommand == "L") {
    left();
  } else if (currentCommand == "R") {
    right();
  } else if (currentCommand == "Q") {
    blink(ledA4);
  } else if (currentCommand == "E") {
    blink(ledA5);
  }
}

float measureDistance() {
  long duration;
  float distance;

  // Send a 10-microsecond pulse to the TRIG pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the ECHO pin to calculate the duration
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in cm
  distance = (duration * 0.034) / 2;
  return distance;
}

void idle() {
  motion.writeMicroseconds(1500);  // Neutral position (stop)
  directie.write(90);               // Center the direction servo
}

void forwardLeft() {
  motion.writeMicroseconds(1600);  // Forward motion
  directie.write(120);              // Steer left
}

void forwardRight() {
  motion.writeMicroseconds(1600);  // Forward motion
  directie.write(60);               // Steer right
}

void backwardLeft() {
  motion.writeMicroseconds(1400);  // Backward motion
  directie.write(120);              // Steer left
}

void backwardRight() {
  motion.writeMicroseconds(1400);  // Backward motion
  directie.write(60);               // Steer right
}

void forward() {
  motion.writeMicroseconds(1600);  // Forward motion
  directie.write(90);               // Center direction
}

void backward() {
  motion.writeMicroseconds(1400);  // Backward motion
  directie.write(90);               // Center direction
}

void left() {
  directie.write(120);              // Steer left
}

void right() {
  directie.write(60);               // Steer right
}

void blink(int led) {
  for (int i = 0; i < 5; i++) {
    digitalWrite(led, HIGH);  // Turn the LED on
    delay(500);               
    digitalWrite(led, LOW);   // Turn the LED off
    delay(500);               
  }
}
