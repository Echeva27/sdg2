/**
 * @file port_system.c
 * @brief File that defines the functions that are related to the access to the specific HW of the microcontroller.
 * @author Sistemas Digitales II
 * @date 2023-01-01
 * makelele
 */

/* Includes ------------------------------------------------------------------*/
#include "port_system.h"

/* Defines -------------------------------------------------------------------*/
#define HSI_VALUE ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz */

/* GLOBAL VARIABLES */
static volatile uint32_t msTicks = 0; /*!< Variable to store millisecond ticks. @warning **It must be declared volatile!** Just because it is modified in an ISR. **Add it to the definition** after *static*. */

/* These variables are declared extern in CMSIS (system_stm32f4xx.h) */
uint32_t SystemCoreClock = HSI_VALUE;                                               /*!< Frequency of the System clock */
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9}; /*!< Prescaler values for AHB bus */
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};                          /*!< Prescaler values for APB bus */

//------------------------------------------------------
// SYSTEM CONFIGURATION
//------------------------------------------------------
/**
 * @brief  Setup the microcontroller system
 *         Initialize the FPU setting, vector table location and External memory
 *         configuration.
 *
 * @note   This function is called at startup by CMSIS in startup_stm32f446xx.s.
 */
void SystemInit(void)
{
/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif

#if defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
  SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#endif                                                 /* USER_VECT_TAB_ADDRESS */
}

/**
 * @brief System Clock Configuration
 *
 * @attention This function should NOT be accesible from the outside to avoid configuration problems.
 * @note This function starts a system timer that generates a SysTick every 1 ms.
 * @retval None
 */
static void system_clock_config(void)
{
  /** Configure the main internal regulator output voltage */
  /* Power controller (PWR) */
  /* Control the main internal voltage regulator output voltage to achieve a trade-off between performance and power consumption when the device does not operate at the maximum frequency */
  PWR->CR &= ~PWR_CR_VOS; // Clean and set value
  PWR->CR |= (PWR_CR_VOS & (POWER_REGULATOR_VOLTAGE_SCALE3 << PWR_CR_VOS_Pos));

  /* Initializes the RCC Oscillators. */
  /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
  RCC->CR &= ~RCC_CR_HSITRIM; // Clean and set value
  RCC->CR |= (RCC_CR_HSITRIM & (RCC_HSI_CALIBRATION_DEFAULT << RCC_CR_HSITRIM_Pos));

  /* RCC Clock Config */
  /* Initializes the CPU, AHB and APB buses clocks */
  /* To correctly read data from FLASH memory, the number of wait states (LATENCY)
      must be correctly programmed according to the frequency of the CPU clock
      (HCLK) and the supply voltage of the device. */

  /* Increasing the number of wait states because of higher CPU frequency */
  FLASH->ACR = FLASH_ACR_LATENCY_2WS; /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */

  /* Change in clock source is performed in 16 clock cycles after writing to CFGR */
  RCC->CFGR &= ~RCC_CFGR_SW; // Clean and set value
  RCC->CFGR |= (RCC_CFGR_SW & (RCC_CFGR_SW_HSI << RCC_CFGR_SW_Pos));

  /* Update the SystemCoreClock global variable */
  SystemCoreClock = HSI_VALUE >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];

  /* Configure the source of time base considering new system clocks settings */
  SysTick_Config(SystemCoreClock / (1000U / TICK_FREQ_1KHZ)); /* Set Systick to 1 ms */
}

size_t port_system_init()
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  /* Configure Flash prefetch, Instruction cache, Data cache */
  /* Instruction cache enable */
  FLASH->ACR |= FLASH_ACR_ICEN;

  /* Data cache enable */
  FLASH->ACR |= FLASH_ACR_DCEN;

  /* Prefetch cache enable */
  FLASH->ACR |= FLASH_ACR_PRFTEN;

  /* Set Interrupt Group Priority */
  NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_4);

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  /* Configure the SysTick IRQ priority. It must be the highest (lower number: 0)*/
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0U, 0U)); /* Tick interrupt priority */

  /* Init the low level hardware */
  /* Reset and clock control (RCC) */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; /* Syscfg clock enabling */

  /* Peripheral clock enable register */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* PWREN: Power interface clock enable */

  /* Configure the system clock */
  system_clock_config();

  return 0;
}

//------------------------------------------------------
// TIMER RELATED FUNCTIONS
//------------------------------------------------------
void port_system_delay_ms(uint32_t ms)
{
  uint32_t tickstart = port_system_get_millis();

  while ((port_system_get_millis() - tickstart) < ms)
  {
  }
}

void port_system_delay_until_ms(uint32_t *p_t, uint32_t ms)
{
  uint32_t until = *p_t + ms;
  uint32_t now = port_system_get_millis();
  if (until > now)
  {
    port_system_delay_ms(until - now);
  }
  *p_t = port_system_get_millis();
}

//------------------------------------------------------
// GPIO RELATED FUNCTIONS
//------------------------------------------------------

