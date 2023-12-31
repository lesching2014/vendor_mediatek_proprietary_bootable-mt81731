/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/******************************************************************************
 * pmic_wrapper.c - MT6589 Linux pmic_wrapper Driver
 *
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     This file provid the other drivers PMIC relative functions
 *
 ******************************************************************************/
#include <stdio.h>

#include <platform/mt_pmic_wrap_init.h>

/******************************************************************************
 wrapper timeout
******************************************************************************/
#define PWRAP_TIMEOUT
//use the same API name with kernel driver
//however,the timeout API in uboot use tick instead of ns
#ifdef PWRAP_TIMEOUT
static U64 _pwrap_get_current_time(void)
{
	return gpt4_get_current_tick();
}
//_pwrap_timeout_tick,use the same API name with kernel driver
static BOOL _pwrap_timeout_ns (U64 start_time, U64 elapse_time)
{
	return gpt4_timeout_tick(start_time, elapse_time);
}
//_pwrap_time2tick_us
static U64 _pwrap_time2ns (U64 time_us)
{
	return gpt4_time2tick_us(time_us);
}

#else
static U64 _pwrap_get_current_time(void)
{
	return 0;
}
static BOOL _pwrap_timeout_ns (U64 start_time, U64 elapse_time)//,U64 timeout_ns)
{
	return FALSE;
}
static U64 _pwrap_time2ns (U64 time_us)
{
	return 0;
}

#endif
//#####################################################################
//define macro and inline function (for do while loop)
//#####################################################################
typedef U32 (*loop_condition_fp)(U32);//define a function pointer

static inline U32 wait_for_fsm_idle(U32 x)
{
	return (GET_WACS0_FSM( x ) != WACS_FSM_IDLE );
}
static inline U32 wait_for_fsm_vldclr(U32 x)
{
	return (GET_WACS0_FSM( x ) != WACS_FSM_WFVLDCLR);
}
static inline U32 wait_for_sync(U32 x)
{
	return (GET_SYNC_IDLE0(x) != WACS_SYNC_IDLE);
}
static inline U32 wait_for_idle_and_sync(U32 x)
{
	return ((GET_WACS0_FSM(x) != WACS_FSM_IDLE) || (GET_SYNC_IDLE0(x) != WACS_SYNC_IDLE)) ;
}
static inline U32 wait_for_wrap_idle(U32 x)
{
	return ((GET_WRAP_FSM(x) != 0x0) || (GET_WRAP_CH_DLE_RESTCNT(x) != 0x0));
}
static inline U32 wait_for_wrap_state_idle(U32 x)
{
	return ( GET_WRAP_AG_DLE_RESTCNT( x ) != 0 ) ;
}
static inline U32 wait_for_man_idle_and_noreq(U32 x)
{
	return ( (GET_MAN_REQ(x) != MAN_FSM_NO_REQ ) || (GET_MAN_FSM(x) != MAN_FSM_IDLE) );
}
static inline U32 wait_for_man_vldclr(U32 x)
{
	return  (GET_MAN_FSM( x ) != MAN_FSM_WFVLDCLR) ;
}
static inline U32 wait_for_cipher_ready(U32 x)
{
	return (x!=3) ;
}
static inline U32 wait_for_stdupd_idle(U32 x)
{
	return ( GET_STAUPD_FSM(x) != 0x0) ;
}

static inline U32 wait_for_state_ready_init(loop_condition_fp fp,U32 timeout_us,U32 wacs_register,U32 *read_reg)
{

	U64 start_time_ns=0, timeout_ns=0;
	U32 reg_rdata=0x0;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);
	do {
		reg_rdata = WRAP_RD32(wacs_register);

		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("timeout when waiting for idle\n");
			return E_PWR_TIMEOUT;
		}
	} while ( fp(reg_rdata)); //IDLE State
	if (read_reg)
		*read_reg=reg_rdata;
	return 0;
}


