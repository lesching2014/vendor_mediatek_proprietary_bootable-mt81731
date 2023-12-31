/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef PLL_H
//#include "custom_emi.h"
#define PLL_H

#define APMIXED_BASE            (0x10209000)
#define CKSYS_BASE              (0x10000000)
#define MMSYS_CONFIG_BASE       (0x14000000)
#define INFRACFG_AO_BASE        (0x10001000)
#define MCUCFG_BASE             (0x10200000)
#define CA15L_CONFIG_BASE       (0x10200200)

/* APMIXEDSYS Register */
#define AP_PLL_CON0             (APMIXED_BASE + 0x00)
#define AP_PLL_CON1             (APMIXED_BASE + 0x04)
#define AP_PLL_CON2             (APMIXED_BASE + 0x08)
#define AP_PLL_CON3             (APMIXED_BASE + 0x0C)
#define AP_PLL_CON4             (APMIXED_BASE + 0x10)
#define AP_PLL_CON5             (APMIXED_BASE + 0x14)
#define AP_PLL_CON6             (APMIXED_BASE + 0x18)
#define AP_PLL_CON7             (APMIXED_BASE + 0x1C)
#define CLKSQ_STB_CON0          (APMIXED_BASE + 0x20)
#define PLL_PWR_CON0            (APMIXED_BASE + 0x24)
#define PLL_PWR_CON1            (APMIXED_BASE + 0x28)
#define PLL_ISO_CON0            (APMIXED_BASE + 0x2C)
#define PLL_ISO_CON1            (APMIXED_BASE + 0x30)
#define PLL_STB_CON0            (APMIXED_BASE + 0x34)
#define DIV_STB_CON0            (APMIXED_BASE + 0x38)
#define PLL_CHG_CON0            (APMIXED_BASE + 0x3C)
#define PLL_TEST_CON0           (APMIXED_BASE + 0x40)

#define ARMCA15PLL_CON0         (APMIXED_BASE + 0x200)
#define ARMCA15PLL_CON1         (APMIXED_BASE + 0x204)
#define ARMCA15PLL_CON2         (APMIXED_BASE + 0x208)
#define ARMCA15PLL_PWR_CON0     (APMIXED_BASE + 0x20C)
                                                
#define ARMCA7PLL_CON0          (APMIXED_BASE + 0x210)
#define ARMCA7PLL_CON1          (APMIXED_BASE + 0x214)
#define ARMCA7PLL_CON2          (APMIXED_BASE + 0x218)
#define ARMCA7PLL_PWR_CON0      (APMIXED_BASE + 0x21C)
                                                
#define MAINPLL_CON0            (APMIXED_BASE + 0x220)
#define MAINPLL_CON1            (APMIXED_BASE + 0x224)
#define MAINPLL_PWR_CON0        (APMIXED_BASE + 0x22C)
                                                
#define UNIVPLL_CON0            (APMIXED_BASE + 0x230)
#define UNIVPLL_CON1            (APMIXED_BASE + 0x234)
#define UNIVPLL_PWR_CON0        (APMIXED_BASE + 0x23C)
                                                
#define MMPLL_CON0              (APMIXED_BASE + 0x240)
#define MMPLL_CON1              (APMIXED_BASE + 0x244)
#define MMPLL_CON2              (APMIXED_BASE + 0x248)
#define MMPLL_PWR_CON0          (APMIXED_BASE + 0x24C)
                                                
#define MSDCPLL_CON0            (APMIXED_BASE + 0x250)
#define MSDCPLL_CON1            (APMIXED_BASE + 0x254)
#define MSDCPLL_PWR_CON0        (APMIXED_BASE + 0x25C)
                                                
#define VENCPLL_CON0            (APMIXED_BASE + 0x260)
#define VENCPLL_CON1            (APMIXED_BASE + 0x264)
#define VENCPLL_PWR_CON0        (APMIXED_BASE + 0x26C)
                                                
#define TVDPLL_CON0             (APMIXED_BASE + 0x270)
#define TVDPLL_CON1             (APMIXED_BASE + 0x274)
#define TVDPLL_PWR_CON0         (APMIXED_BASE + 0x27C)
                                                
#define MPLL_CON0               (APMIXED_BASE + 0x280)
#define MPLL_CON1               (APMIXED_BASE + 0x284)
#define MPLL_PWR_CON0           (APMIXED_BASE + 0x28C)
                                                
