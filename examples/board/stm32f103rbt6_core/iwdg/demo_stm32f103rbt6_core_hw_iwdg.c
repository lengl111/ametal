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
 * \brief IWDG ���̣�ͨ�� HW ��ӿ�ʵ��
 *
 * - ʵ������
 *   1. �޸ĺ궨�� __IWDG_FEED_TIME_MS ��ֵ������ 1500ms(���� 5ms ���)��оƬ��λ��
 *   2. �޸ĺ궨�� __IWDG_FEED_TIME_MS ��ֵ��С�� 1500ms(���� 5ms ���)�������������С�
 *
 * \par Դ����
 * \snippet demo_stm32f103rbt6_hw_iwdg.c src_stm32f103rbt6_hw_iwdg
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-26  sdy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_stm32f103rbt6_hw_iwdg
 * \copydoc demo_stm32f103rbt6_hw_iwdg.c
 */

/** [src_stm32f103rbt6_hw_iwdg] */
#include "ametal.h"
#include "am_board.h"
#include "am_vdebug.h"
#include "amhw_stm32f103rbt6_rcc.h"
#include "amhw_stm32f103rbt6_iwdg.h"
#include "demo_stm32f103rbt6_entries.h"
#include "demo_stm32f103rbt6_core_entries.h"

/**
 * \brief ���Ź���ʱʱ��
 *
 * \note ��Ϊ���Ź��ڲ�ʱ��WDTOSC����������__IWDG_TIMEOUT_MS��Ӧ��ʵ��ʱ��
 *       ������
 */
#define __IWDG_TIMEOUT_MS       100

/**
 * \brief ���Ź�ι��ʱ�䣬��ι��ʱ�䳬��__IWDG_TIMEOUT_MS��ֵ(�������Ź�ʱ�Ӳ���ȷ�������ܲ�ֵ��),��������Ź��¼���
 */
#define __IWDG_FEED_TIME_MS     1000

/**
 * \brief �������
 */
void demo_stm32f103rbt6_core_hw_iwdg_entry (void)
{
    AM_DBG_INFO("demo stm32f103rbt6_core std iwdg!\r\n");

    amhw_stm32f103rbt6_rcc_lsi_enable();

    demo_stm32f103rbt6_hw_iwdg_entry(STM32F103RBT6_IWDG,
                           __IWDG_TIMEOUT_MS,
                           __IWDG_FEED_TIME_MS);
}
/** [src_stm32f103rbt6_hw_iwdg] */

/* end of file */