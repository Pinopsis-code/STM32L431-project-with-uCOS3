#ifndef __SMG_H
#define __SMG_H
#include "main.h"

#define     SMG_TASK_PRIO         	4
#define     SMG_TASK_STACK_SIZE   	256
extern CPU_STK     smg_task_stack[SMG_TASK_STACK_SIZE];
extern OS_TCB      smg_task_tcb;
void smg_task(void);
void smg_task_start(void);
void smg_task_end(void);

extern const uint8_t segCodes[10];
void SMG_SetNum(u8 pos,u8 num);
void SMG_SetDot(u8 pos);
void SMG_Refresh(void);
void SMG_Clear(void);

#endif
