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
 * @file xgbox_sms.h
 * @brief Short Message Service
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#ifndef __XGBOX_SMS_H
#define __XGBOX_SMS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "xgbox.h"

/**
 * @brief get all sms 
*/
xgbox_ret_t xgbox_sms_all_get (struct xgbox_tty *p_tty, 
                               char             *p_buf, 
                               int               len);

/**
 * @brief cmd sms handler
 */
void xgbox_sms_handler (struct xgbox_tty *p_tty, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __XGBOX_SMS_H */

/* end of file */