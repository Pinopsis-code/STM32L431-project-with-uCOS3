#include "tasks.h"
#include "key.h"
#include "oled.h"
#include "rtc.h"
#include "stdio.h"
#include "usart.h"
#include "dht11.h"
#include "fatfs.h"
#include "micro_str.h"
#include "smg.h"

CPU_STK     ucos3_test_task_stack[UCOS3_TEST_TASK_STACK_SIZE];
OS_TCB      ucos3_test_task_tcb;

#define     UCOS3_TEST_TASKS_PRIO         	5
#define     UCOS3_TEST_TASKS_STACK_SIZE   	256
CPU_STK     ucos3_test_tasks_stack[6][UCOS3_TEST_TASKS_STACK_SIZE];
OS_TCB      ucos3_test_tasks_tcb[6];
void ucos3_test_1_task(void);
void ucos3_test_2_task(void);
void ucos3_test_3_task(void);
void ucos3_test_4_task(void);
void ucos3_test_5_task(void);
void ucos3_test_6_task(void);
OS_TASK_PTR	ucos3_test_tasks_ptr[6] = {
	(OS_TASK_PTR)ucos3_test_1_task,
	(OS_TASK_PTR)ucos3_test_2_task,
	(OS_TASK_PTR)ucos3_test_3_task,
	(OS_TASK_PTR)ucos3_test_4_task,
	(OS_TASK_PTR)ucos3_test_5_task,
	(OS_TASK_PTR)ucos3_test_6_task,
};

void ucos3_tasks_start(u8 task);
void ucos3_tasks_delete(u8 task);
void ucos3_tasks_suspend(u8 task);
void ucos3_tasks_resume(u8 task);

static u8 currentSelection = 0;
u8 tasks_state[6] = {0};

//rtc
static RTC_TimeTypeDef RTC_TimeStruct; 
static RTC_DateTypeDef RTC_DateStruct; 
//UART
static char str[100];
//DHT11
static u16 temp;
static u8 hum;
//SDMMC&FATFS
static UINT bw;

void ucos3_test_ui1(void);
void ucos3_test_ui2(void);
void ucos3_test_keyhandle(u8 key);

void ucos3_test_task(void){
	u8 key = 0, prekey = 0;
	u16 keyhold = 0;
	currentSelection = 0;
	ucos3_test_ui1();
	while(1){
		prekey = key;
		key = key_scan();
		if(key == prekey) keyhold++;
		else keyhold = 0;
		if(key != 0 &&(keyhold == 0 || keyhold > 10))
		ucos3_test_keyhandle(key);
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	}
}

void ucos3_test_ui1(void){
	
	OLED_Clear();
	OLED_ShowString(31,0,"uC-OS3 Test",12,1);
	OLED_ShowString(6,12,"=>RunLight",12,1);
	OLED_ShowString(6,24,"=>RTCclock",12,1);
	OLED_ShowString(6,36,"=>TMP&HAM",12,1);
	OLED_ShowString(6,48,"=>Beep",12,1);
	OLED_DrawSqrt(1,17 + currentSelection % 4 * 12,3,20 + currentSelection % 4 * 12,1);
	for(u8 i = 0; i < 4; i++){
		switch(tasks_state[i])
		{
			case 0:
				OLED_ShowString(72,12+i*12,"Stop",12,1);
			break;
			case 1:
				OLED_ShowString(72,12+i*12,"Running",12,1);
			break;
			case 2:
				OLED_ShowString(72,12+i*12,"Suspend",12,1);
			break;
		}
	}
	OLED_Refresh();
}

void ucos3_test_ui2(void){
	OLED_Clear();
	OLED_ShowString(31,0,"uC-OS3 Test",12,1);
	OLED_ShowString(6,12,"=>DataSave",12,1);
	OLED_ShowString(6,24,"=>SMGClock",12,1);
	OLED_ShowString(6,36,"=>STOP ALL!",12,1);
	OLED_ShowString(6,48,"=>START ALL!",12,1);
	OLED_DrawSqrt(1,17 + currentSelection % 4 * 12,3,20 + currentSelection % 4 * 12,1);
	for(u8 i = 0; i < 2; i++){
		switch(tasks_state[4 + i])
		{
			case 0:
				OLED_ShowString(72,12+i*12,"Stop",12,1);
			break;
			case 1:
				OLED_ShowString(72,12+i*12,"Running",12,1);
			break;
			case 2:
				OLED_ShowString(72,12+i*12,"Suspend",12,1);
			break;
		}
	}
	OLED_Refresh();
}

