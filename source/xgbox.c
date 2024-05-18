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
 * @file main.c
 * @brief 
 * 
 * @internal
 * @par modification history:
 * - 1.00 24-05-13  win, first implementation
 * @endinternal
 */

#include "xgbox.h"

volatile int g_stop = 0;

/*
 * @brief print help information
 */
static void __xgbox_usage_print (char *p_program_name)
{
    printf("Usage: %s [OPTION]\n\n", p_program_name);
    printf("-b <baudrate>\tSelect baud rate, for example 115200 and 9600.\n");
    printf("-p <parity>\tSelect parity check, for example n N e E o O.\n");
    printf("-s <data_bits>\tSelect data bit, for example 8.\n");
    printf("-s <stop_bits>\tSelect stop bit, for example 1 and 2.\n");
    printf("-m <serial_name>\tSelect device file, for example /dev/ttyUSB0.\n");
    printf("-h, --help\tPrinting Help Information.\n\n"); 
    printf("For example:./xgbox -b 115200 -p n -s 1 -m /dev/ttyUSB0 \n\n");

}

/*
 * @brief print help information
 */
static void __xgbox_subcmd_usage_print (void)
{
    printf("Usage: <subcmd> [OPTION]\n\n");
    printf("help\tPrinting Help Information.\n");
    printf("sms\tShort message service.\n");
    printf("quit\tExit.\n\n");
}

/*
 * @brief signal handler
 */
static void __xgbox_handler (int sig)
{
    switch (sig) {

    case SIGINT:
        printf("Process captured SIGINT signal!");
        g_stop = 1;
        break;

    case SIGTERM:
        printf("Process captured SIGTERM signal!");
        g_stop = 1;
        break;

    case SIGSEGV:
        printf("Process captured SIGSEGV signal!");
        g_stop = 1;
        exit(0);
        break;

    case SIGPIPE:
        printf("Process captured SIGPIPE signal!");
        g_stop = 1;
        break;

    default:
        break;
    }
}

/*
 * @brief register signal
 */
static void __xgbox_signal_register (void)
{
    struct sigaction sigact;

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags   = 0;
    sigact.sa_handler = __xgbox_handler;

    sigaction(SIGINT,  &sigact, 0);
    sigaction(SIGTERM, &sigact, 0);
    sigaction(SIGPIPE, &sigact, 0);
    sigaction(SIGSEGV, &sigact, 0);

    return ;
}

/**
 * @brief remove '\r' or '\n'
 */
static void __xgbox_newline_remove (char *p_str)
{
    char *p_ptr = p_str;

    while (*p_ptr != '\0') {
        if (*p_ptr == '\r' || *p_ptr == '\n') {
            *p_ptr = '\0';
            break;
        }
        p_ptr++;
    }
}

/**
 * @brief main
 */
