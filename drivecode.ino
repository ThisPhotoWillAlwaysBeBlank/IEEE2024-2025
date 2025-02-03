#include <TMCStepper.h>

#define EN_PIN 4 // Enable Pin for all Motors (LOW to enable the driver)

// Motor 1 Pin Definitions
#define DIR_PIN1 3
#define STEP_PIN1 2
#define PDN_UART_PIN1 16
#define R_SENSE1 0.11f

// Motor 2 Pin Definitions
#define DIR_PIN2 5
#define STEP_PIN2 4
#define PDN_UART_PIN2 18
#define R_SENSE2 0.11f

// Motor 3 Pin Definitions
#define DIR_PIN3 7
#define STEP_PIN3 6
#define PDN_UART_PIN3 14
#define R_SENSE3 0.11f

// Motor 4 Pin Definitions
#define DIR_PIN4 9
#define STEP_PIN4 8
#define PDN_UART_PIN4 1
#define R_SENSE4 0.11f

#define DRIVER_ADDRESS 0b00 // Default address for TMC2209

// TMC2209 Configuration
TMC2209Stepper driver1(&Serial, R_SENSE1, DRIVER_ADDRESS);
TMC2209Stepper driver2(&Serial1, R_SENSE2, DRIVER_ADDRESS);
TMC2209Stepper driver3(&Serial1, R_SENSE3, DRIVER_ADDRESS);
TMC2209Stepper driver4(&Serial1, R_SENSE4, DRIVER_ADDRESS);

// Speed variables for each motor (in microseconds for delay)
int speed1 = 500;
int speed2 = 500;
int speed3 = 500;
int speed4 = 500;

// Timers to track step pulses for each motor
unsigned long lastStepTime1 = 0;
unsigned long lastStepTime2 = 0;
unsigned long lastStepTime3 = 0;
unsigned long lastStepTime4 = 0;

// Stop flags for each motor
bool stopMotor1 = false;
bool stopMotor2 = false;
bool stopMotor3 = false;
bool stopMotor4 = false;

// Parse and validate speed from command
int parseSpeed(String speedString) {
    int newSpeed = speedString.toInt();
    if (newSpeed >= 300) {
        Serial.print("New speed set to: ");
        Serial.println(newSpeed);
        return newSpeed;
    } else {
        Serial.println("Dangerous speed! Minimum is 300 microseconds.");
        return 500; // Default speed
    }
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    // Initialize motor pins
    pinMode(EN_PIN, OUTPUT);
    pinMode(DIR_PIN1, OUTPUT);
    pinMode(STEP_PIN1, OUTPUT);
    pinMode(DIR_PIN2, OUTPUT);
    pinMode(STEP_PIN2, OUTPUT);
    pinMode(DIR_PIN3, OUTPUT);
    pinMode(STEP_PIN3, OUTPUT);
    pinMode(DIR_PIN4, OUTPUT);
    pinMode(STEP_PIN4, OUTPUT);

    // Enable all motors
    digitalWrite(EN_PIN, LOW);

    // Initialize drivers
    driver1.begin();
    driver1.toff(5);
    driver1.rms_current(1000);
    driver1.microsteps(16);
    driver1.pwm_autoscale(true);

    driver2.begin();
    driver2.toff(5);
    driver2.rms_current(1000);
    driver2.microsteps(16);
    driver2.pwm_autoscale(true);

    driver3.begin();
    driver3.toff(5);
    driver3.rms_current(1000);
    driver3.microsteps(16);
    driver3.pwm_autoscale(true);

    driver4.begin();
    driver4.toff(5);
    driver4.rms_current(1000);
    driver4.microsteps(16);
    driver4.pwm_autoscale(true);
}

void loop() {
    unsigned long currentTime = micros();

    // Generate step pulses for Motor 1
    if (!stopMotor1 && currentTime - lastStepTime1 >= speed1) {
        digitalWrite(STEP_PIN1, HIGH);
        delayMicroseconds(1); // Short pulse duration
        digitalWrite(STEP_PIN1, LOW);
        lastStepTime1 = currentTime;
    }

    // Generate step pulses for Motor 2
    if (!stopMotor2 && currentTime - lastStepTime2 >= speed2) {
        digitalWrite(STEP_PIN2, HIGH);
        delayMicroseconds(1); // Short pulse duration
        digitalWrite(STEP_PIN2, LOW);
        lastStepTime2 = currentTime;
    }

    // Generate step pulses for Motor 3
    if (!stopMotor3 && currentTime - lastStepTime3 >= speed3) {
        digitalWrite(STEP_PIN3, HIGH);
        delayMicroseconds(1); // Short pulse duration
        digitalWrite(STEP_PIN3, LOW);
        lastStepTime3 = currentTime;
    }

    // Generate step pulses for Motor 4
    if (!stopMotor4 && currentTime - lastStepTime4 >= speed4) {
        digitalWrite(STEP_PIN4, HIGH);
        delayMicroseconds(1); // Short pulse duration
        digitalWrite(STEP_PIN4, LOW);
        lastStepTime4 = currentTime;
    }

    // Check for serial commands
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command.equalsIgnoreCase("STOP1")) {
            stopMotor1 = true;
            driver1.toff(0);
            Serial.println("Motor 1 stopped.");
        } else if (command.equalsIgnoreCase("STOP2")) {
            stopMotor2 = true;
            driver2.toff(0);
            Serial.println("Motor 2 stopped.");
        } else if (command.equalsIgnoreCase("STOP3")) {
            stopMotor3 = true;
            driver3.toff(0);
            Serial.println("Motor 3 stopped.");
        } else if (command.equalsIgnoreCase("STOP4")) {
            stopMotor4 = true;
            driver4.toff(0);
            Serial.println("Motor 4 stopped.");
        } else if (command.equalsIgnoreCase("START1")) {
            stopMotor1 = false;
            driver1.toff(5);
            Serial.println("Motor 1 started.");
        } else if (command.equalsIgnoreCase("START2")) {
            stopMotor2 = false;
            driver2.toff(5);
            Serial.println("Motor 2 started.");
        } else if (command.equalsIgnoreCase("START3")) {
            stopMotor3 = false;
            driver3.toff(5);
            Serial.println("Motor 3 started.");
        } else if (command.equalsIgnoreCase("START4")) {
            stopMotor4 = false;
            driver4.toff(5);
            Serial.println("Motor 4 started.");
        } else if (command.startsWith("M1:")) {
            speed1 = parseSpeed(command.substring(3));
        } else if (command.startsWith("M2:")) {
            speed2 = parseSpeed(command.substring(3));
        } else if (command.startsWith("M3:")) {
            speed3 = parseSpeed(command.substring(3));
        } else if (command.startsWith("M4:")) {
            speed4 = parseSpeed(command.substring(3));
        } else {
            Serial.println("Unknown command.");
        }
    }
}