void ucos3_test_keyhandle(u8 key){
	switch(key)
	{
		case Key2:
			if(currentSelection != 0 && currentSelection != 4){
				OLED_DrawSqrt(1,17 + currentSelection % 4 * 12,3,20 + currentSelection % 4 * 12,0);
				currentSelection--;
				OLED_DrawSqrt(1,17 + currentSelection % 4 * 12,3,20 + currentSelection % 4 * 12,1);
			} else if (currentSelection == 4){
				currentSelection--;
				ucos3_test_ui1();
			}
			OLED_Refresh();
		break;
		case Key11:
			if(currentSelection != 3 && currentSelection != 7){
				OLED_DrawSqrt(1,17 + currentSelection % 4 * 12,3,20 + currentSelection % 4 * 12,0);
				currentSelection++;
				OLED_DrawSqrt(1,17 + currentSelection % 4 * 12,3,20 + currentSelection % 4 * 12,1);
			} else if (currentSelection == 3){
				currentSelection++;
				ucos3_test_ui2();
			}
			OLED_Refresh();
		break;
		case Key4:
		case Key7:
			if(currentSelection < 6){
				if(tasks_state[currentSelection] == 1) {
					tasks_state[currentSelection] = 0;
					ucos3_tasks_delete(currentSelection);
					OLED_ShowString(72,12+currentSelection%4*12,"Stop   ",12,1);
					OLED_Refresh();
				}
				else if(tasks_state[currentSelection] == 2) {
					tasks_state[currentSelection] = 1;
					ucos3_tasks_resume(currentSelection);
					OLED_ShowString(72,12+currentSelection%4*12,"Running",12,1);
					OLED_Refresh();
				}
			} else if(currentSelection == 6){
				for(u8 i = 0; i < 6; i++){
					if(tasks_state[i] == 1) {
						ucos3_tasks_delete(i);
					}
					if(tasks_state[i] == 2) {
						ucos3_tasks_resume(i);
						ucos3_tasks_delete(i);
					}
					tasks_state[i] = 0;
				}
				ucos3_test_ui2();
			} else if(currentSelection == 7){
				for(u8 i = 0; i < 6; i++){
					if(tasks_state[i] == 0) ucos3_tasks_start(i);
					if(tasks_state[i] == 2) ucos3_tasks_resume(i);
					tasks_state[i] = 1;
				}
				ucos3_test_ui2();
			}
		break;
		case Key6:
		case Key9:
			if(currentSelection < 6){
				if(tasks_state[currentSelection] == 0) {
					tasks_state[currentSelection] = 1;
					ucos3_tasks_start(currentSelection);
					OLED_ShowString(72,12+currentSelection%4*12,"Running",12,1);
					OLED_Refresh();
				}
				else if(tasks_state[currentSelection] == 1) {
					tasks_state[currentSelection] = 2;
					ucos3_tasks_suspend(currentSelection);
					OLED_ShowString(72,12+currentSelection%4*12,"Suspend",12,1);
					OLED_Refresh();
				}
			} else if(currentSelection == 6){
				for(u8 i = 0; i < 6; i++){
					if(tasks_state[i] == 1) {
						ucos3_tasks_delete(i);
					}
					if(tasks_state[i] == 2) {
						ucos3_tasks_resume(i);
						ucos3_tasks_delete(i);
					}
					tasks_state[i] = 0;
				}
				ucos3_test_ui2();
			} else if(currentSelection == 7){
				for(u8 i = 0; i < 6; i++){
					if(tasks_state[i] == 0) ucos3_tasks_start(i);
					if(tasks_state[i] == 2) ucos3_tasks_resume(i);
					tasks_state[i] = 1;
				}
				ucos3_test_ui2();
			}
		break;
		case KeyB:
			for(u8 i = 0; i < 6; i++){
				if(tasks_state[i] == 1) {
					ucos3_tasks_delete(i);
				}
				if(tasks_state[i] == 2) {
					ucos3_tasks_resume(i);
					ucos3_tasks_delete(i);
				}
				tasks_state[i] = 0;
			}
			state_back();
	}
}

