// David Wu, 21 Nov. 2017

#ifndef DDC_H
#define DDC_H

#include "common.h"

#define kDDCBrightness 0x10

typedef struct {
    UInt8 control_id;
    UInt8 new_value;
} DDCWriteCommand;

typedef struct {
    UInt8 control_id;
    bool  success;
    UInt8 max_value;
    UInt8 current_value;
} DDCReadCommand;

bool ddc_write(DDCWriteCommand *comm, display_info_t *display, char *err);

#endif