static inline U32 wait_for_state_ready(loop_condition_fp fp,U32 timeout_us,U32 wacs_register,U32 *read_reg)
{

	U64 start_time_ns=0, timeout_ns=0;
	U32 reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);
	do {
		reg_rdata = WRAP_RD32(wacs_register);

		if ( GET_INIT_DONE0( reg_rdata ) != WACS_INIT_DONE) {
			PWRAPERR("initialization isn't finished \n");
			return E_PWR_NOT_INIT_DONE;
		}
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("timeout when waiting for idle\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
	} while ( fp(reg_rdata)); //IDLE State
	if (read_reg)
		*read_reg=reg_rdata;
	return 0;
}
//******************************************************************************
//--external API for pmic_wrap user-------------------------------------------------
//******************************************************************************
S32 pwrap_read( U32  adr, U32 *rdata )
{
	return pwrap_wacs2( 0, adr,0,rdata );
}

S32 pwrap_write( U32  adr, U32  wdata )
{
	return pwrap_wacs2( 1, adr,wdata,0 );
}
//--------------------------------------------------------
//    Function : pwrap_wacs2()
// Description :
//   Parameter :
//      Return :
//--------------------------------------------------------
S32 pwrap_wacs2( U32  write, U32  adr, U32  wdata, U32 *rdata )
{
	U32 reg_rdata=0;
	U32 wacs_write=0;
	U32 wacs_adr=0;
	U32 wacs_cmd=0;
	U32 return_value=0;

	// Check argument validation
	if ( (write & ~(0x1))    != 0)  return E_PWR_INVALID_RW;
	if ( (adr   & ~(0xffff)) != 0)  return E_PWR_INVALID_ADDR;
	if ( (wdata & ~(0xffff)) != 0)  return E_PWR_INVALID_WDAT;

	// Check IDLE & INIT_DONE in advance
	return_value=wait_for_state_ready(wait_for_fsm_idle,TIMEOUT_WAIT_IDLE,PMIC_WRAP_WACS2_RDATA,0);
	if (return_value!=0) {
		PWRAPERR("wait_for_fsm_idle fail,return_value=%d\n",return_value);
		goto FAIL;
	}
	wacs_write  = write << 31;
	wacs_adr    = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;

	WRAP_WR32(PMIC_WRAP_WACS2_CMD,wacs_cmd);
	if ( write == 0 ) {
		if (NULL == rdata) {
			PWRAPERR("rdata is a NULL pointer\n");
			return_value= E_PWR_INVALID_ARG;
			goto FAIL;
		}
		return_value=wait_for_state_ready(wait_for_fsm_vldclr,TIMEOUT_READ,PMIC_WRAP_WACS2_RDATA,&reg_rdata);
		if (return_value!=0) {
			PWRAPERR("wait_for_fsm_vldclr fail,return_value=%d\n",return_value);
			return_value+=1;//E_PWR_NOT_INIT_DONE_READ or E_PWR_WAIT_IDLE_TIMEOUT_READ
			goto FAIL;
		}
		*rdata = GET_WACS0_RDATA( reg_rdata );
		WRAP_WR32(PMIC_WRAP_WACS2_VLDCLR , 1);
	}
FAIL:
	return return_value;
}
//******************************************************************************
//--internal API for pwrap_init-------------------------------------------------
//******************************************************************************

//--------------------------------------------------------
//    Function : _pwrap_wacs2_nochk()
// Description :
//   Parameter :
//      Return :
//--------------------------------------------------------

static S32 pwrap_read_nochk( U32  adr, U32 *rdata )
{
	return _pwrap_wacs2_nochk( 0, adr,  0, rdata );
}

