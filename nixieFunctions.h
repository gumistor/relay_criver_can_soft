/* 
 * File:   nixieFunctions.h
 * Author: Krzysztof
 *
 * Created on 22 kwiecie? 2015, 19:02
 */

#ifndef NIXIEFUNCTIONS_H
#define	NIXIEFUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

//numbers of laps in order:
//H1 H0   M1 M0   S1 S0
//H1 - hour: most significant digit
//H1 - hour: less significant digit
//M1 - minute: most significant digit
//M0 - minute: less significant digit
//S1 - second: most significant digit
//S0 - second: less significant digit
#define LAMP_H1 0
#define LAMP_H0 1
#define LAMP_M1 2
#define LAMP_M0 3
#define LAMP_S1 4
#define LAMP_S0 5

//pin numbers asociated with lamps
#define LAMP_H1_PIN 5 //B
#define LAMP_H0_PIN 4 //B
#define LAMP_M1_PIN 1 //B
#define LAMP_M0_PIN 0 //B
#define LAMP_S1_PIN 7 //C
#define LAMP_S0_PIN 6 //C

/*
 #define LAMP_H1_PIN 6
#define LAMP_H0_PIN 7
#define LAMP_M1_PIN 5
#define LAMP_M0_PIN 4
#define LAMP_S1_PIN 1
#define LAMP_S0_PIN 0
 */

//defined values for action tracking in main program
//values are usualy returned by interrutps
#define UPDATE_DIGITS       0x01    //writing to digits values
#define UPDATE_RESERVED     0x02    //not used
#define UPDATE_CONFIG       0x04    //writing to configuration values
#define UPDATE_LAMPS        0x08   //update of the anode occured

//default values for configuration purposes
//values recalled after power up
#define DEFALUT_REFRESH_RATE    0xD8    //refresh of the lamps, counter value
#define DEFAULT_PWM_FREQUENCY   0x70    //counter value to establish pwm frequency
#define DEFAULT_PWM_DUTYCYCLE   0x45    //counter value to set duty cycle
                                        //in regards to pwm frequency

//aditional margins to keep other values of PWM out of configuration
//it might happend, that the value will be 100% what will cause high
//level on the input of dc/dc converter and damage of a transistor
#define DEFAULT_PWM_DUTYCYCLE_MARGIN    0x10
#define DEFAULT_PWM_FREQUENCY_MARGIN    0x10

uint8_t lampsDigits[6];     //values of digits according to defined values
uint8_t lampsDots;          //values of digits stored in one byte
                            // 0 | 0 | H1 | H0 | M1 | M0 | S1 | S0
                            // 7 | 6 | 5  | 4  | 3  | 2  | 1  | 0

//variables with configuration informations
//current values
uint8_t lampsRefreshRate;   //refresh rate of the digits
uint8_t lampsPWMDutyCycle;  //PWM duty cycle value
uint8_t lampsPWMFrequency;  //PWM frequency value
uint8_t lampsNixieOnStatus; //ON/OFF satatus

//variables with configuration informations
//new, received values
uint8_t newlampsRefreshRate;    //refresh rate of the digits
uint8_t newlampsPWMDutyCycle;   //PWM duty cycle value
uint8_t newlampsPWMFrequency;   //PWM frequency value
uint8_t newlampsNixieOnStatus;  //ON/OFF satatus

//action tracker, indicates what data has been recevied from interrupts
uint8_t updateEnable;

//fucntions to set '1' of an output
void setPortCPinOn(uint8_t);
void setPortBPinOn(uint8_t);
void setPortAPinOn(uint8_t);

//fucntions to set '0' of an output
void setPortCPinOff(uint8_t);
void setPortBPinOff(uint8_t);
void setPortAPinOff(uint8_t);

//function to set active one of the lamps' anodes: H1...S0
void setLamp(uint8_t);

//function to set value of the lamp
//lamps are multiplexed so the combination of setLamp() and setNumber will
//switch on a digit
void setNumber(uint8_t);

//set dots for actual lamp
void setDot(uint8_t);

//function that send to the bus frame with current configuration values
void sendStatusFrame(void);

#ifdef	__cplusplus
}
#endif

#endif	/* NIXIEFUNCTIONS_H */

