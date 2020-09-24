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
 * \brief amf19x bootloader�����õ�Ӧ�ù���
 *
 * \internal
 * \par Modification history
 * - 1.00 19-10-13  zp, first implementation
 * \endinternal
 */

/**
 * \brief �������
 */
#include "ametal.h"
#include "am_board.h"
#include "am_vdebug.h"
#include "am_delay.h"
#include "am_softimer.h"
#include "demo_amf19x_core_entries.h"

int am_main (void)
{

    AM_DBG_INFO("Start up successful!\r\n");

    /* ����bootloader �Ĳ���Ӧ�ó��� demo */
    demo_hc32f19x_core_single_application_entry();

    /* uart ˫��bootloader �Ĳ���Ӧ�ó��� demo */
//    demo_hc32f19x_core_double_application_entry();


    while (1) {

    }
}

/* end of file */