static S32 pwrap_write_nochk( U32  adr, U32  wdata )
{
	return _pwrap_wacs2_nochk( 1, adr,wdata,0 );
}
static S32 _pwrap_wacs2_nochk( U32 write, U32 adr, U32 wdata, U32 *rdata )
{
	U32 reg_rdata=0x0;
	U32 wacs_write=0x0;
	U32 wacs_adr=0x0;
	U32 wacs_cmd=0x0;
	U32 return_value=0x0;

	// Check argument validation
	if ( (write & ~(0x1))    != 0)  return E_PWR_INVALID_RW;
	if ( (adr   & ~(0xffff)) != 0)  return E_PWR_INVALID_ADDR;
	if ( (wdata & ~(0xffff)) != 0)  return E_PWR_INVALID_WDAT;

	// Check IDLE
	return_value=wait_for_state_ready_init(wait_for_fsm_idle,TIMEOUT_WAIT_IDLE,PMIC_WRAP_WACS2_RDATA,0);
	if (return_value!=0) {
		if (NULL == rdata)
			return E_PWR_INVALID_ARG;
		PWRAPERR("_pwrap_wacs2_nochk write command fail,return_value=%x\n", return_value);
		return return_value;
	}

	wacs_write  = write << 31;
	wacs_adr    = (adr >> 1) << 16;
	wacs_cmd= wacs_write | wacs_adr | wdata;
	WRAP_WR32(PMIC_WRAP_WACS2_CMD,wacs_cmd);

	if ( write == 0 ) {

		// wait for read data ready
		return_value=wait_for_state_ready_init(wait_for_fsm_vldclr,TIMEOUT_WAIT_IDLE,PMIC_WRAP_WACS2_RDATA,&reg_rdata);
		if (return_value!=0) {
			PWRAPERR("_pwrap_wacs2_nochk read fail,return_value=%x\n", return_value);
			return return_value;
		}
		*rdata = GET_WACS0_RDATA( reg_rdata );
		WRAP_WR32(PMIC_WRAP_WACS2_VLDCLR , 1);
	}
	return 0;
}
//--------------------------------------------------------
//    Function : _pwrap_init_dio()
// Description :call it in pwrap_init,mustn't check init done
//   Parameter :
//      Return :
//--------------------------------------------------------
static S32 _pwrap_init_dio( U32 dio_en )
{
	U32 arb_en_backup=0x0;
	U32 rdata=0x0;
	U32 return_value=0;

	//PWRAPFUC();
	arb_en_backup = WRAP_RD32(PMIC_WRAP_HIPRIO_ARB_EN);
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , WACS2); // only WACS2
	pwrap_write_nochk(DEW_DIO_EN, dio_en);

	// Check IDLE & INIT_DONE in advance
	return_value=wait_for_state_ready_init(wait_for_idle_and_sync,TIMEOUT_WAIT_IDLE,PMIC_WRAP_WACS2_RDATA,0);
	if (return_value!=0) {
		PWRAPERR("_pwrap_init_dio fail,return_value=%x\n", return_value);
		return return_value;
	}
	WRAP_WR32(PMIC_WRAP_DIO_EN , dio_en);
	// Read Test
	pwrap_read_nochk(DEW_READ_TEST,&rdata);
	if ( rdata != DEFAULT_VALUE_READ_TEST ) {
		PWRAPERR("[Dio_mode][Read Test] fail,dio_en = %x, READ_TEST rdata=%x, exp=0x5aa5\n", dio_en, rdata);
		return E_PWR_READ_TEST_FAIL;
	}
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , arb_en_backup);
	return 0;
}

