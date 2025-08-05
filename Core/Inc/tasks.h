#ifndef __TASKS_H
#define __TASKS_H

#include "main.h"
#define     PINOS_SYS_TASK_PRIO         	3
#define     PINOS_SYS_TASK_STACK_SIZE   	512
extern CPU_STK     pinos_sys_task_stack[PINOS_SYS_TASK_STACK_SIZE];
extern OS_TCB      pinos_sys_task_tcb;
void pinos_sys_task(void);

#define     HARDWARE_TEST_TASK_PRIO         	4
#define     HARDWARE_TEST_TASK_STACK_SIZE   	512
extern CPU_STK     hardware_test_task_stack[HARDWARE_TEST_TASK_STACK_SIZE];
extern OS_TCB      hardware_test_task_tcb;
void hardware_test_task(void);
void hardware_test_start(void);

#define     UCOS3_TEST_TASK_PRIO         	4
#define     UCOS3_TEST_TASK_STACK_SIZE   	512
extern CPU_STK     ucos3_test_task_stack[UCOS3_TEST_TASK_STACK_SIZE];
extern OS_TCB      ucos3_test_task_tcb;
void ucos3_test_task(void);
void ucos3_test_start(void);

#define     SIGGNSCOPE_TASK_PRIO         	4
#define     SIGGNSCOPE_TASK_STACK_SIZE   	512
extern CPU_STK     siggenscope_task_stack[SIGGNSCOPE_TASK_STACK_SIZE];
extern OS_TCB      siggenscope_task_tcb;
void siggenscope_task(void);
void siggenscope_start(void);

#define     SMART_ACCESS_SYS_TASK_PRIO         	4
#define     SMART_ACCESS_SYS_TASK_STACK_SIZE   	512
extern CPU_STK     smart_access_sys_task_stack[SMART_ACCESS_SYS_TASK_STACK_SIZE];
extern OS_TCB      smart_access_sys_task_tcb;
void smart_access_sys_task(void);
void smart_access_sys_start(void);

extern u32 adc_value[3];
extern u16 dac_value[2];

void state_back(void);

#endif
