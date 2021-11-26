/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        16000000L
#define FCY             SYS_FREQ/2
#define SYS_1m          16000L//SYS_FREQ/1000
#define SYS_1u          SYS_1m/1000


/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */

//void delay_us(uint8_t);
//void delay_ms(uint8_t);