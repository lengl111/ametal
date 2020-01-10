/*******************************************************************************
 *                                 AMetal
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn/
 *******************************************************************************/

/**
 * \file
 * \brief UART ����ʵ��
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10  ari, first implementation
 * \endinternal
 */
#include "am_hc32f460_uart.h"
#include "am_clk.h"
#include "am_int.h"
#include "hw/amhw_hc32f460_uart.h"
#include "hc32f460_irq_handle.h"
#include "hc32f460_intctrl.h"

/*******************************************************************************
 * Functions declaration
 *******************************************************************************/

/**
 * \brief ����ģʽ����ѯ���жϣ�����
 */
int __uart_mode_set(am_hc32f460_uart_dev_t *p_dev, uint32_t new_mode);

/**
 * \brief ����Ӳ������
 */
int __uart_opt_set(am_hc32f460_uart_dev_t *p_dev, uint32_t opts);

/* ZLG ���������������� */
static int __uart_ioctl(void *p_drv, int, void *);

static int __uart_tx_startup(void *p_drv);

static int __uart_callback_set(void *p_drv, int callback_type,
        void *pfn_callback, void *p_arg);

static int __uart_poll_getchar(void *p_drv, char *p_char);

static int __uart_poll_putchar(void *p_drv, char outchar);

#if 0
static int __uart_connect (void *p_drv);
#endif

static void __uart_irq_handler(void *p_arg);

/** \brief ��׼��ӿں���ʵ�� */
static const struct am_uart_drv_funcs __g_uart_drv_funcs = { 
    __uart_ioctl,
    __uart_tx_startup, 
    __uart_callback_set, 
    __uart_poll_getchar,
    __uart_poll_putchar, 
};

/******************************************************************************/

/**
 * \brief �豸���ƺ���
 *
 * ���а������û�ȡ�����ʣ�ģʽ���ã��ж�/��ѯ������ȡ֧�ֵ�ģʽ��Ӳ��ѡ�����õȹ��ܡ�
 */
static int __uart_ioctl(void *p_drv, int request, void *p_arg)
{
    am_hc32f460_uart_dev_t *p_dev = (am_hc32f460_uart_dev_t *) p_drv;
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;

    int status = AM_OK;

    switch (request) {

    /* ���������� */
    case AM_UART_BAUD_SET:

        /* ֻ���ڵ�ǰ������ɵĻ����ϲ������޸Ĳ����� */
        while (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
        AMHW_HC32F460_UART_TX_COMPLETE_FALG) == AM_FALSE)
            ;
        status = amhw_hc32f460_uart_baudrate_set(p_hw_uart,
                am_clk_rate_get(p_dev->p_devinfo->clk_num), (uint32_t) p_arg);

        if (status > 0) {
            p_dev->baud_rate = status;
            status = AM_OK;
        }
        else {
            status = -AM_EIO;
        }

        break;

        /* �����ʻ�ȡ */
    case AM_UART_BAUD_GET:
        *(int *) p_arg = p_dev->baud_rate;
        break;

        /* ģʽ���� */
    case AM_UART_MODE_SET:
        status = (__uart_mode_set(p_dev, (int) p_arg) == AM_OK) ? AM_OK : -AM_EIO;
        break;

        /* ģʽ��ȡ */
    case AM_UART_MODE_GET:
        *(int *) p_arg = p_dev->channel_mode;
        break;

        /* ��ȡ���ڿ����õ�ģʽ */
    case AM_UART_AVAIL_MODES_GET:
        *(int *) p_arg = AM_UART_MODE_INT | AM_UART_MODE_POLL;
        break;

        /* ����ѡ������ */
    case AM_UART_OPTS_SET:
        status = (__uart_opt_set(p_dev, (int) p_arg) == AM_OK) ? AM_OK : -AM_EIO;
        break;

        /* ����ѡ���ȡ */
    case AM_UART_OPTS_GET:
        *(int *) p_arg = p_dev->options;
        break;

    case AM_UART_RS485_SET:
        if (p_dev->rs485_en != (am_bool_t) (int) p_arg) {
            p_dev->rs485_en = (am_bool_t) (int) p_arg;
        }
        break;

    case AM_UART_RS485_GET:
        *(int *) p_arg = p_dev->rs485_en;
        break;

    default:
        status = -AM_EIO;
        break;
    }

    return (status);
}

