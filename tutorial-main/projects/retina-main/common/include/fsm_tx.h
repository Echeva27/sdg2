/**
 * @file fsm_tx.h
 * @brief Header for fsm_tx.c file.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

#ifndef FSM_TX_H_
#define FSM_TX_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
/* NEC transmission macros */
#define NEC_TX_TIMER_TICK_BASE_US 56.25 /*!< Time base in microseconds to create the ticks for the timer of symbols */
#define NEC_TX_PROLOGUE_TICKS_ON 160    /*!< Number of time base ticks for prologue ON in transmission  */
#define NEC_TX_PROLOGUE_TICKS_OFF 80    /*!< Number of time base ticks for prologue OFF in transmission  */
#define NEC_TX_SYM_0_TICKS_ON 10        /*!< Number of time base ticks for symbol 0 ON in transmission  */
#define NEC_TX_SYM_0_TICKS_OFF 10       /*!< Number of time base ticks for symbol 0 OFF in transmission  */
#define NEC_TX_SYM_1_TICKS_ON 10        /*!< Number of time base ticks for symbol 1 ON in transmission  */
#define NEC_TX_SYM_1_TICKS_OFF 30       /*!< Number of time base ticks for symbol 1 OFF in transmission  */
#define NEC_TX_EPILOGUE_TICKS_ON 10     /*!< Number of time base ticks for epilogue ON in transmission  */
#define NEC_TX_EPILOGUE_TICKS_OFF 3560  /*!< Number of time base ticks for epilogue OFF in transmission ~200 miliseconds */
#define NEC_PWM_FREQ_HZ 38000           /*!< PWM timer frequency in Hz */
#define NEC_PWM_DC 0.5                  /*!< PWM duty cycle 0-1  */

/* Function prototypes and explanation ----------------------------------------*/


/**
 * @brief Create a new infrared transmitter FSM.

This FSM waits until there is a code different than '0x00' in the field code of the FSM struct.

At start and reset, the code value must be '0x00'. A value of '0x00' means that there is no code to transmit.
 * 
 * The FSM contains information of the transmitter ID. This ID is a unique identifier that is managed by the user in the port. That is where the user provides identifiers and HW information for all the transmitters on his system. The FSM does not have to know anything of the underlying HW.
 * 
 * @param tx_id	Unique infrared transmitter identifier number
 *
*/
fsm_t *fsm_tx_new(uint8_t tx_id);

/**
 * @briefInitialize an infrared transmitter FSM.
 *
This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 * 
 * 
 * 
 * 
 * @param p_this	Pointer to an fsm_t struct than contains an fsm_tx_t
 * @param tx_id	Unique infrared transmitter identifier number.
 * 
*/
void fsm_tx_init(fsm_t *p_this, uint8_t tx_id);

/**
 * @brief^Set the code given.
 * 
 * 
 * 
 * @param p_this	Pointer to an fsm_t struct than contains an fsm_tx_t.
 * @param code	Code of the NEC command to be transmitted.
*/
void fsm_tx_set_code(fsm_t *p_this, uint32_t code);

/**
 * @brief Start the process to transmit the code stored.
 * 
 * 
 * @param tx_id	Unique infrared transmitter identifier number
 * @param code	Code of the NEC command to be transmitted.

*/
void fsm_send_NEC_code(uint8_t tx_id, uint32_t code);

/**
 * @brief Check if the transmitter FSM is active, or not. As the system is the one controlling the output (PWM and symbol timer), this FSM will always be inactive and will wake up at each interruption of the symbol timer or PWM timer.
 * 
 * 
 * @param p_this	Pointer to an fsm_t struct than contains an fsm_rx_t.
 *
 * @return true or false
*/
bool fsm_tx_check_activity(fsm_t *p_this);
#endif
