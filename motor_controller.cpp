#include <stdint.h>
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>
#include <limits.h>

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

void setMotorSpeed(const uint16_t speed)
{
    MOTOR_SPEED = speed;
}

int computeControl(const int sensor, const int divisor)
{
    const int value = 0;

    if (sensor > SENSOR_HIGH_THRESHOLD) {
        if (divisor == 0) {
            return 0;
        }
        return sensor / divisor;
    } else if (sensor > SENSOR_LOW_THRESHOLD) {
        return sensor * MULTIPLIER;
    } else {
        return DEFAULT_VALUE;
    }
    return value; // parasoft-cov-suppress LC "Unreachable: every branch above returns."
}

int processCommand(const char* const cmd, const char* const arg)
{
    if (strcmp(cmd, "SET") == 0) {
        char* end = nullptr;
        errno = 0;
        const long val = strtol(arg, &end, 10);
        if ((errno != 0) || (end == arg) || (*end != '\0') || // parasoft-cov-suppress MCDC "On ARM32, long and int have identical ranges; the remaining val < INT_MIN and val > INT_MAX conditions cannot be true. The three reachable conditions are covered."
            (val < INT_MIN) || (val > INT_MAX)) {
            return -1;
        }
        setMotorSpeed((uint16_t)val);
        return static_cast<int>(val);
    }

    if (strcmp(cmd, "READ") == 0) {
        return readSensor();
    }
    return -1; 
}

int processor(const char* const cmd, const char* const arg)
{
    const uint16_t sensor = readSensor();
    const int control = computeControl(sensor, 0);
    return processCommand(cmd, arg) + control;
}
