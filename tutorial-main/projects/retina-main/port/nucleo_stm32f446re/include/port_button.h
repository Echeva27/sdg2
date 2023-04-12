/**
 * @file port_button.h
 * @brief Header for port_button.c file.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

#ifndef PORT_BUTTON_H_
#define PORT_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "port_system.h"

/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define BUTTON_0_ID 0 /* ID of button */
#define BUTTON_0_GPIO GPIOC /*PORT of button*/
#define BUTTON_0_PIN 13 /*PIN of button*/
#define BUTTON_0_DEBOUNCE_TIME_MS 150 /*DEBOUNCE TIME paramether in ms*/

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given button.
 *
 * @param button_id	Button ID. This index is used to select the element of the buttons_arr[] array
 *
 */
void port_button_init(uint32_t button_id);

/**
 * @brief Return the status of the button (pressed or not)
 *
 * @param button_id	Button ID. This index is used to select the element of the buttons_arr[] array
 *
 * @return True If the button has been pressed
 * @return False If the button has not been pressed
 */
bool port_button_is_pressed(uint32_t button_id);

/**
 * @brief Return the count of the System tick in milliseconds.
 *
 *
 * @return uint32_t
 *
 */
uint32_t port_button_get_tick();

#endif
