/**
 * @file port_button.c
 * @brief File containing functions related to the HW of the button FSM.
 *
 * This files defines an internal struct which coontains the HW information of the
 *
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

/* Includes ------------------------------------------------------------------*/
#include "port_button.h"

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
    GPIO_TypeDef *p_port;
    uint8_t pin;
    bool flag_pressed;
} port_button_hw_t;

/* Global variables ------------------------------------------------------------*/

static port_button_hw_t buttons_arr[] = {
    [BUTTON_0_ID] = {.p_port = BUTTON_0_GPIO, .pin = BUTTON_0_PIN, .flag_pressed = false},
};

void port_button_init(uint32_t button_id)
{
    GPIO_TypeDef *p_port = buttons_arr[button_id].p_port;
    uint8_t pin = buttons_arr[button_id].pin;

    port_system_gpio_config(p_port, pin, GPIO_MODE_IN, GPIO_PUPDR_NOPULL);
    port_system_gpio_config_exti(p_port, pin, 15);
    port_system_gpio_exti_enable(pin, 1, 0);
}

bool port_button_is_pressed(uint32_t button_id)
{
    return buttons_arr[button_id].flag_pressed;
}

uint32_t port_button_get_tick()
{
    return port_system_get_millis();
}

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------
/**
 * @brief  This function handles Px10-Px15 global interrupts.
 *
 * First, this function identifies the line/ pin which has raised the interruption. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
 *
 * > **TO-DO alumnos**
 * >
 * > ✅ 1. Read the value of the GPIO og the user button. Use the #BUTTON_0_ID index to get the port and pin from the `buttons_arr` \n
 * > ✅ 2. If the value is #HIGH means that the button as been released, therefore set the field `flag_pressed` to `false` \n
 * > ✅ 3. If the value is #LOW means that the button as been pressed, therefore set the field `flag_pressed` to `true` \n
 * > ✅ 4. Clean the corresponding bit of the `PR` register \n
 * > &nbsp;&nbsp;&nbsp;&nbsp;💡 Recommendation: use the macro #BIT_POS_TO_MASK \n
 * > &nbsp;&nbsp;&nbsp;&nbsp;💡 **To clean an interrupt in the *Pending Register* (`PR`) we must write '1' in the corresponding bit**. *Look at the example in section "3.9.7. Ejemplo de interrupción externa" of the reference book.* You can also find this out by consulting the *reference manual* (section "10.3.6 Pending register (EXTI_PR)") * \n
 * \n
 *
 */
void EXTI15_10_IRQHandler(void)
{
    /* ISR user button in PC13 */
    if (EXTI->PR & BIT_POS_TO_MASK(buttons_arr[BUTTON_0_ID].pin))
    {
        if (port_system_gpio_read((buttons_arr[BUTTON_0_ID].p_port), (buttons_arr[BUTTON_0_ID].pin)))
        {
            (buttons_arr[BUTTON_0_ID].flag_pressed) = false;
        }
        else
        {
            (buttons_arr[BUTTON_0_ID].flag_pressed) = true;
        }
        EXTI->PR |= (BIT_POS_TO_MASK(buttons_arr[BUTTON_0_ID].pin));
    }
}