int main (int argc, char *argv[])
{
    //int               rv_fd       = -1;
    char              tx_buf[128];
    char              rx_buf[128];
    //fd_set            rdset;
    struct xgbox_tty  tty;
    struct xgbox_tty *p_tty       = &tty;
    int               ch;
    //int               i;
    xgbox_ret_t       ret;

    struct option opts[] = {
        {"baudrate",    required_argument, NULL, 'b'},
        {"data_bits",   required_argument, NULL, 'd'},
        {"parity",      required_argument, NULL, 'p'},
        {"stop_bits",   required_argument, NULL, 's'},
        {"serial_name", required_argument, NULL, 'm'},
        {"help",        no_argument,       NULL, 'h'},
        {NULL,          0,                 NULL, 0}
    };

    memset(tx_buf, 0, sizeof(tx_buf));
    memset(rx_buf, 0, sizeof(rx_buf));

    memset(&tty,   0, sizeof(&tty));

    while((ch = getopt_long(argc, 
                            argv, 
                            "b:d:p:s:m:h", 
                            opts, 
                            NULL)) != -1) {

        switch (ch) {

        case 'b':
            p_tty->baudrate  = atoi(optarg);
            break;

        case 'd':
            p_tty->data_bits = atoi(optarg);
            break;

        case 'p':
            p_tty->parity    = optarg[0];
            break;

        case 's':
            p_tty->stop_bits = atoi(optarg);
            break;

        case 'm':
            strncpy(p_tty->name, optarg, XGBOX_TTY_NAME);
            break;

        default:
        case 'h':
            __xgbox_usage_print(argv[0]);
            return 0;
        }
    }

    if (0 == strlen(p_tty->name)) {
        return -XGBOX_RET_EINVAL;
    }

    xgbox_log_init(XGBOX_LOG_LEVEL_DEBUG, "./xgbox.log");

    __xgbox_signal_register();

    ret = xgbox_tty_open(p_tty);
    if (ret != XGBOX_RET_OK) {
        printf("Failed to open %s device.\n", p_tty->name);
        return ret;
    }

    ret = xgbox_tty_start(p_tty);
    if (ret != XGBOX_RET_OK) {
        printf("Failed to initialize the serial port\n");
        return ret;
    }

    ret = xgbox_at_all_ready_check(p_tty);
    if (ret != XGBOX_RET_OK) {
        printf("[XGBOX] tty of SIM status is not OK\n");
        return ret;
    }

    while (!g_stop) {
        char  str[256];
        int   ac;
        char *av[16];

        ac = 0;
        memset(str, 0, sizeof(str));

        optind = 1;

        printf("\n");
        printf("(Enter \"help\" for help information.)\n");
        printf("(Enter \"quit\" to exit.)\n");
        printf("> ");
        fgets(str, sizeof(str), stdin);
        __xgbox_newline_remove(str);
        
        av[ac] = strtok(str, " ");

        if (av[ac] == NULL) {
            continue;
        }

        do {
            ac++;
            av[ac] = strtok(NULL, " ");
        } while (av[ac] != NULL);

        if (strncmp(av[0], "help", strlen("help")) == 0 || 
            strncmp(av[0], "HELP", strlen("HELP")) == 0 ||
            strncmp(av[0], "h", strlen("h")) == 0) {
            __xgbox_subcmd_usage_print();
            continue;
        }

        if (strcmp(av[0], "quit") == 0 || strcmp(av[0], "QUIT") == 0 ||
            strcmp(av[0], "q") == 0) {
            break;
        }

        if (strcmp(av[0], "sms") == 0 || strcmp(av[0], "SMS") == 0) {
            xgbox_sms_handler(p_tty, ac, av);
            continue;
        }
    }

#if 0
    while(!g_stop) {

        FD_ZERO(&rdset);
        FD_SET(p_tty->fd, &rdset);
        FD_SET(STDIN_FILENO, &rdset);

        rv_fd = select(ttyusb_ctx_ptr->fd + 1, &rdset, NULL, NULL, NULL);
        if (rv_fd <= 0) {
            printf("Select listening for file descriptor error!\n");
            ret = -XGBOX_RET_EPERM;
            goto __cleanup;
        } 
        
        if (FD_ISSET(STDIN_FILENO, &rdset)) {

            memset(tx_buf, 0, sizeof(tx_buf));

            fgets(tx_buf, sizeof(tx_buf), stdin);

            i = strlen(tx_buf);
            strcpy(&tx_buf[i-1], "\r");

            if (xgbox_tty_send(p_tty, tx_buf, strlen(tx_buf)) < 0) {
                printf("[XGBOX] Failed to send data through the serial port\n");
                ret = -XGBOX_RET_EIO;
                goto __cleanup;
            }

            printf("[XGBOX] Succeeded in send data serial port data:%s\n", 
                   tx_buf);
            fflush(stdin);

        } else if(FD_ISSET(p_tty->fd, &rdset)) {

            memset(rx_buf, 0, sizeof(rx_buf));

            if (xgbox_tty_recv(p_tty, rx_buf, sizeof(rx_buf), 0) < 0) {
                printf("Failed to receive serial port data!\n");
                ret = -XGBOX_RET_EIO;
                goto __cleanup;
            }

            printf("[XGBOX] Succeeded in receiving serial port data:%s\n", 
                   recv_buf);
            fflush(stdout);
        }
    }
#endif

    xgbox_tty_close(p_tty);
    xgbox_log_deinit();
    return XGBOX_RET_OK;
}

/* end of file */