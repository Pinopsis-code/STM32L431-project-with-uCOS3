#include "tasks.h"
#include "key.h"
#include "oled.h"
#include "usart.h"
#include "stdio.h"
#include "dht11.h"
#include "dac.h"
#include "smg.h"
#include "24cxx.h"
#include "rtc.h"
#include "fatfs.h"

CPU_STK     hardware_test_task_stack[HARDWARE_TEST_TASK_STACK_SIZE];
OS_TCB      hardware_test_task_tcb;

//DHT11
static u16 temp;
static u8 hum;

//UART
static char str[100];

//SMG
static u8 pos,num;

//EEPROM:ST24C16
static u8 mem_buffer[20];

//SDMMC&FATFS
static char buffer[128];
static UINT br;

//rtc
static RTC_TimeTypeDef RTC_TimeStruct; 
static RTC_DateTypeDef RTC_DateStruct; 

void hardware_test_ui(void);
void hardware_test_keyhandle(u8 key);

void hardware_test_task(void){
	u8 key = 0, prekey = 0;
	u16 keyhold = 0;
	pos=0;
	num=0;
	hardware_test_ui();
	smg_task_start();
	while(1){
		prekey = key;
		key = key_scan();
		if(key == prekey) keyhold++;
		else keyhold = 0;
		if(key != 0 &&(keyhold == 0 || keyhold > 10))
		hardware_test_keyhandle(key);
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	}
}

void hardware_test_ui(void){
	OLED_Clear();
	OLED_ShowString(25,0,"Hardware Test",12,1);
	OLED_ShowString(4,12,"1.LED 2.BEEP  3.UART",12,1);
	OLED_ShowString(4,24,"4.ADC 5.DAC   6.DHT",12,1);
	OLED_ShowString(4,36,"7.SMG 8.RELAY 9.ROM",12,1);
	OLED_ShowString(4,48,"10.SD 11.RTC  12.WK",12,1);
	OLED_Refresh();
}

void hardware_test_keyhandle(u8 key){
	u8 data = 1;
	switch(key){
		
		//LED
		case Key1:
			wLED0(!rLED0);
			wLED1(!rLED1);
			wLED2(!rLED2);
			wLED3(!rLED3);break;
		
		//BEEP
		case Key2:
			wBeep(!rBeep);break;
		
		//UART
		case Key3:
			USART_SendString("Hello UART\r\n");break;
		
		//ADC
		case Key4:
			sprintf(str,"ADC:%d %d %d\r\n",adc_value[0]/2,adc_value[1]/2,adc_value[2]/2);
			USART_SendString(str);
		break;
		
		//DAC
		case Key5:
			if (dac_value[0] < 100) dac_value[0] = 4096;
			else dac_value[0] -= 100;
			if (dac_value[1] < 50) dac_value[1] = 4096;
			else dac_value[1] -= 50;
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value[0]);
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac_value[1]);
			sprintf(str,"DAC:%d %d\r\n",dac_value[0],dac_value[1]);
			USART_SendString(str);
		break;
		
		//DHT11
		case Key6:
			DHT11_Read(&temp,&hum);
			sprintf(str,"Temprature:%d.%dC\r\nHumidity:%d%%\r\n",temp/10,temp%10,hum);
			USART_SendString(str);
		break;
			
		//SMG
		case Key7:
			SMG_SetNum(pos,num);
			pos = (pos + 1) % 8;
			num = (num + 1) % 10;
		break;
		
		//RELAY
		case Key8:
			wRelay(!rRelay);
		break;
		
		//EEPROM&AT24C02
		case Key9:
			AT24CXX_Write(1,(uint8_t *)"hello",6);
			AT24CXX_Read(1,mem_buffer,6);
			USART_SendString((char*)mem_buffer);
			USART_SendString("\r\n");
		break;
		
		//SDMMC&FatFs
		case Key10:
			retUSER = f_open(&USERFile, "0:/test.txt", FA_READ);
			if (retUSER  == FR_OK) {
				f_read(&USERFile, buffer, sizeof(buffer), &br);
				f_close(&USERFile);
				buffer[br] = '\0';
				USART_SendString(buffer);
				USART_SendString("\r\n");
			}
		break;
			
		//RTC
		case Key11:
			HAL_RTC_GetTime(&hrtc,&RTC_TimeStruct,RTC_FORMAT_BIN); 
			HAL_RTC_GetDate(&hrtc,&RTC_DateStruct,RTC_FORMAT_BIN); 
			sprintf(str,"Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
			USART_SendString(str);
			sprintf(str,"Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); 
			USART_SendString(str);
		break;
		
		//Stand By&Wake Up
		case Key12:
			data = 1;
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
			AT24CXX_Write(0,&data,1);
			OLED_Clear();
			OLED_ShowString(40,26,"Sleep...",12,1);
			OLED_Refresh();
			HAL_PWR_EnterSTANDBYMode();
		break;
		
		//Back
		case KeyB:
			wLED0(0);
			wLED1(0);
			wLED2(1);
			wLED3(1);
			wBeep(1);
			wRelay(1);
			smg_task_end();
			state_back();
	}
}

void hardware_test_start(void){
	OSTaskCreate (&hardware_test_task_tcb,
                 "Hardware Test",
                 (OS_TASK_PTR)hardware_test_task,
                 NULL,
                 HARDWARE_TEST_TASK_PRIO,
                 hardware_test_task_stack,
                 HARDWARE_TEST_TASK_STACK_SIZE / 10,
                 HARDWARE_TEST_TASK_STACK_SIZE,
                 0,
                 0,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);
}
