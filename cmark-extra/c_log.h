//
//  c_log.h
//  QLMarkdown
//
//  Created by Sbarex on 21/03/22.
//

#ifndef c_log_h
#define c_log_h

#include <stddef.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <os/log.h>
#else
typedef void* os_log_t;
#define os_log_create(subsystem, category) ((void*)0)
#define os_log_error(log, format, ...) (void)0
#define os_log_info(log, format, ...) (void)0
#define os_log_debug(log, format, ...) (void)0
#endif

os_log_t getLogCategory(void);
os_log_t getLogForImageExt(void);
os_log_t getLogForHeadsExt(void);
os_log_t getLogForEmojiExt(void);

#endif /* c_log_h */