/**
 * \brief �������ڷ���(�����ж�ģʽ)
 */
int __uart_tx_startup(void *p_drv)
{
    char data = 0;

    am_hc32f460_uart_dev_t *p_dev = (am_hc32f460_uart_dev_t *) p_drv;
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;

    /* ʹ�� 485 ���Ϳ������� */
    if (p_dev->rs485_en && p_dev->p_devinfo->pfn_rs485_dir) {
        p_dev->p_devinfo->pfn_rs485_dir(AM_TRUE);
    }

    /* �ȴ���һ�δ������ */
    while (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_TX_COMPLETE_FALG) == AM_FALSE)
        ;

//    /* ��ȡ�������ݲ����� */
//    if ((p_dev->pfn_txchar_get(p_dev->txget_arg, &data)) == AM_OK) {
//        amhw_hc32f460_uart_data_write(p_hw_uart, data);
//    }

    /* ʹ�ܷ����ж� */
    amhw_hc32f460_uart_int_enable(p_hw_uart, AMHW_HC32F460_UART_INT_TX_EMPTY_ENABLE | (1 << 3));
    return AM_OK;
}

/**
 * \brief �����жϷ���ص�����
 */
static int __uart_callback_set(void *p_drv, int callback_type,
        void *pfn_callback, void *p_arg)
{
    am_hc32f460_uart_dev_t *p_dev = (am_hc32f460_uart_dev_t *) p_drv;

    switch (callback_type) {

    /* ���÷��ͻص������еĻ�ȡ�����ַ��ص����� */
    case AM_UART_CALLBACK_TXCHAR_GET:
        p_dev->pfn_txchar_get = (am_uart_txchar_get_t) pfn_callback;
        p_dev->txget_arg = p_arg;
        return (AM_OK);

        /* ���ý��ջص������еĴ�Ž����ַ��ص����� */
    case AM_UART_CALLBACK_RXCHAR_PUT:
        p_dev->pfn_rxchar_put = (am_uart_rxchar_put_t) pfn_callback;
        p_dev->rxput_arg = p_arg;
        return (AM_OK);

        /* ���ô����쳣�ص����� */
    case AM_UART_CALLBACK_ERROR:
        p_dev->pfn_err = (am_uart_err_t) pfn_callback;
        p_dev->err_arg = p_arg;
        return (AM_OK);

    default:
        return (-AM_ENOTSUP);
    }
}

/**
 * \brief ��ѯģʽ�·���һ���ַ�
 */
static int __uart_poll_putchar(void *p_drv, char outchar)
{
    am_hc32f460_uart_dev_t *p_dev = (am_hc32f460_uart_dev_t *) p_drv;
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;

    /* ����ģ���Ƿ����, AM_FALSE:æ; TURE: ���� */
    if (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_TX_EMPTY_FLAG) == AM_FALSE) {
        return (-AM_EAGAIN);
    }
    else {
        if ((p_dev->rs485_en) && (p_dev->p_devinfo->pfn_rs485_dir != NULL)) {
            /* ���� 485 Ϊ����ģʽ */
            p_dev->p_devinfo->pfn_rs485_dir(AM_TRUE);
        }

        /* ����һ���ַ� */
        amhw_hc32f460_uart_data_write(p_hw_uart, outchar);

        if (p_dev->rs485_en && p_dev->p_devinfo->pfn_rs485_dir) {

            /* �ȴ�������� */
            while (!(p_hw_uart->SR_f.TC & 0x01))
                ;

            p_dev->p_devinfo->pfn_rs485_dir(AM_FALSE);
        }
    }

    return (AM_OK);
}

/**
 * \brief ��ѯģʽ�½����ַ�
 */
static int __uart_poll_getchar(void *p_drv, char *p_char)
{
    am_hc32f460_uart_dev_t *p_dev = (am_hc32f460_uart_dev_t *) p_drv;
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;
    uint8_t *p_inchar = (uint8_t *) p_char;

    /* ����ģ���Ƿ���У�AM_FALSE:æ,���ڽ���; TURE: �Ѿ����յ�һ���ַ� */
    if (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_RX_VAL_FLAG) == AM_FALSE) {
        return (-AM_EAGAIN);
    }
    else {
        /* ����һ���ַ� */
        *p_inchar = amhw_hc32f460_uart_data_read(p_hw_uart);
    }

    return (AM_OK);
}

