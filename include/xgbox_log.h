/*******************************************************************************
*                                 AT
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2024 loracc.com.
* All rights reserved.
*
* Contact information:
* web site:    https://xgbox.loracc.com
* e-mail:      win@loracc.com
*******************************************************************************/

/**
 * @file xgbox_log.h
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#ifndef __XGBOX_LOG_H
#define __XGBOX_LOG_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "xgbox.h"

#define XGBOX_LOG_LEVEL_TRACE     (0)
#define XGBOX_LOG_LEVEL_DEBUG     (1)
#define XGBOX_LOG_LEVEL_INFO      (2)
#define XGBOX_LOG_LEVEL_WARN      (3)
#define XGBOX_LOG_LEVEL_ERROR     (4)

#define XGBOX_LOG_TRACE(...) \
    xgbox_log(XGBOX_LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)

#define XGBOX_LOG_DEBUG(...) \
    xgbox_log(XGBOX_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#define XGBOX_LOG_INFO(...) \
    xgbox_log(XGBOX_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)

#define XGBOX_LOG_WARN(...) \
    xgbox_log(XGBOX_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)

#define XGBOX_LOG_ERROR(...) \
    xgbox_log(XGBOX_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief log initialize
 */
xgbox_ret_t xgbox_log_init (int level, const char *p_log_file);

/**
 * @brief log deinit
 */
void xgbox_log_deinit (void);

/**
 * @bief log
 */
xgbox_ret_t xgbox_log (int         level, 
                       const char *p_file, 
                       int         line, 
                       const char *p_fmt, 
                       ...);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __XGBOX_LOG_H */

/* end of file */