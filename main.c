
#include "main.h"
#include "string.h"
#include "UartUser.h"

#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */

void auto_PIN_on (GPIO_TypeDef * PORT, uint16_t numPIN, uint16_t mode);  // Mode и Cfg для переданных портов и пинов
void delay (uint16_t millisec);                                             // Задержка
void init_USARTIT(void);                                                 // Включение прерываний от USART
void startTIM2(uint16_t period);

/* Объявляем переменную типа USART_Settings */
USART_Settings usart;

char str[10] = {};

int main(void)
{
    /*Включение тактирования портов*/
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_IOPAEN_Msk;   // Разрешаем тактирование порта GPIOA
	RCC->APB1ENR = RCC->APB1ENR | RCC_APB1ENR_USART2EN_Msk; // Включаем тактирование USART1
	RCC->APB1ENR = RCC->APB1ENR | RCC_APB1ENR_TIM2EN_Msk;  // Включаем тактирование TIM2
    
    auto_PIN_on(GPIOA, 2, 1);  // Настройка вывода PA9 (TX) MODE = 11, CNF = 10
    auto_PIN_on(GPIOA, 3, 0); // Настройка вывода PA10 (RX) MODE = 00, CNF = 10 
    auto_PIN_on(GPIOC, 13, 2);
    auto_PIN_on(GPIOA, 5, 2);
    
    /* Инициализация USART */
    usart.UartPtr = USART2;
    usart.baude = 19200;
    usart.freq = 8000000;
    UsartInitStruct(&usart);
    
    /* Разрешение прерываний от USART1 */
    init_USARTIT();

    while(1)
    {
        UsartRxIT(USART2, &str, 10);
        delay(500);
        UsartTxIT(USART2, &str, 10);
        delay(500);
    }    
}

/*Задержка*/
void delay (uint16_t millisec)
{
    startTIM2(millisec);
    while (!(TIM2->SR & TIM2_SR_UIF))
    {
    
    }
    TIM2->SR &= ~TIM2_SR_UIF;
    TIM2->CR1 &= ~TIM2_CR1_CEN;
}

/*Включение прерываний от USART1*/
void init_USARTIT(void)
{
	//Включение тактирования на блок альтернативных функций
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_AFIOEN;
	/* Разрешение прерываний */
	NVIC->ISER[(((uint32_t)USART1_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)USART1_IRQn) & 0x1FUL));
    NVIC->ISER[(((uint32_t)USART2_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)USART2_IRQn) & 0x1FUL));
}

void startTIM2(uint16_t period)
{
    uint16_t psc = 7999;
    uint16_t ar = period;
    TIM2->CNT = 0;
    TIM2->SR = 0;
    TIM2->PSC = psc;
    TIM2->ARR = ar;
    TIM2->CR1 |= TIM2_CR1_CEN;
}


/*Функция задает Mode и Cfg для переданных портов и пинов*/
void auto_PIN_on (GPIO_TypeDef * PORT, uint16_t numPIN, uint16_t mode)
{
    if (mode == 1) // запись AF (CFG 10 MOD 11)
    {
        if (numPIN < 8) // младшие
        {
            PORT->CRL &= ~( 3UL << 2U+4U*numPIN | 3UL << 4U*numPIN );             // очистка
            PORT->CRL |= ( 0x02UL << 2U+4U*numPIN | 0x3UL << 4U*numPIN );         // CNF | MOD
        }
        else // старшие
        {
            PORT->CRH &= ~( 3UL << 2U+4U*(numPIN-8) | 3UL << 4U*(numPIN-8) );     // очистка
            PORT->CRH |= ( 0x02UL << 2U+4U*(numPIN-8) | 0x3UL << 4U*(numPIN-8) ); // CNF | MOD
        }
    }
    else if (mode == 2) // запись GP (CFG 00 MOD 11)
    {
        if (numPIN < 8) // младшие
        {
            PORT->CRL &= ~( 3UL << 2U+4U*numPIN | 3UL << 4U*numPIN );             // очистка
            PORT->CRL |= ( 0x00UL << 2U+4U*numPIN | 0x3UL << 4U*numPIN );         // CNF | MOD
        }
        else // старшие
        {
            PORT->CRH &= ~( 3UL << 2U+4U*(numPIN-8) | 3UL << 4U*(numPIN-8) );     // очистка
            PORT->CRH |= ( 0x00UL << 2U+4U*(numPIN-8) | 0x3UL << 4U*(numPIN-8) ); // CNF | MOD
        }
    }
    else //чтение
    {
        if (numPIN < 8) // младшие
        {
            PORT->CRL &= ~( 3UL << 2U+4U*numPIN | 3UL << 4U*numPIN );             // очистка
            PORT->CRL |= ( 0x02UL << 2U+4U*numPIN | 0x00UL << 4U*numPIN );        // CNF | MOD
            PORT->ODR |= ( 1 << numPIN );                                         // подтяжка к питанию
        }
        else // старшие
        {
            PORT->CRH &= ~( 3UL << 2U+4U*(numPIN - 8) | 3UL << 4U*(numPIN - 8) );      // очистка
            PORT->CRH |= ( 0x02UL << 2U+4U*(numPIN - 8) | 0x00UL << 4U*(numPIN - 8) ); // CNF | MOD
            PORT->ODR |= ( 1 << numPIN );                                              // подтяжка к питанию
        }
    }
}

void TIM2_IRQHandler (void)
{
    
}



void HardFault_Handler (void)
{
	while(1)
	{}
}