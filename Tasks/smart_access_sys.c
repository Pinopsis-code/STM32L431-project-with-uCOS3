#include "tasks.h"
#include "key.h"
#include "oled.h"
#include "micro_str.h"
#include "24cxx.h"
#include "fatfs.h"
#include "stdio.h"

#define USERNAME_WINDOW 0
#define PASSWORD_WINDOW 1
#define UNLOCK_WINDOW 2
#define ADMIN_WINDOW 3
#define ADMIN_USERNAME_WINDOW 4
#define ADMIN_PASSWORD_WINDOW 5

CPU_STK     smart_access_sys_task_stack[SMART_ACCESS_SYS_TASK_STACK_SIZE];
OS_TCB      smart_access_sys_task_tcb;

u16 sleeptime;

u8 enter_name[10];
u8 enter_password[6];

u8 store_name[5][10];
u8 store_password[5][6];

u8 admin_name[10];

u8 enter_pos;
u8 current_window;
u8 user_pos;
u8 user_check_flag;
u8 admin_pos;
u8 password_wrong_count;

static u8 temp;

char user_log[50];
UINT bw;

void username_ui(void);
void password_ui(void);
void admin_ui(void);
void username_keyhandle(u8 key);
void password_keyhandle(u8 key);
void admin_keyhandle(u8 key);

void write_data(void);

void smart_access_sys_task(void){
	u8 key = 0, prekey = 0;
	u16 keyhold = 0;
	sleeptime = 0;
	enter_pos = 0;
	current_window = USERNAME_WINDOW;
	m_memset(enter_name,10,10);
	m_memset(enter_password,10,6);
	for(u8 i = 0; i < 5; i++){
		AT24CXX_Read(i * 32 + 32,store_name[i],10);
		AT24CXX_Read(i * 32 + 48,store_password[i],6);
	}
	username_ui();
	while(1){
		prekey = key;
		key = key_scan();
		if(key == prekey) keyhold++;
		else keyhold = 0;
		if(key != 0 &&(keyhold == 0 || keyhold > 10)){
			switch(current_window){
				case USERNAME_WINDOW:
				case ADMIN_USERNAME_WINDOW:
					username_keyhandle(key);break;
				case PASSWORD_WINDOW:
				case ADMIN_PASSWORD_WINDOW:
					password_keyhandle(key);break;
				case UNLOCK_WINDOW:
					if(key == KeyY){
						wRelay(1);
						username_ui();
						current_window = USERNAME_WINDOW;
					}
				break;
				case ADMIN_WINDOW:
					admin_keyhandle(key);break;
			}
		}
		if(current_window == USERNAME_WINDOW && enter_pos == 0)sleeptime++;
		else sleeptime = 0;
		if(sleeptime > 100){
			temp = 2;
			AT24CXX_Write(0,&temp,1);
			__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
			OLED_Clear();
			OLED_ShowString(40,26,"Sleep...",12,1);
			OLED_Refresh();
			HAL_PWR_EnterSTANDBYMode();
		}
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	}
}

void username_ui(void){
	OLED_Clear();
	OLED_ShowString(3,51,"A Del",12,1);
	OLED_ShowString(102,51,"OK X",12,1);
	
	OLED_DrawLine(0,50,11,50,1);
	OLED_DrawLine(0,51,0,63,1);
	OLED_DrawLine(0,63,11,63,1);
	OLED_DrawLine(10,51,10,63,1);
	
	OLED_DrawLine(117,50,128,50,1);
	OLED_DrawLine(117,51,117,63,1);
	OLED_DrawLine(117,63,128,63,1);
	OLED_DrawLine(127,51,127,63,1);
	
	OLED_ShowString(21,0,"Enter Username",12,1);
	OLED_Refresh();
}
void password_ui(void){
	OLED_Clear();
	OLED_ShowString(3,51,"A Del",12,1);
	OLED_ShowString(102,51,"OK X",12,1);
	
	OLED_DrawLine(0,50,11,50,1);
	OLED_DrawLine(0,51,0,63,1);
	OLED_DrawLine(0,63,11,63,1);
	OLED_DrawLine(10,51,10,63,1);
	
	OLED_DrawLine(117,50,128,50,1);
	OLED_DrawLine(117,51,117,63,1);
	OLED_DrawLine(117,63,128,63,1);
	OLED_DrawLine(127,51,127,63,1);
	
	OLED_ShowString(21,0,"Enter Password",12,1);
	OLED_Refresh();
}

