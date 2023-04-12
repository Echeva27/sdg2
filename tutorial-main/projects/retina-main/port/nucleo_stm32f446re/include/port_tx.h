/**
 * @file port_tx.h
 * @brief Header for port_tx.c file.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

#ifndef PORT_TX_H_
#define PORT_TX_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "port_system.h"
/* Standard C includes */

/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define IR_TX_0_ID 0 /* ID of tx*/

#define IR_TX_0_GPIO GPIOB /* PORT of tx*/

#define IR_TX_0_PIN 10 /* PIN of tx*/
/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Configure the HW specifications of a given infrared transmitter.
 * 
 * 
 * @param tx_id	Transmitter ID. This index is used to select the element of the transmitters_arr[] array
 * @param status	To indicate if PWM starts, or not, from the beginning
*/
void port_tx_init(uint8_t tx_id, bool status);

/**
 * @brief Set the PWM ON or OFF.
 *
 *   Considering the ID (if your system has more tha one transmitter):
 * 
 * 
 * @param tx_id	Transmitter ID. This index is used to select the element of the transmitters_arr[] array
 * @param status	To indicate if PWM starts, or not, from the beginning
 * 
*/
void port_tx_pwm_timer_set(uint8_t tx_id, bool status);

/**
 * @brief Start the symbol timer and reset the count of ticks.
 * 
*/
void port_tx_symbol_tmr_start();

/**
 * @brief Stop the symbol timer.
 * 
*/
void port_tx_symbol_tmr_stop();

/**
 * @brief Get the count of the symbol ticks.
 * 
 * @return uint32_t
*/
uint32_t port_tx_tmr_get_tick();
#endif