//--------------------------------------------------------
//    Function : _pwrap_init_cipher()
// Description :
//   Parameter :
//      Return :
//--------------------------------------------------------
static S32 _pwrap_init_cipher( void )
{
	U32 arb_en_backup=0;
	U32 rdata=0;
	U32 return_value=0;
	U32 start_time_ns=0, timeout_ns=0;

	arb_en_backup = WRAP_RD32(PMIC_WRAP_HIPRIO_ARB_EN);

	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , WACS2); // only WACS0

	WRAP_WR32(PMIC_WRAP_CIPHER_SWRST , 1);
	WRAP_WR32(PMIC_WRAP_CIPHER_SWRST , 0);
	WRAP_WR32(PMIC_WRAP_CIPHER_KEY_SEL , 1);
	WRAP_WR32(PMIC_WRAP_CIPHER_IV_SEL  , 2);
	WRAP_WR32(PMIC_WRAP_CIPHER_EN   , 1);

	//Config CIPHER @ PMIC
	pwrap_write_nochk(DEW_CIPHER_SWRST,   0x1);
	pwrap_write_nochk(DEW_CIPHER_SWRST,   0x0);
	pwrap_write_nochk(DEW_CIPHER_KEY_SEL, 0x1);
	pwrap_write_nochk(DEW_CIPHER_IV_SEL,  0x2);
	pwrap_write_nochk(DEW_CIPHER_LOAD,    0x1);
	pwrap_write_nochk(DEW_CIPHER_START,   0x1);

	//wait for cipher data ready@AP
	return_value=wait_for_state_ready_init(wait_for_cipher_ready,TIMEOUT_WAIT_IDLE,PMIC_WRAP_CIPHER_RDY,0);
	if (return_value!=0) {
		PWRAPERR("wait for cipher data ready@AP fail,return_value=%x\n", return_value);
		return return_value;
	}

	//wait for cipher data ready@PMIC
	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(TIMEOUT_WAIT_IDLE);
	do {
		pwrap_read_nochk(DEW_CIPHER_RDY,&rdata);
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("timeout when waiting for idle\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
	} while ( rdata != 0x1 ); //cipher_ready

	pwrap_write_nochk(DEW_CIPHER_MODE, 0x1);
	//wait for cipher mode idle
	return_value=wait_for_state_ready_init(wait_for_idle_and_sync,TIMEOUT_WAIT_IDLE,PMIC_WRAP_WACS2_RDATA,0);
	if (return_value!=0) {
		PWRAPERR("wait for cipher mode idle fail,return_value=%x\n", return_value);
		return return_value;
	}
	WRAP_WR32(PMIC_WRAP_CIPHER_MODE , 1);

	// Read Test
	pwrap_read_nochk(DEW_READ_TEST,&rdata);
	if ( rdata != DEFAULT_VALUE_READ_TEST ) {
		PWRAPERR("_pwrap_init_cipher,read test error,error code=%x, rdata=%x\n", 1, rdata);
		return E_PWR_READ_TEST_FAIL;
	}

	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , arb_en_backup);
	return 0;
}

//--------------------------------------------------------
//    Function : _pwrap_init_sidly()
// Description :
//   Parameter :
//      Return :
//--------------------------------------------------------
static S32 _pwrap_init_sidly( void )
{
	U32 arb_en_backup = 0;
	U32 rdata = 0;
	S32 ind = 0;
	U32 tmp1 = 0;
	U32 tmp2 = 0;
	U32 result_faulty = 0;
	U32 result = 0;
	U32 leading_one, tailing_one;

	arb_en_backup = WRAP_RD32(PMIC_WRAP_HIPRIO_ARB_EN);
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , WACS2); // only WACS2

	/* --------------------------------------------------------------------- */
	/* Scan all possible input strobe by READ_TEST */
	/* --------------------------------------------------------------------- */
	/* 24 sampling clock edge */
	for (ind = 0; ind < 24; ind++) { /* 24 sampling clock edge */
		WRAP_WR32(PMIC_WRAP_SI_CK_CON, (ind >> 2) & 0x7);
		WRAP_WR32(PMIC_WRAP_SIDLY, 0x3 - (ind & 0x3));
		_pwrap_wacs2_nochk(0, DEW_READ_TEST, 0, &rdata);
		if ( rdata == DEFAULT_VALUE_READ_TEST ) {
			PWRAPLOG("_pwrap_init_sidly [Read Test] pass,index=%d rdata=%x\n", ind,rdata);
			result |= (0x1 << ind);
		} else
			PWRAPLOG("_pwrap_init_sidly [Read Test] fail,index=%d,rdata=%x\n", ind,rdata);

	}
	/* --------------------------------------------------------------------- */
	/* Locate the leading one and trailing one of PMIC 1/2 */
	/* --------------------------------------------------------------------- */
	for ( ind=23 ; ind>=0 ; ind-- ) {
		if ( result & (0x1 << ind) ) break;
	}
	leading_one = ind;

	for ( ind=0 ; ind<24 ; ind++ ) {
		if ( result & (0x1 << ind) ) break;
	}
	tailing_one = ind;

	/* --------------------------------------------------------------------- */
	/* Check the continuity of pass range */
	/* --------------------------------------------------------------------- */
	tmp1 = (0x1 << (leading_one+1)) - 1;
	tmp2 = (0x1 << tailing_one) - 1;
	if ( (tmp1 - tmp2) != result ) {
		/*TERR = "[DrvPWRAP_InitSiStrobe] Fail, tmp1:%d, tmp2:%d", tmp1, tmp2*/
		PWRAPERR("_pwrap_init_sidly Fail,tmp1=%x,tmp2=%x\n", tmp1,tmp2);
		result_faulty = 0x1;
	}


	/* --------------------------------------------------------------------- */
	/* Config SICK and SIDLY to the middle point of pass range */
	/* --------------------------------------------------------------------- */
	ind = (leading_one + tailing_one)/2;
	WRAP_WR32(PMIC_WRAP_SI_CK_CON , (ind >> 2) & 0x7);
	WRAP_WR32(PMIC_WRAP_SIDLY , 0x3 - (ind & 0x3));

	/* --------------------------------------------------------------------- */
	/* Restore */
	/* --------------------------------------------------------------------- */
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , arb_en_backup);
	if ( result_faulty == 0 )
		return 0;
	else {
		PWRAPERR("_pwrap_init_sidly Fail,result=%x\n", result);
		return result_faulty;
	}

}

