#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "time_user.h"
#include "usart_user.h"
#include "LED_user.h"
#include "myNextion.h"
#include "user_ds18b20.h"
#include "i2c_ee.h"
#include "tm_stm32f4_i2c.h"


uint8_t dataBuffer[DATA_BUFFER_SIZE];

__IO uint32_t TimeOut = 0x00;

RCC_ClocksTypeDef RCC_Clocks;
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_wr_index, rx_rd_index, rx_counter;
extern volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
extern volatile uint16_t tx_wr_index, tx_rd_index, tx_counter;

uint8_t isEmpty=1;
uint16_t temper=0;
uint16_t tempBuf[512];
uint32_t average=0;
/********************************************************
* MAIN
********************************************************/

void main(void) {
    uint8_t status;
    timers_init();
    SysTick_Config(SystemCoreClock/1000);
    LEDInit();
    InitUSART2();
    I2C_EE_Init();
    status=ds18b20_init(DS18B20_Resolution_12_bit);
    if (status){
      while(1);
    }
    TIM_Cmd(TIM6, ENABLE);
    
    while (1){      
       //LEDToggle();
      
       if(getFLAG_END_LINE() >= 3)
          nextionEvent();           
       
       temper = ds18b20_get_temperature();
       /*
       TM_I2C_WriteMulti(I2C1,0xA0,0x01,tempBuf0,7);
       delay_1_ms(5);
       TM_I2C_ReadMulti(I2C1,0xA0,0x01,tempBuf1,7);
       */

       setToEE(temper);

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
       else if(getNowPage()== 2 && getReqBigBuf()){
         resetReqBigBuf();
         getFromEE(tempBuf,512);
         for(int i=0;i<512;i++){
          Nextion_SetValue_Number("add 1,0,",tempBuf[i]*0.667);
          average+=tempBuf[i];
         }
          Nextion_SetValue_Number("n0.val=",average/5120);
       }
       
       delay_1_ms(29000);
    }       
}



#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
    while (1){  
    }
}
#endif
