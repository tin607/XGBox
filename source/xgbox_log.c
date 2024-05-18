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
 * @file xgbox.c
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#include "xgbox.h"

static FILE *__g_log_fp = NULL;

/**
 * @brief 
 */
static inline int __log_lvl_translate (int level)
{
    int lvl;

    switch (level) {

    case XGBOX_LOG_LEVEL_TRACE:
        lvl = LOG_TRACE;
        break;

    case XGBOX_LOG_LEVEL_DEBUG:
        lvl = LOG_DEBUG;
        break;

    case XGBOX_LOG_LEVEL_INFO:
        lvl = LOG_INFO;
        break;

    case XGBOX_LOG_LEVEL_WARN:
        lvl = LOG_WARN;
        break;

    case XGBOX_LOG_LEVEL_ERROR:
        lvl = LOG_ERROR;
        break;

    default:
        lvl = LOG_DEBUG;
        break;
    }

    return lvl;
}

/**
 * @brief log initialize
 */
xgbox_ret_t xgbox_log_init (int level, const char *p_log_file)
{
    int          lvl;
    xgbox_ret_t  ret;
    FILE        *p_fp;

    lvl = __log_lvl_translate(level);

    log_set_quiet(true);
    log_set_level(lvl);

    p_fp = fopen(p_log_file, "r");
    if (p_fp == NULL) {
        p_fp = fopen(p_log_file, "w");
    } else {
        fclose(p_fp);
        p_fp = fopen(p_log_file, "a");
    }

    if (p_fp == NULL) {
        ret = -XGBOX_RET_ENOENT;
        goto __exit;
    }

    log_add_fp(p_fp, lvl);

    __g_log_fp = p_fp;

__exit:
    return ret;
}

/**
 * @brief log deinit
 */
void xgbox_log_deinit (void)
{
    fclose(__g_log_fp);
}

/**
 * @brief log
 */
xgbox_ret_t xgbox_log (int         level, 
                       const char *p_file, 
                       int         line, 
                       const char *p_fmt, 
                       ...)
{
    int     lvl;
    va_list vlist;
    char    str[256];

    lvl = __log_lvl_translate(level);

    va_start(vlist, p_fmt);
    vsnprintf(str, sizeof(str), p_fmt, vlist);
    va_end(vlist);

    log_log(lvl, p_file, line, str);

    return XGBOX_RET_OK;
}

/* end of file */