/**
 * \brief ���ô���ģʽ
 */
int __uart_mode_set(am_hc32f460_uart_dev_t *p_dev, uint32_t new_mode)
{
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;

    /* ��֧������ģʽ */
    if ((new_mode != AM_UART_MODE_POLL) && (new_mode != AM_UART_MODE_INT)) {
        return (AM_ERROR);
    }

    if (new_mode == AM_UART_MODE_INT) {
        if (p_dev->p_devinfo->dev_id == 1) {
            am_int_connect(p_dev->p_devinfo->inum, IRQ136_Handler,
                           NULL);
            amhw_hc32f460_intc_int_vssel_bits_set(p_dev->p_devinfo->inum, (0x1f << 22));
        } else if (p_dev->p_devinfo->dev_id == 2) {
            am_int_connect(p_dev->p_devinfo->inum, IRQ136_Handler,
                           NULL);
            amhw_hc32f460_intc_int_vssel_bits_set(p_dev->p_devinfo->inum, (0x1f << 27));
        } else if (p_dev->p_devinfo->dev_id == 3) {
            am_int_connect(p_dev->p_devinfo->inum, IRQ137_Handler,
                           NULL);
            amhw_hc32f460_intc_int_vssel_bits_set(p_dev->p_devinfo->inum, (0x1f << 0));
        } else if (p_dev->p_devinfo->dev_id == 4) {
            am_int_connect(p_dev->p_devinfo->inum, IRQ137_Handler,
                           NULL);
            amhw_hc32f460_intc_int_vssel_bits_set(p_dev->p_devinfo->inum, (0x1f << 5));
        }

        am_int_enable(p_dev->p_devinfo->inum);

        /* ʹ�ܽ�������ж� */
        amhw_hc32f460_uart_int_enable(p_hw_uart,AMHW_HC32F460_UART_INT_RX_VAL_ENABLE | (1 << 2));
    }
    else {

        /* �ر����д����ж� */
        amhw_hc32f460_uart_int_disable(p_hw_uart,
                AMHW_HC32F460_UART_INT_TIME_OUT_ENABLE    |\
                AMHW_HC32F460_UART_INT_RX_VAL_ENABLE      |\
                AMHW_HC32F460_UART_INT_TX_COMPLETE_ENABLE |\
                AMHW_HC32F460_UART_INT_TX_EMPTY_ENABLE    |\
                AMHW_HC32F460_UART_INT_ALL_ENABLE_MASK);
    }

    p_dev->channel_mode = new_mode;

    return (AM_OK);
}

/**
 * \brief ����ѡ������
 */
