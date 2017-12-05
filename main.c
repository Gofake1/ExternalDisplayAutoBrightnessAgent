// David Wu, 1 Nov. 2017

// * assumes only one builtin display
// * assumes builtin display uses native IO

#include "ddc.h"
#include "display.h"

#define kExternalDisplayListMaxSize 10
#define kTimerRate (CFTimeInterval)30 // seconds

// TODO: fix possible race condition when display_reconfiguration_callback 
// writes while timer_callback reads

display_info_t _builtin_display;
display_info_t _external_displays[kExternalDisplayListMaxSize];
char _display_strs[kExternalDisplayListMaxSize+1][kDescriptionStringSize];

void display_reconfiguration_callback(CGDirectDisplayID,
    CGDisplayChangeSummaryFlags, void *);
int  initialize_display_info();
void timer_callback();

int
main(int argc, const char *argv[]) {
    int n = initialize_display_info();
    if (n < 0) {
        printf("Error: initialize_display_info\n"
            "\t%s\n", _display_strs[0]);
    } else {
        char display_list_str[kDescriptionStringSize * 
            (kExternalDisplayListMaxSize+1)] = "";
        for (size_t i = 0; i < n; i++) {
            char display_list_item_str[kDescriptionStringSize];
            sprintf(display_list_item_str, "\t%zu: %s\n", i, _display_strs[i]);
            strcat(display_list_str, display_list_item_str);
        }
        printf("Info: initialize_display_info\n"
            "\t%d displays found\n"
            "%s", n, display_list_str);
    }
    CGDisplayRegisterReconfigurationCallback(display_reconfiguration_callback,
        NULL);
    void (^callback)(CFRunLoopTimerRef) = ^(CFRunLoopTimerRef _){
        timer_callback(); };
    CFRunLoopTimerRef timer = CFRunLoopTimerCreateWithHandler(NULL,
        CFAbsoluteTimeGetCurrent(), kTimerRate, 0, 0, callback);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopDefaultMode);
    CFRunLoopRun();
    return 0;
}

// --- main ---

void builtin_display_added(CGDirectDisplayID);
void builtin_display_removed(CGDirectDisplayID);
void external_display_added(CGDirectDisplayID);
void external_display_removed(CGDirectDisplayID);

void 
display_reconfiguration_callback(
    CGDirectDisplayID id,
    CGDisplayChangeSummaryFlags flags,
    void *user_info)
{
    if (flags & kCGDisplayAddFlag) {
        if (CGDisplayIsBuiltin(id)) {
            builtin_display_added(id);
        } else {
            external_display_added(id);
        }
    } else if (flags & kCGDisplayRemoveFlag) {
        if (CGDisplayIsBuiltin(id)) {
            builtin_display_removed(id);
        } else {
            external_display_removed(id);
        }
    }
}

// Returns number of displays, or -1 if error occured
int
initialize_display_info() {
    int display_list_max_size = kExternalDisplayListMaxSize+1;
    CGDirectDisplayID display_ids[display_list_max_size];
    uint32_t ndisplays;
    CGDisplayErr err = CGGetOnlineDisplayList((uint32_t)display_list_max_size,
        display_ids, &ndisplays);
    if (err != CGDisplayNoErr) {
        sprintf(_display_strs[0], "CGGetOnlineDisplayList: %d", err);
        return -1;
    }
    size_t iexternal = 0;
    for (size_t i = 0; i < ndisplays; i++) {
        display_info_t *display;
        if (CGDisplayIsBuiltin(display_ids[i])) {
            display = &_builtin_display;
        } else {
            display = &_external_displays[iexternal++];
        }
        char err[kErrorStringSize] = "";
        if (display_info_create(display_ids[i], display, err)) {
            char description[kDescriptionStringSize];
            display_info_description(display, description, err);
            strcpy(_display_strs[i], description);
        } else {
            sprintf(_display_strs[i], "id: %d\terror: %s", display->id, err);
        }
    }
    for (size_t i = iexternal; i < kExternalDisplayListMaxSize; i++) {
        _external_displays[i] = kDisplayInfoEmpty;
    }
    return ndisplays;
}

bool read_builtin_display_brightness(float *, char *);
int  write_external_displays_brightness(float);

void
timer_callback() {
    if (_builtin_display.id == -1) {
        return;
    }
    float builtin_display_brightness;
    char err[kErrorStringSize];
    if (!read_builtin_display_brightness(&builtin_display_brightness, err)) {
        printf("Error: read_builtin_display_brightness\n"
            "\t%s\n", err);
        return;
    }
    int n = write_external_displays_brightness(builtin_display_brightness);
    if (n > 0) {
        puts("Error: write_external_displays_brightness");
        for (int i = 0; i < n; i++) {
            if (_display_strs[0] == 0) {
                continue;
            }
            printf("\t%s\n", _display_strs[i]);
        }
    }
}

