
#include "UartUser.h"

static char UartUserBufTX[100] = {};
static int16_t SizeUartTx = 0;
static int16_t Counter = 0;

static uint16_t w = 1;
static uint16_t ucnt = 0; 
static uint8_t d = 0;
static char *UartUserBufRX;
static int16_t SizeUartRx = 0;
static USART_TypeDef *UartRxNum;
static uint8_t s;
    
/* Передача через USART без прерываний */
uint8_t UsartTx(USART_TypeDef *UartNum, char *buf, int16_t SizeData)
{
    UartNum->CR1 |= USART_CR1_TE;
    uint16_t i = 0;
    uint8_t s = 0;
    uint8_t j = 0;

    while (i < 1000)
    {
        i++;
        if (UartNum->SR & USART_SR_TC)
        {
            UartNum->DR = buf[j];
            i = 0;
            j++;
            if (j == SizeData)
            {
                s = 1;
                break;  
            }
        }
    }
    UartNum->CR1 &= ~USART_CR1_TE;
    return s;
}

/* Прием через USART без прерываний */
uint8_t UsartRx(USART_TypeDef *UartNum, char *buf, int16_t SizeData)
{
    uint16_t i = 0;
    uint8_t s = 0;
    uint8_t j = 0;
    UartNum->CR1 |= USART_CR1_RE;
    
        while (i < 1000)
        {
            i++;
            if (UartNum->SR & USART_SR_RXNE)
            {
                buf[j] = UartNum->DR;
                i = 0;
                j++;
                if (j == SizeData)
                {
                    s = 1;
                    break;
                } 
            }
        }
    UartNum->CR1 &= ~USART_CR1_RE;
    return s;
}

/* Передача через USART через прерывания */
void UsartTxIT(USART_TypeDef *UartNum, char *buf, int16_t SizeData)
{
    SizeUartTx = SizeData;
    memcpy(UartUserBufTX, buf, SizeData);
    UartNum->CR1 |= USART_CR1_TXEIE | USART_CR1_TE;
}

/* Прием через USART через прерывания */
void UsartRxIT(USART_TypeDef *UartNum, char *buf, int16_t SizeData)
{
    SizeUartRx = SizeData;
    UartUserBufRX = buf;
    UartRxNum = UartNum;
    UartNum->CR1 |= USART_CR1_RXNEIE | USART_CR1_RE;
}

/* Быстрая инициализация USART */
void UsartInit(USART_TypeDef *UartNum, int32_t baude, int32_t freq)
{
    double brr_d;
    brr_d = freq/(16*baude);
    int16_t brr_m, brr_f;
    brr_m = (int16_t)brr_d;
    brr_f = (int16_t)(round((brr_d-(double)brr_m)*16));
    
    UartNum->BRR = (uint32_t)(brr_m << 4 | brr_f);
    UartNum->CR1 = USART_CR1_UE;
    UartNum->CR2 = 0;
    UartNum->CR3 = 0;
}

/* Инициализация USART (передаём структуру) */
void UsartInitStruct(USART_Settings *usart)
{
    double brr_d;
    USART_TypeDef *UartNum = usart->UartPtr;
    int32_t baudeFun = usart->baude;
    int32_t freqFun = usart->freq;
    
    brr_d = freqFun/(16*baudeFun);
    int16_t brr_m, brr_f;
    brr_m = (int16_t)brr_d;
    brr_f = (int16_t)(round((brr_d-(double)brr_m)*16));
    
    UartNum->BRR = (uint32_t)(brr_m << 4 | brr_f);
    UartNum->CR1 = USART_CR1_UE;
    UartNum->CR2 = 0;
    UartNum->CR3 = 0;
}

/* Обработчик прерываний для USART1 */
void USART1_IRQHandler(void)
{
    if (USART1->CR1 & USART_CR1_TE)
    {
        if(Counter <= SizeUartTx)
        {
            USART1->DR = UartUserBufTX[Counter];
            Counter = Counter + 1;
        }
        else
        {
            USART1->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TE);
            Counter = 0;
            USART1_TX_Callback();
        }
    }
    else if (USART1->CR1 & USART_CR1_RE)
    {
        if(ucnt < SizeUartRx)
        {
            UartUserBufRX[ucnt] = USART1->DR;
            ucnt = ucnt + 1;
        }
        else
        {
            USART1->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_RE);
            ucnt = 0;
            USART1_RX_Callback();
        }
    }
}

/* Callback функция после отправки байтов */
__weak void USART1_TX_Callback(void)
{
    
}

/* Callback функция после приема байтов */
__weak void USART1_RX_Callback(void)
{
    
}

/* Обработчик прерываний для USART2 */
void USART2_IRQHandler(void)
{
    if (USART2->CR1 & USART_CR1_TE)
    {
        if(Counter <= SizeUartTx)
        {
            USART2->DR = UartUserBufTX[Counter];
            Counter = Counter + 1;
        }
        else
        {
            USART2->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TE);
            Counter = 0;
            USART2_TX_Callback();
        }
    }
    else if (USART2->CR1 & USART_CR1_RE)
    {
        if(ucnt < SizeUartRx)
        {
            UartUserBufRX[ucnt] = USART2->DR;
            ucnt = ucnt + 1;
        }
        else
        {
            USART2->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_RE);
            ucnt = 0;
            USART2_RX_Callback();
        }
    }
}

/* Callback функция после отправки байтов */
__weak void USART2_TX_Callback(void)
{
    
}

/* Callback функция после приема байтов */
__weak void USART2_RX_Callback(void)
{
    
}