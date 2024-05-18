/*******************************************************************************
*                                 AT
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2024 loracc.com.
* All rights reserved.
*
* Contact information:
* web site:    https ://xgbox.loracc.com
* e-mail:      win@loracc.com
*******************************************************************************/

/**
 * @file xgbox_at.h
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#ifndef __XGBOX_AT_H
#define __XGBOX_AT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "xgbox.h"

/**
 * @brief send at cmd
*/
xgbox_ret_t xgbox_at_cmd_send (struct xgbox_tty *p_tty, 
                               char             *p_input_buf, 
                               const char       *p_expect_buf, 
                               char             *p_output_buf, 
                               int               output_buf_size);

/**
 * @brief check at ready
 */
xgbox_ret_t xgbox_at_tty_ready_check (struct xgbox_tty *p_tty);

/**
 * @brief check sim exist
 */
xgbox_ret_t xgbox_at_sim_exist_check (struct xgbox_tty *p_tty);

/**
 * @brief check sim login
 */
xgbox_ret_t xgbox_at_sim_login_check (struct xgbox_tty *p_tty);

/**
 * @brief check sim signal
 */
xgbox_ret_t xgbox_at_sim_signal_check (struct xgbox_tty *p_tty);

/**
 * @brief set at echo mode
 */
xgbox_ret_t xgbox_at_echo_set (struct xgbox_tty *p_tty, xgbox_bool_t enable);

/**
 * @brief check all ready
 */
xgbox_ret_t xgbox_at_all_ready_check (struct xgbox_tty *p_tty);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __XGBOX_H */

/* end of file */