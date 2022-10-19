
#include "stm32f101xb.h"

/* Структура для инициализации */
typedef struct
{
    USART_TypeDef *UartPtr;
    int32_t baude;
    int32_t freq;
    int32_t WordLength;
    int32_t StopBits;
    int32_t Mode;
    
} USART_Settings;

/* Функции отправки байтов */
uint8_t UsartTx(USART_TypeDef *UartNum, char *buf, int16_t SizeData);
void UsartTxIT(USART_TypeDef *UartNum, char *buf, int16_t SizeData);

/* Функции приема байтов */
uint8_t UsartRx(USART_TypeDef *UartNum, char *buf, int16_t SizeData);
void UsartRxIT(USART_TypeDef *UartNum, char *buf, int16_t SizeData);

/* Функции инициализации */
void UsartInit(USART_TypeDef *UartNum, int32_t baude, int32_t freq);
void UsartInitStruct(USART_Settings *usart);

/* Callback функция USART1 после отправки байтов */
void USART1_TX_Callback(void);

/* Callback функция USART1 после приема байтов */
void USART1_RX_Callback(void);

/* Callback функция USART2 после отправки байтов */
void USART2_TX_Callback(void);

/* Callback функция USART2 после приема байтов */
void USART2_RX_Callback(void);