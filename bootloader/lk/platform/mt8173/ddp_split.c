#define LOG_TAG "SPLIT"


#include "platform/ddp_log.h"
#include "platform/ddp_info.h"
#include "platform/ddp_reg.h"
#include "platform/ddp_split.h"

//#include <linux/clk.h>
//#include <linux/delay.h>


static unsigned int split_index(DISP_MODULE_ENUM module)
{
	int idx = 0;
	switch (module) {
		case DISP_MODULE_SPLIT0:
			idx = 0;
			break;
		case DISP_MODULE_SPLIT1:
			idx = 1;
			break;
		default:
			DDPERR("invalid split module=%d \n", module);// invalid module
			ASSERT(0);
	}
	return idx;
}

static char * split_state(unsigned int state)
{
	switch (state) {
		case 1:
			return "idle";
		case 2:
			return "wait";
		case 4:
			return "busy";
		default:
			return "unknow";
	}
	return "unknow";
}

static int split_clock_on(DISP_MODULE_ENUM module,void * handle)
{
	ddp_enable_module_clock(module);
	return 0;
}

static int split_clock_off(DISP_MODULE_ENUM module,void * handle)
{
	ddp_disable_module_clock(module);
	return 0;
}

static int split_init(DISP_MODULE_ENUM module,void * handle)
{
	ddp_enable_module_clock(module);
	return 0;
}

static int split_deinit(DISP_MODULE_ENUM module,void * handle)
{
	ddp_enable_module_clock(module);
	return 0;
}

static int split_start(DISP_MODULE_ENUM module,void * handle)
{
	int idx = split_index(module);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_SPLIT_ENABLE, 0x01);
	return 0;
}

static int split_stop(DISP_MODULE_ENUM module,void * handle)
{
	int idx = split_index(module);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_SPLIT_ENABLE, 0x0);
	return 0;
}

static int split_busy(DISP_MODULE_ENUM module)
{
	int idx = split_index(module);
	unsigned int state = DISP_REG_GET_FIELD(DEBUG_FLD_SPLIT_FSM,DISP_REG_SPLIT_DEBUG+DISP_INDEX_OFFSET*idx);
	return (state & 0x4);
}

static int split_idle(DISP_MODULE_ENUM module)
{
	int idx = split_index(module);
	unsigned int state = DISP_REG_GET_FIELD(DEBUG_FLD_SPLIT_FSM,DISP_REG_SPLIT_DEBUG+DISP_INDEX_OFFSET*idx);
	return (state & 0x3);
}

int split_reset(DISP_MODULE_ENUM module,void * handle)
{
	unsigned int delay_cnt = 0;
	int idx = split_index(module);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_SPLIT_SW_RESET, 0x1);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_SPLIT_SW_RESET, 0x0);
	/*always use cpu do reset*/
	while ((DISP_REG_GET_FIELD(DEBUG_FLD_SPLIT_FSM, DISP_REG_SPLIT_DEBUG+DISP_INDEX_OFFSET*idx)
	        & 0x3)==0) {
		delay_cnt++;
		//udelay(10);
		if (delay_cnt>2000) {
			DDPERR("split%d_reset() timeout!\n",idx);
			break;
		}
	}
	return 0;
}

static int split_dump(DISP_MODULE_ENUM module,int level)
{
	int idx = split_index(module);
	DDPMSG("== DISP SPLIT%d  ==\n", idx);
	DDPMSG("(0x000)S_ENABLE       =0x%x\n", DISP_REG_GET(DISP_REG_SPLIT_ENABLE+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x004)S_SW_RST       =0x%x\n", DISP_REG_GET(DISP_REG_SPLIT_SW_RESET+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x008)S_DEBUG        =0x%x\n", DISP_REG_GET(DISP_REG_SPLIT_DEBUG+DISP_INDEX_OFFSET*idx));
	return 0;
}

static int split_debug(DISP_MODULE_ENUM module)
{
	int idx = split_index(module);
	unsigned int pixel = DISP_REG_GET_FIELD(DEBUG_FLD_IN_PIXEL_CNT,DISP_REG_SPLIT_DEBUG+DISP_INDEX_OFFSET*idx);
	unsigned int state = DISP_REG_GET_FIELD(DEBUG_FLD_SPLIT_FSM,DISP_REG_SPLIT_DEBUG+DISP_INDEX_OFFSET*idx);

	DDPMSG("== DISP��PLIT%d��DEBUG INFO  ==\n", idx);

	DDPMSG("cur_pixel %u, state %s\n",pixel,split_state(state));
	return 0;
}

DDP_MODULE_DRIVER ddp_driver_split0 = {
	.module          = DISP_MODULE_SPLIT0,
	.init            = split_init,
	.deinit          = split_deinit,
	.config          = NULL,
	.start           = split_start,
	.trigger         = NULL,
	.stop            = split_stop,
	.reset           = split_reset,
	.power_on        = split_clock_on,
	.power_off       = split_clock_off,
	.is_idle         = split_idle,
	.is_busy         = split_busy,
	.dump_info       = split_dump,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};

DDP_MODULE_DRIVER ddp_driver_split1 = {
	.module          = DISP_MODULE_SPLIT1,
	.init            = split_init,
	.deinit          = split_deinit,
	.config          = NULL,
	.start           = split_start,
	.trigger         = NULL,
	.stop            = split_stop,
	.reset           = split_reset,
	.power_on        = split_clock_on,
	.power_off       = split_clock_off,
	.is_idle         = split_idle,
	.is_busy         = split_busy,
	.dump_info       = split_dump,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};


