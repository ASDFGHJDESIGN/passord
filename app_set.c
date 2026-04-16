#include "app_set.h"
#include "OLED.h"
#include "bsp_key.h"
#include "bsp_encoder.h"

PasswordQueue_t TurePW;
PasswordQueue_t TestPW;

void vQueue_Init(PasswordQueue_t *q)
{
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    for(int i = 0; i < PW_LENGTH; i++)
    {
        q->data[i] = 0;
    }
}

uint8_t xQueue_IsEmpty(PasswordQueue_t *q)
{
    return (q->count == 0);
}

uint8_t xQueue_IsFull(PasswordQueue_t *q)
{
    return (q->count == PW_LENGTH);
}

void vQueue_Enqueue(PasswordQueue_t *q, uint8_t val)
{
    if (xQueue_IsFull(q))
    {
        xQueue_Dequeue(q);
    }
    q->data[q->tail] = val;
    q->tail = (q->tail + 1) % PW_LENGTH;
    q->count++;
}

uint8_t xQueue_Dequeue(PasswordQueue_t *q)
{
    if (xQueue_IsEmpty(q))
    {
        return 0;
    }
    uint8_t val = q->data[q->head];
    q->head = (q->head + 1) % PW_LENGTH;
    q->count--;
    return val;
}

uint8_t xQueue_Compare(PasswordQueue_t *q1, PasswordQueue_t *q2)
{
    if (q1->count != PW_LENGTH || q2->count != PW_LENGTH)
    {
        return 0;
    }
    uint8_t n1 = q1->head;
    uint8_t n2 = q2->head;
    for (int i = 0; i < PW_LENGTH; i++)
    {
        if (q1->data[n1] != q2->data[n2])
        {
            return 0;
        }
        n1 = (n1 + 1) % PW_LENGTH;
        n2 = (n2 + 1) % PW_LENGTH;
    }
    return 1;
}

uint8_t xQueue_GetItem(PasswordQueue_t *q, uint8_t i)
{
    if (i >= q->count) return 0;
    uint8_t n = (q->head + i) % PW_LENGTH;
    return q->data[n];
}



int num;
uint8_t state;
uint8_t Update_Flag = 1;

void vKey_Situation()
{
    if (Key_Check(KEY_1, KEY_SINGLE))
    {
        vKey_Single();
    }

    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
        vKey_Double();
    }

    if (Key_Check(KEY_1, KEY_LONG))
    {
        vKey_Long();
    }
}

void vKey_Single()
{
    if (state == 0)
    {
        vQueue_Enqueue(&TurePW, num);
        if(xQueue_IsFull(&TurePW))
        {
            state = 1;
            num = 0;
            OLED_Clear();
            OLED_ShowString(2,1,"GUESSING");
        }
    }
    else if (state == 1)
    {
        vQueue_Enqueue(&TestPW, num);
    }
    Update_Flag = 1;
}

void vKey_Double()
{
    if (state == 1)
    {
        if (xQueue_Compare(&TurePW, &TestPW))
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            OLED_ShowString(3,1,"VICTORY ");
        }
        else
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
            OLED_ShowString(3,1,"WRONG!  ");
            vQueue_Init(&TestPW);
        }
    }
    Update_Flag = 1;
}

void vKey_Long()
{
    if (state == 1)
    {
        vQueue_Init(&TestPW);
        uint32_t n,m;
        n = xQueue_GetItem(&TestPW, 1)*100000 + xQueue_GetItem(&TestPW, 2)*10000 + xQueue_GetItem(&TestPW, 3)*1000 + xQueue_GetItem(&TestPW, 4)*100 + xQueue_GetItem(&TestPW, 5)*10 + xQueue_GetItem(&TestPW, 6);
        m = xQueue_GetItem(&TurePW, 1)*100000 + xQueue_GetItem(&TurePW, 2)*10000 + xQueue_GetItem(&TurePW, 3)*1000 + xQueue_GetItem(&TurePW, 4)*100 + xQueue_GetItem(&TurePW, 5)*10 + xQueue_GetItem(&TurePW, 6);
        while (!(n==m))
        {
            n++;
        }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        OLED_ShowString(3,1,"VICTORY ");
    }
}

void vOLED_Situation()
{
    if (Update_Flag == 1)
    {
        Update_Flag = 0;
        if (state == 0)
        {
            for(int i = 0; i < PW_LENGTH; i++)
            {
                uint8_t position = 1 + i * 2;

                if (i < TurePW.count)
                {
                    uint8_t val = xQueue_GetItem(&TurePW, i);
                    OLED_ShowSignedNum(4, position, val, 1);
                }
                else if (i == TurePW.count)
                {
                    OLED_ShowSignedNum(4, position, num, 1);
                }
                else
                {
                    OLED_ShowString(4, position, "-");
                }
            }
        }
        else if (state == 1)
        {
            for(int i = 0; i < PW_LENGTH; i++)
            {
                uint8_t col = 1 + i * 2;

                if (i < TestPW.count)
                {
                    uint8_t val = xQueue_GetItem(&TestPW, i);
                    OLED_ShowSignedNum(4, col, val, 1);
                }
                else if (i == TestPW.count)
                {
                    OLED_ShowSignedNum(4, col, num, 1);
                }
                else
                {
                    OLED_ShowString(4, col, "-");
                }
            }
        }

    }
}

void vEncoder()
{
    int16_t diff = Encoder_Get();
    if(diff != 0)
    {
        num += (diff > 0) ? 1 : -1;
        if (num > 9) num = 0;
        if (num < 0) num = 9;

        Update_Flag = 1;
    }
}