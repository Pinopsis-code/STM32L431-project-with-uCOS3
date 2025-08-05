#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "main.h"

//----- ��������ֵ���� -----
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
 * @brief  ��ʼ��������SD��
 * @param  None
 * @retval SD_OK: �ɹ�, ����: ʧ�ܴ���
 */
uint8_t SD_Card_Init(void);

/**
 * @brief  ��ȡ������SD����������Ϣ
 * @param  total_space: ���ڴ洢������(MB)��ָ��
 * @param  free_space: ���ڴ洢ʣ������(MB)��ָ��
 * @retval SD_OK: �ɹ�, ����: ʧ��
 */
uint8_t SD_Card_Get_Info(uint32_t *total_space, uint32_t *free_space);

/**
 * @brief  ��SD��д��һ�������ļ�
 * @param  filename: �ļ���
 * @param  content: Ҫд����ַ�������
 * @retval SD_OK: �ɹ�, ����: ʧ��
 */
uint8_t SD_Card_Write_Test(const char* filename, const char* content);

/**
 * @brief  ��SD����ȡһ���ļ�������
 * @param  filename: �ļ���
 * @param  read_buffer: ���ڴ�Ŷ�ȡ���ݵĻ�����
 * @param  buffer_size: ����������󳤶�
 * @retval SD_OK: �ɹ�, ����: ʧ��
 */
uint8_t SD_Card_Read_Test(const char* filename, char* read_buffer, uint16_t buffer_size);


#endif /* __SD_CARD_H */
