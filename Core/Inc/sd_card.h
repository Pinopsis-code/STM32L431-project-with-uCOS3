#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "main.h"

//----- 函数返回值定义 -----
#define SD_OK        0
#define SD_MOUNT_ERR 1
#define SD_OPEN_ERR  2
#define SD_WRITE_ERR 3
#define SD_READ_ERR  4

uint8_t SD_Card_Init(void);
uint8_t SD_Card_Get_Info(uint32_t *total_space, uint32_t *free_space);
uint8_t SD_Card_Write_Test(const char* filename, const char* content);
uint8_t SD_Card_Read_Test(const char* filename, char* read_buffer, uint16_t buffer_size);
uint8_t SD_Card_Read_Admin_Password(char* admin_pass_buffer, uint16_t buffer_size);

/**
 * @brief  初始化并挂载SD卡
 * @param  None
 * @retval SD_OK: 成功, 其他: 失败代码
 */
uint8_t SD_Card_Init(void);

/**
 * @brief  获取并返回SD卡的容量信息
 * @param  total_space: 用于存储总容量(MB)的指针
 * @param  free_space: 用于存储剩余容量(MB)的指针
 * @retval SD_OK: 成功, 其他: 失败
 */
uint8_t SD_Card_Get_Info(uint32_t *total_space, uint32_t *free_space);

/**
 * @brief  向SD卡写入一个测试文件
 * @param  filename: 文件名
 * @param  content: 要写入的字符串内容
 * @retval SD_OK: 成功, 其他: 失败
 */
uint8_t SD_Card_Write_Test(const char* filename, const char* content);

/**
 * @brief  从SD卡读取一个文件的内容
 * @param  filename: 文件名
 * @param  read_buffer: 用于存放读取内容的缓冲区
 * @param  buffer_size: 缓冲区的最大长度
 * @retval SD_OK: 成功, 其他: 失败
 */
uint8_t SD_Card_Read_Test(const char* filename, char* read_buffer, uint16_t buffer_size);


#endif /* __SD_CARD_H */