//--------------------------------------------------------
//    Function : _pwrap_reset_spislv()
// Description :
//   Parameter :
//      Return :
//--------------------------------------------------------
static S32 _pwrap_reset_spislv( void )
{
	U32 ret=0;
	U32 return_value=0;

	// This driver does not using _pwrap_switch_mux
	// because the remaining requests are expected to fail anyway

	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN , 0);
	WRAP_WR32(PMIC_WRAP_WRAP_EN , 0);
	WRAP_WR32(PMIC_WRAP_MUX_SEL , 1);
	WRAP_WR32(PMIC_WRAP_MAN_EN ,1);
	WRAP_WR32(PMIC_WRAP_DIO_EN , 0);

	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_CSL  << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_OUTS << 8)); //to reset counter
	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_CSH  << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD , (OP_WR << 13) | (OP_OUTS << 8));

	return_value=wait_for_state_ready_init(wait_for_sync,TIMEOUT_WAIT_IDLE,PMIC_WRAP_WACS2_RDATA,0);
	if (return_value!=0) {
		PWRAPERR("_pwrap_reset_spislv fail,return_value=%x\n", return_value);
		ret=E_PWR_TIMEOUT;
		goto timeout;
	}

	WRAP_WR32(PMIC_WRAP_MAN_EN , 0);
	WRAP_WR32(PMIC_WRAP_MUX_SEL , 0);

timeout:
	WRAP_WR32(PMIC_WRAP_MAN_EN , 0);
	WRAP_WR32(PMIC_WRAP_MUX_SEL , 0);
	return ret;
}

