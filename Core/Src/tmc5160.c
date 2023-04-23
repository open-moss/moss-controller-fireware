#include <stdlib.h>
#include "common.h"
#include "tmc5160.h"

MOTOR* new_MOTOR(GPIO_TypeDef* cs_gpio, uint16_t cs_pin) {
    MOTOR* pDriver = NULL;
    pDriver = (MOTOR*)malloc(sizeof(MOTOR));
    if(pDriver == NULL)
        return NULL;
    pDriver->pDriver = pDriver;
    pDriver->cs_gpio = cs_gpio;
    pDriver->cs_pin = cs_pin;
    pDriver->test = test;
    pDriver->delete = delete_MOTOR;
    return pDriver;
}

void delete_MOTOR(MOTOR* const pInstance) {
    free(pInstance);
}

void test(void) {

}