void ucos3_test_start(void){
	OSTaskCreate (&ucos3_test_task_tcb,
                 "UCOS3 Test",
                 (OS_TASK_PTR)ucos3_test_task,
                 NULL,
                 UCOS3_TEST_TASK_PRIO,
                 ucos3_test_task_stack,
                 UCOS3_TEST_TASK_STACK_SIZE / 10,
                 UCOS3_TEST_TASK_STACK_SIZE,
                 0,
                 0,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);
}
void ucos3_test_1_task(void){
	wLED0(0);
	wLED1(0);
	wLED2(1);
	wLED3(1);
	while(1){
		wLED0(!rLED0);
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
		wLED1(!rLED1);
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
		wLED2(!rLED2);
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
		wLED3(!rLED3);
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	}
}
void ucos3_test_2_task(void){
	while(1){
		HAL_RTC_GetTime(&hrtc,&RTC_TimeStruct,RTC_FORMAT_BIN); 
		HAL_RTC_GetDate(&hrtc,&RTC_DateStruct,RTC_FORMAT_BIN); 
		sprintf(str,"Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
		USART_SendString(str);
		sprintf(str,"Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); 
		USART_SendString(str);
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}
void ucos3_test_3_task(void){
	while(1){
		DHT11_Read(&temp,&hum);
		sprintf(str,"Temprature:%d.%dC\r\nHumidity:%d%%\r\n",temp/10,temp%10,hum);
		USART_SendString(str);
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}
void ucos3_test_4_task(void){
	while(1){
		wBeep(!rBeep);
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}

void ucos3_test_5_task(void){
	while(1){
		retUSER = f_open(&USERFile, "0:/data.txt", FA_WRITE | FA_CREATE_ALWAYS);
		if (retUSER == FR_OK) {
			sprintf(str,"20%d/%02d/%02d %02d:%02d:%02d TMP:%d.%dC HUMI:%d%%",
							RTC_DateStruct.Year,
							RTC_DateStruct.Month,
							RTC_DateStruct.Date,
							RTC_TimeStruct.Hours,
							RTC_TimeStruct.Minutes,
							RTC_TimeStruct.Seconds,
							temp/10,
							temp%10,
							hum
			);
			f_write(&USERFile, str, m_strlen(str), &bw);
			f_close(&USERFile);
			sprintf(str,"Data Stored:%dbyte\r\n", bw);
			USART_SendString(str);
		}
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}
void ucos3_test_6_task(void){
	SMG_Clear();
	smg_task_start();
	while(1){
		SMG_SetNum(0,RTC_TimeStruct.Hours/10);
		SMG_SetNum(1,RTC_TimeStruct.Hours%10);
		SMG_SetNum(3,RTC_TimeStruct.Minutes/10);
		SMG_SetNum(4,RTC_TimeStruct.Minutes%10);
		SMG_SetNum(6,RTC_TimeStruct.Seconds/10);
		SMG_SetNum(7,RTC_TimeStruct.Seconds%10);
		OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
	}
}
void ucos3_tasks_start(u8 task){
	OSTaskCreate (&ucos3_test_tasks_tcb[task],
									 "UCOS3 Tests",
									 ucos3_test_tasks_ptr[task],
									 NULL,
									 UCOS3_TEST_TASKS_PRIO,
									 ucos3_test_tasks_stack[task],
									 UCOS3_TEST_TASKS_STACK_SIZE / 10,
									 UCOS3_TEST_TASKS_STACK_SIZE,
									 0,
									 0,
									 0,
									 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
									 &err);
}

void ucos3_tasks_delete(u8 task){
	switch(task){
		case 0: 
			wLED0(0);
			wLED1(0);
			wLED2(1);
			wLED3(1);
		break;
		case 4:
			wBeep(1);
		break;
		case 5: 
			smg_task_end();
		break;
	}
	OSTaskDel(&ucos3_test_tasks_tcb[task], &err);
}

void ucos3_tasks_suspend(u8 task){
	OSTaskSuspend(&ucos3_test_tasks_tcb[task], &err);
}

void ucos3_tasks_resume(u8 task){
	OSTaskResume(&ucos3_test_tasks_tcb[task], &err);
}
