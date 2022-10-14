
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
void delay (uint32_t ticks);                                             // Задержка
void init_USARTIT(void);                                                 // Включение прерываний от USART

/* Объявляем переменную типа USART_Settings */
USART_Settings usart;

int main(void)
{
    /*Включение тактирования портов*/
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_IOPAEN_Msk;   // Разрешаем тактирование порта GPIOA
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_USART1EN_Msk; // Включаем тактирование USART1

    auto_PIN_on(GPIOA, 9, 1);  // Настройка вывода PA9 (TX) MODE = 11, CNF = 10
    auto_PIN_on(GPIOA, 10, 0); // Настройка вывода PA10 (RX) MODE = 00, CNF = 10 
    
    /* Инициализация USART */
    usart.UartPtr = USART1;
    usart.baude = 19200;
    usart.freq = 8000000;
    UsartInitStruct(&usart);
    
    /* Разрешение прерываний от USART1 */
    init_USARTIT();

    while(1)
    {
        char str[] = "Hello Piter\r\n";
        UsartTxIT(USART1, &str, strlen(str));
        delay(1000000);
    }    
}

/*Задержка*/
void delay (uint32_t ticks)
{
	for (uint32_t i = 0; i < ticks; i++)
	{
	}
}

/*Включение прерываний от USART1*/
void init_USARTIT(void)
{
	//Включение тактирования на блок альтернативных функций
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_AFIOEN;
	/* Разрешение прерываний */
	NVIC->ISER[(((uint32_t)USART1_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)USART1_IRQn) & 0x1FUL));
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

void HardFault_Handler (void)
{
	while(1)
	{}
}