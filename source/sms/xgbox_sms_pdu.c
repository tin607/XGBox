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
 * @file xgbox_sms_pdu.c
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#include "xgbox.h"

/**
 * @brief string to byte
 */
static xgbox_ret_t __xgbox_sms_pdu_str_to_byte (char *p_src, 
                                                char *p_dst, 
                                                int   src_len)
{
    char high_ch;
    char low_ch;
    int  i;

    if (src_len % 2 != 0) {
        return -XGBOX_RET_EINVAL;
    }

    for (i = 0; i < src_len; i += 2) {
        high_ch = toupper(p_src[i]);
        low_ch  = toupper(p_src[i + 1]);

        if (high_ch >= 'A') {
            high_ch -= '7';
        } else {
            high_ch -= '0';
        }

        if (low_ch >= 'A') {
            low_ch -= '7';
        } else {
            low_ch -= '0';
        }

        p_dst[i / 2] =  (high_ch << 4) | low_ch;
    }

    return 0;
}

/**
 * @brief unicode to utif-8
 */
xgbox_ret_t xgbox_sms_pdu_unicode_to_utf8 (char   *p_src, 
                                           size_t  src_len, 
                                           char   *p_dst, 
                                           size_t  dst_len)
{
    int      i;
    char     ch;
    int      ret = -1;
    iconv_t  cd;
    char     tmp[512] = {0};
    char    *p_src_ptr;    

    if (p_src == NULL || p_dst == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    memset(tmp, 0, sizeof(tmp));

    __xgbox_sms_pdu_str_to_byte(p_src, 
                                tmp, 
                                strlen(p_src));

    for (i = 0; i < sizeof(tmp); i +=2) {
        ch         = tmp[i];
        tmp[i]     = tmp[i + 1];
        tmp[i + 1] = ch;
    }

    cd = iconv_open("UTF-8","UNICODE");
    if (cd < 0) {
        return -XGBOX_RET_ENOENT;
    }

    p_src_ptr = tmp;    

    ret =  iconv(cd, &p_src_ptr, &src_len, &p_dst, &dst_len);
    if(ret < 0) {
        iconv_close(cd);
        return -XGBOX_RET_EPERM;
    }

    iconv_close(cd);
    return XGBOX_RET_OK;
}

/**
 * \@brief english decode 7bit
 */
static xgbox_ret_t __xgbox_sms_pdu_en_7bit_decode (const uint8_t *p_src, 
                                                   char          *p_dst, 
                                                   int            src_len)
{
    int     nsrc  = 0;  
    int     ndst  = 0;
    int     nbyte = 0;
    uint8_t nleft = 0;

    if (p_src == NULL || p_dst == NULL || src_len <= 0) {
        return -XGBOX_RET_EINVAL;
    }
    
    while (nsrc < src_len) {
        *p_dst = ((*p_src << nbyte) | nleft) & 0x7f;
        nleft  = *p_src >> (7 - nbyte);

        p_dst++;
        ndst++;
        nbyte++;

        if (nbyte == 7) {
            *p_dst = nleft;
            p_dst++;
            ndst++;
            nbyte = 0;
            nleft = 0;
        }

        p_src++;
        nsrc++;
    }

    *p_dst = 0;

    return (xgbox_ret_t)ndst;
}

/**
 * @brief english decode
 */
xgbox_ret_t xgbox_sms_pdu_en_decode (char *p_src, char *p_dst, int src_len)
{
    char tmp[512];

    memset(tmp, 0, sizeof(tmp));

    if (p_src == NULL || p_dst == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    __xgbox_sms_pdu_str_to_byte(p_src, tmp, src_len);
    __xgbox_sms_pdu_en_7bit_decode(tmp, p_dst, strlen(tmp));

    return XGBOX_RET_OK;
}

/* end of file */