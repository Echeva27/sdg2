/**
 * @file port_tx.c
 * @brief Portable functions to interact with the infrared transmitter FSM library.
 * @author Álvaro García Ruiz-Escribano
 * @author Jorge Echevarria de Uribarri
 * @date 29/03/2023
 */

/* Includes ------------------------------------------------------------------*/
#include "port_tx.h"
#include "fsm_tx.h"
/* Defines --------------------------------------------------------------------*/
#define ALT_FUNC1_TIM2 1 /*!< TIM2 Alternate Function mapping */

/* IMPORTANT
The timer symbol is the same for all the TX, so it is not in the structure of TX. It has been decided to be the TIM1. It is like a systick but faster.
*/

/* Typedefs --------------------------------------------------------------------*/
typedef struct
{
  GPIO_TypeDef *p_port;
  uint8_t pin;
  uint8_t alt_func;

} port_tx_hw_t;

/* Global variables ------------------------------------------------------------*/
static volatile uint32_t symbol_tick;

static port_tx_hw_t transmitters_arr[] = {[IR_TX_0_ID] = {.p_port = IR_TX_0_GPIO, .pin = IR_TX_0_PIN, .alt_func = ALT_FUNC1_TIM2}};
/* Infrared transmitter private functions */
static void _timer_symbol_setup()
{
  /* TO-Do alumnos */
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  TIM1->CR1 &= ~BIT_POS_TO_MASK(0);
  TIM1->CR1 |= BIT_POS_TO_MASK(7);

  TIM1->CNT = 0;
  uint32_t arr = 899;
  TIM1->ARR = arr;

  TIM1->PSC = 0;

  TIM1->EGR |= BIT_POS_TO_MASK(0);

  TIM1->SR &= ~BIT_POS_TO_MASK(0);

  TIM1->DIER |= BIT_POS_TO_MASK(0);
  NVIC_SetPriority(TIM1_UP_TIM10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0)); /* Priority 1, sub-priority 0 */
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);                                                          /* Enable interrupt */
}

static void _timer_pwm_setup(uint32_t tx_id)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  TIM2->CR1 &= ~BIT_POS_TO_MASK(0);
  TIM2->CR1 |= BIT_POS_TO_MASK(7);

  TIM2->CNT = 0;

  double arr = (SystemCoreClock / NEC_PWM_FREQ_HZ) - 1;
  TIM2->ARR = arr;
  TIM2->PSC = 0;

  TIM2->EGR |= BIT_POS_TO_MASK(0);

  TIM2->CCER &= ~TIM_CCER_CC3E;
#define TIM_AS_PWM1_MASK 0x0060
  TIM2->CCMR2 |= TIM_CCMR2_OC3PE;
  TIM2->CCMR2 |= TIM_AS_PWM1_MASK;

  TIM2->CCR3 = NEC_PWM_DC * (arr + 1);
}

/* Public functions */
void port_tx_init(uint8_t tx_id, bool status)
{
  GPIO_TypeDef *p_port = transmitters_arr[tx_id].p_port;
  uint8_t pin = transmitters_arr[tx_id].pin;
  uint8_t alt_func = transmitters_arr[tx_id].alt_func;

  port_system_gpio_config(p_port, pin, GPIO_MODE_ALTERNATE, GPIO_PUPDR_NOPULL);
  port_system_gpio_config_alternate(p_port, pin, alt_func);
  _timer_symbol_setup();
  _timer_pwm_setup(tx_id);
  port_tx_pwm_timer_set(tx_id, status);
}

/* In order to make the academic effort of configuring the PWM, the values: timer, channel and masks are hardcoded and this function is not generic for any timer/channel. It is not the best way, but it is as it. */
void port_tx_pwm_timer_set(uint8_t tx_id, bool status)
{
  if (status)
  {
    TIM2->CCER |= BIT_POS_TO_MASK(8);
    TIM2->CR1 |= BIT_POS_TO_MASK(0);
  }
  else
  {
    TIM2->CCER &= ~BIT_POS_TO_MASK(8);
    TIM2->CR1 &= ~BIT_POS_TO_MASK(0);
  }
}

void port_tx_symbol_tmr_start()
{
  TIM1->CNT = 0;
  TIM1->CR1 |= BIT_POS_TO_MASK(0);
  symbol_tick = 0;
}

void port_tx_symbol_tmr_stop()
{
  TIM1->CR1 &= ~BIT_POS_TO_MASK(0);
}

uint32_t port_tx_tmr_get_tick()
{
  return symbol_tick;
}

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------

void TIM1_UP_TIM10_IRQHandler(void)
{
  TIM1->SR &= ~BIT_POS_TO_MASK(0);
  symbol_tick++;
}