int __uart_opt_set(am_hc32f460_uart_dev_t *p_dev, uint32_t options)
{
    if (p_dev == NULL) {
        return -AM_EINVAL;
    }

    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;
    uint8_t cfg_flags = 0;

    /* �ڸı�UART�Ĵ���ֵǰ ���շ��ͽ��� */
    /* Set default value */
    p_hw_uart->CR1 = (uint32_t) 0xFFFFFFF3ul;
    p_hw_uart->CR1 = (uint32_t) 0x80000000ul;
    p_hw_uart->CR2 = (uint32_t) 0x00000000ul;
    p_hw_uart->CR3 = (uint32_t) 0x00000000ul;
    p_hw_uart->BRR = (uint32_t) 0x0000FFFFul;
    p_hw_uart->PR = (uint32_t) 0x00000000ul;

    /* �������ݳ��� */
    switch (options & AM_UART_CSIZE) {

    case AM_UART_CS5:
        cfg_flags |= AMHW_HC32F460_UART_DATA_5BIT;
        break;

    case AM_UART_CS6:
        cfg_flags |= AMHW_HC32F460_UART_DATA_6BIT;
        break;

    case AM_UART_CS7:
        cfg_flags |= AMHW_HC32F460_UART_DATA_7BIT;
        break;

    case AM_UART_CS8:
        cfg_flags |= AMHW_HC32F460_UART_DATA_8BIT;
        break;

    default:
        break;
    }

    /* ����ֹͣλ */
    if (options & AM_UART_STOPB) {
        cfg_flags &= ~(0x1 << 2);
        cfg_flags |= AMHW_HC32F460_UART_STOP_2BIT;
    }
    else {
        cfg_flags &= ~(0x1 << 2);
        cfg_flags |= AMHW_HC32F460_UART_STOP_1BIT;
    }

    /* ���ü��鷽ʽ */
    if (options & AM_UART_PARENB) {
        cfg_flags &= ~(0x3 << 0);

        if (options & AM_UART_PARODD) {
            cfg_flags |= AMHW_HC32F460_UART_PARITY_ODD;
        }
        else {
            cfg_flags |= AMHW_HC32F460_UART_PARITY_EVEN;
        }
    }
    else {
        cfg_flags &= ~(0x3 << 0);
        cfg_flags |= AMHW_HC32F460_UART_PARITY_NO;
    }

    /* �������Ч���� */

    if (AMHW_HC32F460_UART_STOP_1BIT
            == (cfg_flags & AMHW_HC32F460_UART_STOP_1BIT)) {
        amhw_hc32f460_uart_stop_bit_sel(p_hw_uart, 0);
    }
    else if (AMHW_HC32F460_UART_STOP_2BIT
            == (cfg_flags & AMHW_HC32F460_UART_STOP_2BIT)) {
        amhw_hc32f460_uart_stop_bit_sel(p_hw_uart, 1);
    }

    if (AMHW_HC32F460_UART_DATA_8BIT
            == (cfg_flags & AMHW_HC32F460_UART_DATA_8BIT)) {
        /* hc32f460 only support 8(0) or 9(1) bits */
        amhw_hc32f460_uart_data_length(p_hw_uart, 0);
    }
    else if (AMHW_HC32F460_UART_DATA_9BIT
            == (cfg_flags & AMHW_HC32F460_UART_DATA_9BIT)) {
        /* hc32f460 only support 8(0) or 9(1) bits */
        amhw_hc32f460_uart_data_length(p_hw_uart, 1);
    }

    switch ((cfg_flags & 0x3)) {
    case AMHW_HC32F460_UART_PARITY_NO:
        p_hw_uart->CR1_f.PCE = (uint32_t) 0ul;
        break;
    case AMHW_HC32F460_UART_PARITY_EVEN:
        p_hw_uart->CR1_f.PS = (uint32_t) 0ul;
        p_hw_uart->CR1_f.PCE = (uint32_t) 1ul;
        break;
    case AMHW_HC32F460_UART_PARITY_ODD:
        p_hw_uart->CR1_f.PS = (uint32_t) 1ul;
        p_hw_uart->CR1_f.PCE = (uint32_t) 1ul;
        break;
    default:
        break;
    }

    p_hw_uart->CR3_f.CTSE  = (uint32_t) (0);   //0-RTS���� 1-CTS����
    p_hw_uart->CR1_f.SBS   = (uint32_t) (1);    //1-��������RX�ܽ��½�����Ϊ��ʼλ
    p_hw_uart->CR1_f.OVER8 = (uint32_t) (1);  //1-8λUART������ģʽ
    p_dev->options = options;
    return (AM_OK);
}

/*******************************************************************************
 UART interrupt request handler
 *******************************************************************************/

/**
 * \brief ���ڽ����жϷ���
 */
void __uart_irq_rx_handler(am_hc32f460_uart_dev_t *p_dev)
{
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;
    char data;

    /* �Ƿ�Ϊ����Rx�ж� */
//    if (amhw_hc32f460_uart_int_flag_check(p_hw_uart, AMHW_HC32F460_UART_INT_RX_VAL_FLAG) == AM_TRUE) {     
    if (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_RX_VAL_FLAG) == AM_TRUE) {

//        amhw_hc32f460_uart_int_flag_clr(p_hw_uart, AMHW_HC32F460_UART_INT_RX_VAL_FLAG_CLR);

        /* ��ȡ�½������� */
        data = amhw_hc32f460_uart_data_read(p_hw_uart);

        /* ����½������� */
        p_dev->pfn_rxchar_put(p_dev->rxput_arg, data);
    }
}

/**
 * \brief ���ڷ����жϷ���
 */
