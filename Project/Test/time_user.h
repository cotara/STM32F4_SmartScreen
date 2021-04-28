#include "stm32f4xx_tim.h"
#define DLY_100US  16800

void timers_init(void);
int RTC_init(void);
void update_Time(RTC_TimeTypeDef *time, unsigned char* RTC_time);
void DelayResolution100us(unsigned int Dly);
void SysTickStart(uint32_t Tick);
void SysTickStop(void);
void delay_1_mcs(volatile uint32_t nTime);
void TimingDelay_1mcs_Decrement(void);
