#include "tasks.h"
#include "key.h"
#include "oled.h"
#include "dac.h"
#include "adc.h"
#include "smg.h"

CPU_STK     siggenscope_task_stack[SIGGNSCOPE_TASK_STACK_SIZE];
OS_TCB      siggenscope_task_tcb;

#define     ADC_TASK_PRIO         	5
#define     ADC_TASK_STACK_SIZE   	256
CPU_STK     adc_task_stack[ADC_TASK_STACK_SIZE];
OS_TCB      adc_task_tcb;
void adc_task(void);

#define     DAC_TASK_PRIO         	5
#define     DAC_TASK_STACK_SIZE   	256
CPU_STK     dac_task_stack[DAC_TASK_STACK_SIZE];
OS_TCB      dac_task_tcb;
void dac_task(void);

static u8 currentSelection = 0;
static u8 adc_state = 0;
static u8 dac_state = 0;

#define DAC_CH1 0
#define DAC_CH2 1
static u8 dac_channel = DAC_CH1;
#define NONE 0
#define SIN 1
#define TRI 2
#define SQRT 3
static u8 dac_type[2];
static u8 dac_step[2] = {1,1};
static u16 dac_vpp[2] = {4000,4000};
static u8 dac_time[2] = {0,0};
static u8 dac_screen_pos;
static u8 dac_data[2][108];

#define ADC_CH1 0
#define ADC_CH2 1
#define ADC_CH1_2 2
static u8 adc_channel = ADC_CH1;
static u8 adc_span = 4;
static i8 adc_bias = 0;
static u8 adc_step = 10;
static u8 adc_screen_pos;
static u8 adc_data[2][108];

u8 sin_code[120]={0x87,0x8D,0x94,0x9B,0xA1,0xA8,0xAE,0xB4,0xBA,0xC0,
0xC6,0xCB,0xD1,0xD5,0xDB,0xDF,0xE3,0xE7,0xE8,0xEF,0xF2,0xF5,0xF7,0xFA,0xFC,
0xFD,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFD,0xFC,0xFA,0xF7,0xF5,0xF2,0xEF,
0xE8,0xE7,0xE3,0xDF,0xDB,0xD5,0xD1,0xCB,0xC6,0xC0,0xBA,0xB4,0xAE,0xA8,0xA1,
0x9B,0x94,0X8D,0x87,0x80,0x79,0x73,0x6C,0x65,0x5F,0x58,0x52,0x4C,0x46,0x40,
0x3A,0x35,0x2F,0x2A,0x25,0x21,0x1D,0x18,0x15,0x11,0x0E,0x0B,0x08,0x06,0x04,
0x03,0x02,0x01,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x06,0x09,0x0B,0x0E,0x11,
0x15,0x18,0x1D,0x21,0x25,0x2A,0x2F,0x35,0x3A,0x40,0x46,0x4C,0x52,0x58,0x5F,
0x65,0x6C,0x73,0x79,0x80};

void siggenscope_ui(void);
void dac_ui(void);
void adc_ui(void);

void dac_data_update(u8 ch);
void dac_type_ui(u8 ch);

void siggenscope_keyhandler(u8 key);
void dac_keyhandler(u8 key);
void adc_keyhandler(u8 key);

void siggenscope_end(void);

void siggenscope_task(void){
	u8 key = 0, prekey = 0;
	u16 keyhold = 0;
	currentSelection = 0;
	siggenscope_ui();
	smg_task_start();
	while(1){
		prekey = key;
		key = key_scan();
		if(key == prekey) keyhold++;
		else keyhold = 0;
		if(key != 0 &&(keyhold == 0 || keyhold > 10))
		{
			switch(currentSelection){
				case 0:siggenscope_keyhandler(key);break;
				case 1:dac_keyhandler(key);break;
				case 2:adc_keyhandler(key);break;
			}
		}
		if(currentSelection == 1 && dac_type[dac_channel] != NONE){
			SMG_SetNum(0,1200 / dac_step[dac_channel] / 1000);
			SMG_SetNum(1,1200 / dac_step[dac_channel] / 100 % 10);
			SMG_SetNum(2,1200 / dac_step[dac_channel] / 10 % 10);
			SMG_SetNum(3,1200 / dac_step[dac_channel] % 10);
			SMG_SetNum(5,dac_vpp[dac_channel] * 33 / 40960);
			SMG_SetDot(5);
			SMG_SetNum(6,dac_vpp[dac_channel] * 33 / 4096 % 10);
		}
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);      //—” ±100ms
	}
}

