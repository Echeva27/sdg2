/**
 * @file retina.c
 * @brief Main file.
 * @author Sistemas Digitales II
 * @date 2023-01-01
 */

/* Includes ------------------------------------------------------------------*/
#include "retina.h"
#include "fsm_button.h"
#include "port_button.h"
#include "fsm_tx.h"
#include "fsm_retina.h"
/* Variable initialization functions */
#define CHANGE_MODE_BUTTON_TIME 3000
/* State machine input or transition functions */

/* State machine output or action functions */

/* Other auxiliary functions */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /*
    port_system_init();
    fsm_t *p_fsm_user_button = fsm_button_new(150,0);
    port_system_gpio_config(GPIOA,5,1,0);


    while (1)
    {
    fsm_fire(p_fsm_user_button);
    uint32_t but_press_duration = fsm_button_get_duration(p_fsm_user_button);
    if(but_press_duration){
        if(but_press_duration >= 3000){
            port_system_gpio_toggle(GPIOA,5);
        }
        fsm_button_reset_duration(p_fsm_user_button);
    }

    }*/
    port_system_init();
    fsm_t *p_fsm_button = fsm_button_new(150, 0);
    fsm_t *p_fsm_tx = fsm_tx_new(0);
    fsm_t *p_fsm_retina = fsm_retina_new(p_fsm_button, CHANGE_MODE_BUTTON_TIME, p_fsm_tx);
    while (1)
    {
        fsm_fire(p_fsm_button);
        fsm_fire(p_fsm_tx);
        fsm_fire(p_fsm_retina);
    }
    fsm_destroy(p_fsm_button);
    fsm_destroy(p_fsm_tx);
    fsm_destroy(p_fsm_retina);
}
