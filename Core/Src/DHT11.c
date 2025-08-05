#include "dht11.h"
void DHT11_Delay_us(uint16_t us)
{
    uint32_t ticks = SystemCoreClock / 1000000 * us / 5;
    while(ticks--) {
        __NOP();
    }
}
void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = DHT11_Pin,
        .Mode = GPIO_MODE_OUTPUT_OD,  // 开漏输出
        .Pull = GPIO_PULLUP,         // 配合外部上拉
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

void DHT11_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = DHT11_Pin,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP,  // 启用内部上拉
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

uint8_t DHT11_Read(u16 *temperature, u8 *humidity)
{
    uint8_t data[5] = {0};
    
    // 1. 启动通信（主机拉低18ms）
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    HAL_Delay(18);
    
    // 2. 主机释放总线（设为输入）
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    DHT11_Delay_us(40);
    DHT11_SetInput();
    
    // 3. 检测DHT11响应
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET);
    
    DHT11_Delay_us(80);
    
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET);
    
    // 4. 读取40位数据
    for(uint8_t i = 0; i < 40; i++) {
        // 等待低电平结束（50μs）
        while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET);
        
        // 等待高电平
        uint32_t start = DWT->CYCCNT;
        while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET);
        uint32_t duration = DWT->CYCCNT - start;
        
        // 确定位值（高电平>40μs表示1）
        data[i/8] <<= 1;
        if(duration > SystemCoreClock / 1000000 * 40) {
            data[i/8] |= 1;
        }
    }
		
		// 5. 数据转换
    *humidity = data[0];
    *temperature = data[2] * 10 + data[3];
    
    // 6. 校验和验证
    if(data[4] != (data[0] + data[1] + data[2] + data[3])) {
        return 0;
    }
    
    // 7. 总线状态恢复
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    
    return 1;
}
