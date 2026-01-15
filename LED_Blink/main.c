#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

int main(void) {
    /* 1. Enable Clock for GPIOA */
    RCC->AHB1ENR |= (1 << 0);

    /* 2. Configure PA1 as Output (LED) */
    GPIOA->MODER &= ~(3 << 2);  /* Clear bits 2 and 3 */
    GPIOA->MODER |= (1 << 2);   /* Set bit 2 to 1 (Output Mode) */

    /* 3. Configure PA0 as Input (Button) */
    /* Mode 00 = Input mode (Default after reset) */
    GPIOA->MODER &= ~(3 << 0);  /* Clear bits 0 and 1 for Input mode */
    
    /* Optional: Set Pull-down for PA0 if your board doesn't have external resistor */
    // GPIOA->PUPDR &= ~(3 << 0);
    // GPIOA->PUPDR |= (2 << 0); /* 10: Pull-down */

    while (1) {
        /* 4. Check if button at PA0 is pressed (High) */
        /* Use IDR (Input Data Register) to read the pin state */
        if (GPIOA->IDR & (1 << 0)) {
            /* Button pressed: Turn ON PA1 */
            GPIOA->ODR |= (1 << 1);
        } else {
            /* Button released: Turn OFF PA1 */
            GPIOA->ODR &= ~(1 << 1);
        }
    }
}