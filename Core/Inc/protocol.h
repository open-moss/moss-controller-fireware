#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "stm32f1xx_hal.h"

#define DATA_PACKET_HEAD_HIGH 0xEB  //数据帧头高位
#define DATA_PACKET_HEAD_LOW 0x90  //数据帧头低位
#define DATA_PACKET_MAX_SIZE 128  //数据包最大大小
#define DATA_PACKET_MIN_SIZE 12  //数据包最小大小
#define DATA_PACKET_HEAD_SIZE 7  //数据头大小
#define DATA_PACKET_BODY_MAX_SIZE DATA_PACKET_MAX_SIZE - 9  //数据包主体大小
#define DATA_PACKRT_SIGN_SIZE 4  //数据包签名大小
#define DATA_PACKET_EOF 0x0D  //数据包结束符

typedef enum {
    DATA_PACKET_HEARTBEAT = 0  //心跳
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
void Protocol_PrintDataPacket(DataPacket* const pdata);
void Protocol_DataPacketSign(DataPacket* const pdata);
uint8_t* Protocol_DataPacketToBuffer(DataPacket* const pdata);
DataPacket* Protocol_BufferToDataPacket(uint8_t* const buffer);
void Protocol_FreeBuffer(uint8_t* const buffer);
void Protocol_FreeDataPacket(DataPacket* pdata);

#endif
