
#include <platform/ddp_reg.h>
#include <platform/ddp_misc.h>
#include "ddp_info.h"

void ddp_bypass_color(int idx, unsigned int width, unsigned int height, void * handle)
{
	DISP_REG_SET(handle,DISP_COLOR_INTERNAL_IP_WIDTH+(idx*DISP_INDEX_OFFSET), width); // width
	DISP_REG_SET(handle,DISP_COLOR_INTERNAL_IP_HEIGHT+(idx*DISP_INDEX_OFFSET), height); // height
	DISP_REG_SET(handle,DISP_COLOR_CFG_MAIN+(idx*DISP_INDEX_OFFSET), 0x200032bc); //bypass color engine
	DISP_REG_SET(handle,DISP_COLOR_START+(idx*DISP_INDEX_OFFSET), 0x2); // start
	DISP_REG_SET(handle,DISP_COLOR_START+(idx*DISP_INDEX_OFFSET), 0x3); // start
}

void ddp_bypass_ufo(unsigned int width, unsigned int height, void * handle)
{
	DISP_REG_SET(handle,DISP_REG_UFO_START, 0x4); //bypass mode
}

void ddp_bypass_aal(unsigned int width, unsigned int height, void * handle)
{
	DISP_REG_SET(handle,DISP_AAL_EN, 0x0); //bypass mode
	DISP_REG_SET_FIELD(handle,CFG_FLD_RELAY_MODE,   DISP_AAL_CFG, 0x1); //relay_mode
	DISP_REG_SET_FIELD(handle,SIZE_FLD_HSIZE,   DISP_AAL_SIZE, width); //width
	DISP_REG_SET_FIELD(handle,SIZE_FLD_VSIZE,   DISP_AAL_SIZE, height); //height
}

void ddp_bypass_gamma(unsigned int width, unsigned int height, void * handle)
{
	//Disable means bypass, so don't need to set any thing
	DISP_REG_SET(handle,DISP_REG_GAMMA_EN, 0x0); //bypass mode
	DISP_REG_SET(handle,DISP_REG_GAMMA_SIZE, (width<<16)|height);
}

void ddp_bypass_od(unsigned int width, unsigned int height, void * handle)
{
	DISP_REG_SET(handle,DISP_REG_OD_SIZE, (width<<16)|height);
	DISP_REG_SET(handle,DISP_REG_OD_CFG, 0x1);
	DISP_REG_SET(handle,DISP_REG_OD_EN, 0x1);
}

static int config_color(DISP_MODULE_ENUM module, disp_ddp_path_config* pConfig, void* cmdq)
{
	int color_idx;
	if (module == DISP_MODULE_COLOR0)
		color_idx = 0;
	else
		color_idx = 1;

	if (!pConfig->dst_dirty) {
		return 0;
	}
	ddp_bypass_color(color_idx,pConfig->dst_w, pConfig->dst_h,cmdq);
	return 0;
}

static int config_aal(DISP_MODULE_ENUM module, disp_ddp_path_config* pConfig, void* cmdq)
{
	if (!pConfig->dst_dirty) {
		return 0;
	}
	ddp_bypass_aal(pConfig->dst_w, pConfig->dst_h,cmdq);
	return 0;
}

static int config_od(DISP_MODULE_ENUM module, disp_ddp_path_config* pConfig, void* cmdq)
{
	if (!pConfig->dst_dirty) {
		return 0;
	}
	ddp_bypass_od(pConfig->dst_w, pConfig->dst_h,cmdq);
	return 0;
}

static int config_ufoe(DISP_MODULE_ENUM module, disp_ddp_path_config* pConfig, void* cmdq)
{
	if (!pConfig->dst_dirty) {
		return 0;
	}
	ddp_bypass_ufo(pConfig->dst_w, pConfig->dst_h,cmdq);
	return 0;
}

static int config_gamma(DISP_MODULE_ENUM module, disp_ddp_path_config* pConfig, void* cmdq)
{
	if (!pConfig->dst_dirty) {
		return 0;
	}
	ddp_bypass_gamma(pConfig->dst_w, pConfig->dst_h,cmdq);
	return 0;
}

static int clock_on(DISP_MODULE_ENUM module,void * handle)
{
	ddp_enable_module_clock(module);
	return 0;
}

static int clock_off(DISP_MODULE_ENUM module,void * handle)
{
	ddp_disable_module_clock(module);
	return 0;
}

///////////////////////////////////////////////////////////

// color
DDP_MODULE_DRIVER ddp_driver_color = {
	.init            = clock_on,
	.deinit          = clock_off,
	.config          = config_color,
	.start           = NULL,
	.trigger         = NULL,
	.stop            = NULL,
	.reset           = NULL,
	.power_on        = clock_on,
	.power_off       = clock_off,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};

// aal
DDP_MODULE_DRIVER ddp_driver_aal = {
	.init            = clock_on,
	.deinit          = clock_off,
	.config          = config_aal,
	.start           = NULL,
	.trigger         = NULL,
	.stop            = NULL,
	.reset           = NULL,
	.power_on        = clock_on,
	.power_off       = clock_off,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};

// od
DDP_MODULE_DRIVER ddp_driver_od = {
	.init            = clock_on,
	.deinit          = clock_off,
	.config          = config_od,
	.start           = NULL,
	.trigger         = NULL,
	.stop            = NULL,
	.reset           = NULL,
	.power_on        = clock_on,
	.power_off       = clock_off,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};
#if 0
// ufoe
DDP_MODULE_DRIVER ddp_driver_ufoe = {
	.init            = clock_on,
	.deinit          = clock_off,
	.config          = config_ufoe,
	.start           = NULL,
	.trigger         = NULL,
	.stop            = NULL,
	.reset           = NULL,
	.power_on        = clock_on,
	.power_off       = clock_off,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};
#endif
// gamma
DDP_MODULE_DRIVER ddp_driver_gamma = {
	.init            = clock_on,
	.deinit          = clock_off,
	.config          = config_gamma,
	.start           = NULL,
	.trigger         = NULL,
	.stop            = NULL,
	.reset           = NULL,
	.power_on        = clock_on,
	.power_off       = clock_off,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};
