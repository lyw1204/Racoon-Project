//Pin Mapping
#define STEPPER_P1 4
#define STEPPER_P2 6
#define STEPPER_P3 5
#define STEPPER_P4 7
#define SERVO_P1 3
#define LIM_SW1 2
#define SONIC_TRIG 9
#define SONIC_ECHO 8

#define POWER_PIN 10 

#define DET1_PIN 11
#define DET2_PIN 12
#define DET3_PIN 13

//Operation Constants
#define STEPS_PER_ROT 2048
#define STEPPER_SPD 5

//UNUSED
#define X_RANGE 270
#define Y_RANGE 90

//Offsets and backlash compensations
#define Y_OFFSET 170
#define BACKLASH 5 //degrees of gear backlash



//Matrix Parameters
#define X_RES 27 // size of depth map is 3888 bits
#define Y_RES 9
#define SAMPLES 3
#define MAXDIST 200

//I2C Configuration
#define SLAVE_ADDR 8
