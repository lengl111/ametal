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
 * \brief ������ bmp280 �����ļ�
 *
 * \internal
 * \par Modification history
 * - 1.00 18-11-29  yrz, first implementation.
 * \endinternal
 */

#include <am_aml165_inst_init.h>
#include "am_sensor_bmp280.h"
#include "am_common.h"
#include "zlg116_pin.h"

/** \brief ������ bmp280 �豸��Ϣʵ�� */
am_const am_local struct am_sensor_bmp280_devinfo __g_bmp280_info = {
        0x77               /*< \breif bmp280 I2C��ַ */
};

/** \breif ������ bmp280 �豸�ṹ�嶨�� */
am_local struct am_sensor_bmp280_dev __g_bmp280_dev;

/** \brief ������ bmp280 �豸ʵ���� */
am_sensor_handle_t am_sensor_bmp280_inst_init (void)
{
    return am_sensor_bmp280_init(&__g_bmp280_dev,
                                 &__g_bmp280_info,
                                 am_zlg116_i2c1_inst_init());
}

/** \brief ������ bmp280 ʵ�����ʼ�� */
am_err_t am_sensor_bmp280_inst_deinit (am_sensor_handle_t handle)
{
    return am_sensor_bmp280_deinit(handle);
}

/* end of file */