/* CoreGraphics - CGDirectDisplayMetal.h
   Copyright (c) 2000-2015 Apple Inc.
   All rights reserved. */

#ifndef CGDIRECTDISPLAYMETAL_H_
#define CGDIRECTDISPLAYMETAL_H_


/* This file only makes sense to Objective C clients */
#if defined(__OBJC__)

#import <CoreGraphics/CGDirectDisplay.h>

@protocol MTLDevice;

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef NS_RETURNS_RETAINED
# if __has_feature(attribute_ns_returns_retained)
#  define NS_RETURNS_RETAINED __attribute__((ns_returns_retained))
# else
#  define NS_RETURNS_RETAINED
# endif
#endif

/* Return the Metal device for the GPU currently being used to drive a given display */
/* Note: On systems with automatic graphics switching enabled, this value can change at
   almost any time. */
CG_EXTERN id<MTLDevice> __nullable CGDirectDisplayCopyCurrentMetalDevice(CGDirectDisplayID display) NS_RETURNS_RETAINED API_AVAILABLE(macos(10.11));

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* __OBJC__ */

#endif /* CGDIRECTDISPLAYMETAL_H_ */
