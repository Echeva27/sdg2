/**
 * @file commands.h
 * @brief Commands definition for the different remotes used in the system using the NEC protocol
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
/* Device: Liluco IR remote */
/* The Liluco IR remote and receiver work on NEC protocol */
#define LIL_ON_BUTTON
#define LIL_OFF_BUTTON
#define LIL_RED_BUTTON 0x00F720DF   /*!< Liluco IR remote command for button RED */
#define LIL_GREEN_BUTTON 0x00F7A05F /*!< Liluco IR remote command for button GREEN */
#define LIL_BLUE_BUTTON 0x00F7609F  /*!< Liluco IR remote command for button BLUE */
#define LIL_WHITE_BUTTON
#define LIL_YELLOW_BUTTON
#define LIL_CYAN_BUTTON
#define LIL_MAGENTA_BUTTON

#define LIL_NUMBER_OF_BUTTONS 9

/* Other devices */
#endif /* COMMANDS_H_ */