static S32 _pwrap_init_reg_clock( U32 regck_sel )
{
	U32 wdata=0;
	U32 rdata=0;

	/* Set reg clk freq */
	pwrap_read_nochk(PMIC_TOP_CKCON2, &rdata);
	if (regck_sel == 1)
		wdata = (rdata & (~(0x3 << 10))) | (0x1 << 10);
	else
		wdata = rdata & (~(0x3 << 10));

	pwrap_write_nochk(PMIC_TOP_CKCON2, wdata);
	pwrap_read_nochk(PMIC_TOP_CKCON2, &rdata);
	if (rdata != wdata) {
		PWRAPERR("pwrap_init_reg_clock,PMIC_TOP_CKCON2 Write [15]=1 Fail, rdata=%x\n",
		         rdata);
		return E_PWR_INIT_REG_CLOCK;
	}
	// Config SPI Waveform according to reg clk
	if ( regck_sel == 1 ) { //12Mhz
		WRAP_WR32(PMIC_WRAP_RDDMY           , 0xc);
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE    , 0x0);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ     , 0x4);
		WRAP_WR32(PMIC_WRAP_CSLEXT_START    , 0x0);
		WRAP_WR32(PMIC_WRAP_CSLEXT_END      , 0x4);
	} else if ( regck_sel == 2 ) { // 24Mhz
		WRAP_WR32(PMIC_WRAP_RDDMY           , 0xc);
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE    , 0x0);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ     , 0x4);
		WRAP_WR32(PMIC_WRAP_CSLEXT_START    , 0x2);
		WRAP_WR32(PMIC_WRAP_CSLEXT_END      , 0x2);
	} else { //Safe mode
		WRAP_WR32(PMIC_WRAP_RDDMY           , 0xf);
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE    , 0xf);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ     , 0xf);
		WRAP_WR32(PMIC_WRAP_CSLEXT_START    , 0xf);
		WRAP_WR32(PMIC_WRAP_CSLEXT_END      , 0xf);
	}
	return 0;

}
S32 pwrap_init ( void )
{
	S32 sub_return=0;
	S32 sub_return1=0;
	U32 rdata=0x0;
	PWRAPFUC();

	WRAP_SET_BIT(1<<7,INFRA_GLOBALCON_RST0);
	PWRAPLOG("the reset register =%x\n",WRAP_RD32(INFRA_GLOBALCON_RST0));
	PWRAPLOG("PMIC_WRAP_STAUPD_GRPEN =0x%x,it should be equal to 0xc\n",WRAP_RD32(PMIC_WRAP_STAUPD_GRPEN));
	//clear reset bit
	WRAP_CLR_BIT(1<<7,INFRA_GLOBALCON_RST0);

	//###############################
	// Set SPI_CK_freq = 26MHz
	//###############################
	WRAP_WR32(CLK_CFG_5_CLR,CLK_SPI_CK_26M);

	//###############################
	//Enable DCM
	//###############################
	WRAP_WR32(PMIC_WRAP_DCM_EN , 3);
	WRAP_WR32(PMIC_WRAP_DCM_DBC_PRD ,0);

	//###############################
	//Reset SPISLV
	//###############################
	sub_return=_pwrap_reset_spislv();
	if ( sub_return != 0 ) {
		PWRAPERR("error,_pwrap_reset_spislv fail,sub_return=%x\n",sub_return);
		return E_PWR_INIT_RESET_SPI;
	}

	//###############################
	// Enable WACS2
	//###############################
	WRAP_WR32(PMIC_WRAP_WRAP_EN,1);//enable wrap
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN,WACS2); //Only WACS2
	WRAP_WR32(PMIC_WRAP_WACS2_EN,1);

	//###############################
	// SIDLY setting
	//###############################
	sub_return = _pwrap_init_sidly();
	if ( sub_return != 0 ) {
		PWRAPERR("error,_pwrap_init_sidly fail,sub_return=%x\n",sub_return);
		return E_PWR_INIT_SIDLY;
	}
	//###############################
	// SPI Waveform Configuration
	// 0:safe mode, 1:12MHz, 2:24MHz //2
	//###############################
	sub_return = _pwrap_init_reg_clock(2);
	if ( sub_return != 0)  {
		PWRAPERR("error,_pwrap_init_reg_clock fail,sub_return=%x\n",sub_return);
		return E_PWR_INIT_REG_CLOCK;
	}

	//###############################
	// Enable PMIC
	// (May not be necessary, depending on S/W partition)
	//###############################
	pwrap_read_nochk(PMIC_WRP_CKPDN,&rdata);
	sub_return= pwrap_write_nochk(PMIC_WRP_CKPDN,   rdata & 0x50);
	sub_return1=pwrap_write_nochk(PMIC_WRP_RST_CON, 0);//clear dewrap reset bit
	if (( sub_return != 0 )||( sub_return1 != 0 )) {
		PWRAPERR("Enable PMIC fail, sub_return=%x sub_return1=%x\n", sub_return,sub_return1);
		return E_PWR_INIT_ENABLE_PMIC;
	}

	//###############################
	// Enable DIO mode
	//###############################
	sub_return = _pwrap_init_dio(3);
	if ( sub_return != 0 ) {
		PWRAPERR("_pwrap_init_dio test error,error code=%x, sub_return=%x\n", 0x11, sub_return);
		return E_PWR_INIT_DIO;
	}

	//###############################
	// Enable Encryption
	//###############################
	sub_return = _pwrap_init_cipher();
	if ( sub_return != 0 ) {
		PWRAPERR("Enable Encryption fail, return=%x, sub_return=%d\n", 0x21, sub_return);
		return E_PWR_INIT_CIPHER;
	}

	//###############################
	// Write test using WACS2
	//###############################
	sub_return = pwrap_write_nochk(DEW_WRITE_TEST, WRITE_TEST_VALUE);
	sub_return1 = pwrap_read_nochk(DEW_WRITE_TEST,&rdata);
	if (( rdata != WRITE_TEST_VALUE )||( sub_return != 0 )||( sub_return1 != 0 )) {
		PWRAPERR("write test error,rdata=%x,exp=0xa55a,sub_return=%d,sub_return1=%d\n", rdata,sub_return,sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}

	//###############################
	// Signature Checking - Using CRC
	// should be the last to modify WRITE_TEST
	//###############################
	sub_return=pwrap_write_nochk(DEW_CRC_EN, 0x1);
	if ( sub_return != 0 ) {
		PWRAPERR("enable CRC fail,sub_return=%x\n", sub_return);
		return E_PWR_INIT_ENABLE_CRC;
	}
	WRAP_WR32(PMIC_WRAP_CRC_EN ,0x1);
	WRAP_WR32(PMIC_WRAP_SIG_MODE, 0x0);
	WRAP_WR32(PMIC_WRAP_SIG_ADR , DEW_CRC_VAL);


	//###############################
	// PMIC_WRAP enables
	//###############################
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN,0x1ff);
	WRAP_WR32(PMIC_WRAP_WACS0_EN,0x1);
	WRAP_WR32(PMIC_WRAP_WACS1_EN,0x1);
	WRAP_WR32(PMIC_WRAP_WACS2_EN,0x1);//already enabled
	WRAP_WR32(PMIC_WRAP_STAUPD_PRD, 0x5);  //0x1:20us,for concurrence test,MP:0x5;  //100us
	WRAP_WR32(PMIC_WRAP_STAUPD_GRPEN,0xf1);
	WRAP_WR32(PMIC_WRAP_WDT_UNIT,0xf);
	WRAP_WR32(PMIC_WRAP_WDT_SRC_EN,0xffffffff);
	WRAP_WR32(PMIC_WRAP_TIMER_EN,0x1);
	WRAP_WR32(PMIC_WRAP_INT_EN,0x7fffffff); //except for [31] debug_int

	//###############################
	// switch event pin from usbdl mode to normal mode for pmic interrupt, NEW@MT6397
	//###############################
	pwrap_read_nochk(PMIC_TOP_CKCON3,&rdata);
	sub_return = pwrap_write_nochk(PMIC_TOP_CKCON3, (rdata & 0x0007));
	if ( sub_return != 0 ) {
		PWRAPERR("!!switch event pin fail,sub_return=%d\n", sub_return);
	}

	//###############################
	// PERI_PWRAP_BRIDGE enables
	//###############################
	WRAP_WR32(PERI_PWRAP_BRIDGE_IORD_ARB_EN, 0x7f);
	WRAP_WR32(PERI_PWRAP_BRIDGE_WACS3_EN , 0x1);
	WRAP_WR32(PERI_PWRAP_BRIDGE_WACS4_EN ,0x1);
	WRAP_WR32(PERI_PWRAP_BRIDGE_WDT_UNIT , 0xf);
	WRAP_WR32(PERI_PWRAP_BRIDGE_WDT_SRC_EN , 0xffff);
	WRAP_WR32(PERI_PWRAP_BRIDGE_TIMER_EN  , 0x1);
	WRAP_WR32(PERI_PWRAP_BRIDGE_INT_EN    , 0x7ff);

	//###############################
	// PMIC_DEWRAP enables
	//###############################
	sub_return  = pwrap_write_nochk(DEW_EVENT_OUT_EN, 0x1);
	sub_return1 = pwrap_write_nochk(DEW_EVENT_SRC_EN, 0xffff);
	if (( sub_return != 0 )||( sub_return1 != 0 )) {
		PWRAPERR("enable dewrap fail,sub_return=%d,sub_return1=%d\n", sub_return,sub_return1);
		return E_PWR_INIT_ENABLE_DEWRAP;
	}

	//###############################
	// Initialization Done
	//###############################
	WRAP_WR32(PMIC_WRAP_INIT_DONE2 , 0x1);

	return 0;
}
#ifdef MACH_FPGA
S32 pwrap_init_lk ( void )
{
	u32 pwrap_ret=0,i=0;
	PWRAPFUC();
	for (i=0; i<3; i++) {
		pwrap_ret = pwrap_init();
		if (pwrap_ret!=0) {
			dprintf(CRITICAL, "[PMIC_WRAP]wrap_init fail,the return value=%x.\n",pwrap_ret);
		} else {
			dprintf(CRITICAL, "[PMIC_WRAP]wrap_init pass,the return value=%x.\n",pwrap_ret);
			break;//init pass
		}
	}
	return 0;
}


