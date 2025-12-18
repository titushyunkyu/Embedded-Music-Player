#include "bsp.h"

#define FALSE 0
#define TRUE 1

void Error_Handler(void)
{
    __disable_irq();

    while (1) {
        __NOP();
    }
}

void ActivatePortAandB(void)
{
    // Reset ports A and B, enable power.
    GPIOA->GPRCM.RSTCTL =
      (GPIO_RSTCTL_KEY_UNLOCK_W |
       GPIO_RSTCTL_RESETSTKYCLR_CLR |
       GPIO_RSTCTL_RESETASSERT_ASSERT);

    // This is for port B
    GPIOB->GPRCM.RSTCTL = 
      (GPIO_RSTCTL_KEY_UNLOCK_W |
       GPIO_RSTCTL_RESETSTKYCLR_CLR |
       GPIO_RSTCTL_RESETASSERT_ASSERT);

    GPIOA->GPRCM.PWREN =
      (GPIO_PWREN_KEY_UNLOCK_W |
       GPIO_PWREN_ENABLE_ENABLE);

    // This is for port B
    GPIOB->GPRCM.PWREN =
      (GPIO_PWREN_KEY_UNLOCK_W |
       GPIO_PWREN_ENABLE_ENABLE);

    // Wait for power to stabilize.
    ClockDelay(POWER_STARTUP_DELAY);
}

void InitializeLaunchpad(uint32_t ClockFrequency)
{
    ActivatePortAandB();

    // IOMUX configuration
    IOMUX->SECCFG.PINCM[RED_LED_INDEX] =
      (IOMUX_PINCM_PC_CONNECTED | IOMUX_MODE1);
    IOMUX->SECCFG.PINCM[S1_INDEX] =
      (IOMUX_PINCM_INENA_ENABLE |
       IOMUX_PINCM_PIPD_ENABLE |
       IOMUX_PINCM_PC_CONNECTED |
       IOMUX_MODE1);


    IOMUX->SECCFG.PINCM[BLUE_LED_INDEX] =
      (IOMUX_PINCM_PC_CONNECTED | IOMUX_MODE1);
    
    IOMUX->SECCFG.PINCM[RED2_LED_INDEX] =
      (IOMUX_PINCM_PC_CONNECTED | IOMUX_MODE1);
   
    IOMUX->SECCFG.PINCM[GREEN_LED_INDEX] =
      (IOMUX_PINCM_PC_CONNECTED | IOMUX_MODE1);

    IOMUX->SECCFG.PINCM[S2_INDEX] =
      (IOMUX_PINCM_INENA_ENABLE |
       IOMUX_PINCM_PIPU_ENABLE |
       IOMUX_PINCM_PC_CONNECTED |
       IOMUX_MODE1);


    // Enable outputs in the Data Output Enable (DOE) register.
    RED_LED_PORT->DOE31_0 |= RED_LED_MASK;
    
    BLUE_LED_PORT->DOE31_0 |= BLUE_LED_MASK;
    RED2_LED_PORT->DOE31_0 |= RED2_LED_MASK;
    GREEN_LED_PORT->DOE31_0 |= GREEN_LED_MASK;
    
    // Turn off LEDs initially.
    TurnOffLED1();
    TurnOffLED2Red();
    TurnOffLED2Green();
    TurnOffLED2Blue();

    // Finally, initialize the clocking system.
    switch (ClockFrequency) {
        case 40000000: ClockInitialization_40MHz(); break;
        case 80000000: ClockInitialization_80MHz(0); break;// disable clk_out
        default: ClockInitialization_4MHz_32MHz(ClockFrequency);
    }
}

uint32_t ReadS1(void)
{
    uint32_t ReturnValue = 0;
    ReturnValue = S1_PORT->DIN31_0 & S1_MASK;
    ReturnValue = ReturnValue >> S1_PIN;

    return ReturnValue;
}

uint32_t ReadS2(void)
{
    uint32_t ReturnValue = 0;
    ReturnValue = S2_PORT->DIN31_0 ^ S2_MASK;
    ReturnValue = ReturnValue >> S2_PIN;

    return ReturnValue;
}


void TurnOnLED1(void)
{
    RED_LED_PORT->DOUT31_0 &= ~(RED_LED_MASK);
}

void TurnOffLED1(void)
{
    RED_LED_PORT->DOUT31_0 |= (RED_LED_MASK);
}

void ToggleLED1(void)
{
    RED_LED_PORT->DOUT31_0 ^= (RED_LED_MASK);
}

void TurnOnLED2Red(void)
{
    RED2_LED_PORT->DOUT31_0 |= (RED2_LED_MASK);
}

void TurnOffLED2Red(void)
{
    RED2_LED_PORT->DOUT31_0 &= ~(RED2_LED_MASK);
}

void ToggleLED2Red(void)
{
    RED2_LED_PORT->DOUT31_0 ^= (RED2_LED_MASK);
}

void TurnOnLED2Green(void)
{
    GREEN_LED_PORT->DOUT31_0 |= (GREEN_LED_MASK); 
}

void TurnOffLED2Green(void)
{
    GREEN_LED_PORT->DOUT31_0 &= ~(GREEN_LED_MASK);
}

void ToggleLED2Green(void)
{
    GREEN_LED_PORT->DOUT31_0 ^= (GREEN_LED_MASK);
}

void TurnOnLED2Blue(void)
{
    BLUE_LED_PORT->DOUT31_0 |= (BLUE_LED_MASK);
}

void TurnOffLED2Blue(void)
{
    BLUE_LED_PORT->DOUT31_0 &= ~(BLUE_LED_MASK);
}

void ToggleLED2Blue(void)
{
    BLUE_LED_PORT->DOUT31_0 ^= (BLUE_LED_MASK);
}