void admin_ui(void){
	OLED_Clear();
	OLED_ShowString(3,51,"A Pre",12,1);
	OLED_ShowString(90,51,"Next X",12,1);
	
	OLED_DrawLine(0,50,11,50,1);
	OLED_DrawLine(0,51,0,63,1);
	OLED_DrawLine(0,63,11,63,1);
	OLED_DrawLine(10,51,10,63,1);
	
	OLED_DrawLine(117,50,128,50,1);
	OLED_DrawLine(117,51,117,63,1);
	OLED_DrawLine(117,63,128,63,1);
	OLED_DrawLine(127,51,127,63,1);
	
	OLED_ShowString(48,0,"USER:",12,1);
	
	AT24CXX_Read(admin_pos * 32 + 32,admin_name,10);
	
	if(admin_name[0] != 10)
	for(u8 i = 0; i < 10; i++){
		OLED_ShowNum(33 + i * 6, 24,admin_name[i],1,12,1);
		OLED_ShowString(30,36,"1.Set 2.Del",12,1);
	}
	else {
		OLED_ShowString(48,24,"Empty",12,1);
		OLED_ShowString(48,36,"1.Add",12,1);
	}
	OLED_Refresh();
}

void username_keyhandle(u8 key){
	switch(key){
		case Key1:
			if(enter_pos < 10){
				enter_name[enter_pos] = 1;
				OLED_ShowChar(33 + enter_pos * 6,24,'1',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key2:
			if(enter_pos < 10){
				enter_name[enter_pos] = 2;
				OLED_ShowChar(33 + enter_pos * 6,24,'2',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key3:
			if(enter_pos < 10){
				enter_name[enter_pos] = 3;
				OLED_ShowChar(33 + enter_pos * 6,24,'3',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key4:
			if(enter_pos < 10){
				enter_name[enter_pos] = 4;
				OLED_ShowChar(33 + enter_pos * 6,24,'4',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key5:
			if(enter_pos < 10){
				enter_name[enter_pos] = 5;
				OLED_ShowChar(33 + enter_pos * 6,24,'5',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key6:
			if(enter_pos < 10){
				enter_name[enter_pos] = 6;
				OLED_ShowChar(33 + enter_pos * 6,24,'6',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key7:
			if(enter_pos < 10){
				enter_name[enter_pos] = 7;
				OLED_ShowChar(33 + enter_pos * 6,24,'7',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key8:
			if(enter_pos < 10){
				enter_name[enter_pos] = 8;
				OLED_ShowChar(33 + enter_pos * 6,24,'8',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key9:
			if(enter_pos < 10){
				enter_name[enter_pos] = 9;
				OLED_ShowChar(33 + enter_pos * 6,24,'9',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key11:
			if(enter_pos < 10){
				enter_name[enter_pos] = 0;
				OLED_ShowChar(33 + enter_pos * 6,24,'0',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case KeyA:
			if(enter_pos > 0){
				enter_pos--;
				enter_name[enter_pos] = 10;
				OLED_ShowChar(33 + enter_pos * 6,24,' ',12,1);
				OLED_Refresh();
			}
		break;
		case KeyX:
			if(current_window == USERNAME_WINDOW){
				user_pos = 5;
				if(password_wrong_count < 5){
					for(u8 i = 0; i < 5; i++){
						user_check_flag = 1;
						for(u8 j = 0; j < 10; j++){
							if(enter_name[j] != store_name[i][j]) user_check_flag = 0;
						}
						if(user_check_flag == 1) user_pos = i;
					}
					if(user_pos != 5) {
						current_window = PASSWORD_WINDOW;
						enter_pos = 0;
						m_memset(enter_name,10,10);
						password_ui();
					}
					else {
						OLED_ShowString(24,36,"-- No User --",12,1);
						OLED_Refresh();
						OSTimeDly(500, OS_OPT_TIME_DLY, &err);
						OLED_ShowString(24,36,"             ",12,1);
						OLED_Refresh();
					}
				}else {
					user_check_flag = 1;
						for(u8 i = 0; i < 10; i++){
						if(enter_name[i] != store_name[0][i]) user_check_flag = 0;
					}
					if(user_check_flag == 1){
						user_pos = 0;
						current_window = PASSWORD_WINDOW;
						enter_pos = 0;
						m_memset(enter_name,10,10);
						password_ui();
					}
					else {
						OLED_ShowString(18,36,"-- Not Admin --",12,1);
						OLED_Refresh();
						OSTimeDly(500, OS_OPT_TIME_DLY, &err);
						OLED_ShowString(18,36,"               ",12,1);
						OLED_Refresh();
					}
				}
			} else if (current_window == ADMIN_USERNAME_WINDOW){
				user_pos = 5;
				for(u8 i = 0; i < 5; i++){
					user_check_flag = 1;
					for(u8 j = 0; j < 10; j++){
						if(enter_name[j] != store_name[i][j]) user_check_flag = 0;
					}
					if(user_check_flag == 1) user_pos = i;
				}
				if(user_pos != 5){
					OLED_ShowString(9,36,"--Name Duplicate--",12,1);
					OLED_Refresh();
					OSTimeDly(500, OS_OPT_TIME_DLY, &err);
					OLED_ShowString(9,36,"                  ",12,1);
					OLED_Refresh();
				} else {
					AT24CXX_Write(admin_pos * 32 + 32,enter_name,10);
					m_memset(enter_name,10,10);
					enter_pos = 0;
					current_window = ADMIN_PASSWORD_WINDOW;
					password_ui();
				}
			}
		break;
		case KeyB:
			if(current_window == USERNAME_WINDOW)
			state_back();
			else if (current_window == ADMIN_USERNAME_WINDOW){
				m_memset(enter_name,10,6);
				enter_pos = 0;
				current_window = ADMIN_WINDOW;
				admin_ui();
			}
		break;
	}
}
void password_keyhandle(u8 key){
	switch(key){
		case Key1:
			if(enter_pos < 6){
				enter_password[enter_pos] = 1;
				OLED_ShowChar(46 + enter_pos * 6,24,'1',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key2:
			if(enter_pos < 6){
				enter_password[enter_pos] = 2;
				OLED_ShowChar(46 + enter_pos * 6,24,'2',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key3:
			if(enter_pos < 6){
				enter_password[enter_pos] = 3;
				OLED_ShowChar(46 + enter_pos * 6,24,'3',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key4:
			if(enter_pos < 6){
				enter_password[enter_pos] = 4;
				OLED_ShowChar(46 + enter_pos * 6,24,'4',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key5:
			if(enter_pos < 6){
				enter_password[enter_pos] = 5;
				OLED_ShowChar(46 + enter_pos * 6,24,'5',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key6:
			if(enter_pos < 6){
				enter_password[enter_pos] = 6;
				OLED_ShowChar(46 + enter_pos * 6,24,'6',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key7:
			if(enter_pos < 6){
				enter_password[enter_pos] = 7;
				OLED_ShowChar(46 + enter_pos * 6,24,'7',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key8:
			if(enter_pos < 6){
				enter_password[enter_pos] = 8;
				OLED_ShowChar(46 + enter_pos * 6,24,'8',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key9:
			if(enter_pos < 6){
				enter_password[enter_pos] = 9;
				OLED_ShowChar(46 + enter_pos * 6,24,'9',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case Key11:
			if(enter_pos < 6){
				enter_password[enter_pos] = 0;
				OLED_ShowChar(46 + enter_pos * 6,24,'0',12,1);
				OLED_Refresh();
				enter_pos++;
			}
		break;
		case KeyA:
			if(enter_pos > 0){
				enter_pos--;
				enter_password[enter_pos] = 10;
				OLED_ShowChar(46 + enter_pos * 6,24,' ',12,1);
				OLED_Refresh();
			}
		break;
		case KeyX:
			if(current_window == PASSWORD_WINDOW){
				user_check_flag = 1;
				for(u8 i = 0; i < 6; i++){
					if(enter_password[i] != store_password[user_pos][i]) user_check_flag = 0;
				}
				if(user_check_flag == 1) {
					if(user_pos == 0){
						OLED_Clear();
						OLED_ShowString(34,24,"Admin Mode",12,1);
						OLED_Refresh();
						admin_pos = 1;
						enter_pos = 0;
						m_memset(enter_password,10,6);
						current_window = ADMIN_WINDOW;
						OSTimeDly(500, OS_OPT_TIME_DLY, &err);
						admin_ui();
					} else {
						OLED_ShowString(27,36,"-- Opened --",12,1);
						OLED_Refresh();
						wRelay(0);
						sprintf(user_log,"user:%d%d%d%d%d%d%d%d%d%d",enter_name[0],enter_name[1],enter_name[2],enter_name[3],enter_name[4],enter_name[5],enter_name[6],enter_name[7],enter_name[8],enter_name[9]);
						retUSER = f_open(&USERFile, "0:/userdata.txt", FA_WRITE | FA_CREATE_ALWAYS);
						if(retUSER == FR_OK){
							f_write(&USERFile, user_log, sizeof(user_log), &bw);
							f_close(&USERFile);
						}
						enter_pos = 0;
						password_wrong_count = 0;
						OSTimeDly(500, OS_OPT_TIME_DLY, &err);
						OLED_Clear();
						OLED_ShowString(4,36,"--Press Y to Close--",12,1);
						OLED_Refresh();
						m_memset(enter_password,10,6);
						current_window = UNLOCK_WINDOW;
					}
				}
				else {
					password_wrong_count++;
					if(password_wrong_count < 5){
						OLED_ShowString(30,36,"-- Wrong --",12,1);
						OLED_Refresh();
						OSTimeDly(500, OS_OPT_TIME_DLY, &err);
						OLED_ShowString(30,36,"           ",12,1);
						OLED_Refresh();
					} else {
						OLED_ShowString(27,36,"-- Locked! --",12,1);
						OLED_Refresh();
						OSTimeDly(100, OS_OPT_TIME_DLY, &err);
						wLED0(!rLED0);
						OSTimeDly(100, OS_OPT_TIME_DLY, &err);
						wLED0(!rLED0);
						OSTimeDly(100, OS_OPT_TIME_DLY, &err);
						wLED0(!rLED0);
						OSTimeDly(100, OS_OPT_TIME_DLY, &err);
						wLED0(!rLED0);
						OSTimeDly(100, OS_OPT_TIME_DLY, &err);
						wLED0(!rLED0);
						OSTimeDly(100, OS_OPT_TIME_DLY, &err);
						wLED0(!rLED0);
						m_memset(enter_password,10,6);
						enter_pos = 0;
						current_window = USERNAME_WINDOW;
						username_ui();
					}
				}
			} else if (current_window == ADMIN_PASSWORD_WINDOW){
				AT24CXX_Write(admin_pos * 32 + 48,enter_password,10);
				m_memset(enter_password,10,6);
				enter_pos = 0;
				current_window = ADMIN_WINDOW;
				admin_ui();
			}
		break;
		case KeyB:
			m_memset(enter_password,10,6);
			enter_pos = 0;
			if(current_window == PASSWORD_WINDOW) current_window = USERNAME_WINDOW;
			else if(current_window == ADMIN_PASSWORD_WINDOW) current_window = ADMIN_USERNAME_WINDOW;
			username_ui();
		break;
	}
}

void admin_keyhandle(u8 key){
	switch(key){
		case KeyA:
			if(admin_pos>1){
				admin_pos--;
				admin_ui();
			}
		break;
		case KeyX:
			if(admin_pos<4){
				admin_pos++;
				admin_ui();
			}
		break;
		case Key2:
			if(admin_name[0] != 10){
				temp = 10;
				AT24CXX_Write(admin_pos * 32 + 32,&temp,1);
				m_memset(store_name[admin_pos],10,10);
				admin_ui();
			}
		break;
		case Key1:
			if(admin_name[0] == 10){
				current_window = ADMIN_USERNAME_WINDOW;
				username_ui();
			} else {
				current_window = ADMIN_PASSWORD_WINDOW;
				password_ui();
			}
		break;
		case KeyB:
			for(u8 i = 0; i < 5; i++){
				AT24CXX_Read(i * 32 + 32,store_name[i],10);
				AT24CXX_Read(i * 32 + 48,store_password[i],6);
			}
			current_window = USERNAME_WINDOW;
			username_ui();
		break;
	}
}

void smart_access_sys_start(void){
	OSTaskCreate (&smart_access_sys_task_tcb,
                 "Smart Access System",
                 (OS_TASK_PTR)smart_access_sys_task,
                 NULL,
                 SMART_ACCESS_SYS_TASK_PRIO,
                 smart_access_sys_task_stack,
                 SMART_ACCESS_SYS_TASK_STACK_SIZE / 10,
                 SMART_ACCESS_SYS_TASK_STACK_SIZE,
                 0,
                 0,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);
}

void write_data(void){
	u8 name_data[5][10] = {
	{1,0,1,0,2,2,2,2,2,1},
	{3,2,5,7,4,9,5,7,2,7},
	{3,5,4,4,1,8,5,4,6,9},
	{1,9,5,8,5,7,6,8,4,8},
	{0,0,0,0,0,0,0,0,0,0}
	};
	u8 password_data[5][6] = {
	{6,6,6,5,5,5},
	{1,2,3,4,5,6},
	{1,2,3,4,5,6},
	{1,2,3,4,5,6},
	{1,2,3,6,5,4}
	};
	for(u8 i = 0; i < 5; i++){
		uint16_t name_addr = i * 32 + 32;
    uint16_t pass_addr = i * 32 + 48;
		AT24CXX_Write(name_addr,name_data[i],10);
		AT24CXX_Write(pass_addr,password_data[i],6);
	}
}
