#include "sd_card.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>

#define ADMIN_PASS_FILE "admin.txt" 
/**
 * @brief  初始化并挂载SD卡
 */
uint8_t SD_Card_Init(void)
{
    FRESULT res;
    res = f_mount(&USERFatFS, (TCHAR const*)"/", 1);
    if (res != FR_OK)
    {
        return SD_MOUNT_ERR;
    }
    return SD_OK;
}

/**
 * @brief  获取SD卡容量信息
 */
uint8_t SD_Card_Get_Info(uint32_t *total_space, uint32_t *free_space)
{
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res;
    res = f_getfree((TCHAR const*)SDPath, &fre_clust, &fs);
    if (res != FR_OK)
    {
        return res;
    }
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    *total_space = tot_sect / 2 / 1024; 
    *free_space = fre_sect / 2 / 1024;

    return SD_OK;
}


/**
 * @brief  写入测试文件
 */
uint8_t SD_Card_Write_Test(const char* filename, const char* content)
{
    FRESULT res;
    uint32_t byteswritten;

    res = f_open(&SDFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        return SD_OPEN_ERR;
    }

    res = f_write(&SDFile, content, strlen(content), (void *)&byteswritten);
    if ((byteswritten == 0) || (res != FR_OK))
    {
        f_close(&SDFile);
        return SD_WRITE_ERR;
    }

    f_close(&SDFile);
    
    return SD_OK;
}

/**
 * @brief  读取测试文件
 */
uint8_t SD_Card_Read_Test(const char* filename, char* read_buffer, uint16_t buffer_size)
{
    FRESULT res;
    uint32_t bytesread;

    res = f_open(&SDFile, filename, FA_READ);
    if (res != FR_OK)
    {
        return SD_OPEN_ERR;
    }
    
    memset(read_buffer, 0, buffer_size);

    res = f_read(&SDFile, read_buffer, buffer_size - 1, (UINT*)&bytesread);
    if ((bytesread == 0) || (res != FR_OK))
    {
        f_close(&SDFile);
        return SD_READ_ERR;
    }
    
    read_buffer[bytesread] = '\0'; 

    f_close(&SDFile);

    return SD_OK;
}

/**
 * @brief  从SD卡读取管理员密码
 * @param  admin_pass_buffer: 用于存储密码的缓冲区
 * @param  buffer_size: 缓冲区大小
 * @retval SD_OK 或 错误码
 */
uint8_t SD_Card_Read_Admin_Password(char* admin_pass_buffer, uint16_t buffer_size)
{
    FRESULT res;
    uint32_t bytesread;

    res = f_mount(&USERFatFS, (TCHAR const*)SDPath, 1);
    if (res != FR_OK && res != FR_EXIST) 
    {
        return SD_MOUNT_ERR;
    }

    res = f_open(&SDFile, ADMIN_PASS_FILE, FA_READ);
    if (res != FR_OK)
    {
        return SD_OPEN_ERR;
    }
    
    memset(admin_pass_buffer, 0, buffer_size);

    res = f_read(&SDFile, admin_pass_buffer, buffer_size - 1, (UINT*)&bytesread);
    if (res != FR_OK)
    {
        f_close(&SDFile);
        return SD_READ_ERR;
    }
    
    admin_pass_buffer[bytesread] = '\0';

    f_close(&SDFile);
    f_mount(NULL, (TCHAR const*)SDPath, 1);

    return SD_OK;
}
