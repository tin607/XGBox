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
 * @brief open tty
 */
xgbox_ret_t xgbox_tty_open (struct xgbox_tty *p_tty)
{
    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    p_tty->fd = open(p_tty->name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (p_tty->fd < 0) {
        XGBOX_LOG_ERROR("Open %s failure!", p_tty->name);
        return -XGBOX_RET_ENODEV;
    }

    if (isatty(p_tty->fd) == 0) {
        XGBOX_LOG_ERROR("Open %s failure!", p_tty->name);
        return -XGBOX_RET_ENOENT;
    }

    XGBOX_LOG_INFO("Open %s successfully!", p_tty->name);
    return XGBOX_RET_OK;
}

/**
 * @brief close tty
 */
xgbox_ret_t xgbox_tty_close (struct xgbox_tty *p_tty)
{
    int ret;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    ret = tcflush(p_tty->fd, TCIOFLUSH);
    if (ret < 0) {
        return -XGBOX_RET_EPERM;
    }
    
    /*
     * Restore the original properties of the serial port
     */
    ret = tcsetattr(p_tty->fd, TCSANOW, &(p_tty->termios));
    if (ret < 0) {
        return -XGBOX_RET_EPERM;
    }

    close(p_tty->fd);
    
    XGBOX_LOG_INFO("Close %s successfully!", p_tty->name);
    return XGBOX_RET_OK;
}

/**
 * @brief tty start
 */
xgbox_ret_t xgbox_tty_start (struct xgbox_tty *p_tty)
{
    int             ret      = -1;
    speed_t         baudrate = B115200;
    struct  termios termios;

    if (p_tty == NULL) {
        return -XGBOX_RET_EINVAL;
    }

    memset(&termios, 0, sizeof(termios));
    memset(&(p_tty->termios), 0, sizeof(p_tty->termios));

    ret = tcgetattr(p_tty->fd, &(p_tty->termios));
    if (ret < 0) {
        return -XGBOX_RET_EPERM;
    }

    ret = tcgetattr(p_tty->fd, &termios);
    if (ret < 0) {
        return -XGBOX_RET_EPERM;
    }

    termios.c_cflag |= CLOCAL;
    termios.c_cflag |= CREAD;

    termios.c_cflag &= ~CSIZE;
    termios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);
    termios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | INLCR | ICRNL | INPCK | 
                         ISTRIP | IXON);
    termios.c_oflag &= ~(OPOST);

    /* baudrate */
    if (p_tty->baudrate != 0) {

        switch (p_tty->baudrate) {

        case 57600:
            baudrate = B57600;
            break;

        case 115200:
            baudrate = B115200;
            break;

        case 230400:
            baudrate = B230400;
            break;

        case 460800:
            baudrate = B460800;
            break;

        default:
            break;
        }

    }

    cfsetispeed(&termios, baudrate);
    cfsetospeed(&termios, baudrate);

    /* databits */
    switch (p_tty->data_bits) {

    case 5:
        termios.c_cflag |= CS5;   /* 5bit */
        break;

    case 6:
        termios.c_cflag |= CS6;   /* 6bit */
        break;

    case 7:
        termios.c_cflag |= CS7;   /* 7bit */
        break;

    case 8:
        termios.c_cflag |= CS8;   /* 8bit */
        break;

    default:
        termios.c_cflag |= CS8;   /* default */
        break; 

    }

    /* parity */
    switch(p_tty->parity) {

    case 'n':
    case 'N':
        termios.c_cflag &= ~PARENB;  /* no parity */
        break;

    case 'o':
    case 'O':
        termios.c_cflag |= (PARODD | PARENB);  /* odd parity */
        break;

    case 'e':
    case 'E':
        termios.c_cflag &= ~PARENB;    /* even parity */
        termios.c_cflag &= ~PARODD;

    case 'b':
    case 'B':
        termios.c_cflag &= ~PARENB;
        termios.c_cflag &= ~CSTOP;     /* space */

    default:
        termios.c_cflag &= ~PARENB;    /* default */
        break;

    }

    /* stopbits */
    switch (p_tty->stop_bits) {

    case 1:
        termios.c_cflag &= ~CSTOPB; /* 1 stop bit */
        break;

    case 2:
        termios.c_cflag |= CSTOPB;  /* 2 stop bit */
        break;

    default:
        termios.c_cflag &= ~CSTOPB; /* default */
        break;

    }

    termios.c_cc[VTIME] = 0;
    termios.c_cc[VMIN]  = 0;

    p_tty->tx_len       = 128;

    ret = tcflush(p_tty->fd, TCIOFLUSH);
    if (ret < 0) {
        return -XGBOX_RET_EPERM;
    }

    ret = tcsetattr(p_tty->fd, TCSANOW, &termios);
    if(ret < 0) {
        XGBOX_LOG_ERROR("Failed to set new properties.");
        return -4;
    }

    XGBOX_LOG_INFO("Successfully set new properties!");
    return XGBOX_RET_OK;
}

