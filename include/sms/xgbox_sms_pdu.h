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
 * @file xgbox_sms_pdu.h
 * @brief Short Mesasge Service PDU process
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#ifndef __XGBOX_SMS_PDU_H
#define __XGBOX_SMS_PDU_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "xgbox.h"

/**
 * @brief unicode to utif-8
 */
xgbox_ret_t xgbox_sms_pdu_unicode_to_utf8 (char   *p_src, 
                                           size_t  src_len, 
                                           char   *p_dst, 
                                           size_t  dst_len);

/**
 * @brief english decode
 */
xgbox_ret_t xgbox_sms_pdu_en_decode (char *p_src, char *p_dst, int src_len);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __XGBOX_SMS_PDU_H */

/* end of file */