void __uart_irq_tx_handler(am_hc32f460_uart_dev_t *p_dev)
{
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;

    char data;

//    if (amhw_hc32f460_uart_int_flag_check(p_hw_uart, AMHW_HC32F460_UART_INT_TX_EMPTY_FLAG) == AM_TRUE) {
    if (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_TX_EMPTY_FLAG) == AM_TRUE) {

//        amhw_hc32f460_uart_int_flag_clr(p_hw_uart, AMHW_HC32F460_UART_INT_TX_EMPTY_FLAG_CLR);

        /* ��ȡ�������ݲ����� */
        if ((p_dev->pfn_txchar_get(p_dev->txget_arg, &data)) == AM_OK) {
            amhw_hc32f460_uart_data_write(p_hw_uart, data);
        }
        else {

            /* û�����ݴ��;͹رշ����ж� */
            amhw_hc32f460_uart_int_disable(p_hw_uart,
            AMHW_HC32F460_UART_INT_TX_EMPTY_ENABLE);

            /* ����485���Ϳ������� */
            if ((p_dev->rs485_en) && (p_dev->p_devinfo->pfn_rs485_dir)) {

                /* ���� 485 Ϊ����ģʽ */
                p_dev->p_devinfo->pfn_rs485_dir(AM_FALSE);
            }
        }
    }
}

/**
 * \brief �����жϷ�����
 */
void __uart_irq_handler(void *p_arg)
{
    am_hc32f460_uart_dev_t *p_dev = (am_hc32f460_uart_dev_t *) p_arg;
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;

//    uint32_t uart_int_stat        = amhw_hc32f460_uart_int_flag_get(p_hw_uart);

//    if (amhw_hc32f460_uart_int_flag_check(p_hw_uart,AMHW_HC32F460_UART_INT_RX_VAL_FLAG) == AM_TRUE) {
    if (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_RX_VAL_FLAG) == AM_TRUE) {
        __uart_irq_rx_handler(p_dev);
//    } else if (amhw_hc32f460_uart_int_flag_check(p_hw_uart,AMHW_HC32F460_UART_INT_TX_EMPTY_FLAG) == AM_TRUE) {
    }
    else if (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_TX_EMPTY_FLAG) == AM_TRUE) {
        __uart_irq_tx_handler(p_dev);
    }
    else {

    }

    /* �����ж� */
//    if ((p_dev->other_int_enable & uart_int_stat) != 0) {
//        uart_int_stat &= p_dev->other_int_enable;
//        if (p_dev->pfn_err != NULL) {
//            p_dev->pfn_err(p_dev->err_arg,
//                           AM_HC32F460_UART_ERRCODE_UART_OTHER_INT,
//                           (void *)p_hw_uart,
//                           1);
//        }
//    }
}

/**
 * \brief Ĭ�ϻص�����
 *
 * \returns AW_ERROR
 */
static int __uart_dummy_callback(void *p_arg, char *p_outchar)
{
    return (AM_ERROR);
}

/**
 * \brief ���ڳ�ʼ������
 */
