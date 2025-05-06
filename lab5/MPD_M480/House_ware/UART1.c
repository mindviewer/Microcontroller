#include "UART1.h"
#include "M480.h"

static CQ_t TxBuf;
static CQ_t RxBuf;

void UART1_Initial(void) {

  /* Select HCLK clock source as HIRC and HCLK clock divider as 1 */
  CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

  /* Enable UART module clock */ 
  CLK_EnableModuleClock(UART1_MODULE);

  /* Select UART module clock source as HIRC and UART module clock divider as 1 */
  CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HIRC, CLK_CLKDIV0_UART1(1));
	
	/* Set PB multi-function pins for UART1 RXD=PG.1 and TXD=PG.0 */
  SYS->GPG_MFPL = ((SYS->GPG_MFPL & (~(SYS_GPG_MFPL_PG0MFP_Msk | SYS_GPG_MFPL_PG1MFP_Msk))) 
                                  | (SYS_GPG_MFPL_PG1MFP_UART1_RXD | SYS_GPG_MFPL_PG0MFP_UART1_TXD));
																		
	/* Reset UART1 */
	SYS_ResetModule(UART1_RST);
	
	/* Configure UART1 and set UART1 baud rate */
  UART_Open(UART1, BAUDRATE);

	/* Enable UART RDA and TX EMPTY  interrupt */
	NVIC_EnableIRQ(UART1_IRQn);
	
//	UART_EnableInt(UART1, UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk);
		UART_EnableInt(UART1, UART_INTEN_RDAIEN_Msk);

	CQ_Clear(&TxBuf);
	CQ_Clear(&RxBuf);
	
}


void UART1_TxTask(void){
	/* If transmitter holding register(THR) is empty and Tx buffer is not empty */
  if((UART_GET_INT_FLAG(UART1,UART_INTSTS_THREIF_Msk)) && !CQ_IsEmpty(&TxBuf)){
	unsigned char TxBufTemp;
	TxBufTemp = CQ_DeQ(&TxBuf);
	UART_Write(	UART1, &TxBufTemp ,1);
	}
}
	
void UART1_IRQHandler(void){	
	unsigned char c;
	if(UART_GET_INT_FLAG(UART1,UART_INTSTS_RDAIF_Msk)) {
		UART_Read	(UART1,&c,1);
		if(!CQ_IsFull(&RxBuf)){
			CQ_EnQ(&RxBuf, c);
		}
	}
}


uint8_t UART1_IsRxDataReady(void){
	return !CQ_IsEmpty(&RxBuf);
}

uint8_t UART1_IsTxBufFull(void){
	return CQ_IsFull(&TxBuf);
}
/* No full check before enQ, should do that before call this */
void UART1_WriteByte(uint8_t Data){
	CQ_EnQ(&TxBuf, Data);
}

/* No empty check before deQ, should do that before call this */
uint8_t UART1_ReadByte(void){
	return CQ_DeQ(&RxBuf);
}
static void CQ_EnQ(CQ_t *ptrBuf, uint8_t Data){
	ptrBuf->data[ptrBuf->head++] = Data;
	ptrBuf->head &= 0x3F;
}

static uint8_t CQ_DeQ(CQ_t *ptrBuf){
	uint8_t ret;
	ret = ptrBuf->data[ptrBuf->tail++];
	ptrBuf->tail &= 0x3F;
	return ret;
}

static uint8_t CQ_IsFull(CQ_t *ptrBuf){
	return (((ptrBuf->head + 1) & 0x3F) == ptrBuf->tail);
}
static uint8_t CQ_IsEmpty(CQ_t *ptrBuf){
	return (ptrBuf->head == ptrBuf->tail);
}

static void CQ_Clear(CQ_t *ptrBuf){
	ptrBuf->head = 0;
	ptrBuf->tail = 0;
}

//////////////////////////
///change uart baudrate///
//////////////////////////
void ChangeBaudRate (unsigned int BaudRate) {
	UART_Open(UART1, BaudRate);
}

uint8_t StrPush(char *str){
	uint8_t i = 0;
	while(str[i] && !UART1_IsTxBufFull()){
		UART1_WriteByte(str[i++]);
	}
	return i;
}
