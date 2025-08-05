#include "key.h"

u8 key_scan(void){
	OS_ERR err;
	
	u8 key = 0;

	HAL_GPIO_WritePin(Key_H1_GPIO_Port, Key_H1_Pin, GPIO_PIN_RESET);
	OSTimeDly(5, OS_OPT_TIME_DLY, &err);
	if (HAL_GPIO_ReadPin(Key_L1_GPIO_Port, Key_L1_Pin) == GPIO_PIN_RESET) key = 1;
	if (HAL_GPIO_ReadPin(Key_L2_GPIO_Port, Key_L2_Pin) == GPIO_PIN_RESET) key = 2;
	if (HAL_GPIO_ReadPin(Key_L3_GPIO_Port, Key_L3_Pin) == GPIO_PIN_RESET) key = 3;
	if (HAL_GPIO_ReadPin(Key_L4_GPIO_Port, Key_L4_Pin) == GPIO_PIN_RESET) key = 4;
	HAL_GPIO_WritePin(Key_H1_GPIO_Port, Key_H1_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(Key_H2_GPIO_Port, Key_H2_Pin, GPIO_PIN_RESET);
	OSTimeDly(5, OS_OPT_TIME_DLY, &err);
	if (HAL_GPIO_ReadPin(Key_L1_GPIO_Port, Key_L1_Pin) == GPIO_PIN_RESET) key = 5;
	if (HAL_GPIO_ReadPin(Key_L2_GPIO_Port, Key_L2_Pin) == GPIO_PIN_RESET) key = 6;
	if (HAL_GPIO_ReadPin(Key_L3_GPIO_Port, Key_L3_Pin) == GPIO_PIN_RESET) key = 7;
	if (HAL_GPIO_ReadPin(Key_L4_GPIO_Port, Key_L4_Pin) == GPIO_PIN_RESET) key = 8;
	HAL_GPIO_WritePin(Key_H2_GPIO_Port, Key_H2_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(Key_H3_GPIO_Port, Key_H3_Pin, GPIO_PIN_RESET);
	OSTimeDly(5, OS_OPT_TIME_DLY, &err);
	if (HAL_GPIO_ReadPin(Key_L1_GPIO_Port, Key_L1_Pin) == GPIO_PIN_RESET) key = 9;
	if (HAL_GPIO_ReadPin(Key_L2_GPIO_Port, Key_L2_Pin) == GPIO_PIN_RESET) key = 10;
	if (HAL_GPIO_ReadPin(Key_L3_GPIO_Port, Key_L3_Pin) == GPIO_PIN_RESET) key = 11;
	if (HAL_GPIO_ReadPin(Key_L4_GPIO_Port, Key_L4_Pin) == GPIO_PIN_RESET) key = 12;
	HAL_GPIO_WritePin(Key_H3_GPIO_Port, Key_H3_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(Key_H4_GPIO_Port, Key_H4_Pin, GPIO_PIN_RESET);
	OSTimeDly(5, OS_OPT_TIME_DLY, &err);
	if (HAL_GPIO_ReadPin(Key_L1_GPIO_Port, Key_L1_Pin) == GPIO_PIN_RESET) key = 13;
	if (HAL_GPIO_ReadPin(Key_L2_GPIO_Port, Key_L2_Pin) == GPIO_PIN_RESET) key = 14;
	if (HAL_GPIO_ReadPin(Key_L3_GPIO_Port, Key_L3_Pin) == GPIO_PIN_RESET) key = 15;
	if (HAL_GPIO_ReadPin(Key_L4_GPIO_Port, Key_L4_Pin) == GPIO_PIN_RESET) key = 16;
	HAL_GPIO_WritePin(Key_H4_GPIO_Port, Key_H4_Pin, GPIO_PIN_SET);
	
	return key;
}

