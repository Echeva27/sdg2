/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

/* Includes ------------------------------------------------------------------*/
#include "fsm_button.h"
#include "port_button.h"
#include <stdio.h>

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
    fsm_t f;                /*!< Internal FSM from the library */
    uint32_t debounce_time; /*!< Button debounce time in ms */
    uint32_t next_timeout;  /*!< Next timeout for the debounce in ms */
    uint32_t tick_pressed;  /*!< Number of system ticks when the button was pressed */
    uint32_t duration;      /*!< How much time the button has been pressed */
    uint32_t button_id;

} fsm_button_t;

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
enum FSM_BUTTON_STATES
{
    BUTTON_RELEASED = 0,
    BUTTON_PRESSED_WAIT,
    BUTTON_PRESSED,
    BUTTON_RELEASED_WAIT
};

/* State machine input or transition functions */
static bool check_button_pressed(fsm_t *p_fsm)
{
    fsm_button_t *p_this = (fsm_button_t *)(p_fsm);
    return port_button_is_pressed(p_this->button_id);
}
static bool check_button_released(fsm_t *p_fsm)
{
    fsm_button_t *p_this = (fsm_button_t *)(p_fsm);
    return !port_button_is_pressed(p_this->button_id);
}
static bool check_timeout(fsm_t *p_fsm)
{
    fsm_button_t *p_this = (fsm_button_t *)(p_fsm);
    uint32_t value = port_button_get_tick();
    if (value > p_this->next_timeout)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* State machine output or action functions */
static void do_store_tick_pressed(fsm_t *p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    p_fsm->tick_pressed = port_button_get_tick();
    p_fsm->next_timeout = port_button_get_tick() + p_fsm->debounce_time;
}

static void do_set_duration(fsm_t *p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);

    uint32_t value = port_button_get_tick();
    p_fsm->duration = value - p_fsm->tick_pressed;
    p_fsm->next_timeout = value + p_fsm->debounce_time;
}

static fsm_trans_t fsm_trans_button[] = {
    {BUTTON_RELEASED, check_button_pressed, BUTTON_PRESSED_WAIT, do_store_tick_pressed},
    {BUTTON_PRESSED_WAIT, check_timeout, BUTTON_PRESSED, NULL},
    {BUTTON_PRESSED, check_button_released, BUTTON_RELEASED_WAIT, do_set_duration},
    {BUTTON_RELEASED_WAIT, check_timeout, BUTTON_RELEASED, NULL},
    {-1, NULL, -1, NULL}};

/* Other auxiliary functions */

fsm_t *fsm_button_new(uint32_t debounce_time, uint32_t button_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_init(p_fsm, debounce_time, button_id);
    return p_fsm;
}

void fsm_button_init(fsm_t *p_this, uint32_t debounce_time, uint32_t button_id)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    fsm_init(p_this, fsm_trans_button);
    p_fsm->debounce_time = debounce_time;
    p_fsm->button_id = button_id;
    p_fsm->duration = 0;
    p_fsm->tick_pressed = 0;
    port_button_init(button_id);
    
}


uint32_t fsm_button_get_duration(fsm_t *p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    return p_fsm->duration;
}

void fsm_button_reset_duration(fsm_t *p_this)
{
    fsm_button_t *p_fsm = (fsm_button_t *)(p_this);
    p_fsm->duration = 0;
}