#define VCODECPLL_CON0          (APMIXED_BASE + 0x290)
#define VCODECPLL_CON1          (APMIXED_BASE + 0x294)
#define VCODECPLL_PWR_CON0      (APMIXED_BASE + 0x29C)
                                                
#define APLL1_CON0              (APMIXED_BASE + 0x2A0)
#define APLL1_CON1              (APMIXED_BASE + 0x2A4)
#define APLL1_CON2              (APMIXED_BASE + 0x2A8)
#define APLL1_CON3              (APMIXED_BASE + 0x2AC)
#define APLL1_PWR_CON0          (APMIXED_BASE + 0x2B0)
                                                
#define APLL2_CON0              (APMIXED_BASE + 0x2B4)
#define APLL2_CON1              (APMIXED_BASE + 0x2B8)
#define APLL2_CON2              (APMIXED_BASE + 0x2BC)
#define APLL2_CON3              (APMIXED_BASE + 0x2C0)
#define APLL2_PWR_CON0          (APMIXED_BASE + 0x2C4)
                                                
#define LVDSPLL_CON0            (APMIXED_BASE + 0x02D0)
#define LVDSPLL_CON1            (APMIXED_BASE + 0x02D4)
#define LVDSPLL_CON2            (APMIXED_BASE + 0x02D8)
#define LVDSPLL_PWR_CON0        (APMIXED_BASE + 0x02DC)

#define MSDCPLL2_CON0           (APMIXED_BASE + 0x02F0)
#define MSDCPLL2_CON1           (APMIXED_BASE + 0x02F4)
#define MSDCPLL2_CON2           (APMIXED_BASE + 0x02F8)
#define MSDCPLL2_PWR_CON0       (APMIXED_BASE + 0x02FC)

#define AP_AUXADC_CON0          (APMIXED_BASE + 0x400)
#define AP_AUXADC_CON1          (APMIXED_BASE + 0x404)
#define TS_CON0                 (APMIXED_BASE + 0x600)
#define TS_CON1                 (APMIXED_BASE + 0x604)
#define AP_ABIST_MON_CON0       (APMIXED_BASE + 0x800)
#define AP_ABIST_MON_CON1       (APMIXED_BASE + 0x804)
#define AP_ABIST_MON_CON2       (APMIXED_BASE + 0x808)
#define AP_ABIST_MON_CON3       (APMIXED_BASE + 0x80C)
#define OCCSCAN_CON             (APMIXED_BASE + 0x810)
#define CLKDIV_CON0             (APMIXED_BASE + 0x814)

/* TOPCKGEN Register */
#define CLK_MODE                (CKSYS_BASE + 0x000)
#define DCM_CFG                 (CKSYS_BASE + 0x004)
#define TST_SEL_0               (CKSYS_BASE + 0x020)
#define TST_SEL_1               (CKSYS_BASE + 0x024)
#define TST_SEL_2               (CKSYS_BASE + 0x028)
#define CLK_CFG_0               (CKSYS_BASE + 0x040)
#define CLK_CFG_1               (CKSYS_BASE + 0x050)
#define CLK_CFG_2               (CKSYS_BASE + 0x060)
#define CLK_CFG_3               (CKSYS_BASE + 0x070)
#define CLK_CFG_4               (CKSYS_BASE + 0x080)
#define CLK_CFG_5               (CKSYS_BASE + 0x090)
#define CLK_CFG_6               (CKSYS_BASE + 0x0A0) 
#define CLK_CFG_7               (CKSYS_BASE + 0x0B0)
#define CLK_CFG_8               (CKSYS_BASE + 0x100)
#define CLK_CFG_9               (CKSYS_BASE + 0x104)
#define CLK_CFG_10              (CKSYS_BASE + 0x108)
#define CLK_CFG_11              (CKSYS_BASE + 0x10C)
#define CLK_CFG_12              (CKSYS_BASE + 0x0C0)
#define CLK_CFG_13              (CKSYS_BASE + 0x0D0)
#define CLK_SCP_CFG_0           (CKSYS_BASE + 0x200)
#define CLK_SCP_CFG_1           (CKSYS_BASE + 0x204)
#define CLK_MISC_CFG_0          (CKSYS_BASE + 0x210)
#define CLK_MISC_CFG_1          (CKSYS_BASE + 0x214)
#define CLK_MISC_CFG_2          (CKSYS_BASE + 0x218)
#define CLK26CALI_0             (CKSYS_BASE + 0x220)
#define CLK26CALI_1             (CKSYS_BASE + 0x224)
#define CLK26CALI_2             (CKSYS_BASE + 0x228)
#define CKSTA_REG               (CKSYS_BASE + 0x22C)
#define TEST_MODE_CFG           (CKSYS_BASE + 0x230)
#define MBIST_CFG_0             (CKSYS_BASE + 0x308)
#define MBIST_CFG_1             (CKSYS_BASE + 0x30C)
#define RESET_DEGLITCH_KEY      (CKSYS_BASE + 0x310)
#define MBIST_CFG_3             (CKSYS_BASE + 0x314)

