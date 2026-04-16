#ifndef __SET_H
#define __SET_H

#include "stm32f4xx_hal.h"

#define PW_LENGTH 6

typedef struct
{
    uint8_t data[PW_LENGTH];
    uint8_t head;            // 队头
    uint8_t tail;            // 队尾
    uint8_t count;           // 元素个数
} PasswordQueue_t;

extern PasswordQueue_t TurePW;
extern PasswordQueue_t TestPW;

void vQueue_Init(PasswordQueue_t *q);
uint8_t xQueue_IsEmpty(PasswordQueue_t *q);
uint8_t xQueue_IsFull(PasswordQueue_t *q);
void vQueue_Enqueue(PasswordQueue_t *q, uint8_t val);
uint8_t xQueue_Dequeue(PasswordQueue_t *q);
uint8_t xQueue_Compare(PasswordQueue_t *q1, PasswordQueue_t *q2);
uint8_t xQueue_GetItem(PasswordQueue_t *q, uint8_t index);

void vKey_Situation();
void vKey_Single();
void vKey_Double();
void vKey_Long();
void vOLED_Situation();
void vEncoder();

#endif