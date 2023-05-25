#include "stdio.h"
#include "string.h"

#include "stm32f1xx_hal.h"

#include "common.h"
#include "config.h"
#include "protocol.h"
#include "crc32.h"

/**
 * 上位机串口通信协议
 * 
 * [0] -> 数据包类型
 * [1] -> 数据包总长度 高8位
 * [2] -> 数据包总长度 低8位
 * [3] -> 数据包ID 高8位
 * [4] -> 数据包ID 低8位
 * [5-(N-8)] -> 数据主体载荷
 * [(N-8)-(N-1)] -> 数据包4位签名
 * [N] -> 0x0D 回车结束符
 */

uint16_t dataPacketId = 1;  //当前数据包ID

uint16_t Protocol_GenerateDataPacketID(void) {
    if(dataPacketId > 65535)
        dataPacketId = 1;
    return dataPacketId++;
}

DataPacket* Protocol_BuildDataPacket(DataPacketType type, uint8_t* body, uint16_t bodySize) {
    DataPacket* pData = pvPortMalloc(sizeof(DataPacket));
    memset(pData, 0, sizeof(DataPacket));
    pData->type = type;
    pData->size = bodySize + DATA_PACKET_MIN_SIZE;
    pData->id = Protocol_GenerateDataPacketID();
    pData->body = pvPortMalloc(sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memset(pData->body, 0, sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memcpy(pData->body, body, bodySize);
    pData->sign = pvPortMalloc(sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    memset(pData->sign, 0, sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    Protocol_DataPacketSign(pData);
    return pData;
}

void Protocol_DataPacketSign(DataPacket* const pData) {
    uint8_t* buffer = Protocol_DataPacketToBuffer(pData);
    uint32_t crc32 = CRC32(buffer, pData->size + sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);  //CRC32值
    vPortFree(buffer);
    buffer = NULL;
    pData->sign[0] = (crc32 >> (3 * 8)) & 0xFF;
    pData->sign[1] = (crc32 >> (2 * 8)) & 0xFF;
    pData->sign[2] = (crc32 >> (1 * 8)) & 0xFF;
    pData->sign[3] = crc32 & 0xFF;
}

uint8_t* Protocol_DataPacketToBuffer(DataPacket* const pData) {
    uint8_t* buffer = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * DATA_PACKET_MAX_SIZE);
    buffer[0] = pData->type;  //数据包类型
    buffer[1] = extractUint8High(pData->size);  //数据包大小高8位
    buffer[2] = extractUint8Low(pData->size);  //数据包大小低8位
    buffer[3] = extractUint8High(pData->id);  //数据包ID高8位
    buffer[4] = extractUint8Low(pData->id);  //数据包ID低8位
    uint16_t bodySize = pData->size - DATA_PACKET_MIN_SIZE;  //数据包主体大小
    memcpy(buffer + DATA_PACKET_HEAD_SIZE, pData->body, bodySize);  //数据包主体数据
    buffer[pData->size - 1] = DATA_PACKET_EOF;  //数据包结尾
    return buffer;
}

DataPacket* Protocol_BufferToDataPacket(uint8_t* const buffer) {
    DataPacket* pData = pvPortMalloc(sizeof(DataPacket));  //分配数据包内存
    memset(pData, 0, sizeof(DataPacket));
    pData->type = (DataPacketType)buffer[0];  //数据包类型
    pData->size = mergeToUint16(buffer[1], buffer[2]);  //数据包大小
    pData->id = mergeToUint16(buffer[3], buffer[4]);  //数据包ID
    pData->body = pvPortMalloc(sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memset(pData->body, 0, sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memcpy(pData->body, buffer + DATA_PACKET_HEAD_SIZE, pData->size - DATA_PACKET_MIN_SIZE);  //拷贝数据包主体数据
    pData->sign = pvPortMalloc(sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    memset(pData->sign, 0, sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    memcpy(pData->sign, buffer + (pData->size - DATA_PACKET_HEAD_SIZE), DATA_PACKRT_SIGN_SIZE);  //拷贝签名数据
    return pData;
}

void Protocol_FreeDataPacket(DataPacket* pData) {
    vPortFree(pData->body);
    pData->body = NULL;
    vPortFree(pData->sign);
    pData->sign = NULL;
    vPortFree(pData);
    pData = NULL;
}
