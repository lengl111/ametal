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
 * \brief HC32F460 UART �û������ļ�
 * \sa am_hc32f460_hwconfig_uart.c
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10  ari, first implementation
 * \endinternal
 */

#include "am_gpio.h"
#include "am_hc32f460_uart.h"
#include "am_clk.h"
#include "am_hc32f460.h"
#include "hw/amhw_hc32f460_gpio.h"

/**
 * \addtogroup am_if_src_hwconf_hc32f460_uart
 * \copydoc am_hwconf_hc32f460_uart.c
 * @{
 */

/** \brief ����3ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart3_init(void)
{
    am_clk_enable(CLK_USART3);
    am_gpio_pin_cfg(PIOE_4, GPIO_AFIO(AMHW_HC32F460_AFIO_USART3_RX));
    am_gpio_pin_cfg(PIOE_5, GPIO_AFIO(AMHW_HC32F460_AFIO_USART3_TX));
}

/** \brief �������1ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart3_deinit(void)
{
    am_clk_disable(CLK_USART3);

    am_gpio_pin_cfg(PIOE_4, AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOE_5, AM_GPIO_INPUT);
}

/** \brief ����3�豸��Ϣ */
static const am_hc32f460_uart_devinfo_t __g_uart3_devinfo = {

        HC32F460_UART3_BASE, /**< \brief ����3 */
        INUM_USART3,              /**< \brief ����3���жϱ�� */
        3,               /**< \brief ����3���豸��� */
        CLK_USART3, /**< \brief ����3��ʱ�� */

        AMHW_HC32F460_UART_DATA_8BIT | /**< \brief 8λ���� */
        AMHW_HC32F460_UART_PARITY_NO | /**< \brief �޼��� */
        AMHW_HC32F460_UART_STOP_1BIT, /**< \brief 1��ֹͣλ */

        115200, /**< \brief ���õĲ����� */

        0, /**< \brief �������ж� */

        NULL, /**< \brief UART1ʹ��RS485 */
        __hc32f460_plfm_uart3_init, /**< \brief UART1��ƽ̨��ʼ�� */
        __hc32f460_plfm_uart3_deinit, /**< \brief UART1��ƽ̨ȥ��ʼ�� */
};

/**< \brief ���崮��3 �豸 */
am_hc32f460_uart_dev_t __g_uart3_dev;

/** \brief UART1ʵ����ʼ�������uart1��׼������ */
am_uart_handle_t am_hc32f460_uart3_inst_init(void)
{
    return am_hc32f460_uart_init(&__g_uart3_dev, &__g_uart3_devinfo);
}

/** \brief UART1ʵ�����ʼ�� */
void am_hc32f460_uart3_inst_deinit(am_uart_handle_t handle)
{
    am_hc32f460_uart_deinit((am_hc32f460_uart_dev_t *) handle);
}

/*******************************************************************************/
#if 0
/** \brief ����2ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart2_init (void)
{
    am_gpio_pin_cfg(PIOA_2, PIOA_2_UART2_TX | PIOA_2_AF_PP);
    am_gpio_pin_cfg(PIOA_3, PIOA_3_UART2_RX | PIOA_3_INPUT_FLOAT);
}

/** \brief �������2 ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart2_deinit (void)
{

    /* �ر�UART2ʱ�� */
    am_clk_disable(CLK_UART2);

    am_gpio_pin_cfg(PIOA_3, AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOA_2, AM_GPIO_INPUT);
}

/** \brief ����2 �豸��Ϣ */
static const am_hc32f460_uart_devinfo_t __g_uart2_devinfo = {

    ZMF159_UART2_BASE, /**< \brief ����2 */
    INUM_UART2, /**< \brief ����2���жϱ�� */
    CLK_UART2, /**< \brief ����2��ʱ�� */

    AMHW_HC32F460_UART_DATA_8BIT | /**< \brief 8λ���� */
    AMHW_HC32F460_UART_PARITY_NO | /**< \brief �޼��� */
    AMHW_HC32F460_UART_STOP_1BIT, /**< \brief 1��ֹͣλ */

    115200, /**< \brief ���õĲ����� */

    0, /**< \brief �������ж� */

    NULL, /**< \brief UART2ʹ��RS485 */
    __hc32f460_plfm_uart2_init, /**< \brief UART2��ƽ̨��ʼ�� */
    __hc32f460_plfm_uart2_deinit, /**< \brief UART2��ƽ̨ȥ��ʼ�� */
};

/**< \brief ���崮��2�豸 */
static am_hc32f460_uart_dev_t __g_uart2_dev;