void siggenscope_ui(void){
	OLED_Clear();
	OLED_ShowString(46,0,"Signal",12,1);
	OLED_ShowString(13,12,"Generator & Scope",12,1);
	OLED_ShowString(10,36," Gen  task:",12,1);
	OLED_ShowString(10,48,"Scope task:",12,1);
	switch(dac_state){
		case 0:
			OLED_ShowString(76,36,"   Stop",12,1);
			break;
		case 1:
			OLED_ShowString(76,36,"Running",12,1);
			break;
		case 2:
			OLED_ShowString(76,36,"Suspend",12,1);
			break;
	}
	switch(adc_state){
		case 0:
			OLED_ShowString(76,48,"   Stop",12,1);
			break;
		case 1:
			OLED_ShowString(76,48,"Running",12,1);
			break;
		case 2:
			OLED_ShowString(76,48,"Suspend",12,1);
			break;
	}
	OLED_Refresh();
}

void dac_ui(void){
	OLED_Clear();
	OLED_DrawLine(10,6,119,6,1);
	OLED_DrawLine(10,7,10,30,1);
	OLED_DrawLine(118,7,118,30,1);
	OLED_DrawLine(10,30,119,30,1);
	OLED_ShowString(10,36,"CH:CH     Type:",12,1);
	
	dac_type_ui(dac_channel);
	
	OLED_ShowNum(40,36,dac_channel+1,1,12,1);
	
	OLED_ShowString(10,48,"T:    ms  U: . V",12,1);
	
	OLED_ShowNum(22,48,1200/dac_step[dac_channel],4,12,1);
	OLED_ShowNum(82,48,dac_vpp[dac_channel]*33/40960,1,12,1);
	OLED_ShowNum(94,48,dac_vpp[dac_channel]*33/4096%10,1,12,1);
	
	OLED_Refresh();
}

void adc_ui(void){
	OLED_Clear();
	OLED_DrawLine(10,6,119,6,1);
	OLED_DrawLine(10,7,10,30,1);
	OLED_DrawLine(118,7,118,30,1);
	OLED_DrawLine(10,30,119,30,1);
	OLED_ShowString(10,36,"CH:CH     Span: V",12,1);
	switch(adc_channel){
		case ADC_CH1:
			OLED_ShowString(40,36,"1  ",12,1);
		break;
		case ADC_CH2:
			OLED_ShowString(40,36,"2  ",12,1);
		break;
		case ADC_CH1_2:
			OLED_ShowString(40,36,"1/2",12,1);
		break;
	}
	OLED_ShowNum(100,36,adc_span,1,12,1);
	OLED_ShowString(10,48,"D:  . V   T:   ms",12,1);
	if(adc_bias < 0)
	{
		OLED_ShowString(22,48,"-",12,1);
		OLED_ShowNum(28,48,(-adc_bias)/10,1,12,1);
		OLED_ShowNum(40,48,(-adc_bias)%10,1,12,1);
	} else
	{
		OLED_ShowString(19,48," ",12,1);
		OLED_ShowNum(28,48,adc_bias/10,1,12,1);
		OLED_ShowNum(40,48,adc_bias%10,1,12,1);
	}
	
	OLED_ShowNum(82,48,adc_step,3,12,1);
	OLED_Refresh();
}

