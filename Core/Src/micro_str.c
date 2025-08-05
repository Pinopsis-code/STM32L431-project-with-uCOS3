#include "micro_str.h"

u16 m_strlen(const char *str) {
    u16 len = 0;
    while (*str++) len++;
    return len;
}

void m_memset(void *ptr, u8 value, u16 size) {
    u8 *p = (u8*)ptr;
    for (u16 i = 0; i < size; i++) {
        *p++ = value;
    }
}
