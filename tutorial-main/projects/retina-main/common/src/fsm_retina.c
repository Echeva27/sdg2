/**
 * @file fsm_retina.c
 * @brief Retina FSM main file.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

/* Includes ------------------------------------------------------------------*/
#include "fsm_retina.h"
#include "commands.h"
#include "fsm_button.h"
#include "fsm_tx.h"
/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define COMMANDS_MEMORY_SIZE 3 /*!< Number of NEC commands stored in the memory of the system Retina */

/* Enums */
enum
{
    WAIT_TX = 0, /*!< **Single state in Version 2**. State to wait in transmission mode */
};

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the Retina FSM.
 */
typedef struct
{
    fsm_t f; /*!< Retina FSM  */
    fsm_t *p_fsm_button;
    fsm_t *p_fsm_tx;
    uint32_t long_button_press_ms;
    uint32_t tx_codes_arr[COMMANDS_MEMORY_SIZE];
    uint8_t tx_codes_index;

} fsm_retina_t;

/* State machine input or transition functions */

static bool check_short_pressed(fsm_t *p_this)
{
    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    uint32_t duracion = fsm_button_get_duration(p_fsm->p_fsm_button);
    if ((p_fsm->long_button_press_ms > duracion) && (duracion != 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* State machine output or action functions */

static void do_send_next_msg(fsm_t *p_this)
{
    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    uint32_t index = p_fsm->tx_codes_index;
    fsm_tx_set_code(p_fsm->p_fsm_tx, p_fsm->tx_codes_arr[index]);
    fsm_button_reset_duration(p_fsm->p_fsm_button);
    p_fsm->tx_codes_index = (index + 1) % COMMANDS_MEMORY_SIZE;
}
static fsm_trans_t fsm_trans_retina[] = {{WAIT_TX, check_short_pressed, WAIT_TX, do_send_next_msg},
                                         {-1, NULL, -1, NULL}};
/* Other auxiliary functions */
fsm_t *fsm_retina_new(fsm_t *p_fsm_button, uint32_t button_press_time, fsm_t *p_fsm_tx)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_retina_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_retina_init(p_fsm, p_fsm_button, button_press_time, p_fsm_tx);
    return p_fsm;
}

void fsm_retina_init(fsm_t *p_this, fsm_t *p_fsm_button, uint32_t button_press_time, fsm_t *p_fsm_tx)
{
    fsm_retina_t *p_fsm = (fsm_retina_t *)(p_this);
    p_fsm->p_fsm_button = p_fsm_button;
    p_fsm->p_fsm_tx = p_fsm_tx;

    fsm_init(p_this, fsm_trans_retina);
    p_fsm->long_button_press_ms = button_press_time;
    p_fsm->tx_codes_index = 0;
    p_fsm->tx_codes_arr[0] = LIL_RED_BUTTON;   // 16195807
    p_fsm->tx_codes_arr[1] = LIL_GREEN_BUTTON; // 16228447
    p_fsm->tx_codes_arr[2] = LIL_BLUE_BUTTON;  // 16212127
}
