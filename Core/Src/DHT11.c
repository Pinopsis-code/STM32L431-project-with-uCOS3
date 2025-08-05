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
        .Mode = GPIO_MODE_OUTPUT_OD,  // ��©���
        .Pull = GPIO_PULLUP,         // ����ⲿ����
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

void DHT11_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = DHT11_Pin,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP,  // �����ڲ�����
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

uint8_t DHT11_Read(u16 *temperature, u8 *humidity)
{
    uint8_t data[5] = {0};
    
    // 1. ����ͨ�ţ���������18ms��
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    HAL_Delay(18);
    
    // 2. �����ͷ����ߣ���Ϊ���룩
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    DHT11_Delay_us(40);
    DHT11_SetInput();
    
    // 3. ���DHT11��Ӧ
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET);
    
    DHT11_Delay_us(80);
    
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET);
    
    // 4. ��ȡ40λ����
    for(uint8_t i = 0; i < 40; i++) {
        // �ȴ��͵�ƽ������50��s��
        while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET);
        
        // �ȴ��ߵ�ƽ
        uint32_t start = DWT->CYCCNT;
        while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET);
        uint32_t duration = DWT->CYCCNT - start;
        
        // ȷ��λֵ���ߵ�ƽ>40��s��ʾ1��
        data[i/8] <<= 1;
        if(duration > SystemCoreClock / 1000000 * 40) {
            data[i/8] |= 1;
        }
    }
		
		// 5. ����ת��
    *humidity = data[0];
    *temperature = data[2] * 10 + data[3];
    
    // 6. У�����֤
    if(data[4] != (data[0] + data[1] + data[2] + data[3])) {
        return 0;
    }
    
    // 7. ����״̬�ָ�
    DHT11_SetOutput();
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    
    return 1;
}
