#include "24cxx.h"
#include "i2c.h"
//初始化IIC接口

#define AT24CXX_HANDLE	(&hi2c2)	//IIC接口
#define AT24C_DEV_ADDR  (0XA0) //设备地址

void AT24CXX_Init(void)
{
	AT24CXX_Check();
}

/*****************************************
函数名：void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
参数：WriteAddr :要写入数据的地址  pBuffer：要写入的数据的首地址 NumToWrite：要写入数据的长度
功能描述：从指定地址开始写入多个字节数据
返回值：无
*****************************************/
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	uint8_t block = (WriteAddr >> 8) & 0x07;   // 提取高3位(A10-A8)
  uint8_t dev_addr = 0xA0 | (block << 1); // 组合设备地址

  HAL_I2C_Mem_Write(AT24CXX_HANDLE, dev_addr, 
                    WriteAddr & 0xFF, I2C_MEMADD_SIZE_8BIT,
                    pBuffer, NumToWrite, HAL_MAX_DELAY);
	HAL_Delay(5);
}
/*****************************************
函数名：AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
参数： ReadAddr：要读取数据的地址 pBuffer：回填数据首地址 NumToRead:数据长度
功能描述：从指定地址开始读取多个个字节数据
返回值：无
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
函数名：uint8_t AT24CXX_Check(void)
参数：无
功能描述：检查AT24CXX是否正常，这里用了24XX的最后一个地址(255)来存储标志字.如果用其他24C系列,这个地址要修改
返回值：检测成功返回0 失败返回1
*****************************************/
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	uint8_t data = 0XAB;
	AT24CXX_Read(EE_TYPE,&temp,1);//避免每次开机都写AT24CXX			   
	if(temp != 0XAB)
		return 1;
	else//排除第一次初始化的情况
	{
		AT24CXX_Write(EE_TYPE,&data,1);
	    AT24CXX_Read(EE_TYPE,&temp,1);;	  
		if(temp != 0XAB)
			return 1;
	}
	return 0;											  
}

