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
 * @file xgbox_at.c
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#include "xgbox.h"

/**
 * @brief send at cmd
*/
xgbox_ret_t xgbox_at_cmd_send (struct xgbox_tty *p_tty, 
                               char             *p_input_buf, 
                               const char       *p_expect_buf, 
                               char             *p_output_buf, 
                               int               output_buf_size)
{
    char tmp[1024];

    if (p_input_buf == NULL || p_expect_buf == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    if (xgbox_tty_send(p_tty, p_input_buf, strlen(p_input_buf)) < 0) {
        XGBOX_LOG_ERROR("Send AT commond failed:%s", p_input_buf);
        return -XGBOX_RET_EIO;
    }
    
    usleep(50000);

    memset(tmp, 0, sizeof(tmp));

    if (xgbox_tty_recv(p_tty, tmp, sizeof(tmp)) <= 0) {
        XGBOX_LOG_ERROR("Recving message failed");
        return -XGBOX_RET_EIO;
    }

    if (strstr(tmp, p_expect_buf) == NULL) {
        XGBOX_LOG_ERROR("Can't find what you expect to receive[%s]", 
                        p_expect_buf);
        return -XGBOX_RET_EIO;
    }

    if (p_output_buf != NULL && output_buf_size != 0) {
        memset(p_output_buf, 0, output_buf_size);
        strncpy(p_output_buf, tmp, output_buf_size);
    }
    
    return XGBOX_RET_OK;
}

/**
 * @brief check at ready
 */
xgbox_ret_t xgbox_at_tty_ready_check (struct xgbox_tty *p_tty)
{
    xgbox_ret_t ret;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    ret = xgbox_at_cmd_send(p_tty, "AT\r", "OK", NULL, 0);
    if (ret != XGBOX_RET_OK) {
        XGBOX_LOG_ERROR("The serial port is not ready!");
        return ret;
    }

    XGBOX_LOG_INFO("The serial port is ok!");
    return XGBOX_RET_OK;
}

/**
 * @brief check sim exist
 */
xgbox_ret_t xgbox_at_sim_exist_check (struct xgbox_tty *p_tty)
{
    xgbox_ret_t ret;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    ret = xgbox_at_cmd_send(p_tty, "AT+CPIN?\r", "READY", NULL, 0);
    if (ret != XGBOX_RET_OK) {
        XGBOX_LOG_ERROR("The SIM card is not exist");
        return -XGBOX_RET_EIO;
    }

    XGBOX_LOG_INFO("The SIM card is exist!");
    return XGBOX_RET_OK;
}

/**
 * @brief check sim login
 */
xgbox_ret_t xgbox_at_sim_login_check (struct xgbox_tty *p_tty)
{
    xgbox_ret_t ret;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    ret = xgbox_at_cmd_send(p_tty, "AT+CREG?\r", "0,1", NULL, 0);
    if (ret != XGBOX_RET_OK) {
        XGBOX_LOG_ERROR("The SIM card does not exist!");
        return -XGBOX_RET_EIO;
    }
    
    ret = xgbox_at_cmd_send(p_tty, "AT+CREG?\r", "0,3", NULL, 0);
    if (ret != XGBOX_RET_OK) {
        XGBOX_LOG_ERROR("The SIM card does not exist!");
        return -XGBOX_RET_EIO;
    }

    XGBOX_LOG_INFO("SIM card is exist!");
    return XGBOX_RET_OK;
}

/**
 * @brief check sim signal
 */
xgbox_ret_t xgbox_at_sim_signal_check (struct xgbox_tty *p_tty)
{
    xgbox_ret_t  ret;
    char         output_buf[256];

    char         separator[] = " ,";
    int          signal      = -1;
    char        *p_token     = NULL;
    int          i           = 1;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    ret = xgbox_at_cmd_send(p_tty, 
                            "AT+CSQ\r", 
                            "+CSQ", 
                            output_buf, 
                            sizeof(output_buf));
    if (ret != XGBOX_RET_OK) {
        XGBOX_LOG_ERROR("Not found SIM signal!");
        return -XGBOX_RET_EIO;
    }
   
    p_token = strtok(output_buf, separator);
    while (p_token != NULL) {
        ++i;
        p_token = strtok(NULL, separator);
        XGBOX_LOG_INFO("[AT] i: %d \ntoken:%s", i, p_token);

        if (2 == i) {
            signal = atoi(p_token);

            if ((signal < 8) || (signal > 31)) {
                XGBOX_LOG_ERROR(
                    "The signal1 value is: %d, is not normal!", signal);
                return -XGBOX_RET_EIO;
            } else {
                XGBOX_LOG_INFO("The signal1 value is: %d, normal!", 
                               signal);
                break;
            }
        }

    }

    return XGBOX_RET_OK;
}

/**
 * @brief set at echo mode
 */
xgbox_ret_t xgbox_at_echo_set (struct xgbox_tty *p_tty, xgbox_bool_t enable)
{
    xgbox_ret_t ret;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    if (enable) {
        ret = xgbox_at_cmd_send(p_tty, 
                                "ATE 1\r", 
                                "OK", 
                                NULL, 
                                0);
        if (ret != XGBOX_RET_OK) {
            XGBOX_LOG_ERROR("Echo on fail!");
            return -XGBOX_RET_EIO;
        }
    } else {
        ret = xgbox_at_cmd_send(p_tty, 
                                "ATE 0\r", 
                                "OK", 
                                NULL, 
                                0);
        if (ret != XGBOX_RET_OK) {
            XGBOX_LOG_ERROR("Echo off fail!");
            return -XGBOX_RET_EIO;
        }
    }

    XGBOX_LOG_INFO("Echo mode %s", enable ? "on" : "off");

    return ret;
}

/**
 * @brief check all ready
 */
xgbox_ret_t xgbox_at_all_ready_check (struct xgbox_tty *p_tty)
{
    xgbox_ret_t ret;

    ret = xgbox_at_tty_ready_check(p_tty);
    if (ret != XGBOX_RET_OK) {
        return ret;
    }

XGBOX_LOG_INFO("[AT] tty is ok!");

    ret = xgbox_at_sim_exist_check(p_tty);
    if (ret != XGBOX_RET_OK) {
        return ret;
    }

    XGBOX_LOG_INFO("[AT] SIM is exist!");

#if 0
    ret = xgbox_at_sim_login_check(p_tty);
    if (ret != XGBOX_RET_OK) {
        return ret;
    }

    printf("[AT] SIM is login!\n");

    ret = xgbox_at_sim_signal_check(p_tty);
    if (ret != XGBOX_RET_OK) {
        return ret;
    }

    printf("[AT] SIM signal is ok!\n");
#endif

    ret = xgbox_at_echo_set (p_tty, XGBOX_TRUE);
    if (ret != XGBOX_RET_OK) {
        return ret;
    }

    printf("SIM and tty are ready!\n");
    return XGBOX_RET_OK;
}

/* end of file */
