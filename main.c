#include "main.h"
#include "string.h"

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

void auto_LED_on (GPIO_TypeDef * PORT, uint16_t numPIN, uint16_t state); // Состояние numPIN-ого пина
void auto_PIN_on (GPIO_TypeDef * PORT, uint16_t numPIN, uint16_t mode);  // Mode и Cfg для переданных портов и пинов
uint16_t LED_var1(uint16_t numLED);                                      // Функция для Вар. 1 (бегущие огни)
uint16_t LED_var2(uint16_t numLED);                                      // Функция для Вар. 2 (бегущие огни с реверсом)
void LED_ON(uint8_t num_LED);                                            // Функция зажигает светодиоды
void delay (uint32_t ticks);                                             // Задержка для светодиодов
void init_Button(void);                                                  // Включение прерываний от пина 13
int LOAD_TOAGLE(void);                                                   // Функция читает с тумблера значение
int16_t takt_brr(double baude, double freq);                             // Функция расчета коэффициента деления скорости передачи

void UartTx(USART_TypeDef *UartNum, char *buf, int16_t SizeData);

/*Не менять*/
uint32_t PUSH_num = 0;           // Количество нажатий кнопки
uint32_t state;                  // Текущее состояние огней
uint32_t pb = 1;                 // Параметр перехода между состояниями 0 и 1
uint16_t n = 1;                  // Параметр организации реверса
uint16_t cd = 0;                 // Значение с тумблера
uint16_t s_led = 1;              // Начальное значение светодиодов

/*Параметры скорости огонька*/
float f0 = 0.0000005;            // Константное значение частоты огонька [попугай]
float f = 0.0000005;             // Частота по умолчанию [попугай]

/* Скорость передачи [бод] и частота тактирования [Гц]*/
double bd = 19200, fq = 8000000; // bd - скорость в бодах, fq - частота тактирования USART1

int8_t d;

int main(void)
{
    /*Включение тактирования портов*/
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_IOPAEN_Msk;   // Разрешаем тактирование порта GPIOA
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_USART1EN_Msk; // Включаем тактирование USART1

    /*Настройка порта для светодиодов. Режим записи*/       // 1 запись AF (CFG 10 MOD 11)
    for (uint16_t i = 0; i <= 7; i++)                       // 2 запись GP (CFG 00 MOD 11)
    {                                                       // 3 чтение    (CFG 10 MOD 00)
        auto_PIN_on(GPIOA, i, 2);
    }

    // Настройка вывода PA9 (TX) MODE = 11, CNF = 10
    auto_PIN_on(GPIOA, 9, 1);

    // Настройка вывода PA10 (RX) MODE = 00, CNF = 10 
    auto_PIN_on(GPIOA, 10, 0);         
    
    // Включаем USART1
    USART1->CR1 = USART_CR1_UE;  // zz10 0000 0000 0000
    
    // Разрешаем приемник и передатчик
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    
    // Регистр установки коэффициента деления BRR
    USART1->BRR = (uint32_t)(takt_brr(bd, fq));
    
    /*Настройка кнопки*/
	GPIOC->CRH = (GPIOC->CRH) & ( ~( GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1));
	GPIOC->CRH = ((GPIOC->CRH) | ( GPIO_CRH_CNF13_0 | GPIO_CRH_MODE13_1));
	GPIOC->ODR = ((GPIOC->ODR) | ( GPIO_PIN_13));
    init_Button();
    
    while(1)
    {
        char str[] = "Hello Andrey, how are you?\r\n";
        UartTx(USART1, &str, strlen(str));
        delay(100000);
    }    
}

/* Функция расчета коэффициента деления скорости передачи*/
void UartTx(USART_TypeDef *UartNum, char *buf, int16_t SizeData)
{
    for (uint16_t i = 0; i < SizeData; i++)
    {
        while(!(UartNum->SR & USART_SR_TXE))
        {
        }
        UartNum->DR = buf[i];
    }
                                                                                                        
}

/*Задержка для светодиодов*/
void delay (uint32_t ticks)
{
	for (uint32_t i = 0; i < ticks; i++)
	{
	}
}

/* Функция расчета коэффициента деления скорости передачи*/
int16_t takt_brr(double baude, double freq)
{
    double brr_d;
    brr_d = freq/(16*baude);
    int16_t brr_m, brr_f;
    brr_m = (int16_t)brr_d;
    brr_f = (int16_t)(round((brr_d-(double)brr_m)*16));
    
    return brr_m << 4 | brr_f;
}

/*Включение прерываний от пина 13*/
void init_Button(void)
{
	//Включение тактирования на блок альтернативных функций
	RCC->APB2ENR = RCC->APB2ENR | RCC_APB2ENR_AFIOEN;
	
	/* Разрешение прерываний */
	NVIC->ISER[(((uint32_t)USART1_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)USART1_IRQn) & 0x1FUL));
}

void USART1_IRQHandler (void)
{
    USART1->DR = d;
}

/*Состояние numPIN-ого пина*/
void auto_LED_on (GPIO_TypeDef * PORT, uint16_t numPIN, uint16_t state)
{
    uint16_t GPIO_PIN = 1 << numPIN;
    if (state == 0)
        PORT->BRR = (PORT->BRR | GPIO_PIN);
    else
        PORT->BSRR = (PORT->BSRR | GPIO_PIN);
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