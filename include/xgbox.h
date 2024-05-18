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
 * @file xgbox.h
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#ifndef __XGBOX_H
#define __XGBOX_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>
#include <iconv.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <getopt.h>
#include <signal.h>

#include "log/src/log.h"

/**
 * @name type
 * @{
 */

typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned long     uint32_t;

/**
 * @}
 */

/**
 * @name XGBOX error value
 * @{
 */

#define XGBOX_RET_OK         0
#define XGBOX_RET_EPERM      1   /**< @brief Operation not permitted */
#define XGBOX_RET_ENOENT     2   /**< @brief No such file or directory */
#define XGBOX_RET_EIO        5   /**< @brief I/O error */
#define XGBOX_RET_ENXIO      6   /**< @brief No such device or address */
#define XGBOX_RET_EAGAIN     11
#define XGBOX_RET_ENOMEM     12
#define XGBOX_RET_EBUSY      16
#define XGBOX_RET_EEXIST     17
#define XGBOX_RET_ENODEV     19
#define XGBOX_RET_EINVAL     22
#define XGBOX_RET_ENOTTY     25  /**< @brief Not a typewriter */

#define XGBOX_RET_MAX        160

typedef long                 xgbox_ret_t;
typedef bool                 xgbox_bool_t;

#define XGBOX_TRUE           true
#define XGBOX_FALSE          false

/**
 * @brief Pointer return valut translate to error value
 */
static inline xgbox_ret_t xgbox_ptr_to_err (void *p_ptr) 
{
    return (xgbox_ret_t)p_ptr;
}

/**
 * @brief error value to pointer
 */
static inline void *xgbox_err_to_ptr (xgbox_ret_t err)
{
    return (void *)err;
} 

/**
 * @brief pointer Is the pointer an error value
 */
static inline xgbox_bool_t xgbox_ptr_is_err (void *p_ptr)
{
    return !!((xgbox_ret_t)(p_ptr) >= (xgbox_ret_t)(-XGBOX_RET_MAX));
}

#include "xgbox_log.h"

#include "tty/xgbox_tty.h"
#include "at/xgbox_at.h"
#include "sms/xgbox_sms_pdu.h"
#include "sms/xgbox_sms.h"

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __XGBOX_H */

/* end of file */