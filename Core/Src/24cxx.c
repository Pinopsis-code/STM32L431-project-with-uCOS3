#include "24cxx.h"
#include "i2c.h"
//��ʼ��IIC�ӿ�

#define AT24CXX_HANDLE	(&hi2c2)	//IIC�ӿ�
#define AT24C_DEV_ADDR  (0XA0) //�豸��ַ

void AT24CXX_Init(void)
{
	AT24CXX_Check();
}

/*****************************************
��������void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
������WriteAddr :Ҫд�����ݵĵ�ַ  pBuffer��Ҫд������ݵ��׵�ַ NumToWrite��Ҫд�����ݵĳ���
������������ָ����ַ��ʼд�����ֽ�����
����ֵ����
*****************************************/
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint8_t block = (WriteAddr >> 8) & 0x07;   // ��ȡ��3λ(A10-A8)
  uint8_t dev_addr = 0xA0 | (block << 1); // ����豸��ַ

  HAL_I2C_Mem_Write(AT24CXX_HANDLE, dev_addr, 
                    WriteAddr & 0xFF, I2C_MEMADD_SIZE_8BIT,
                    pBuffer, NumToWrite, HAL_MAX_DELAY);
	HAL_Delay(5);
}
/*****************************************
��������AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
������ ReadAddr��Ҫ��ȡ���ݵĵ�ַ pBuffer�����������׵�ַ NumToRead:���ݳ���
������������ָ����ַ��ʼ��ȡ������ֽ�����
����ֵ����
*****************************************/
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	uint8_t block = (ReadAddr >> 8) & 0x07;
  uint8_t dev_addr = 0xA0 | (block << 1);
  
  HAL_I2C_Mem_Read(AT24CXX_HANDLE, dev_addr, 
                   ReadAddr & 0xFF, I2C_MEMADD_SIZE_8BIT,
                   pBuffer, NumToRead, HAL_MAX_DELAY);
} 
/*****************************************
��������uint8_t AT24CXX_Check(void)
��������
�������������AT24CXX�Ƿ���������������24XX�����һ����ַ(255)���洢��־��.���������24Cϵ��,�����ַҪ�޸�
����ֵ�����ɹ�����0 ʧ�ܷ���1
*****************************************/
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	uint8_t data = 0XAB;
	AT24CXX_Read(EE_TYPE,&temp,1);//����ÿ�ο�����дAT24CXX			   
	if(temp != 0XAB)
		return 1;
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_Write(EE_TYPE,&data,1);
	    AT24CXX_Read(EE_TYPE,&temp,1);;	  
		if(temp != 0XAB)
			return 1;
	}
	return 0;											  
}

