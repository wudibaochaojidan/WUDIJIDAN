#include "tim2.h"

void TIM2_Init(u16 psc, u16 arr)
{
    /*
    1. RCC 时钟使能
    */
    RCC->APB1ENR |= APB1_TIM2_CLOCK_ENABLE;

    /*
    2. 【明确关闭当前 TIM2 定时器】
    */
    TIM2->CR1 &= ~(0x01);

    /*
    3. 将 PSC 和 ARR 数据赋值给 TIM2 定时器，控制当前定时器工作周期
    */
    TIM2->PSC = psc - 1;
    TIM2->ARR = arr - 1;

    /*
    4. 设置当前定时器为【周期性工作模式】
    */
    TIM2->CR1 &= ~(0x01 << 3);

    // 5. 定时器中断使能
    TIM2->DIER |= 0x01;

    /*
    6. 注册 TIM2 定时器中断
    */
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);

    // 7. 开启定时器工作
    TIM2->CR1 |= 0x01;
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & 0x01)
    {
        TIM2->SR &= ~(0x01);

        USART1_SendString("TIM2 Handler!\r\n");
        LED0_Toggle;
    }
}