// --- display_reconfiguration_callback ---

void
builtin_display_added(CGDirectDisplayID display_id) {
    char err[kErrorStringSize];
    if (display_info_create(display_id, &_builtin_display, err)) {
        char description[kDescriptionStringSize];
        display_info_description(&_builtin_display, description, NULL);
        printf("Info: builtin_display_added\n"
            "\t%s\n", description);
    } else {
        printf("Error: builtin_display_added\n"
            "\t%s\n", err);
    }
}

void
builtin_display_removed(CGDirectDisplayID id) {
    if (id != _builtin_display.id) {
        printf("Warning: builtin_display_removed\n"
            "\tUnknown display with id %d\n", id);
        return;
    }
    char description[kDescriptionStringSize];
    display_info_description(&_builtin_display, description, NULL);
    display_info_release(&_builtin_display);
    _builtin_display = kDisplayInfoEmpty;
    printf("Info: builtin_display_removed\n"
        "\t%s\n", description);
}

void
external_display_added(CGDirectDisplayID display_id) {
    for (size_t i = 0; i < kExternalDisplayListMaxSize; i++) {
        if (_external_displays[i].id != -1) {
            continue;
        }
        char err[kErrorStringSize];
        if (display_info_create(display_id, &_external_displays[i], err)) {
            char description[kDescriptionStringSize];
            display_info_description(&_external_displays[i], description,
                NULL);
            printf("Info: external_display_added\n"
                "\t%s\n", description);
        } else {
            printf("Error: external_display_added\n"
                "\t%s\n", err);
        }
        return;
    }
}

void
external_display_removed(CGDirectDisplayID display_id) {
    for (size_t i = 0; i < kExternalDisplayListMaxSize; i++) {
        if (_external_displays[i].id != display_id) {
            continue;
        }
        char description[kDescriptionStringSize]; 
        display_info_description(&_external_displays[i], description, NULL);
        display_info_release(&_external_displays[i]);
        _external_displays[i] = kDisplayInfoEmpty;
        printf("Info: external_display_removed\n"
            "\t%s\n", description);
        return;
    }
    printf("Warning: external_display_removed\n"
        "\tUnknown display with id %d\n", display_id);
}

// --- timer_callback ---

bool
read_builtin_display_brightness(float *brightness, char *err) {
    IOReturn ret = IODisplayGetFloatParameter(_builtin_display.framebuffer,
        kNilOptions, kDisplayBrightness, brightness);
    if (ret != kIOReturnSuccess) {
        sprintf(err, "IODisplayGetFloatParameter: %d", ret);
        return false;
    }
    return true;
}

bool write_brightness_ddc(display_info_t *, float, char *);
bool write_brightness_native(io_service_t, float, char *);

// Returns number of failed writes
int
write_external_displays_brightness(float brightness) {
    int nfailed = 0;
    for (size_t i = 0; i < kExternalDisplayListMaxSize; i++) {
        display_info_t display = _external_displays[i];
        if (display.id == -1) {
            continue;
        }
        char err[kErrorStringSize];
        switch (display.io_kind) {
        case kDisplayDDC:
            if (!write_brightness_ddc(&display, brightness, err)) {
                sprintf(_display_strs[nfailed++], "%d, DDC, %s", display.id,
                    err);
            }
            break;
        case kDisplayNative:
            if (!write_brightness_native(display.framebuffer, brightness,
                err)) {
                sprintf(_display_strs[nfailed++], "%d, native, %s", display.id,
                    err);
            }
            break;
        case kDisplayUnsupported:
            break;
        }
    }
    return nfailed;
}

// --- write_external_displays_brightness ---

bool
write_brightness_ddc(display_info_t *display, float brightness, char *err) {
    DDCWriteCommand comm = {};
    comm.control_id = kDDCBrightness;
    comm.new_value = (uint)roundf(brightness * 100);
    return ddc_write(&comm, display, err);
}

bool
write_brightness_native(io_service_t service, float brightness, char *err) {
    IOReturn ret = IODisplaySetFloatParameter(service, kNilOptions,
        kDisplayBrightness, brightness);
    if (ret != kIOReturnSuccess) {
        sprintf(err, "IODisplaySetFloatParameter: %d", ret);
        return false;
    }
    return true;
}
