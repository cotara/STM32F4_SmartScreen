#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "time_user.h"
#include "usart_user.h"
#include "LED_user.h"
#include "myNextion.h"
#include "user_ds18b20.h"

uint8_t dataBuffer[DATA_BUFFER_SIZE];

__IO uint32_t TimeOut = 0x00;

RCC_ClocksTypeDef RCC_Clocks;
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_wr_index, rx_rd_index, rx_counter;
extern volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
extern volatile uint16_t tx_wr_index, tx_rd_index, tx_counter;

uint8_t isEmpty=1;
uint16_t temper=0;
/********************************************************
* MAIN
********************************************************/

void main(void) {
    uint8_t status;
    timers_init();
    //SystemInit();
    LEDInit();
    InitUSART2();
    status=ds18b20_init(DS18B20_Resolution_9_bit);
    if (status){
      while(1);
    }
    TIM_Cmd(TIM6, ENABLE);
    
    while (1){      
       //LEDToggle();
      
       if(getFLAG_END_LINE() >= 3)
          nextionEvent();           
       
       temper = ds18b20_get_temperature();
       
       if (getNowPage()==0){
       Nextion_SetValue_Number("x0.val=",temper);
         if(temper>=-300)
           Nextion_SetValue_Number("temperature.val=",temper*3/10+90);
         else
           Nextion_SetValue_Number("temperature.val=",temper*3/10+450);
       }
       
       else if(getNowPage()== 1){
         Nextion_SetValue_Number("add 2,0,",temper/2);
       }
    }       
}


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    while (1){  
    }
}
#endif
