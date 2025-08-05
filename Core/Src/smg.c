#include "smg.h"

CPU_STK     smg_task_stack[SMG_TASK_STACK_SIZE];
OS_TCB      smg_task_tcb;

u8 smg_num_buffer[8] = {0,0,0,0,0,0,0,0}; // 初始全消隐
u8 currentPosition = 0;
const uint8_t segCodes[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F 	// 9
};
void SMG_sendData(u8 data) {
  HAL_GPIO_WritePin(SMG_LCLK_GPIO_Port,SMG_LCLK_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SMG_SCLK_GPIO_Port, SMG_SCLK_Pin, GPIO_PIN_RESET);
    
  for (u8 i = 0; i < 8; i++) {
    HAL_GPIO_WritePin(SMG_SCLK_GPIO_Port,SMG_SCLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SMG_DATA_GPIO_Port,SMG_DATA_Pin, (data & (0x80 >> i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SMG_SCLK_GPIO_Port,SMG_SCLK_Pin, GPIO_PIN_SET); // 上升沿移位
  }
  
  HAL_GPIO_WritePin(SMG_LCLK_GPIO_Port,SMG_LCLK_Pin, GPIO_PIN_SET); // 上升沿锁存
	HAL_GPIO_WritePin(SMG_LCLK_GPIO_Port, SMG_LCLK_Pin, GPIO_PIN_RESET);
}

void SMG_sendPos(u8 pos) {
	if(pos > 7) return;
  HAL_GPIO_WritePin(SMG_w1_GPIO_Port,SMG_w1_Pin, (pos & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SMG_w2_GPIO_Port,SMG_w2_Pin, (pos & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SMG_w3_GPIO_Port,SMG_w3_Pin, (pos & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void SMG_SetNum(uint8_t position, uint8_t num)
{
	if(position > 7 || num > 9) return;
	smg_num_buffer[position] = segCodes[num];
}

void SMG_SetDot(uint8_t position){
	if(position > 7) return;
	smg_num_buffer[position] |= 0x80;
}

void SMG_Refresh() {
    // 1. 消隐当前显示
    SMG_sendData(0x00); // 发送全灭段码
    
    // 2. 切换到下一位
    currentPosition = (currentPosition + 1) % 8; // 0-7循环
    SMG_sendPos(currentPosition);
    
    // 3. 显示新内容
    SMG_sendData(smg_num_buffer[currentPosition]);
}

void SMG_Clear() {
	for(int i = 0; i <8; i++) smg_num_buffer[i] = 0x00;
}

void smg_task(){
	OS_ERR err;
	while(1){
		SMG_Refresh();
		OSTimeDly(3, OS_OPT_TIME_DLY, &err);
	}
}

void smg_task_start(){
	OS_ERR err;
	OSTaskCreate (&smg_task_tcb,
                 "SMG enable",
                 (OS_TASK_PTR)smg_task,
                 NULL,
                 SMG_TASK_PRIO,
                 smg_task_stack,
                 SMG_TASK_STACK_SIZE / 10,
                 SMG_TASK_STACK_SIZE,
                 0,
                 0,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);
}

void smg_task_end(){
	OS_ERR err;
	SMG_Clear();
	SMG_Refresh();
	OSTaskDel(&smg_task_tcb,&err);
}
