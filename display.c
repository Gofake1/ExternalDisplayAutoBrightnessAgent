// David Wu, 21 Nov. 2017

#include "display.h"

static const CFStringRef kVendor = CFSTR(kDisplayVendorID);
static const CFStringRef kModel  = CFSTR(kDisplayProductID);
static const CFStringRef kSerial = CFSTR(kDisplaySerialNumber);

bool cfnumber_equals_uint32(CFNumberRef, uint32_t);

bool
display_info_create(CGDirectDisplayID id, display_info_t *out, char *err) {
    display_info_t display_info = { id, kDisplayUnsupported, 0,
        dispatch_semaphore_create(1) };

    io_iterator_t iter;
    kern_return_t ret = IOServiceGetMatchingServices(kIOMasterPortDefault,
        IOServiceMatching(IOFRAMEBUFFER_CONFORMSTO), &iter);
    if (ret != KERN_SUCCESS) {
        sprintf(err, "IOServiceGetMatchingServices: %d", ret);
        return false;
    }
    uint32_t vendor = CGDisplayVendorNumber(id);
    uint32_t model  = CGDisplayModelNumber(id);
    uint32_t serial = CGDisplaySerialNumber(id);
    io_service_t service;
    while ((service = IOIteratorNext(iter)) != 0) {
        CFDictionaryRef info = IODisplayCreateInfoDictionary(service,
            kIODisplayNoProductName);
        CFNumberRef cfvendor = CFDictionaryGetValue(info, kVendor);
        CFNumberRef cfmodel  = CFDictionaryGetValue(info, kModel);
        CFNumberRef cfserial = CFDictionaryGetValue(info, kSerial);
        CFRelease(info);
        if (cfnumber_equals_uint32(cfvendor, vendor) &&
            cfnumber_equals_uint32(cfmodel, model) &&
            cfnumber_equals_uint32(cfserial, serial))
        {
            display_info.framebuffer = service;
            // Test for native support
            float brightness;
            IOReturn ret = IODisplayGetFloatParameter(service, kNilOptions,
                kDisplayBrightness, &brightness);
            if (ret == kIOReturnSuccess) {
                display_info.io_kind = kDisplayNative;
                break;
            }
            // Test for DDC support
            EDID edid;
            char i2c_err[kErrorStringSize];
            bool read = edid_read(&edid, &display_info, i2c_err);
            if (read) {
                display_info.io_kind = kDisplayDDC;
                break;
            }
        }
        IOObjectRelease(service);
    }
    IOObjectRelease(iter);

    *out = display_info;
    return true;
}

void
display_info_release(display_info_t *display) {
    IOObjectRelease(display->framebuffer);
    dispatch_release(display->queue);
}

void
display_info_description(display_info_t *display, char *out, char *extra) {
    char display_io_str[16];
    switch (display->io_kind) {
    case kDisplayUnsupported:
        strcpy(display_io_str, "Unsupported");
        break;
    case kDisplayNative:
        strcpy(display_io_str, "Native");
        break;
    case kDisplayDDC:
        strcpy(display_io_str, "DDC");
        break;
    }
    if (extra != NULL && extra[0] != 0) {
        sprintf(out, "id: %d,\tio: %s,\tbus: %d,\textra: %s", display->id,
            display_io_str, display->framebuffer, extra);
    } else {
        sprintf(out, "id: %d,\tio: %s,\tbus: %d", display->id, display_io_str,
            display->framebuffer);
    }
}

bool
edid_read(EDID *out, display_info_t *display, char *err) {
    if (!out) {
        strcpy(err, "`edid` must be non-null");
        return false;
    }
    UInt8 data[kI2CBufferSize] = { 0 };
    IOI2CRequest request = {};
    request.sendAddress = 0xA0;
    request.sendTransactionType = kIOI2CSimpleTransactionType;
    request.sendBuffer = (vm_address_t)data;
    request.sendBytes = 1;
    request.replyAddress = 0xA1;
    request.replyTransactionType = kIOI2CSimpleTransactionType;
    request.replyBuffer = (vm_address_t)data;
    request.replyBytes = kI2CBufferSize;
    if (!i2c_request(&request, display, err)) {
        return false;
    }
    memcpy(out, &data, kI2CBufferSize);
    UInt8 sum = 0;
    for (size_t i = 0; i < request.replyBytes; i++) {
        sum += data[i];
    }
    return !sum;
}

// --- display_info_create ---

bool
cfnumber_equals_uint32(CFNumberRef cfnumber, uint32_t uint32) {
    if (cfnumber == NULL) {
        return uint32 == 0;
    }
    int64_t int64;
    if (!CFNumberGetValue(cfnumber, kCFNumberSInt64Type, &int64)) {
        return false;
    }
    return int64 == uint32;
}
