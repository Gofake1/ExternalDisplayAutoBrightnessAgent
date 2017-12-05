// David Wu, 28 Nov. 2017

#include "common.h"

bool
i2c_request(IOI2CRequest *request, display_info_t *display, char *err) {
    dispatch_semaphore_wait(display->queue, DISPATCH_TIME_FOREVER);
    IOItemCount nbuses;
    IOReturn ret = IOFBGetI2CInterfaceCount(display->framebuffer, &nbuses);
    if (ret != kIOReturnSuccess) {
        sprintf(err, "IOFBGetI2CInterfaceCount: %d", ret);
        dispatch_semaphore_signal(display->queue);
        return false;
    }
    if (nbuses < 1) {
        sprintf(err, "No I2C buses found for framebuffer");
        dispatch_semaphore_signal(display->queue);
        return false;
    }
    bool sent = false;
    for (IOOptionBits i = 0; i < nbuses; i++) {
        io_service_t interface;
        ret = IOFBCopyI2CInterfaceForBus(display->framebuffer, i, &interface);
        if (ret != kIOReturnSuccess) {
            continue;
        }
        IOI2CConnectRef connect;
        ret = IOI2CInterfaceOpen(interface, kNilOptions, &connect);
        IOObjectRelease(interface);
        if (ret != kIOReturnSuccess) {
            continue;
        }
        ret = IOI2CSendRequest(connect, kNilOptions, request);
        IOI2CInterfaceClose(connect, kNilOptions);
        if (ret != kIOReturnSuccess) {
            continue;
        }
        sent = true;
        break;
    }
    if (!sent) {
        strcpy(err, "All buses failed to send request");
        dispatch_semaphore_signal(display->queue);
        return false;
    }
    if (request->replyTransactionType == kIOI2CNoTransactionType) {
        usleep(20000);
    }
    bool request_successful = request->result == kIOReturnSuccess;
    if (!request_successful) {
        sprintf(err, "I2C request result: %d", request->result);
    }
    dispatch_semaphore_signal(display->queue);
    return request_successful;
}
