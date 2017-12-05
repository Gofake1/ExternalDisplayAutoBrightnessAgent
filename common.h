// David Wu, 28 Nov. 2017

#ifndef COMMON_H
#define COMMON_H

#include <CoreGraphics/CoreGraphics.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <IOKit/i2c/IOI2CInterface.h>

#define kI2CBufferSize 128
#define kErrorStringSize 128
#define kDescriptionStringSize 256

typedef enum {
    kDisplayUnsupported,
    kDisplayNative,
    kDisplayDDC
} display_io_t;

typedef struct {
    CGDirectDisplayID       id;
    display_io_t            io_kind;
    io_service_t            framebuffer;
    dispatch_semaphore_t    queue;
} display_info_t;

bool i2c_request(IOI2CRequest *request, display_info_t *display, char *err);

#endif
