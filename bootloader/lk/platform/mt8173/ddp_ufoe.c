#define LOG_TAG "UFOE"
#include "platform/ddp_log.h"

#include "platform/ddp_info.h"
#include "platform/ddp_hal.h"
#include "platform/ddp_reg.h"

static bool ufoe_enable = false;

static void ufoe_dump(void)
{
	printf("==DISP UFOE REGS==\n");
	printf("(0x000)UFOE_START =0x%x\n", DISP_REG_GET(DISP_REG_UFO_START));
	printf("(0x000)UFOE_CFG0 =0x%x\n", DISP_REG_GET(DISP_REG_UFO_CFG_0B));
	printf("(0x000)UFOE_CFG1 =0x%x\n", DISP_REG_GET(DISP_REG_UFO_CFG_1B));
	printf("(0x000)UFOE_WIDTH =0x%x\n", DISP_REG_GET(DISP_REG_UFO_FRAME_WIDTH));
	printf("(0x000)UFOE_HEIGHT =0x%x\n", DISP_REG_GET(DISP_REG_UFO_FRAME_HEIGHT));
	printf("(0x000)UFOE_PAD  =0x%x\n", DISP_REG_GET(DISP_REG_UFO_CR0P6_PAD));
}

static int ufoe_init(DISP_MODULE_ENUM module,void * handle)
{
	return 0;
}

static int ufoe_deinit(DISP_MODULE_ENUM module,void * handle)
{
	return 0;
}

int ufoe_start(DISP_MODULE_ENUM module, void* cmdq)
{
	if (ufoe_enable) {
		DISP_REG_SET_FIELD(cmdq, START_FLD_DISP_UFO_START, DISP_REG_UFO_START, 1);
	}
	DDPMSG("ufoe_start, ufoe_start:0x%x\n", DISP_REG_GET(DISP_REG_UFO_START));
	return 0;
}


int ufoe_stop(DISP_MODULE_ENUM module, void *cmdq_handle)
{
	DISP_REG_SET_FIELD(cmdq_handle, START_FLD_DISP_UFO_START, DISP_REG_UFO_START, 0);
	DDPMSG("ufoe_stop, ufoe_start:0x%x\n", DISP_REG_GET(DISP_REG_UFO_START));
	return 0;
}

static int ufoe_config(DISP_MODULE_ENUM module, disp_ddp_path_config *pConfig, void *handle)
{
	LCM_PARAMS *disp_if_config = &(pConfig->dispif_config);
	LCM_DSI_PARAMS *lcm_config = &(disp_if_config->dsi);
	if (lcm_config->ufoe_enable) {
		ufoe_enable = true;
#ifdef CONFIG_FPGA_EARLY_PORTING     //FOR BRING_UP
		DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_BYPASS, DISP_REG_UFO_START, 1);   /* enable BYPASS ufoe */
#else
		DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_BYPASS, DISP_REG_UFO_START, 0);   /* disable BYPASS ufoe */
#endif
		/* DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_START, DISP_REG_UFO_START, 1); */
		if (lcm_config->ufoe_params.lr_mode_en == 1) {
			DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_LR_EN, DISP_REG_UFO_START, 1);
		} else {
			DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_LR_EN, DISP_REG_UFO_START, 0);
			if (lcm_config->ufoe_params.compress_ratio == 3) {
				unsigned int internal_width =
				    disp_if_config->width + disp_if_config->width % 4;
				DISP_REG_SET_FIELD(handle, CFG_0B_FLD_DISP_UFO_CFG_COM_RATIO,
				                   DISP_REG_UFO_CFG_0B, 1);
				if (internal_width % 6 != 0) {
					DISP_REG_SET_FIELD(handle,
					                   CR0P6_PAD_FLD_DISP_UFO_STR_PAD_NUM,
					                   DISP_REG_UFO_CR0P6_PAD,
					                   ((internal_width / 6 + 1) * 6 -
					                    internal_width));
				}
			} else
				DISP_REG_SET_FIELD(handle, CFG_0B_FLD_DISP_UFO_CFG_COM_RATIO,
				                   DISP_REG_UFO_CFG_0B, 0);

			if (lcm_config->ufoe_params.vlc_disable) {
				DISP_REG_SET_FIELD(handle, CFG_0B_FLD_DISP_UFO_CFG_VLC_EN,
				                   DISP_REG_UFO_CFG_0B, 0);
				DISP_REG_SET(handle, DISP_REG_UFO_CFG_1B, 0x1);
			} else {
				DISP_REG_SET_FIELD(handle, CFG_0B_FLD_DISP_UFO_CFG_VLC_EN,
				                   DISP_REG_UFO_CFG_0B, 1);
				DISP_REG_SET(handle, DISP_REG_UFO_CFG_1B,
				             (lcm_config->ufoe_params.vlc_config ==
				              0) ? 5 : lcm_config->ufoe_params.vlc_config);
			}
		}
		DISP_REG_SET(handle, DISP_REG_UFO_FRAME_WIDTH, disp_if_config->width);
		DISP_REG_SET(handle, DISP_REG_UFO_FRAME_HEIGHT, disp_if_config->height);
	}
	/* ufoe_dump(); */
	return 0;

}

static int ufoe_clock_on(DISP_MODULE_ENUM module,void * handle)
{
	DDPMSG("ufoe_clock on CG 0x%x \n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
	return 0;
}

static int ufoe_clock_off(DISP_MODULE_ENUM module,void * handle)
{
	DDPMSG("ufoe_clock off CG 0x%x \n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
	return 0;
}

static int ufoe_reset(DISP_MODULE_ENUM module, void *handle)
{
	DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_SW_RST_ENGINE, DISP_REG_UFO_START, 1);
	DISP_REG_SET_FIELD(handle, START_FLD_DISP_UFO_SW_RST_ENGINE, DISP_REG_UFO_START, 0);
	DDPMSG("ufoe reset done\n");
	return 0;
}

// ufoe
DDP_MODULE_DRIVER ddp_driver_ufoe = {
	.init            = ufoe_init,
	.deinit          = ufoe_deinit,
	.config          = ufoe_config,
	.start           = ufoe_start,
	.trigger         = NULL,
	.stop            = ufoe_stop,
	.reset           = ufoe_reset,
	.power_on        = ufoe_clock_on,
	.power_off       = ufoe_clock_off,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};