//--------------------------------------------------------
//    Function : _pwrap_status_update_test()
// Description :only for early porting
//   Parameter :
//      Return :
//--------------------------------------------------------
static S32 _pwrap_status_update_test_porting( void )
{
	U32 i, j;
	U32 rdata;
	volatile U32 delay=1000*1000*1;
	PWRAPFUC();
	//disable signature interrupt
	WRAP_WR32(PMIC_WRAP_INT_EN,0x0);
	pwrap_write(DEW_WRITE_TEST, 0x55AA);
	WRAP_WR32(PMIC_WRAP_SIG_ADR,DEW_WRITE_TEST);
	WRAP_WR32(PMIC_WRAP_SIG_VALUE,0xAA55);
	WRAP_WR32(PMIC_WRAP_SIG_MODE, 0x1);

	//pwrap_delay_us(5000);//delay 5 seconds

	while (delay--);

	rdata=WRAP_RD32(PMIC_WRAP_SIG_ERRVAL);
	if ( rdata != 0x55AA ) {
		PWRAPERR("_pwrap_status_update_test error,error code=%x, rdata=%x\n", 1, rdata);
		//return 1;
	}
	WRAP_WR32(PMIC_WRAP_SIG_VALUE,0x55AA);//tha same as write test
	//clear sig_error interrupt flag bit
	WRAP_WR32(PMIC_WRAP_INT_CLR,1<<1);

	//enable signature interrupt
	WRAP_WR32(PMIC_WRAP_INT_EN,0x7fffffff);
#if 0
	WRAP_WR32(PMIC_WRAP_SIG_MODE, 0x0);
	WRAP_WR32(PMIC_WRAP_SIG_ADR , DEW_CRC_VAL);
#endif
	return 0;
}

int  pwrap_init_for_early_porting(void)
{
	int ret = 0;
	U32 res=0;
	PWRAPFUC();

	ret=_pwrap_status_update_test_porting();
	if (ret==0) {
		PWRAPLOG("wrapper_StatusUpdateTest pass.\n");
	} else {
		PWRAPLOG("error:wrapper_StatusUpdateTest fail.\n");
		res+=1;
	}

}
#endif