/**
 * @brief tty send data
 */
xgbox_ret_t xgbox_tty_send (struct xgbox_tty *p_tty, 
                            char             *p_buf, 
                            int               len)
{

    int    ret      = -1;
    int    write_rv = 0;
    char  *p_ptr    = NULL, *p_end = NULL;

    if (p_tty == NULL || p_buf == NULL || len < 0) {
        return -XGBOX_RET_EINVAL;
    }
	
	/* compose tx_len and len */
    if (len > p_tty->tx_len) {

        p_ptr = p_buf;
        p_end = p_buf + len;

        do {   

            if (p_tty->tx_len < (p_end - p_ptr)) {

                ret = write(p_tty->fd, p_ptr, p_tty->tx_len);

                if ((ret <= 0) || (ret != p_tty->tx_len)) {
                    return -XGBOX_RET_EIO;
                }

                write_rv += ret;
                p_ptr    += p_tty->tx_len;

            } else {

                ret = write(p_tty->fd, p_ptr, (p_end - p_ptr));

                if ((ret <= 0) || (ret != (p_end - p_ptr))) {
                    return -XGBOX_RET_EIO;
                }

                write_rv += ret;
                p_ptr    += (p_end - p_ptr);
            }
            
        } while(p_ptr < p_end);
        
    } else {
        ret = write(p_tty->fd, p_buf, len);
        if((ret <= 0) || (ret != len)) {
            return -XGBOX_RET_EIO;
        }
        write_rv += ret;
    }

    XGBOX_LOG_INFO("send_buf:%s", p_buf);
    XGBOX_LOG_INFO("write_rv: %d", write_rv);

    return write_rv;

}

/**
 * @brief tty receive data
*/
xgbox_ret_t xgbox_tty_recv (struct xgbox_tty *p_tty, 
                            char             *p_buf, 
                            int               len)
{
    int             read_rv = -1;
    int             rv_fd   = -1;
    fd_set          rdset;
    struct  timeval time_out;

    if (p_tty == NULL || (len < 0)) {
        return -XGBOX_RET_EINVAL;
    }
    
    if (p_tty->timeout != 0) {

        time_out.tv_sec = (time_t)p_tty->timeout;
        time_out.tv_usec = 0;
        FD_ZERO(&rdset);
        FD_SET(p_tty->fd, &rdset);

        rv_fd = select(p_tty->fd + 1, &rdset, NULL, NULL, &time_out);
        if (rv_fd <= 0) {
            return -XGBOX_RET_EIO;
        } 
    }
    
    usleep(1000);

    read_rv = read(p_tty->fd, p_buf, len);
    if (read_rv <= 0) {
        return -XGBOX_RET_EIO;
    }
    
    //printf("[TTY] recv len=%d\n", read_rv);
    return read_rv;
}

/* end of file */