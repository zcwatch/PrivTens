.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.global g_pfnVectors
.global Reset_Handler

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDGT_IRQHandler
    .word LVD_IRQHandler
    .word TAMPER_IRQHandler
    .word RTC_IRQHandler
    .word FMC_IRQHandler
    .word RCU_CTC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA0_Channel0_IRQHandler
    .word DMA0_Channel1_IRQHandler
    .word DMA0_Channel2_IRQHandler
    .word DMA0_Channel3_IRQHandler
    .word DMA0_Channel4_IRQHandler
    .word DMA0_Channel5_IRQHandler
    .word DMA0_Channel6_IRQHandler
    .word ADC0_1_IRQHandler
    .word USBD_HP_CAN0_TX_IRQHandler
    .word USBD_LP_CAN0_RX0_IRQHandler
    .word CAN0_RX1_IRQHandler
    .word CAN0_EWMC_IRQHandler
    .word EXTI5_9_IRQHandler
    .word TIMER0_BRK_IRQHandler
    .word TIMER0_UP_IRQHandler
    .word TIMER0_TRG_CMT_IRQHandler
    .word TIMER0_Channel_IRQHandler
    .word TIMER1_IRQHandler
    .word TIMER2_IRQHandler
    .word TIMER3_IRQHandler
    .word I2C0_EV_IRQHandler
    .word I2C0_ER_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word SPI0_IRQHandler
    .word SPI1_IRQHandler
    .word USART0_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word EXTI10_15_IRQHandler
    .word RTC_Alarm_IRQHandler
    .word USBD_WKUP_IRQHandler
    .word TIMER7_BRK_IRQHandler
    .word TIMER7_UP_IRQHandler
    .word TIMER7_TRG_CMT_IRQHandler
    .word TIMER7_Channel_IRQHandler
    .word ADC2_IRQHandler
    .word EXMC_IRQHandler
    .word SDIO_IRQHandler
    .word TIMER4_IRQHandler
    .word TIMER5_IRQHandler
    .word TIMER6_IRQHandler
    .word DMA1_Channel0_IRQHandler
    .word DMA1_Channel1_IRQHandler
    .word DMA1_Channel2_IRQHandler
    .word DMA1_Channel3_4_IRQHandler

.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    ldr sp, =_estack
    movs r1, #0
    b CopyData

CopyLoop:
    ldr r3, =_sidata
    ldr r3, [r3, r1]
    ldr r2, =_sdata
    ldr r2, [r2, r1]
    str r2, [r0], #4
    adds r1, r1, #4

CopyData:
    ldr r0, =_sdata
    ldr r2, =_edata
    adds r3, r0, r1
    cmp r3, r2
    bcc CopyLoop
    ldr r2, =_sbss
    b FillZerLoop

FillZer:
    movs r3, #0
    str r3, [r0], #4

FillZerLoop:
    ldr r3, =_ebss
    cmp r2, r3
    bcc FillZer
    bl SystemInit
    bl main

Infinite:
    b Infinite

.align 2
.weak NMI_Handler
NMI_Handler:
    b .

.weak HardFault_Handler
HardFault_Handler:
    b .

.weak MemManage_Handler
MemManage_Handler:
    b .

.weak BusFault_Handler
BusFault_Handler:
    b .

.weak UsageFault_Handler
UsageFault_Handler:
    b .

.weak SVC_Handler
SVC_Handler:
    b .

.weak DebugMon_Handler
DebugMon_Handler:
    b .

.weak PendSV_Handler
PendSV_Handler:
    b .

.weak SysTick_Handler
SysTick_Handler:
    b .

.weak WWDGT_IRQHandler
WWDGT_IRQHandler:
    b .

.weak LVD_IRQHandler
LVD_IRQHandler:
    b .

.weak TAMPER_IRQHandler
TAMPER_IRQHandler:
    b .

.weak RTC_IRQHandler
RTC_IRQHandler:
    b .

.weak FMC_IRQHandler
FMC_IRQHandler:
    b .

.weak RCU_CTC_IRQHandler
RCU_CTC_IRQHandler:
    b .

.weak EXTI0_IRQHandler
EXTI0_IRQHandler:
    b .

.weak EXTI1_IRQHandler
EXTI1_IRQHandler:
    b .

.weak EXTI2_IRQHandler
EXTI2_IRQHandler:
    b .

.weak EXTI3_IRQHandler
EXTI3_IRQHandler:
    b .

.weak EXTI4_IRQHandler
EXTI4_IRQHandler:
    b .

