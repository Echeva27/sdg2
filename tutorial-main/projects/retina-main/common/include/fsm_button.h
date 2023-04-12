/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

#ifndef FSM_BUTTON_H_
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "fsm.h"
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Create a new button FSM.

This FSM implements an anti-debounce mechanism. Debounces (or very fast button presses) lasting less than the debounce_time are filtered out.

The FSM stores the duration of the last button press. The user should ask for it using the function fsm_button_get_duration().

At start and reset, the duration value must be 0 ms. A value of 0 ms means that there has not been a new button press.


 * In other words, the status flag of this FSM is the variable duration. A duration of 0 means that there has been no new button press, a value other than 0 represents that it has been pressed and the value is its duration, and thus it is the user's responsibility to clear this state flag.

The FSM contains Information of the button ID. This ID is a unique identifier that is managed by the user in the port. That is where the user provides identifiers and HW information for all the buttons on his system. The FSM does not have to know anything of the underlying HW.
 * 
 * 
 * 
 * @param debounce_time	Anti-debounce time in milliseconds
 * @param button_id	Unique button identifier number

*/
fsm_t *fsm_button_new(uint32_t debounce_time, uint32_t button_id);

/**
 *  @brief This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
 *
 *
 *
 *
 * @param p_this	Pointer to an fsm_t struct than contains an fsm_button_t.
    @param debounce_time	Anti-debounce time in milliseconds
    @param button_id	Unique button identifier number
 *
*/
void fsm_button_init(fsm_t *p_this, uint32_t debounce_time, uint32_t button_id);

/**
 * @brief Return the duration of the last button press.
 *
 * @param p_this p_this	Pointer to an fsm_t struct than contains an fsm_button_t.
 *
 * @return uint32_t
 *
 */
uint32_t fsm_button_get_duration(fsm_t *p_this);

/**
 * @brief Reset the duration of the last button press.
 *
 * @param p_this	Pointer to an fsm_t struct than contains an fsm_button_t.
 *
 *
*/
void fsm_button_reset_duration(fsm_t *p_this);

/**
 * @brief Check if the button FSM is active, or not.
 *
 *  The button is inactive when it is in the status BUTTON_RELEASED.
 *
 *
 * @param p_this	Pointer to an fsm_t struct than contains an fsm_button_t.
 *
 * @returns true or false
**/
bool fsm_button_check_activity(fsm_t *p_this);
#endif