am_uart_handle_t am_hc32f460_uart_init(am_hc32f460_uart_dev_t *p_dev,
        const am_hc32f460_uart_devinfo_t *p_devinfo)
{
    amhw_hc32f460_uart_t *p_hw_uart;
    uint32_t tmp;

    if (p_devinfo == NULL) {
        return NULL;
    }

    /* ��ȡ���ò��� */
    p_hw_uart = (amhw_hc32f460_uart_t *) p_devinfo->uart_reg_base;
    p_dev->p_devinfo = p_devinfo;
    p_dev->uart_serv.p_funcs = (struct am_uart_drv_funcs *) &__g_uart_drv_funcs;
    p_dev->uart_serv.p_drv = p_dev;
    p_dev->baud_rate = p_devinfo->baud_rate;
    p_dev->options = 0;

    /* ��ʼ��Ĭ�ϻص����� */
    p_dev->pfn_txchar_get = (int (*)(void *, char*)) __uart_dummy_callback;
    p_dev->txget_arg = NULL;
    p_dev->pfn_rxchar_put = (int (*)(void *, char)) __uart_dummy_callback;
    p_dev->rxput_arg = NULL;
    p_dev->pfn_err = (int (*)(void *, int, void*, int)) __uart_dummy_callback;

    p_dev->err_arg = NULL;

//    p_dev->other_int_enable  = p_devinfo->other_int_enable  &
//                               ~(AMHW_HC32F460_UART_INT_TX_EMPTY_ENABLE |
//                                 AMHW_HC32F460_UART_INT_RX_VAL_ENABLE);
    p_dev->other_int_enable = p_devinfo->other_int_enable & ~(0 | 1);
    p_dev->rs485_en = AM_FALSE;

    /* ��ȡ�������ݳ�������ѡ�� */
    tmp = p_devinfo->cfg_flags;
    tmp = (tmp >> 4) & 0x03;

    switch (tmp) {

    case 0:
        p_dev->options |= AM_UART_CS5;
        break;

    case 1:
        p_dev->options |= AM_UART_CS6;
        break;
    case 2:
        p_dev->options |= AM_UART_CS7;
        break;
    case 3:
        p_dev->options |= AM_UART_CS8;
        break;

    default:
        p_dev->options |= AM_UART_CS8;
        break;
    }

    am_clk_enable(p_devinfo->clk_num);

    /* ��ȡ���ڼ��鷽ʽ����ѡ�� */
    tmp = p_devinfo->cfg_flags;
    tmp = (tmp >> 0) & 0x03;

    if (tmp == 1) {
        p_dev->options |= AM_UART_PARENB | AM_UART_PARODD;
    }
    else {
    }

    /* ��ȡ����ֹͣλ����ѡ�� */
    if (p_devinfo->cfg_flags & (AMHW_HC32F460_UART_STOP_2BIT)) {
        p_dev->options |= AM_UART_STOPB;
    }
    else {

    }

    /* �ȴ���һ�δ������ */
    while (amhw_hc32f460_uart_status_flag_check(p_hw_uart,
    AMHW_HC32F460_UART_TX_COMPLETE_FALG) == AM_FALSE)
        ;

    __uart_opt_set(p_dev, p_dev->options);

    /* ���ò����� */
    p_dev->baud_rate = amhw_hc32f460_uart_baudrate_set(p_hw_uart,
            am_clk_rate_get(p_dev->p_devinfo->clk_num), p_devinfo->baud_rate);
    /* Ĭ����ѯģʽ */
    __uart_mode_set(p_dev, AM_UART_MODE_POLL);

    /* uartʹ�� */
//    amhw_hc32f460_uart_enable(p_hw_uart);
//    amhw_hc32f460_uart_rx_enable(p_hw_uart, AM_TRUE);
//    amhw_hc32f460_uart_tx_enable(p_hw_uart, AM_TRUE);

    if (p_dev->p_devinfo->pfn_plfm_init) {
        p_dev->p_devinfo->pfn_plfm_init();
    }

    if (p_dev->p_devinfo->pfn_rs485_dir) {

        /* ��ʼ�� 485 Ϊ����ģʽ */
        p_dev->p_devinfo->pfn_rs485_dir(AM_FALSE);
    }

    return &(p_dev->uart_serv);
}

/**
 * \brief ����ȥ��ʼ��
 */
void am_hc32f460_uart_deinit(am_hc32f460_uart_dev_t *p_dev)
{
    amhw_hc32f460_uart_t *p_hw_uart =
            (amhw_hc32f460_uart_t *) p_dev->p_devinfo->uart_reg_base;
    p_dev->uart_serv.p_funcs = NULL;
    p_dev->uart_serv.p_drv = NULL;

    if (p_dev->channel_mode == AM_UART_MODE_INT) {

        /* Ĭ��Ϊ��ѯģʽ */
        __uart_mode_set(p_dev, AM_UART_MODE_POLL);
    }

    /* �رմ��� */
    amhw_hc32f460_uart_disable(p_hw_uart);

    am_int_disable(p_dev->p_devinfo->inum);

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

void Usart3RxErr_IrqHandler(void) {

}

void Usart3RxEnd_IrqHandler(am_hc32f460_uart_dev_t *p_dev) {
    __uart_irq_rx_handler(p_dev);
}

void Usart3TxEmpty_IrqHandler(am_hc32f460_uart_dev_t *p_dev) {
    __uart_irq_tx_handler(p_dev);
}

void Usart3TxEnd_IrqHandler(void) {

}

void Usart3RxTO_IrqHandler(void) {

}

/* end of file */