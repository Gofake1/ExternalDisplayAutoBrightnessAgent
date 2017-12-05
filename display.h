// David Wu, 21 Nov. 2017

#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.h"

static const CFStringRef kDisplayBrightness = CFSTR(kIODisplayBrightnessKey);
static const display_info_t kDisplayInfoEmpty = { -1, kDisplayUnsupported, 0,
    NULL };

typedef struct {
    UInt8 xresolution;
    UInt8 ratio :2;
    UInt8 vertical :6;
} Timing;

// E-EDID
typedef struct {
    UInt64  header;
    UInt8   :1;
    UInt16  eisaid :15;
    UInt16  productcode;
    UInt32  serial;
    UInt8   week;
    UInt8   year;
    UInt8   versionmajor;
    UInt8   versionminor;
    union videoinput {
        struct digitalinput {
            UInt8 type :1;
            UInt8 :6;
            UInt8 dfp :1;
        } digital;
        struct analoginput {
            UInt8 type :1;
            UInt8 synclevels :2;
            UInt8 pedestal :1;
            UInt8 separate :1;
            UInt8 composite :1;
            UInt8 green :1;
            UInt8 serrated :1;
        } analog;
    } videoinput;
    UInt8   maxh;
    UInt8   maxv;
    UInt8   gamma;
    UInt8   standby :1;
    UInt8   suspend :1;
    UInt8   activeoff :1;
    UInt8   displaytype:2;
    UInt8   srgb :1;
    UInt8   preferredtiming :1;
    UInt8   gtf :1;
    UInt8   redxlsb :2;
    UInt8   redylsb :2;
    UInt8   greenxlsb :2;
    UInt8   greenylsb :2;
    UInt8   bluexlsb :2;
    UInt8   blueylsb :2;
    UInt8   whitexlsb :2;
    UInt8   whiteylsb :2;
    UInt8   redxmsb;
    UInt8   redymsb;
    UInt8   greenxmsb;
    UInt8   greenymsb;
    UInt8   bluexmsb;
    UInt8   blueymsb;
    UInt8   whitexmsb;
    UInt8   whiteymsb;
    UInt8   t720x400a70 :1;
    UInt8   t720x400a88 :1;
    UInt8   t640x480a60 :1;
    UInt8   t640x480a67 :1;
    UInt8   t640x480a72 :1;
    UInt8   t640x480a75 :1;
    UInt8   t800x600a56 :1;
    UInt8   t800x600a60 :1;
    UInt8   t800x600a72 :1;
    UInt8   t800x600a75 :1;
    UInt8   t832x624a75 :1;
    UInt8   t1024x768a87 :1;
    UInt8   t1024x768a60 :1;
    UInt8   t1024x768a72 :1;
    UInt8   t1024x768a75 :1;
    UInt8   t1280x1024a75 :1;
    UInt8   t1152x870a75 :1;
    UInt8   othermodes :7;
    Timing  timing1;
    Timing  timing2;
    Timing  timing3;
    Timing  timing4;
    Timing  timing5;
    Timing  timing6;
    Timing  timing7;
    Timing  timing8;
    union descriptor {
        struct __attribute__ ((packed)) timingdetail {
            UInt16  clock;
            UInt8   hactivelsb;
            UInt8   hblankinglsb;
            UInt8   hactivemsb :4;
            UInt8   hblankingmsb :4;
            UInt8   vactivelsb;
            UInt8   vblankinglsb;
            UInt8   vactivemsb :4;
            UInt8   vblankingmsb :4;
            UInt8   hsyncoffsetlsb;
            UInt8   hsyncpulselsb;
            UInt8   vsyncoffsetlsb :4;
            UInt8   vsyncpulselsb :4;
            UInt8   hsyncoffsetmsb :2;
            UInt8   hsyncpulsemsb :2;
            UInt8   vsyncoffsetmsb :2;
            UInt8   vsyncpulsemsb :2;
            UInt8   hsizelsb;
            UInt8   vsizelsb;
            UInt8   hsizemsb :4;
            UInt8   vsizemsb :4;
            UInt8   hborder;
            UInt8   vborder;
            UInt8   interlaced :1;
            UInt8   stereo :2;
            UInt8   synctype :2;
            UInt8   vsyncpol_serrated :1;
            UInt8   hsyncpol_syncall :1;
            UInt8   twowaystereo :1;
        } timing;
        struct __attribute__ ((packed)) text {
            UInt32  :24;
            UInt8   type;
            UInt8   :8;
            char    data[13];
        } text;
        struct __attribute__ ((packed)) rangelimits {
            UInt64  header :40;
            UInt8   minvfield;
            UInt8   minhfield;
            UInt8   minhline;
            UInt8   minvline;
            UInt8   maxclock;
            UInt8   extended;
            UInt8   :8;
            UInt8   startfreq;
            UInt8   cvalue;
            UInt16  mvalue;
            UInt8   kvalue;
            UInt8   jvalue;
        } range;
        struct __attribute__ ((packed)) whitepoint {
            UInt64  header :40;
            UInt8   index;
            UInt8   :4;
            UInt8   whitexlsb :2;
            UInt8   whiteylsb :2;
            UInt8   whitexmsb;
            UInt8   whiteymsb;
            UInt8   gamma;
            UInt8   index2;
            UInt8   :4;
            UInt8   whitexlsb2 :2;
            UInt8   whiteylsb2 :2;
            UInt8   whitexmsb2;
            UInt8   whiteymsb2;
            UInt8   gamma2;
            UInt32  :24;
        } whitepoint;
    } descriptors[4];
    UInt8   extensions;
    UInt8   checksum;
} EDID;

bool display_info_create(CGDirectDisplayID id, display_info_t *out, char *err);
void display_info_release(display_info_t *display);
void display_info_description(display_info_t *display, char *out, char *extra);
// Returns false on error or incorrect checksum
bool edid_read(EDID *out, display_info_t *display, char *err);

#endif
