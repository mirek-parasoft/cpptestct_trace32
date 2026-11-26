#include <stdint.h>
#include <stdlib.h>  
#include <string.h>  

volatile uint16_t SENSOR_DATA = 0;
volatile uint16_t MOTOR_SPEED = 0;

volatile uint16_t SENSOR_HIGH_THRESHOLD = 500;
volatile uint16_t SENSOR_LOW_THRESHOLD = 100;
volatile uint16_t DEFAULT_VALUE = 10;
volatile uint16_t MULTIPLIER = 3;


uint16_t readSensor()
{
    return SENSOR_DATA; 
}

void setMotorSpeed(uint16_t speed)
{
    MOTOR_SPEED = speed;
}

bool canStartMotor(bool startRequested, bool guardClosed, bool faultActive)
{
    if (startRequested && guardClosed && !faultActive) {
        return true;
    }
    return false;
}

int computeControl(int sensor, int divisor)
{
    int value = 0;
    if (sensor > SENSOR_HIGH_THRESHOLD) {
        value = sensor / divisor;
    } else if (sensor > SENSOR_LOW_THRESHOLD) {
        value = sensor * MULTIPLIER;
    } else {
        value = DEFAULT_VALUE;
    }
    return value;
}

int processCommand(const char* cmd, const char* arg)
{
    if (strcmp(cmd, "SET") == 0) {
        int val = atoi(arg);
        setMotorSpeed((uint16_t)val);
        return val;
    }

    if (strcmp(cmd, "READ") == 0) {
        return readSensor();
    }
    return -1; 
}

int processor(const char* cmd, const char* arg)
{
    uint16_t sensor = readSensor();
    int control = computeControl(sensor, 0); 
    return processCommand(cmd, arg) + control;
}

uint16_t computeStartupSpeed(bool usePreset, uint16_t presetSpeed)
{
    uint16_t startupSpeed;
    if (usePreset) {
        startupSpeed = presetSpeed;
    }
    return startupSpeed;
}