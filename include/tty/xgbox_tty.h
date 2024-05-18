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
 * @file xgbox_tty.h
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#ifndef __XGBOX_TTY_H
#define __XGBOX_TTY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "xgbox.h"

/** @brief tty name length */
#define XGBOX_TTY_NAME 128

/**
 * @brief tty
 */
struct xgbox_tty {
    int            fd;                       /**< @brief file descriptor */
    int            baudrate;                 /**< @brief baudrate */
    int            data_bits;                /**< @brief data bits length */
    char           parity;                   /**< @brief parity */
    int            stop_bits;                /**< @brief stop bit length */
    char           name[XGBOX_TTY_NAME];     /**< @brief tty device name */
    int            tx_len;                   /**< @brief tx max length */
    int            timeout;                  /**< @brief tty read timeout */
    struct termios termios;                  /**< @brief serial termianl */
};

/**
 * @brief open tty
 */
xgbox_ret_t xgbox_tty_open (struct xgbox_tty *p_tty);

/**
 * @brief close tty
 */
xgbox_ret_t xgbox_tty_close (struct xgbox_tty *p_tty);

/**
 * @brief tty start
 */
xgbox_ret_t xgbox_tty_start (struct xgbox_tty *p_tty);

/**
 * @brief tty send data
 */
xgbox_ret_t xgbox_tty_send (struct xgbox_tty *p_tty, 
                            char             *p_buf, 
                            int               len);

/**
 * @brief tty receive data
*/
xgbox_ret_t xgbox_tty_recv (struct xgbox_tty *p_tty, 
                            char             *p_buf, 
                            int               len);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __XGBOX_TTY_H */

/* end of file */