#ifndef __UART1_H__
#define __UART1_H__
#include "NuMicro.h"


#define BAUDRATE 115200
#define UART_TX PG0
#define UART_RX PG1



typedef struct{
	uint8_t data[64];
	uint8_t head;
	uint8_t tail;
}CQ_t;

void UART1_Initial(void);
void UART1_TxTask(void);
void UART1_IRQHandler(void);
uint8_t UART1_IsRxDataReady(void);
uint8_t UART1_IsTxBufFull(void);
void UART1_WriteByte(uint8_t Data);
uint8_t UART1_ReadByte(void);
void ChangeBaudRate (unsigned int BaudRate);

static void CQ_EnQ(CQ_t *ptrBuf, uint8_t Data);
static uint8_t CQ_DeQ(CQ_t *ptrBuf);
static uint8_t CQ_IsFull(CQ_t *ptrBuf);
static uint8_t CQ_IsEmpty(CQ_t *ptrBuf);
static void CQ_Clear(CQ_t *ptrBuf);
uint8_t StrPush(char *str);
unsigned char* CQ_DeQ_addr(CQ_t *ptrBuf);



/**********************************************/
void UART1_TxData(void);


#endif
