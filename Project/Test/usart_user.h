#include "includes.h"
#include <stdio.h>
//#include "main.h"
#define RX_BUFFER_SIZE 100 
#define TX_BUFFER_SIZE 100 


void USART2_put_string_2(unsigned char *string, uint32_t l);
int InitUSART2();
void USART2_put_char(uint8_t c);
uint8_t toBuf(uint8_t n);
uint8_t fromBuf(uint16_t i);
void clear_RXBuffer(void);
void setRxi(uint16_t i);
uint8_t getRxi(void);