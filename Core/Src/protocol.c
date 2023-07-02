#include "stdio.h"
#include "string.h"

#include "stm32f1xx_hal.h"

#include "common.h"
#include "config.h"
#include "protocol.h"
#include "logger.h"
#include "crc32.h"

/**
 * 上位机串口通信协议
 * 
 * [0-1] -> 数据帧头 EB90
 * [2] -> 数据包类型
 * [3] -> 数据包总长度 高8位
 * [4] -> 数据包总长度 低8位
 * [5] -> 数据包ID 高8位
 * [6] -> 数据包ID 低8位
 * [7-(N-8)] -> 数据主体载荷
 * [(N-8)-(N-1)] -> 数据包4位签名
 * [N] -> 0x0D 回车结束符
 */

uint16_t Protocol_GenerateDataPacketID(void) {
    static uint16_t dataPacketId = 1;
    if(dataPacketId > 65535)
        dataPacketId = 1;
    return dataPacketId++;
}

DataPacket* Protocol_BuildDataPacket(DataPacketType type, uint8_t* body, uint16_t bodySize) {
    DataPacket* pdata = pvPortMalloc(sizeof(DataPacket));
    memset(pdata, 0, sizeof(DataPacket));
    pdata->type = type;
    pdata->size = bodySize + DATA_PACKET_MIN_SIZE;
    pdata->id = Protocol_GenerateDataPacketID();
    pdata->body = pvPortMalloc(sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memset(pdata->body, 0, sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memcpy(pdata->body, body, bodySize);
    pdata->sign = pvPortMalloc(sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    memset(pdata->sign, 0, sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    Protocol_DataPacketSign(pdata);
    return pdata;
}

void Protocol_DataPacketSign(DataPacket* const pdata) {
    uint8_t* buffer = Protocol_DataPacketToBuffer(pdata);
    uint32_t crc32 = CRC32(buffer, pdata->size + sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);  //CRC32值
    vPortFree(buffer);
    buffer = NULL;
    pdata->sign[0] = (crc32 >> (3 * 8)) & 0xFF;
    pdata->sign[1] = (crc32 >> (2 * 8)) & 0xFF;
    pdata->sign[2] = (crc32 >> (1 * 8)) & 0xFF;
    pdata->sign[3] = crc32 & 0xFF;
}

uint8_t* Protocol_DataPacketToBuffer(DataPacket* const pdata) {
    static uint8_t head[] = DATA_PACKET_HEAD;
    uint8_t* buffer = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * DATA_PACKET_MAX_SIZE);
    buffer[0] = head[0];
    buffer[1] = head[1];
    buffer[2] = pdata->type;  //数据包类型
    buffer[3] = ExtractUint8High(pdata->size);  //数据包大小高8位
    buffer[4] = ExtractUint8Low(pdata->size);  //数据包大小低8位
    buffer[5] = ExtractUint8High(pdata->id);  //数据包ID高8位
    buffer[6] = ExtractUint8Low(pdata->id);  //数据包ID低8位
    uint16_t bodySize = pdata->size - DATA_PACKET_MIN_SIZE;  //数据包主体大小
    memcpy(buffer + DATA_PACKET_HEAD_SIZE, pdata->body, bodySize);  //数据包主体数据
    buffer[pdata->size - 1] = DATA_PACKET_EOF;  //数据包结尾
    return buffer;
}

DataPacket* Protocol_BufferToDataPacket(uint8_t* const buffer) {
    DataPacket *pdata = pvPortMalloc(sizeof(DataPacket));  //分配数据包内存
    memset(pdata, 0, sizeof(DataPacket));
    pdata->type = (DataPacketType)buffer[2];  //数据包类型
    pdata->size = MergeToUint16(buffer[3], buffer[4]);  //数据包大小
    pdata->id = MergeToUint16(buffer[5], buffer[6]);  //数据包ID
    pdata->body = pvPortMalloc(sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memset(pdata->body, 0, sizeof(uint8_t) * DATA_PACKET_BODY_MAX_SIZE);
    memcpy(pdata->body, buffer + DATA_PACKET_HEAD_SIZE, pdata->size - DATA_PACKET_MIN_SIZE);  //拷贝数据包主体数据
    pdata->sign = pvPortMalloc(sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    memset(pdata->sign, 0, sizeof(uint8_t) * DATA_PACKRT_SIGN_SIZE);
    memcpy(pdata->sign, buffer + (pdata->size - 5), DATA_PACKRT_SIGN_SIZE);  //拷贝签名数据
    return pdata;
}

void Protocol_PrintDataPacket(DataPacket* const pdata) {
    uint16_t bodySize = pdata->size - DATA_PACKET_MIN_SIZE;
    printf("\nType: %d\nSize: %d\nID: %d\nBodySize: %d\nBody: ", pdata->type, pdata->size, pdata->id, bodySize);
    PrintRawHex(pdata->body, bodySize);
    printf("Sign: ");
    PrintRawHex(pdata->sign, DATA_PACKRT_SIGN_SIZE);
}

void Protocol_FreeBuffer(uint8_t* const buffer) {
    vPortFree(buffer);
}

void Protocol_FreeDataPacket(DataPacket* pdata) {
    if(pdata == NULL) return;
    vPortFree(pdata->body);
    pdata->body = NULL;
    vPortFree(pdata->sign);
    pdata->sign = NULL;
    vPortFree(pdata);
    pdata = NULL;
}