/** \brief UART2ʵ����ʼ�������uart2��׼������ */
am_uart_handle_t am_hc32f460_uart2_inst_init (void)
{
    return am_hc32f460_uart_init(&__g_uart2_dev, &__g_uart2_devinfo);
}

/** \brief UART2ʵ�����ʼ�� */
void am_hc32f460_uart2_inst_deinit (am_uart_handle_t handle)
{
    am_hc32f460_uart_deinit((am_hc32f460_uart_dev_t *)handle);
}

/*******************************************************************************/

/** \brief ����3ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart3_init (void)
{
//     am_gpio_pin_cfg(PIOB_10, PIOB_10_UART3_TX | PIOB_10_AF_PP | PIOB_10_SPEED_10MHz);
//     am_gpio_pin_cfg(PIOB_11, PIOB_11_UART3_RX | PIOB_11_INPUT_FLOAT);
}

/** \brief �������3 ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart3_deinit (void)
{
    am_clk_disable(CLK_UART3);

    am_gpio_pin_cfg(PIOB_10, AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOB_11, AM_GPIO_INPUT);
}

/** \brief ����3 �豸��Ϣ */
static const am_hc32f460_uart_devinfo_t __g_uart3_devinfo = {

    ZMF159_UART3_BASE, /**< \brief ����3 */
    INUM_UART3, /**< \brief ����2���жϱ�� */
    CLK_UART3, /**< \brief ����2��ʱ�� */

    AMHW_HC32F460_UART_DATA_8BIT | /**< \brief 8λ���� */
    AMHW_HC32F460_UART_PARITY_NO | /**< \brief �޼��� */
    AMHW_HC32F460_UART_STOP_1BIT, /**< \brief 1��ֹͣλ */

    115200, /**< \brief ���õĲ����� */

    0, /**< \brief �������ж� */

    NULL, /**< \brief UART3ʹ��RS485 */
    __hc32f460_plfm_uart3_init, /**< \brief UART3��ƽ̨��ʼ�� */
    __hc32f460_plfm_uart3_deinit, /**< \brief UART3��ƽ̨ȥ��ʼ�� */
};

/**< \brief ���崮��3�豸 */
static am_hc32f460_uart_dev_t __g_uart3_dev;

/** \brief UART3ʵ����ʼ�������uart3��׼������ */
am_uart_handle_t am_hc32f460_uart3_inst_init (void)
{
    return am_hc32f460_uart_init(&__g_uart3_dev, &__g_uart3_devinfo);
}

/** \brief UART3ʵ�����ʼ�� */
void am_hc32f460_uart3_inst_deinit (am_uart_handle_t handle)
{
    am_hc32f460_uart_deinit((am_hc32f460_uart_dev_t *)handle);
}

/*******************************************************************************/

/** \brief ����4ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart4_init (void)
{
}

/** \brief �������4 ƽ̨��ʼ�� */
static void __hc32f460_plfm_uart4_deinit (void)
{
    am_clk_disable(CLK_UART4);

}

/** \brief ����4 �豸��Ϣ */
static const am_hc32f460_uart_devinfo_t __g_uart4_devinfo = {

    ZMF159_UART4_BASE, /**< \brief ����4 */
    INUM_UART4, /**< \brief ����4���жϱ�� */
    CLK_UART4, /**< \brief ����4��ʱ�� */

    AMHW_HC32F460_UART_DATA_8BIT | /**< \brief 8λ���� */
    AMHW_HC32F460_UART_PARITY_NO | /**< \brief �޼��� */
    AMHW_HC32F460_UART_STOP_1BIT, /**< \brief 1��ֹͣλ */

    115200, /**< \brief ���õĲ����� */

    0, /**< \brief �������ж� */

    NULL, /**< \brief UART4ʹ��RS485 */
    __hc32f460_plfm_uart4_init, /**< \brief UART4��ƽ̨��ʼ�� */
    __hc32f460_plfm_uart4_deinit, /**< \brief UART4��ƽ̨ȥ��ʼ�� */
};

/**< \brief ���崮��4�豸 */
static am_hc32f460_uart_dev_t __g_uart4_dev;

/** \brief UART4ʵ����ʼ�������uart4��׼������ */
am_uart_handle_t am_hc32f460_uart4_inst_init (void)
{
    return am_hc32f460_uart_init(&__g_uart4_dev, &__g_uart4_devinfo);
}

/** \brief UART4ʵ�����ʼ�� */
void am_hc32f460_uart4_inst_deinit (am_uart_handle_t handle)
{
    am_hc32f460_uart_deinit((am_hc32f460_uart_dev_t *)handle);
}
#endif
/**
 * @}
 */

/* end of file */