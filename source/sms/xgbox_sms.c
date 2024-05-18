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
 * @file xgbox_sms.c
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#include "xgbox.h"

/**
 * @brief calculate len
*/
static xgbox_ret_t __sms_len_calculate (char *p_ptr)
{
    int len;
    int highbyte;
    int lowbyte;

    if (p_ptr == NULL) {
        return 0;
    }

    if (*p_ptr < 0x39) {
        highbyte = (*p_ptr - 0x30) * 16;
    } else {
        highbyte = (*p_ptr - 0x37) * 16;
    }

    p_ptr++;
    if (*p_ptr < 0x39) {
        lowbyte = *p_ptr - 0x30;
    } else {
        lowbyte = *p_ptr - 0x37;
    }

    len = highbyte + lowbyte;
    return len;
}

/**
 * @brief get all sms 
*/
xgbox_ret_t xgbox_sms_all_get (struct xgbox_tty *p_tty, 
                               char             *p_buf, 
                               int               len)
{
    xgbox_ret_t  ret;
    char         tmp_buf[1024];  
    char        *p_start_ptr       = NULL;
    int          center_number_len = 0;
    char         phone_number[32];
    int          phone_len         = 0;
    int          phone_flag        = 0;
    int          encoding          = 0;
    int          data_len          = 0;
    char         send_buf[128];
    char         time_buf[13];
    int          index             = 0;
    int          j                 = 0;
    char         temp;
    char         data_buf[1024];

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    ret = xgbox_at_cmd_send(p_tty, "AT+CSDH=1\r", "OK", NULL, 0);
    if(ret < 0) {
        XGBOX_LOG_ERROR("Send command AT+CSDH=1 failure!");
        return ret;
    }

    //memset(tmp_buf,          0, sizeof(tmp_buf));
    memset(phone_number, 0, sizeof(phone_number));
    //memset(send_buf,     0, sizeof(send_buf));
    memset(time_buf,     0, sizeof(time_buf));
    memset(data_buf,     0, sizeof(data_buf));

    while (1) { 
        memset(send_buf, 0 , sizeof(send_buf));
        sprintf(send_buf, "AT+CMGR=%d\r", index);
        memset(tmp_buf, 0, sizeof(tmp_buf));

        ret = xgbox_at_cmd_send(p_tty, 
                                send_buf, 
                                "+CMGR", 
                                tmp_buf, 
                                sizeof(tmp_buf));
        if(ret != XGBOX_RET_OK) {
            XGBOX_LOG_INFO("There is no index:%d", index);
            break;
        }

        //XGBOX_LOG_ERROR("[SMS] %d\n", index);

        p_start_ptr = strstr(tmp_buf, "\r\n");
        if (p_start_ptr == NULL) {
            XGBOX_LOG_INFO("no expect sms information");
            break;
        }

        p_start_ptr = strstr(p_start_ptr, "+CMGR");
        if (p_start_ptr == NULL) {
            XGBOX_LOG_INFO("no expect sms information");
            break;
        }

        p_start_ptr = strstr(p_start_ptr, "\r\n");
        if (p_start_ptr == NULL) {
            XGBOX_LOG_INFO("no expect sms information");
            break;
        }

        p_start_ptr += 2;
        
        center_number_len = __sms_len_calculate(p_start_ptr);
        p_start_ptr       = p_start_ptr + 2 + center_number_len * 2 + 2;
        //printf("center_number_len:%d\n", center_number_len);

        phone_len = __sms_len_calculate(p_start_ptr);

        //printf("phone_len:%d\n", phone_len);
        //如果号码是奇数位，补上一个F计算
        if(phone_len % 2 != 0) {
            phone_len++;
            phone_flag = 1;
        }
        //获取发送人号码

        strncpy(phone_number, p_start_ptr + 2 + 2, phone_len);
        for (j = 0; j < phone_len; j += 2) {
            temp            = phone_number[j];
            phone_number[j] = phone_number[j + 1];

            if (phone_flag && ((phone_len - 2) == j)) {
                phone_number[j + 1] = '\0';
            } else {
                phone_number[j+1] = temp;
            }
        }

        //printf("PDU package:%s\n", tmp_buf);
        printf("SMS Index:%d\n",   index);
        printf("Sender:%s\n",      phone_number);

        p_start_ptr = p_start_ptr + 2 + 2 + phone_len  + 2;
        //printf("数据编码开始:%s\n",start_ptr);
        encoding  = __sms_len_calculate(p_start_ptr);
        p_start_ptr = p_start_ptr + 2 ;

        memset(time_buf, 0 ,sizeof(time_buf));
        snprintf(time_buf, 12 + 1, "%s", p_start_ptr);

        printf("Receive time:");
        for(j = 0; j < 12; j +=2) {
            temp          = time_buf[j];
            time_buf[j]   = time_buf[j+1];
            time_buf[j+1] = temp;
            printf("-%c%c", time_buf[j], time_buf[j+1]);
        }
        printf("\n");
        
        p_start_ptr = p_start_ptr + 12 + 2;
        
        data_len    = __sms_len_calculate(p_start_ptr);
        data_len    = data_len * 2;
        p_start_ptr = p_start_ptr + 2;
        memset(data_buf, 0, data_len);
        snprintf(data_buf, data_len + 1, "%s", p_start_ptr);

        if (encoding) {
            memset(p_buf, 0, len);
            xgbox_sms_pdu_unicode_to_utf8(data_buf, 
                                          sizeof(data_buf), 
                                          p_buf, 
                                          len);
        } else {
            xgbox_sms_pdu_en_decode(p_start_ptr, p_buf, data_len);
        }

        printf("%s\n", p_buf);
        p_start_ptr = NULL;
        index++;

        printf("...\n");
            
    }

    return XGBOX_RET_OK;
}

/*
 * @brief print help information
 */
static void __xgbox_sms_usage_print (char *p_program_name)
{
    printf("Usage: %s [OPTION]\n\n", p_program_name);
    printf("-a, --all\tDisplay all short memssage.\n");
    printf("-h, --help\tPrinting Help Information.\n\n"); 
    printf("For example:sms -a \n\n");

}

/**
 * @brief cmd sms handler
 */
void xgbox_sms_handler (struct xgbox_tty *p_tty, int argc, char *argv[])
{
    char         ch;
    xgbox_ret_t  ret;
    char         sms_buf_utf8[1024];

    xgbox_bool_t all = XGBOX_FALSE;;

    struct option sms_opts[] = {
        {"all",   no_argument, NULL, 'a'},
        {"help",  no_argument, NULL, 'h'},
        {NULL,    0,           NULL, 0}
    };

    while ((ch = getopt_long(argc, 
                             argv, 
                             "::ah", 
                             sms_opts, 
                             NULL)) != -1) {
        
        switch (ch) {
        
        case 'a':
            all = XGBOX_TRUE;
            break;

        default:
        case 'h':
            __xgbox_sms_usage_print("sms");
            return;
        }
    }

    ret = xgbox_at_cmd_send(p_tty,
                            "AT+CMGF=0\r",
                            "OK",
                            NULL, 
                            0);
    if (ret != XGBOX_RET_OK) {
        return;
    }

    ret = xgbox_at_cmd_send(p_tty,
                            "AT+CPMS=\"SM\"\r",
                            "OK",
                            NULL, 
                            0);
    if (ret != XGBOX_RET_OK) {
        return;
    }

    memset(sms_buf_utf8, 0, sizeof(sms_buf_utf8));

    if (all) {
        xgbox_sms_all_get(p_tty,
                            sms_buf_utf8, 
                            sizeof(sms_buf_utf8));
    }
}

/* end of file */