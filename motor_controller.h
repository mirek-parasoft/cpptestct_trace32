#ifndef GOO_H
#define GOO_H

#include <stdint.h>

// Hardware register declarations
extern volatile uint16_t SENSOR_DATA;
extern volatile uint16_t MOTOR_SPEED;
extern volatile uint16_t SENSOR_HIGH_THRESHOLD; 
extern volatile uint16_t SENSOR_LOW_THRESHOLD; 
extern volatile uint16_t DEFAULT_VALUE; 
extern volatile uint16_t MULTIPLIER; 

// Function declarations
uint16_t readSensor();
void setMotorSpeed(const uint16_t speed);
int computeControl(const int sensor, const int divisor);
int processCommand(const char* const cmd, const char* const arg);
int processor(const char* const cmd, const char* const arg);

#endif // GOO_H