void siggenscope_keyhandler(u8 key){
	switch(key){
		case Key6:
			if(dac_state == 0){
				dac_state = 1;
				OSTaskCreate (&dac_task_tcb,
									 "Generator",
									 (OS_TASK_PTR)dac_task,
									 NULL,
									 DAC_TASK_PRIO,
									 dac_task_stack,
									 DAC_TASK_STACK_SIZE / 10,
									 DAC_TASK_STACK_SIZE,
									 0,
									 0,
									 0,
									 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
									 &err);
				OLED_ShowString(76,36,"Running",12,1);
				OLED_Refresh();
			}else if (dac_state == 1){
				dac_state = 2;
				OSTaskSuspend(&dac_task_tcb,&err);
				OLED_ShowString(76,36,"Suspend",12,1);
				OLED_Refresh();
			}
		break;
		case Key4:
			if(dac_state == 1){
				dac_state = 0;
				OSTaskDel(&dac_task_tcb,&err);
				OLED_ShowString(76,36,"   Stop",12,1);
				OLED_Refresh();
			}else if (dac_state == 2){
				dac_state = 1;
				OSTaskResume(&dac_task_tcb,&err);
				OLED_ShowString(76,36,"Running",12,1);
				OLED_Refresh();
			}
		break;
		case Key9:
			if(adc_state == 0){
				adc_state = 1;
				OSTaskCreate (&adc_task_tcb,
									 "Scope",
									 (OS_TASK_PTR)adc_task,
									 NULL,
									 ADC_TASK_PRIO,
									 adc_task_stack,
									 ADC_TASK_STACK_SIZE / 10,
									 ADC_TASK_STACK_SIZE,
									 0,
									 0,
									 0,
									 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
									 &err);
				OLED_ShowString(76,48,"Running",12,1);
				OLED_Refresh();
			}else if (adc_state == 1){
				adc_state = 2;
				OSTaskSuspend(&adc_task_tcb,&err);
				OLED_ShowString(76,48,"Suspend",12,1);
				OLED_Refresh();
			}
		break;
		case Key7:
			if(adc_state == 1){
				adc_state = 0;
				OSTaskDel(&adc_task_tcb,&err);
				OLED_ShowString(76,48,"   Stop",12,1);
				OLED_Refresh();
			}else if (adc_state == 2){
				adc_state = 1;
				OSTaskResume(&adc_task_tcb,&err);
				OLED_ShowString(76,48,"Running",12,1);
				OLED_Refresh();
			}
		break;
		case KeyX:
			currentSelection = 1;
			dac_ui();
		break;
		case KeyB:
			siggenscope_end();
	}
}

void dac_keyhandler(u8 key){
	switch(key){
		case Key1:
		case Key3:
			if(dac_channel == DAC_CH1) dac_channel = DAC_CH2;
			else dac_channel = DAC_CH1;
			OLED_ShowNum(40,36,dac_channel+1,1,12,1);
			dac_type_ui(dac_channel);
			OLED_ShowNum(22,48,1200/dac_step[dac_channel],4,12,1);
			OLED_ShowNum(82,48,dac_vpp[dac_channel]*33/40960,1,12,1);
			OLED_ShowNum(94,48,dac_vpp[dac_channel]*33/4096%10,1,12,1);
			OLED_Refresh();
		break;
		case Key4:
			if(dac_type[dac_channel]>0) dac_type[dac_channel]--;
			dac_type_ui(dac_channel);
			OLED_Refresh();
		break;
		case Key6:
			if(dac_type[dac_channel]<3) dac_type[dac_channel]++;
			dac_type_ui(dac_channel);
			OLED_Refresh();
		break;
		case Key7:
			if(dac_step[dac_channel]>1) dac_step[dac_channel]--;
			OLED_ShowNum(22,48,1200/dac_step[dac_channel],4,12,1);
			OLED_Refresh();
		break;
		case Key9:
			if(dac_step[dac_channel]<10) dac_step[dac_channel]++;
			OLED_ShowNum(22,48,1200/dac_step[dac_channel],4,12,1);
			OLED_Refresh();
		break;
		case Key10:
			if(dac_vpp[dac_channel]>100) dac_vpp[dac_channel]-=100;
			OLED_ShowNum(82,48,dac_vpp[dac_channel]*33/40960,1,12,1);
			OLED_ShowNum(94,48,dac_vpp[dac_channel]*33/4096%10,1,12,1);
			OLED_Refresh();
		break;
		case Key12:
			if(dac_vpp[dac_channel]<4000) dac_vpp[dac_channel]+=100;
			OLED_ShowNum(82,48,dac_vpp[dac_channel]*33/40960,1,12,1);
			OLED_ShowNum(94,48,dac_vpp[dac_channel]*33/4096%10,1,12,1);
			OLED_Refresh();
		break;
		case KeyA:
			currentSelection = 0;
			siggenscope_ui();
		break;
		case KeyX:
			currentSelection = 2;
			adc_ui();
		break;
		case KeyB:
			siggenscope_end();
	}
}