/* INFRASYS Register */
#define TOP_CKMUXSEL            (INFRACFG_AO_BASE + 0x00)
#define TOP_CKDIV1              (INFRACFG_AO_BASE + 0x08)
#define TOP_DCMCTL              (INFRACFG_AO_BASE + 0x10)

#define TOPAXI_PROT_EN          (INFRACFG_AO_BASE + 0x0220)
#define TOPAXI_PROT_STA1        (INFRACFG_AO_BASE + 0x0228)

#define TOPAXI_PROT_EN1         (INFRACFG_AO_BASE + 0x0250)
#define TOPAXI_PROT_STA3        (INFRACFG_AO_BASE + 0x0258)

/* MCUSYS Register */
#define IR_ROSC_CTL             (MCUCFG_BASE + 0x030)
#define CA15L_MON_SEL           (CA15L_CONFIG_BASE + 0x01C)

/* MMSYS Register*/
#define DISP_CG_CON0            (MMSYS_CONFIG_BASE + 0x100)
#define DISP_CG_SET0            (MMSYS_CONFIG_BASE + 0x104)
#define DISP_CG_CLR0            (MMSYS_CONFIG_BASE + 0x108)
#define DISP_CG_CON1            (MMSYS_CONFIG_BASE + 0x110)
#define DISP_CG_SET1            (MMSYS_CONFIG_BASE + 0x114)
#define DISP_CG_CLR1            (MMSYS_CONFIG_BASE + 0x118)

#define SMI_LARB0_BASE          (0x14010000)
#define SMI_LARB0_STAT          ((UINT32P)(SMI_LARB0_BASE+0x000))
#define SMI_LARB0_OSTD_CTRL_EN  ((UINT32P)(SMI_LARB0_BASE+0x064))


enum {
    ARMCA15PLL = 0,
    ARMCA7PLL  = 1,
    MAINPLL    = 2,
    MSDCPLL    = 3,
    UNIVPLL    = 4,
    MMPLL      = 5,
    VENCPLL    = 6,
    TVDPLL     = 7,
    MPLL       = 8,
    VCODECPLL  = 9,
    APLL1      = 10,
    APLL2      = 11,
    NR_PLLS    = 12,
};

/* for MTCMOS */
#define STA_POWER_DOWN  0
#define STA_POWER_ON    1

#define DIS_PWR_STA_MASK    (0x1 << 3)

#define PWR_RST_B           (0x1 << 0)
#define PWR_ISO             (0x1 << 1)
#define PWR_ON              (0x1 << 2)
#define PWR_ON_S            (0x1 << 3)
#define PWR_CLK_DIS         (0x1 << 4)

#define SRAM_PDN            (0xf << 8)
#define DIS_SRAM_ACK        (0x1 << 12)

#define MD1_PROT_MASK     0x04B8//bit 3,4,5,7,10
#define MD_SRAM_PDN         (0x1 << 8)
#define MD1_PWR_STA_MASK    (0x1 << 0)

int spm_mtcmos_ctrl_disp(int state);

enum fod_mode {
	FREQ_OD,
	FREQ_PTPOD,
};

#define FCLK_VENCPLL	(1 << 0)
#define FCLK_VCODECPLL	(1 << 1)
#define FCLK_MMPLL	(1 << 2)
#define FCLK_AXI_CK	(1 << 3)
#define FCLK_VENCLT_CK	(1 << 4)
#define FCLK_CCI400_CK	(1 << 5)

void set_freq_od(enum fod_mode mode, unsigned long clks);

#endif