void port_system_gpio_config(GPIO_TypeDef *port, uint8_t pin, uint8_t mode, uint8_t pupd)
{
  if (port == GPIOA)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // enable clock A
  }
  if (port == GPIOB)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // enable clock B
  }
  if (port == GPIOC)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // enable clock C
  }
  if (port == GPIOH)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; // enable clock H
  }
  if (port == GPIOD)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // enable clock D
  }

  port->MODER &= ~(GPIO_MODER_MODER0_Msk << 2 * pin); // Limpia el moder en el pin x
  port->MODER |= (mode << 2 * pin);                   // configura el mode en su pin
  port->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk << 2 * pin);  // Limpia el pupdr en el pin x
  port->PUPDR |= (pupd << 2 * pin);                   // configura el pupd en su pin
}

void port_system_gpio_config_exti(GPIO_TypeDef *port, uint8_t pin, uint32_t mode)
{
#define GPIO_EXTI 0x0fUL

  uint8_t n = 0;
  uint8_t m = 0;

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // activamos el bit syscfgen
  if (pin < 4)
  { // elegimos el registro
    n = 0;
  }
  else if (pin < 8)
  {
    n = 1;
  }
  else if (pin < 12)
  {
    n = 2;
  }
  else if (pin < 16)
  {
    n = 3;
  }
  if (port == GPIOA)
  { // elegimos el port
    m = 0x00;
  }
  else if (port == GPIOB)
  {
    m = 0x01;
  }
  else if (port == GPIOC)
  {
    m = 0x02;
  }
  else if (port == GPIOD)
  {
    m = 0x03;
  }
  else if (port == GPIOE)
  {
    m = 0x04;
  }
  else if (port == GPIOF)
  {
    m = 0x05;
  }
  else if (port == GPIOG)
  {
    m = 0x06;
  }
  else if (port == GPIOH)
  {
    m = 0x07;
  }
  SYSCFG->EXTICR[n] &= ~(GPIO_EXTI << 4 * (pin % 4)); // Limpia el exticr con n segun el pin
  SYSCFG->EXTICR[n] |= (m << 4 * (pin % 4));          // colocamos m
  if ((mode & 0x01))
  {
    EXTI->RTSR |= BIT_POS_TO_MASK(pin);
  }
  if ((mode & 0x02))
  {
    EXTI->FTSR |= BIT_POS_TO_MASK(pin);
  }
  if ((mode & 0x03))
  {
    EXTI->RTSR |= BIT_POS_TO_MASK(pin);

    EXTI->FTSR |= BIT_POS_TO_MASK(pin);
  }
  if ((mode & 0x04))
  {
    EXTI->EMR |= BIT_POS_TO_MASK(pin);
  }
  if ((mode & 0x08))
  {
    EXTI->IMR |= BIT_POS_TO_MASK(pin);
  }
}
void port_system_gpio_config_alternate(GPIO_TypeDef *p_port, uint8_t pin, uint8_t alternate)
{

  uint32_t mask = BASE_MASK_TO_POS(0b1111, (pin % 8) * 4);
  uint8_t n = 0;
  if (pin < 8)
  {
    n = 0;
  }
  else
  {
    n = 1;
  }
  p_port->AFR[n] &= ~(mask);
  p_port->AFR[n] |= (alternate << 4 * (pin % 8));
}

bool port_system_gpio_read(GPIO_TypeDef *port, uint8_t pin)
{
  bool value = (bool)(port->IDR & BIT_POS_TO_MASK(pin));
  return value;
}

void port_system_gpio_write(GPIO_TypeDef *port, uint8_t pin, bool value)
{
  if (value)
  {
    port->BSRR |= BIT_POS_TO_MASK(pin);
  }
  else
  {
    port->BSRR |= (BIT_POS_TO_MASK(pin) << 16);
  }
}

void port_system_gpio_toggle(GPIO_TypeDef *port, uint8_t pin)
{
  bool valor = port_system_gpio_read(port, pin);
  port_system_gpio_write(port, pin, !valor);
}

void port_system_gpio_exti_enable(uint8_t pin, uint8_t priority, uint8_t subpriority)
{
  NVIC_SetPriority(GET_PIN_IRQN(pin), NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, subpriority));
  NVIC_EnableIRQ(GET_PIN_IRQN(pin));
}

void port_system_gpio_exti_disable(uint8_t pin)
{
  NVIC_DisableIRQ(GET_PIN_IRQN(pin));
}
uint32_t port_system_get_millis()
{
  return msTicks; /* ms */
}
//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------
/**
 * @brief This function handles the System tick timer that increments the system millisecond counter (global variable).
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. **Increment the System tick counter `msTicks` in 1 count.** \n
 * > &nbsp;&nbsp;&nbsp;&nbsp;💡 `msTicks` is a global `static` variable declared in @link port_system.c @endlink. \n
 *
 * @warning **The variable `msTicks` must be declared volatile!** Just because it is modified in this ISR, in order to avoid [*race conditions*](https://en.wikipedia.org/wiki/Race_condition)
. **Add it to the definition** after *static*.
*
*/
void SysTick_Handler(void)
{
  msTicks++;
}
