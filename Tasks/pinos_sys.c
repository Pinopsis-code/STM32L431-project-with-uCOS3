#include "tasks.h"
#include "oled.h"
#include "key.h"
#include "24cxx.h"

CPU_STK     pinos_sys_task_stack[PINOS_SYS_TASK_STACK_SIZE];
OS_TCB      pinos_sys_task_tcb;

//ADC
u32 adc_value[3];
u16 dac_value[2];

u8 state = 0;
static u8 temp;

void state_back(void);
void pinos_sys_ui(void);
void pinos_sys_keyhandle(u8 key);

void state_back(void){
	OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	state = 0;
	pinos_sys_ui();
	OSTaskDel(NULL, &err);
}

void pinos_sys_task(void){
	u8 key;
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB)){
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); 
		AT24CXX_Read(0,&temp,1);
		OLED_ShowString(32,26," Wake Up!",12,1);
		OLED_Refresh();
		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
		if(temp == 1){
			state = 1;
			hardware_test_start();
		} else if(temp == 2){
			state = 4;
			smart_access_sys_start();
		}
		temp = 0;
		AT24CXX_Write(0,&temp,1);
	}
	else pinos_sys_ui();
	while(1)     //永远不停歇（除非被其它任务删除）
	{
		if(state == 0){
			key = key_scan();
			if(key != 0)
			pinos_sys_keyhandle(key);
		}
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);      //延时100ms
	}
}

void pinos_sys_ui(void){
	OLED_Clear();
	OLED_ShowString(1,0,"Welcome to Pino's Sys",12,1);
	OLED_ShowString(19,12,"X.Hardware Test",12,1);
	OLED_ShowString(25,24,"Y.uC-OS3 Test",12,1);
	OLED_ShowString(13,36,"A.2CH-SigGenScope",12,1);
	OLED_ShowString(10,48,"B.Smart Access Sys",12,1);
	OLED_Refresh();
}

void pinos_sys_keyhandle(u8 key){
	switch(key){
		case KeyX:
			state = 1;
			OSTimeDly(100, OS_OPT_TIME_DLY, &err);
			hardware_test_start();
		break;
		case KeyY:
			state = 2;
			OSTimeDly(100, OS_OPT_TIME_DLY, &err);
			ucos3_test_start();
		break;
		case KeyA:
			state = 3;
			OSTimeDly(100, OS_OPT_TIME_DLY, &err);
			siggenscope_start();
		break;
		case KeyB:
			state = 4;
			OSTimeDly(100, OS_OPT_TIME_DLY, &err);
			temp = 2;
			AT24CXX_Write(0,&temp,1);
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
			OLED_Clear();
			OLED_ShowString(40,26,"Sleep...",12,1);
			OLED_Refresh();
			HAL_PWR_EnterSTANDBYMode();
		break;
	}
}
