#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "stm32f1xx_hal.h"

#define DATA_PACKET_MAX_SIZE 128  //数据包最大大小
#define DATA_PACKET_MIN_SIZE 10  //数据包最小大小
#define DATA_PACKET_HEAD_SIZE 5  //数据包头大小
#define DATA_PACKET_BODY_MAX_SIZE DATA_PACKET_MAX_SIZE - 9  //数据包主体大小
#define DATA_PACKRT_SIGN_SIZE 4  //数据包签名大小
#define DATA_PACKET_EOF 0x0D  //数据包结束符

typedef enum {
    Heartbeat  //心跳
} DataPacketType;

typedef struct {
    DataPacketType type;  //数据包类型
    uint16_t size;  //数据包总大小
    uint16_t id;  //数据包ID
    uint8_t *body;  //数据包主体
    uint8_t *sign;  //数据包签名
} DataPacket;  //数据包结构体

uint16_t Protocol_GenerateDataPacketID(void);
DataPacket* Protocol_BuildDataPacket(DataPacketType type, uint8_t *body, uint16_t bodySize);
void Protocol_DataPacketSign(DataPacket* const pData);
uint8_t* Protocol_DataPacketToBuffer(DataPacket* const pData);
DataPacket* Protocol_BufferToDataPacket(uint8_t* const buffer);
void Protocol_FreeDataPacket(DataPacket* pData);

#endif