void adc_keyhandler(u8 key){
	switch(key){
		case Key1:
			if(adc_channel == ADC_CH2) adc_channel = ADC_CH1;
			else if(adc_channel == ADC_CH1_2) adc_channel = ADC_CH2;
			switch(adc_channel){
				case ADC_CH1:
					OLED_ShowString(40,36,"1  ",12,1);
				break;
				case ADC_CH2:
					OLED_ShowString(40,36,"2  ",12,1);
				break;
				case ADC_CH1_2:
					OLED_ShowString(40,36,"1/2",12,1);
				break;
			}
			OLED_Refresh();
		break;
		case Key3:
			if(adc_channel == ADC_CH1) adc_channel = ADC_CH2;
			else if(adc_channel == ADC_CH2) adc_channel = ADC_CH1_2;
			switch(adc_channel){
				case ADC_CH1:
					OLED_ShowString(40,36,"1  ",12,1);
				break;
				case ADC_CH2:
					OLED_ShowString(40,36,"2  ",12,1);
				break;
				case ADC_CH1_2:
					OLED_ShowString(40,36,"1/2",12,1);
				break;
			}
			OLED_Refresh();
		break;
		case Key4:
			if(adc_span>1) adc_span--;
			OLED_ShowNum(100,36,adc_span,1,12,1);
			OLED_Refresh();
		break;
		case Key6:
			if(adc_span<9) adc_span++;
			OLED_ShowNum(100,36,adc_span,1,12,1);
			OLED_Refresh();
		break;
		case Key7:
			if(adc_bias>-15) adc_bias--;
			if(adc_bias < 0)
			{
				OLED_ShowString(19,48,"-",12,1);
				OLED_ShowNum(28,48,(-adc_bias)/10,1,12,1);
				OLED_ShowNum(40,48,(-adc_bias)%10,1,12,1);
			} else
			{
				OLED_ShowString(19,48," ",12,1);
				OLED_ShowNum(28,48,adc_bias/10,1,12,1);
				OLED_ShowNum(40,48,adc_bias%10,1,12,1);
			}
			OLED_Refresh();
		break;
		case Key9:
			if(adc_bias<15) adc_bias++;
			if(adc_bias < 0)
			{
				OLED_ShowString(19,48,"-",12,1);
				OLED_ShowNum(28,48,(-adc_bias)/10,1,12,1);
				OLED_ShowNum(40,48,(-adc_bias)%10,1,12,1);
			} else
			{
				OLED_ShowString(19,48," ",12,1);
				OLED_ShowNum(28,48,adc_bias/10,1,12,1);
				OLED_ShowNum(40,48,adc_bias%10,1,12,1);
			}
			OLED_Refresh();
		break;
		case Key10:
			if(adc_step>1) adc_step--;
			OLED_ShowNum(82,48,adc_step,3,12,1);
			OLED_Refresh();
		break;
		case Key12:
			if(adc_step<255) adc_step++;
			OLED_ShowNum(82,48,adc_step,3,12,1);
			OLED_Refresh();
		break;
		case KeyA:
			currentSelection = 1;
			dac_ui();
		break;
		case KeyB:
			siggenscope_end();
	}
}

void adc_task(void){
	while(1){
		if(++adc_screen_pos >= 107) adc_screen_pos = 0;
		if(currentSelection == 2){
			i16 pos[2];
			OLED_DrawPoint(adc_screen_pos + 11,adc_data[ADC_CH1][adc_screen_pos],0);
			OLED_DrawPoint(adc_screen_pos + 11,adc_data[ADC_CH2][adc_screen_pos],0);
			
			pos[0] = 29 - (adc_value[1] * 33 * 23 / adc_span / 81920 + adc_bias * 23 / 33);
			if(pos[0] < 7) pos[0] = 7;
			if(pos[0] > 29) pos[0] = 29;
			pos[1] = 29 - (adc_value[2] * 33 * 23 / adc_span / 81920 + adc_bias * 23 / 33);
			if(pos[1] < 7) pos[1] = 7;
			if(pos[1] > 29) pos[1] = 29;
			
			adc_data[ADC_CH1][adc_screen_pos] = pos[0];
			adc_data[ADC_CH2][adc_screen_pos] = pos[1];
			
			if(adc_channel == ADC_CH1 || adc_channel == ADC_CH1_2) OLED_DrawPoint(adc_screen_pos + 11,pos[0],1);
			if(adc_channel == ADC_CH2 || adc_channel == ADC_CH1_2) OLED_DrawPoint(adc_screen_pos + 11,pos[1],1);
			OLED_Refresh();
		}
		OSTimeDly(adc_step, OS_OPT_TIME_DLY, &err);
	}
}

