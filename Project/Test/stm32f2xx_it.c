/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "time_user.h"
#include "usart_user.h"
#include "LED_user.h"
#include "myNextion.h"

extern uint32_t delay_decrement_1mcs, strob_decrement;
extern  uint8_t rx_buffer[RX_BUFFER_SIZE];
extern  uint16_t rx_wr_index, rx_rd_index, rx_counter;
extern  uint8_t tx_buffer[TX_BUFFER_SIZE];
extern  uint16_t tx_wr_index, tx_rd_index, tx_counter;
extern uint16_t temper;
uint32_t mcs=0;
uint32_t counter=0;
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

void SysTick_Handler(void) {
    /* Decrement the timeout value */
    TimingDelay_1ms_Decrement();
  
 
}

void USART2_IRQHandler(void) {  
    GPIO_SetBits(GPIOE, GPIO_Pin_0);
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистка признака прерывания
        if ((USART2->SR & (USART_FLAG_FE | USART_FLAG_PE)) == 0) { //нет ошибок
            USART_IRQProcessFunc(USART_ReceiveData(USART2) & 0xFF);
        }                                 //Сообщение из USART пришло без ошибок
        else
           USART_ReceiveData(USART2);
    }
        
    if (USART_GetITStatus(USART2, USART_IT_ORE_RX) == SET) { //прерывание по переполнению буфера
        USART_ReceiveData(USART2); //в идеале пишем здесь обработчик переполнения буфера, но мы просто сбрасываем этот флаг прерывания чтением из регистра данных.
    }
    
    
    if (USART_GetITStatus(USART2, USART_IT_TC) != RESET) { //прерывание по передаче
        USART_ClearITPendingBit(USART2, USART_IT_TC); //очищаем признак прерывания
        if (tx_counter) {                               //если есть что передать
            --tx_counter;           // уменьшаем количество не переданных данных
            USART_SendData(USART2, tx_buffer[tx_rd_index++]); //передаем данные инкрементируя хвост буфера
            if (tx_rd_index == TX_BUFFER_SIZE)
                tx_rd_index = 0;                         //идем по кругу
        }
        else
            USART_ITConfig(USART2, USART_IT_TC, DISABLE); //если нечего передать, запрещаем прерывание по передаче
    }

}
//Таймер для сброса непринятой посылки
void TIM2_IRQHandler(void) {
    //LEDToggle();
    TIM_ClearFlag(TIM2, TIM_IT_Update);
    setRxi(0);                      //1 мс не приходило нового байта. Значит был затык
    TIM_Cmd(TIM2, DISABLE);         //Перестали считать. Ждем следующей посылки.
    TIM2->CNT = 0;
}
//Таймер для задержек
void TIM5_IRQHandler(void) {
  // Clear TIM5 counter
   TIM_ClearFlag(TIM5, TIM_IT_Update);  
   TimingDelay_1mcs_Decrement();
   //mcs++;
   //LEDToggle();
   //if(mcs == 1000000){
   //  LEDToggle();
   //  mcs=0;
   //}
     
}
// каждые 2 секунды генерируется новый буфер
void TIM6_DAC_IRQHandler() {
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        //LEDToggle();
      
    }
}
// таймер для генераци строба
void TIM7_IRQHandler(void) {
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {        
               
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
