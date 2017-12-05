// David Wu, 21 Nov. 2017

#include "ddc.h"

bool
ddc_write(DDCWriteCommand *comm, display_info_t *display, char *err) {
    UInt8 data[kI2CBufferSize] = { 0 };
    data[0] = 0x51;
    data[1] = 0x84;
    data[2] = 0x03;
    data[3] = comm->control_id;
    data[4] = comm->new_value >> 8;
    data[5] = comm->new_value & 255;
    data[6] = 0x6E ^ data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5];
    IOI2CRequest request = {};
    request.commFlags = 0;
    request.sendAddress = 0x6E;
    request.sendTransactionType = kIOI2CSimpleTransactionType;
    request.sendBuffer = (vm_address_t)data;
    request.sendBytes = 7;
    request.replyTransactionType = kIOI2CNoTransactionType;
    request.replyBytes = 0;
    return i2c_request(&request, display, err);
}