.weak DMA0_Channel0_IRQHandler
DMA0_Channel0_IRQHandler:
    b .

.weak DMA0_Channel1_IRQHandler
DMA0_Channel1_IRQHandler:
    b .

.weak DMA0_Channel2_IRQHandler
DMA0_Channel2_IRQHandler:
    b .

.weak DMA0_Channel3_IRQHandler
DMA0_Channel3_IRQHandler:
    b .

.weak DMA0_Channel4_IRQHandler
DMA0_Channel4_IRQHandler:
    b .

.weak DMA0_Channel5_IRQHandler
DMA0_Channel5_IRQHandler:
    b .

.weak DMA0_Channel6_IRQHandler
DMA0_Channel6_IRQHandler:
    b .

.weak ADC0_1_IRQHandler
ADC0_1_IRQHandler:
    b .

.weak USBD_HP_CAN0_TX_IRQHandler
USBD_HP_CAN0_TX_IRQHandler:
    b .

.weak USBD_LP_CAN0_RX0_IRQHandler
USBD_LP_CAN0_RX0_IRQHandler:
    b .

.weak CAN0_RX1_IRQHandler
CAN0_RX1_IRQHandler:
    b .

.weak CAN0_EWMC_IRQHandler
CAN0_EWMC_IRQHandler:
    b .

.weak EXTI5_9_IRQHandler
EXTI5_9_IRQHandler:
    b .

.weak TIMER0_BRK_IRQHandler
TIMER0_BRK_IRQHandler:
    b .

.weak TIMER0_UP_IRQHandler
TIMER0_UP_IRQHandler:
    b .

.weak TIMER0_TRG_CMT_IRQHandler
TIMER0_TRG_CMT_IRQHandler:
    b .

.weak TIMER0_Channel_IRQHandler
TIMER0_Channel_IRQHandler:
    b .

.weak TIMER1_IRQHandler
TIMER1_IRQHandler:
    b .

.weak TIMER2_IRQHandler
TIMER2_IRQHandler:
    b .

.weak TIMER3_IRQHandler
TIMER3_IRQHandler:
    b .

.weak I2C0_EV_IRQHandler
I2C0_EV_IRQHandler:
    b .

.weak I2C0_ER_IRQHandler
I2C0_ER_IRQHandler:
    b .

.weak I2C1_EV_IRQHandler
I2C1_EV_IRQHandler:
    b .

.weak I2C1_ER_IRQHandler
I2C1_ER_IRQHandler:
    b .

.weak SPI0_IRQHandler
SPI0_IRQHandler:
    b .

.weak SPI1_IRQHandler
SPI1_IRQHandler:
    b .

.weak USART0_IRQHandler
USART0_IRQHandler:
    b .

.weak USART1_IRQHandler
USART1_IRQHandler:
    b .

.weak USART2_IRQHandler
USART2_IRQHandler:
    b .

.weak EXTI10_15_IRQHandler
EXTI10_15_IRQHandler:
    b .

.weak RTC_Alarm_IRQHandler
RTC_Alarm_IRQHandler:
    b .

.weak USBD_WKUP_IRQHandler
USBD_WKUP_IRQHandler:
    b .

.weak TIMER7_BRK_IRQHandler
TIMER7_BRK_IRQHandler:
    b .

.weak TIMER7_UP_IRQHandler
TIMER7_UP_IRQHandler:
    b .

.weak TIMER7_TRG_CMT_IRQHandler
TIMER7_TRG_CMT_IRQHandler:
    b .

.weak TIMER7_Channel_IRQHandler
TIMER7_Channel_IRQHandler:
    b .

.weak ADC2_IRQHandler
ADC2_IRQHandler:
    b .

.weak EXMC_IRQHandler
EXMC_IRQHandler:
    b .

.weak SDIO_IRQHandler
SDIO_IRQHandler:
    b .

.weak TIMER4_IRQHandler
TIMER4_IRQHandler:
    b .

.weak TIMER5_IRQHandler
TIMER5_IRQHandler:
    b .

.weak TIMER6_IRQHandler
TIMER6_IRQHandler:
    b .

.weak DMA1_Channel0_IRQHandler
DMA1_Channel0_IRQHandler:
    b .

.weak DMA1_Channel1_IRQHandler
DMA1_Channel1_IRQHandler:
    b .

.weak DMA1_Channel2_IRQHandler
DMA1_Channel2_IRQHandler:
    b .

.weak DMA1_Channel3_4_IRQHandler
DMA1_Channel3_4_IRQHandler:
    b .

.end