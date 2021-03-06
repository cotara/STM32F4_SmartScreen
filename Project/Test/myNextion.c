#include "myNextion.h"
#include "usart_user.h"
#include "time_user.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>
#include <string.h>

uint8_t RX_FLAG_END_LINE = 0;
uint8_t status[21]="t3.txt=";
uint8_t command[11]="sendme";
uint8_t setNumTemper[11]="x0.val";
uint32_t st=0;
uint8_t endMes[3]={0xFF,0xFF,0xFF};
static uint8_t StrBuff[64]; 
uint8_t nowPage=0;
uint8_t reqBigBuf=0;
void USART_IRQProcessFunc(uint8_t RXc){
    toBuf(RXc);
    TIM_Cmd(TIM2, DISABLE);
    TIM2->CNT = 0;
    if (RXc == 0xFF)                                                            //Если пришел признак окончания команды (их должно быть 3)
       RX_FLAG_END_LINE++;
    if(RX_FLAG_END_LINE!=3)                                                     //Если команда пришла еще не полностью, запускаем таймер сброса
        TIM_Cmd(TIM2, ENABLE);   
}

void nextionEvent(void){
      //fromBuf(0) - номер страницы
      //fromBuf(1) - номер кнопки (элемента). 0 - переход на страницу
      //fromBuf(3) - значение 
      
      RX_FLAG_END_LINE=0;
      
      if(fromBuf(0) == 0x00 && fromBuf(1) == 0x08){                             //СТАРТ Массаж 1
        if (fromBuf(3) == 1){
          GPIOC->BSRRH=GPIO_Pin_13;
        }
        else
          GPIOC->BSRRL=GPIO_Pin_13;  
      }
      else if(fromBuf(1) == 0x00){
        nowPage = fromBuf(0);
        if (nowPage == 2) reqBigBuf=1;
      }
     clear_RXBuffer();
}

void Nextion_SetValue_Number(char *ValueName, uint32_t Value)
{
  sprintf((char *)StrBuff, "%s%d", ValueName, Value);
  uint16_t Len = strlen((char *)StrBuff);
  
  USART2_put_string_2(StrBuff, Len);
  USART2_put_string_2((uint8_t *)endMes, 3);
}
void Nextion_SetValue_String(char *ValueName, char *Value)
{
  sprintf((char *)StrBuff, "%s=\"%s\"", ValueName, Value);
  uint16_t Len = strlen((char *)StrBuff);
  
  USART2_put_string_2(StrBuff, Len);
  USART2_put_string_2((uint8_t *)endMes, 3);
}

void incFLAG_END_LINE(void){
  RX_FLAG_END_LINE++;
}
void resetFLAG_END_LINE(void){
  RX_FLAG_END_LINE=0;
}
uint8_t getFLAG_END_LINE(void){
  return RX_FLAG_END_LINE;
}

uint8_t getReqBigBuf(void){
  return reqBigBuf;
}
void resetReqBigBuf(void){
   reqBigBuf=0;
}

uint8_t getNowPage(void){
  return nowPage;
}