void dac_task(void){
	while(1){
		if(++dac_screen_pos >= 107) dac_screen_pos = 0;
		
		if(currentSelection == 1){
			OLED_DrawPoint(dac_screen_pos + 11,29-dac_data[DAC_CH1][dac_screen_pos],0);
			OLED_DrawPoint(dac_screen_pos + 11,29-dac_data[DAC_CH2][dac_screen_pos],0);
		}
		
		if(dac_type[DAC_CH1] != NONE) dac_data_update(DAC_CH1);
		if(dac_type[DAC_CH2] != NONE) dac_data_update(DAC_CH2);
		
		if(currentSelection == 1){
			if(dac_type[DAC_CH1] != NONE) OLED_DrawPoint(dac_screen_pos + 11,29-dac_data[DAC_CH1][dac_screen_pos],1);
			if(dac_type[DAC_CH2] != NONE) OLED_DrawPoint(dac_screen_pos + 11,29-dac_data[DAC_CH2][dac_screen_pos],1);
			OLED_Refresh();
		}
		
		OSTimeDly(10, OS_OPT_TIME_DLY, &err);
	}
}

void dac_data_update(u8 ch){
		u32 dac_channel = ch == DAC_CH1 ? DAC_CHANNEL_1 : DAC_CHANNEL_2;
		dac_time[ch]+=dac_step[ch];
		if(dac_time[ch]>=120) dac_time[ch]=0;
		switch(dac_type[ch]){
			case SIN:
				HAL_DAC_SetValue(&hdac1, dac_channel, DAC_ALIGN_12B_R, sin_code[dac_time[ch]]*dac_vpp[ch]/0xFF);
				dac_data[ch][dac_screen_pos] = sin_code[dac_time[ch]]*dac_vpp[ch]*23/1044480;
			break;
			case SQRT:
				if(dac_time[ch]<60) {
					HAL_DAC_SetValue(&hdac1, dac_channel, DAC_ALIGN_12B_R, dac_vpp[ch]);
					dac_data[ch][dac_screen_pos] = dac_vpp[ch] * 23 / 4096;
				}
				else {
					HAL_DAC_SetValue(&hdac1, dac_channel, DAC_ALIGN_12B_R, 0);
					dac_data[ch][dac_screen_pos] = 0;
				}
			break;
			case TRI:
				if(dac_time[ch]<60) {
					HAL_DAC_SetValue(&hdac1, dac_channel, DAC_ALIGN_12B_R, dac_vpp[ch] * dac_time[ch] / 60);
					dac_data[ch][dac_screen_pos] = dac_vpp[ch] * dac_time[ch] * 23 / 245760;
				}
				else {
					HAL_DAC_SetValue(&hdac1, dac_channel, DAC_ALIGN_12B_R, dac_vpp[ch] * (120 - dac_time[ch])/60);
					dac_data[ch][dac_screen_pos] = dac_vpp[ch] * (120 - dac_time[ch]) * 23 / 245760;
				}
			break;
		}
}

void dac_type_ui(u8 ch){
	switch(dac_type[ch]){
		case NONE:
			OLED_ShowString(100,36,"non",12,1);
		break;
		case SIN:
			OLED_ShowString(100,36,"sin",12,1);
		break;
		case TRI:
			OLED_ShowString(100,36,"tri",12,1);
		break;
		case SQRT:
			OLED_ShowString(100,36,"sqt",12,1);
		break;
	}
}

void siggenscope_start(void){
	OSTaskCreate (&siggenscope_task_tcb,
                 "2CH Signal Generator & Scope",
                 (OS_TASK_PTR)siggenscope_task,
                 NULL,
                 SIGGNSCOPE_TASK_PRIO,
                 siggenscope_task_stack,
                 SIGGNSCOPE_TASK_STACK_SIZE / 10,
                 SIGGNSCOPE_TASK_STACK_SIZE,
                 0,
                 0,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);
}

void siggenscope_end(void){
			smg_task_end();
			if(dac_state == 1){
				dac_state = 0;
				OSTaskDel(&dac_task_tcb,&err);
			}else if (dac_state == 2){
				dac_state = 0;
				OSTaskResume(&dac_task_tcb,&err);
				OSTaskDel(&dac_task_tcb,&err);
			}
			if(adc_state == 1){
				adc_state = 0;
				OSTaskDel(&adc_task_tcb,&err);
			}else if (adc_state == 2){
				adc_state = 1;
				OSTaskResume(&adc_task_tcb,&err);
				OSTaskDel(&adc_task_tcb,&err);
			}
			state_back();
}
