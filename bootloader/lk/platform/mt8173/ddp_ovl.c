#define LOG_TAG "OVL"

#include "platform/ddp_reg.h"
#include "platform/ddp_matrix_para.h"
#include "platform/ddp_info.h"
#include "platform/ddp_log.h"

#include "platform/ddp_ovl.h"
#include "platform/mt_gpt.h"
#define OVL_NUM           (2)
#define OVL_REG_BACK_MAX  (40)

enum OVL_COLOR_SPACE {
	OVL_COLOR_SPACE_RGB = 0,
	OVL_COLOR_SPACE_YUV,
};

#define OVL_COLOR_BASE 30
enum OVL_INPUT_FORMAT {
	OVL_INPUT_FORMAT_BGR565     = 0,
	OVL_INPUT_FORMAT_RGB888     = 1,
	OVL_INPUT_FORMAT_RGBA8888   = 2,
	OVL_INPUT_FORMAT_ARGB8888   = 3,
	OVL_INPUT_FORMAT_VYUY       = 4,
	OVL_INPUT_FORMAT_YVYU       = 5,

	OVL_INPUT_FORMAT_RGB565     = 6,
	OVL_INPUT_FORMAT_BGR888     = 7,
	OVL_INPUT_FORMAT_BGRA8888   = 8,
	OVL_INPUT_FORMAT_ABGR8888   = 9,
	OVL_INPUT_FORMAT_UYVY       = 10,
	OVL_INPUT_FORMAT_YUYV       = 11,

	OVL_INPUT_FORMAT_UNKNOWN    = 32,
};

typedef struct {
	unsigned int address;
	unsigned int value;
} OVL_REG;

static int reg_back_cnt[OVL_NUM];
static OVL_REG reg_back[OVL_NUM][OVL_REG_BACK_MAX];
static enum OVL_INPUT_FORMAT ovl_input_fmt_convert(DpColorFormat fmt);
static unsigned int ovl_input_fmt_byte_swap(enum OVL_INPUT_FORMAT inputFormat);
static unsigned int ovl_input_fmt_bpp(enum OVL_INPUT_FORMAT inputFormat);
static enum OVL_COLOR_SPACE ovl_input_fmt_color_space(enum OVL_INPUT_FORMAT inputFormat);
static unsigned int ovl_input_fmt_reg_value(enum OVL_INPUT_FORMAT inputFormat);
static void ovl_store_regs(int idx);
static void ovl_restore_regs(int idx,void * handle);
static unsigned int ovl_index(DISP_MODULE_ENUM module)
{
	int idx = 0;
	switch (module) {
		case DISP_MODULE_OVL0:
			idx = 0;
			break;
		case DISP_MODULE_OVL1:
			idx = 1;
			break;
		default:
			DDPERR("invalid module=%d \n", module);// invalid module
			ASSERT(0);
	}
	return idx;
}

int OVLStart(DISP_MODULE_ENUM module,void * handle)
{
	int idx = ovl_index(module);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTEN, 0x0E);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_EN, 0x01);

	DISP_REG_SET_FIELD(handle,DATAPATH_CON_FLD_LAYER_SMI_ID_EN,
	                   idx*DISP_INDEX_OFFSET+DISP_REG_OVL_DATAPATH_CON,0x1);
	return 0;
}

int OVLStop(DISP_MODULE_ENUM module,void * handle)
{
	int idx = ovl_index(module);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTEN, 0x00);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_EN, 0x00);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTSTA, 0x00);

	return 0;
}

int OVLReset(DISP_MODULE_ENUM module,void * handle)
{
#define OVL_IDLE (0x3)
	unsigned int delay_cnt = 0;
	int idx = ovl_index(module);
	/*always use cpu do reset*/
	DISP_CPU_REG_SET(idx*DISP_INDEX_OFFSET+DISP_REG_OVL_RST, 0x1);              // soft reset
	DISP_CPU_REG_SET(idx*DISP_INDEX_OFFSET+DISP_REG_OVL_RST, 0x0);
	while (!(DISP_REG_GET(idx*DISP_INDEX_OFFSET+DISP_REG_OVL_FLOW_CTRL_DBG) & OVL_IDLE)) {
		delay_cnt++;
		udelay(10);
		if (delay_cnt>2000) {
			DDPERR("OVL%dReset() timeout! \n",idx);
			break;
		}
	}
	return 0;
}

int OVLROI(DISP_MODULE_ENUM module,
           unsigned int bgW,
           unsigned int bgH,
           unsigned int bgColor,
           void * handle)
{
	int idx = ovl_index(module);

	if ((bgW > OVL_MAX_WIDTH) || (bgH > OVL_MAX_HEIGHT)) {
		DDPERR("OVLROI(), exceed OVL max size, w=%d, h=%d \n", bgW, bgH);
		ASSERT(0);
	}

	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_ROI_SIZE, bgH<<16 | bgW);

	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_ROI_BGCLR, bgColor);

	return 0;
}

int OVLLayerSwitch(DISP_MODULE_ENUM module,
                   unsigned layer,
                   unsigned int en,
                   void * handle)
{
	int idx = ovl_index(module);
	ASSERT(layer<=3);

	switch (layer) {
		case 0:
			DISP_REG_SET_FIELD(handle,SRC_CON_FLD_L0_EN, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_SRC_CON, en);
			break;
		case 1:
			DISP_REG_SET_FIELD(handle,SRC_CON_FLD_L1_EN, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_SRC_CON, en);
			break;
		case 2:
			DISP_REG_SET_FIELD(handle,SRC_CON_FLD_L2_EN, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_SRC_CON, en);
			break;
		case 3:
			DISP_REG_SET_FIELD(handle,SRC_CON_FLD_L3_EN, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_SRC_CON, en);
			break;
		default:
			DDPERR("invalid layer=%d\n", layer);           // invalid layer
			ASSERT(0);
	}

	return 0;
}

int OVL3DConfig(DISP_MODULE_ENUM module,
                unsigned int layer_id,
                unsigned int en_3d,
                unsigned int landscape,
                unsigned int r_first,
                void *handle)
{
	int idx = ovl_index(module);

	ASSERT(layer_id<=3);

	switch (layer_id) {
		case 0:
			DISP_REG_SET_FIELD(handle,L0_CON_FLD_EN_3D,     idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON, en_3d);
			DISP_REG_SET_FIELD(handle,L0_CON_FLD_LANDSCAPE, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON, landscape);
			DISP_REG_SET_FIELD(handle,L0_CON_FLD_R_FIRST,   idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON, r_first);
			break;
		case 1:
			DISP_REG_SET_FIELD(handle,L1_CON_FLD_EN_3D,     idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L1_CON, en_3d);
			DISP_REG_SET_FIELD(handle,L1_CON_FLD_LANDSCAPE, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L1_CON, landscape);
			DISP_REG_SET_FIELD(handle,L1_CON_FLD_R_FIRST,   idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L1_CON, r_first);
			break;
		case 2:
			DISP_REG_SET_FIELD(handle,L2_CON_FLD_EN_3D,     idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L2_CON, en_3d);
			DISP_REG_SET_FIELD(handle,L2_CON_FLD_LANDSCAPE, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L2_CON, landscape);
			DISP_REG_SET_FIELD(handle,L2_CON_FLD_R_FIRST,   idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L2_CON, r_first);
			break;
		case 3:
			DISP_REG_SET_FIELD(handle,L3_CON_FLD_EN_3D,     idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L3_CON, en_3d);
			DISP_REG_SET_FIELD(handle,L3_CON_FLD_LANDSCAPE, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L3_CON, landscape);
			DISP_REG_SET_FIELD(handle,L3_CON_FLD_R_FIRST,   idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L3_CON, r_first);
			break;
		default:
			DDPERR("OVL3DConfig(), invalid layer=%d \n", layer_id);           // invalid layer
			ASSERT(0);
	}
	return 0;
}

int OVLLayerConfig(DISP_MODULE_ENUM module,
                   unsigned int layer,
                   unsigned int source,
                   DpColorFormat format,
                   unsigned int addr,
                   unsigned int src_x,     // ROI x offset
                   unsigned int src_y,     // ROI y offset
                   unsigned int src_pitch,
                   unsigned int dst_x,     // ROI x offset
                   unsigned int dst_y,     // ROI y offset
                   unsigned int dst_w,     // ROT width
                   unsigned int dst_h,     // ROI height
                   unsigned int keyEn,
                   unsigned int key,   // color key
                   unsigned int aen,       // alpha enable
                   unsigned char alpha,
                   void * handle)
{

	int idx = ovl_index(module);

	enum OVL_INPUT_FORMAT fmt  = ovl_input_fmt_convert(format);
	unsigned int bpp           = ovl_input_fmt_bpp(fmt);
	unsigned int input_swap    = ovl_input_fmt_byte_swap(fmt);
	unsigned int input_fmt     = ovl_input_fmt_reg_value(fmt);
	unsigned int layer_offsite = layer * 0x20;
	ASSERT((dst_w <= OVL_MAX_WIDTH) && (dst_h <= OVL_MAX_HEIGHT) && (layer<=3));

	if (addr == 0) {
		DDPERR("source from memory, but addr is 0! \n");
		ASSERT(0);                           // direct link support YUV444 only
	}
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_RDMA0_CTRL+layer_offsite, 0x1);

	DISP_REG_SET_FIELD(handle, L0_CON_FLD_LAYER_SRC, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON+layer_offsite, 0);
	DISP_REG_SET_FIELD(handle, L0_CON_FLD_CLRFMT   , idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON+layer_offsite, input_fmt);
	DISP_REG_SET_FIELD(handle, L0_CON_FLD_ALPHA_EN , idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON+layer_offsite, aen);
	DISP_REG_SET_FIELD(handle, L0_CON_FLD_ALPHA    , idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON+layer_offsite, alpha);
	DISP_REG_SET_FIELD(handle, L0_CON_FLD_SRCKEY_EN, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON+layer_offsite, keyEn);
	DISP_REG_SET_FIELD(handle, L0_CON_FLD_BYTE_SWAP, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_CON+layer_offsite, input_swap);

	DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_SRC_SIZE+layer_offsite, dst_h<<16 | dst_w);
	DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_OFFSET+layer_offsite  , dst_y<<16 | dst_x);
	DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_ADDR+layer_offsite    , addr+src_x*bpp+src_y*src_pitch);
	DISP_REG_SET(handle, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_SRCKEY+layer_offsite  , key);

	DISP_REG_SET_FIELD(handle, L0_PITCH_FLD_L0_SRC_PITCH, idx*DISP_INDEX_OFFSET+DISP_REG_OVL_L0_PITCH+layer_offsite, src_pitch);
	return 0;
}

enum OVL_INPUT_FORMAT ovl_input_fmt_convert(DpColorFormat fmt)
{
	enum OVL_INPUT_FORMAT ovl_fmt = OVL_INPUT_FORMAT_UNKNOWN;
	switch (fmt) {
		case eBGR565           :
			ovl_fmt = OVL_INPUT_FORMAT_BGR565    ;
			break;
		case eRGB565           :
			ovl_fmt = OVL_INPUT_FORMAT_RGB565    ;
			break;
		case eRGB888           :
			ovl_fmt = OVL_INPUT_FORMAT_RGB888    ;
			break;
		case eBGR888           :
			ovl_fmt = OVL_INPUT_FORMAT_BGR888    ;
			break;
		case eRGBA8888         :
			ovl_fmt = OVL_INPUT_FORMAT_RGBA8888  ;
			break;
		case eBGRA8888         :
			ovl_fmt = OVL_INPUT_FORMAT_BGRA8888  ;
			break;
		case eARGB8888         :
			ovl_fmt = OVL_INPUT_FORMAT_ARGB8888  ;
			break;
		case eABGR8888         :
			ovl_fmt = OVL_INPUT_FORMAT_ABGR8888  ;
			break;
		case eVYUY             :
			ovl_fmt = OVL_INPUT_FORMAT_VYUY      ;
			break;
		case eYVYU             :
			ovl_fmt = OVL_INPUT_FORMAT_YVYU      ;
			break;
		case eUYVY             :
			ovl_fmt = OVL_INPUT_FORMAT_UYVY      ;
			break;
		case eYUY2             :
			ovl_fmt = OVL_INPUT_FORMAT_YUYV      ;
			break;
		default:
			DDPERR("ovl_fmt_convert fmt=%d, ovl_fmt=%d \n", fmt, ovl_fmt);
	}
	return ovl_fmt;
}

static unsigned int ovl_input_fmt_byte_swap(enum OVL_INPUT_FORMAT inputFormat)
{
	int input_swap = 0;
	switch (inputFormat) {
		case OVL_INPUT_FORMAT_BGR565:
		case OVL_INPUT_FORMAT_RGB888:
		case OVL_INPUT_FORMAT_RGBA8888:
		case OVL_INPUT_FORMAT_ARGB8888:
		case OVL_INPUT_FORMAT_VYUY:
		case OVL_INPUT_FORMAT_YVYU:
			input_swap = 1;
			break;
		case OVL_INPUT_FORMAT_RGB565:
		case OVL_INPUT_FORMAT_BGR888:
		case OVL_INPUT_FORMAT_BGRA8888:
		case OVL_INPUT_FORMAT_ABGR8888:
		case OVL_INPUT_FORMAT_UYVY:
		case OVL_INPUT_FORMAT_YUYV:
			input_swap = 0;
			break;
		default:
			DDPERR("unknow input ovl format is %d\n", inputFormat);
			ASSERT(0);
	}
	return input_swap;
}

static unsigned int ovl_input_fmt_bpp(enum OVL_INPUT_FORMAT inputFormat)
{
	int bpp = 0;
	switch (inputFormat) {
		case OVL_INPUT_FORMAT_BGR565:
		case OVL_INPUT_FORMAT_RGB565:
		case OVL_INPUT_FORMAT_VYUY:
		case OVL_INPUT_FORMAT_UYVY:
		case OVL_INPUT_FORMAT_YVYU:
		case OVL_INPUT_FORMAT_YUYV:
			bpp = 2;
			break;
		case OVL_INPUT_FORMAT_RGB888:
		case OVL_INPUT_FORMAT_BGR888:
			bpp = 3;
			break;
		case OVL_INPUT_FORMAT_RGBA8888:
		case OVL_INPUT_FORMAT_BGRA8888:
		case OVL_INPUT_FORMAT_ARGB8888:
		case OVL_INPUT_FORMAT_ABGR8888:
			bpp = 4;
			break;
		default:
			DDPERR("unknown ovl input format = %d\n", inputFormat);
			ASSERT(0);
	}
	return  bpp;
}

static enum OVL_COLOR_SPACE ovl_input_fmt_color_space(enum OVL_INPUT_FORMAT inputFormat)
{
	enum OVL_COLOR_SPACE space = OVL_COLOR_SPACE_RGB;
	switch (inputFormat) {
		case OVL_INPUT_FORMAT_BGR565:
		case OVL_INPUT_FORMAT_RGB565:
		case OVL_INPUT_FORMAT_RGB888:
		case OVL_INPUT_FORMAT_BGR888:
		case OVL_INPUT_FORMAT_RGBA8888:
		case OVL_INPUT_FORMAT_BGRA8888:
		case OVL_INPUT_FORMAT_ARGB8888:
		case OVL_INPUT_FORMAT_ABGR8888:
			space = OVL_COLOR_SPACE_RGB;
			break;
		case OVL_INPUT_FORMAT_VYUY:
		case OVL_INPUT_FORMAT_UYVY:
		case OVL_INPUT_FORMAT_YVYU:
		case OVL_INPUT_FORMAT_YUYV:
			space = OVL_COLOR_SPACE_YUV;
			break;
		default:
			DDPERR("unknown ovl input format = %d\n", inputFormat);
			ASSERT(0);
	}
	return space;
}

static unsigned int ovl_input_fmt_reg_value(enum OVL_INPUT_FORMAT inputFormat)
{
	int reg_value = 0;
	switch (inputFormat) {
		case OVL_INPUT_FORMAT_BGR565:
		case OVL_INPUT_FORMAT_RGB565:
			reg_value = 0x0;
			break;
		case OVL_INPUT_FORMAT_RGB888:
		case OVL_INPUT_FORMAT_BGR888:
			reg_value = 0x1;
			break;
		case OVL_INPUT_FORMAT_RGBA8888:
		case OVL_INPUT_FORMAT_BGRA8888:
			reg_value = 0x2;
			break;
		case OVL_INPUT_FORMAT_ARGB8888:
		case OVL_INPUT_FORMAT_ABGR8888:
			reg_value = 0x3;
			break;
		case OVL_INPUT_FORMAT_VYUY:
		case OVL_INPUT_FORMAT_UYVY:
			reg_value = 0x4;
			break;
		case OVL_INPUT_FORMAT_YVYU:
		case OVL_INPUT_FORMAT_YUYV:
			reg_value = 0x5;
			break;
		default:
			DDPERR("unknow ovl input format is %d\n", inputFormat);
			ASSERT(0);
	}
	return reg_value;
}

void OVLRegStore(DISP_MODULE_ENUM module)
{
	int i =0;
	int idx = ovl_index(module);

	static const unsigned int regs[] = {
		//start
		DISP_REG_OVL_INTEN       , DISP_REG_OVL_EN         ,DISP_REG_OVL_DATAPATH_CON,
		// roi
		DISP_REG_OVL_ROI_SIZE    , DISP_REG_OVL_ROI_BGCLR  ,
		//layers enable
		DISP_REG_OVL_SRC_CON,
		//layer0
		DISP_REG_OVL_RDMA0_CTRL  , DISP_REG_OVL_L0_CON     ,DISP_REG_OVL_L0_SRC_SIZE,
		DISP_REG_OVL_L0_OFFSET   , DISP_REG_OVL_L0_ADDR    ,DISP_REG_OVL_L0_PITCH,
		DISP_REG_OVL_L0_SRCKEY   ,
		//layer1
		DISP_REG_OVL_RDMA1_CTRL  , DISP_REG_OVL_L1_CON     ,DISP_REG_OVL_L1_SRC_SIZE,
		DISP_REG_OVL_L1_OFFSET   , DISP_REG_OVL_L1_ADDR    ,DISP_REG_OVL_L1_PITCH,
		DISP_REG_OVL_L1_SRCKEY   ,
		//layer2
		DISP_REG_OVL_RDMA2_CTRL  , DISP_REG_OVL_L2_CON     ,DISP_REG_OVL_L2_SRC_SIZE,
		DISP_REG_OVL_L2_OFFSET   , DISP_REG_OVL_L2_ADDR    ,DISP_REG_OVL_L2_PITCH,
		DISP_REG_OVL_L2_SRCKEY   ,
		//layer3
		DISP_REG_OVL_RDMA3_CTRL  , DISP_REG_OVL_L3_CON     ,DISP_REG_OVL_L3_SRC_SIZE,
		DISP_REG_OVL_L3_OFFSET   , DISP_REG_OVL_L3_ADDR    ,DISP_REG_OVL_L3_PITCH,
		DISP_REG_OVL_L3_SRCKEY   ,
	};

	reg_back_cnt[idx] = sizeof(regs)/sizeof(unsigned int);
	ASSERT(reg_back_cnt[idx]  <= OVL_REG_BACK_MAX);


	for (i =0; i< reg_back_cnt[idx]; i++) {
		reg_back[idx][i].address = regs[i];
		reg_back[idx][i].value   = DISP_REG_GET(regs[i]);
	}
	DDPDBG("store %d cnt registers on ovl %d",reg_back_cnt[idx],idx);

}

void OVLRegRestore(DISP_MODULE_ENUM module,void * handle)
{
	int idx = ovl_index(module);
	int i = reg_back_cnt[idx];
	while (i > 0) {
		i--;
		DISP_REG_SET(handle,reg_back[idx][i].address, reg_back[idx][i].value);
	}
	DDPDBG("restore %d cnt registers on ovl %d",reg_back_cnt[idx],idx);
	reg_back_cnt[idx] = 0;
}


int OVLClockOn(DISP_MODULE_ENUM module,void * handle)
{
	int idx = ovl_index(module);
	ddp_enable_module_clock(module);
	DDPMSG("OVL%dClockOn CG 0x%x \n",idx, DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
	return 0;
}

int OVLClockOff(DISP_MODULE_ENUM module,void * handle)
{
	int idx = ovl_index(module);
	DDPMSG("OVL%dClockOff\n",idx);
	//store registers
	// OVLRegRestore(module,handle);
	DISP_REG_SET(handle, idx * DISP_INDEX_OFFSET + DISP_REG_OVL_EN, 0x00);
	OVLReset(module,handle);
	DISP_REG_SET(handle,idx*DISP_INDEX_OFFSET+DISP_REG_OVL_INTEN, 0x00);
	DISP_REG_SET(handle, idx * DISP_INDEX_OFFSET + DISP_REG_OVL_INTSTA, 0x00);
	ddp_disable_module_clock(module);
	return 0;
}

int OVLInit(DISP_MODULE_ENUM module,void * handle)
{
	//power on, no need to care clock
	int idx = ovl_index(module);
	ddp_enable_module_clock(module);
	DDPMSG("OVL%dInit open CG 0x%x \n",idx, DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
	return 0;
}

int OVLDeInit(DISP_MODULE_ENUM module,void * handle)
{
	int idx = ovl_index(module);
	DDPMSG("OVL%dDeInit close CG \n",idx);
	ddp_disable_module_clock(module);
	return 0;
}

void OVLDump(DISP_MODULE_ENUM module)
{
	int idx = ovl_index(module);
	DDPMSG("== DISP OVL%d  ==\n", idx);
	DDPMSG("(0x000)O_STA        =0x%x\n", DISP_REG_GET(DISP_REG_OVL_STA+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x004)O_INTEN      =0x%x\n", DISP_REG_GET(DISP_REG_OVL_INTEN+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x008)O_INTSTA     =0x%x\n", DISP_REG_GET(DISP_REG_OVL_INTSTA+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x00c)O_EN         =0x%x\n", DISP_REG_GET(DISP_REG_OVL_EN+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x010)O_TRIG       =0x%x\n", DISP_REG_GET(DISP_REG_OVL_TRIG+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x014)O_RST        =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RST+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x020)O_ROI_SIZE   =0x%x\n", DISP_REG_GET(DISP_REG_OVL_ROI_SIZE+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x024)O_PATH_CON   =0x%x\n", DISP_REG_GET(DISP_REG_OVL_DATAPATH_CON+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x028)O_ROI_BGCLR  =0x%x\n", DISP_REG_GET(DISP_REG_OVL_ROI_BGCLR+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x02c)O_SRC_CON    =0x%x\n", DISP_REG_GET(DISP_REG_OVL_SRC_CON+DISP_INDEX_OFFSET*idx));
	if (DISP_REG_GET(DISP_REG_OVL_SRC_CON+DISP_INDEX_OFFSET*idx)&0x1) {
		DDPMSG("(0x030)O0_CON      =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x034)O0_SRCKEY   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_SRCKEY+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x038)O0_SRC_SIZE =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_SRC_SIZE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x03c)O0_OFFSET   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_OFFSET+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0xf40)O0_ADDR     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_ADDR+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x044)O0_PITCH    =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_PITCH+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x048)O0_TILE     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L0_TILE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0c0)O0_R_CTRL   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_RDMA0_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0c8)O0_R_M_GMC_SET1 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0cc)O0_R_M_SLOW_CON =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_SLOW_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0d0)O0_R_FIFO_CTRL  =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA0_FIFO_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x1e0)O0_R_M_GMC_SET2 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA0_MEM_GMC_SETTING2+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x24c)O0_R_DBG   =0x%x\n",      DISP_REG_GET(DISP_REG_OVL_RDMA0_DBG+DISP_INDEX_OFFSET*idx));
	}
	if (DISP_REG_GET(DISP_REG_OVL_SRC_CON+DISP_INDEX_OFFSET*idx)&0x2) {
		DDPMSG("(0x050)O1_CON      =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x054)O1_SRCKEY   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_SRCKEY+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x058)O1_SRC_SIZE =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_SRC_SIZE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x05c)O1_OFFSET   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_OFFSET+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0xf60)O1_ADDR     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_ADDR+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x064)O1_PITCH    =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_PITCH+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x068)O1_TILE     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L1_TILE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0e0)O1_R_CTRL   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_RDMA1_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0e8)O1_R_M_GMC_SET1 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0ec)O1_R_M_SLOW_CON =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_SLOW_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0f0)O1_R_FIFO_CTRL  =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA1_FIFO_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x1e4)O1_R_M_GMC_SET2 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA1_MEM_GMC_SETTING2+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x250)O1_R_DBG =0x%x\n",        DISP_REG_GET(DISP_REG_OVL_RDMA1_DBG+DISP_INDEX_OFFSET*idx));
	}
	if (DISP_REG_GET(DISP_REG_OVL_SRC_CON+DISP_INDEX_OFFSET*idx)&0x4) {
		DDPMSG("(0x070)O2_CON      =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x074)O2_SRCKEY   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_SRCKEY+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x078)O2_SRC_SIZE =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_SRC_SIZE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x07c)O2_OFFSET   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_OFFSET+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0xf80)O2_ADDR     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_ADDR+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x084)O2_PITCH    =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_PITCH+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x088)O2_TILE     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L2_TILE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x100)O2_R_CTRL   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_RDMA2_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x108)O2_R_M_GMC_SET1 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x10c)O2_R_M_SLOW_CON =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_SLOW_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x110)O2_R_FIFO_CTRL  =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA2_FIFO_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x1e8)O2_R_M_GMC_SET2 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA2_MEM_GMC_SETTING2+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x254)O2_R_DBG =0x%x\n",        DISP_REG_GET(DISP_REG_OVL_RDMA2_DBG+DISP_INDEX_OFFSET*idx));
	}
	if (DISP_REG_GET(DISP_REG_OVL_SRC_CON+DISP_INDEX_OFFSET*idx)&0x8) {
		DDPMSG("(0x090)O3_CON      =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x094)O3_SRCKEY   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_SRCKEY+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x098)O3_SRC_SIZE =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_SRC_SIZE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x09c)O3_OFFSET   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_OFFSET+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0xfa0)O3_ADDR     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_ADDR+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0a4)O3_PITCH    =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_PITCH+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x0a8)O3_TILE     =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_L3_TILE+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x120)O3_R_CTRL   =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_RDMA3_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x128)O3_R_M_GMC_SET1 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x12c)O3_R_M_SLOW_CON =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_SLOW_CON+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x130)O3_R_FIFO_CTRL  =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA3_FIFO_CTRL+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x1ec)O3_R_M_GMC_SET2 =0x%x\n", DISP_REG_GET(DISP_REG_OVL_RDMA3_MEM_GMC_SETTING2+DISP_INDEX_OFFSET*idx));
		DDPMSG("(0x258)O3_R_DBG    =0x%x\n",     DISP_REG_GET(DISP_REG_OVL_RDMA3_DBG+DISP_INDEX_OFFSET*idx));
	}
	DDPMSG("(0x1d4)O_DBG_MON_SEL =0x%x\n",       DISP_REG_GET(DISP_REG_OVL_DEBUG_MON_SEL+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x200)O_DUMMY_REG   =0x%x\n",       DISP_REG_GET(DISP_REG_OVL_DUMMY_REG+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x240)O_FLOW_CTRL   =0x%x\n",       DISP_REG_GET(DISP_REG_OVL_FLOW_CTRL_DBG+DISP_INDEX_OFFSET*idx));
	DDPMSG("(0x244)O_ADDCON      =0x%x\n",       DISP_REG_GET(DISP_REG_OVL_ADDCON_DBG+DISP_INDEX_OFFSET*idx));
}

static int OVLConfig_l(DISP_MODULE_ENUM module, disp_ddp_path_config* pConfig, void* handle)
{
	int i=0;
	if (pConfig->dst_dirty) {
		OVLROI(module,
		       pConfig->dst_w, // width
		       pConfig->dst_h, // height
		       0xFF000000,    // background color
		       handle);
	}
	if (!pConfig->ovl_dirty) {
		return 0;
	}
	for (i=0; i<OVL_LAYER_NUM; i++) {
		OVLLayerSwitch(module, i , pConfig->ovl_config[i].layer_en,handle);
		if (pConfig->ovl_config[i].layer_en!=0) {
			if (pConfig->ovl_config[i].addr==0 ||
			        pConfig->ovl_config[i].dst_w==0    ||
			        pConfig->ovl_config[i].dst_h==0    ) {
				DDPERR("ovl parameter invalidate, addr=0x%x, w=%d, h=%d \n",
				       pConfig->ovl_config[i].addr,
				       pConfig->ovl_config[i].dst_w,
				       pConfig->ovl_config[i].dst_h);
				return -1;
			}
			DDPDBG("module %d, layer=%d, en=%d, src=%d, fmt=0x%x, addr=0x%x, x=%d, y=%d, pitch=%d, dst(%d, %d, %d, %d),keyEn=0x%x, key=0x%x, aen=%d, alpha=%d \n",
			       module,
			       i,
			       pConfig->ovl_config[i].layer_en,
			       pConfig->ovl_config[i].source,   // data source (0=memory)
			       pConfig->ovl_config[i].fmt,
			       pConfig->ovl_config[i].addr, // addr
			       pConfig->ovl_config[i].src_x,  // x
			       pConfig->ovl_config[i].src_y,  // y
			       pConfig->ovl_config[i].src_pitch, //pitch, pixel number
			       pConfig->ovl_config[i].dst_x,  // x
			       pConfig->ovl_config[i].dst_y,  // y
			       pConfig->ovl_config[i].dst_w, // width
			       pConfig->ovl_config[i].dst_h, // height
			       pConfig->ovl_config[i].keyEn,  //color key
			       pConfig->ovl_config[i].key,  //color key
			       pConfig->ovl_config[i].aen, // alpha enable
			       pConfig->ovl_config[i].alpha);
			OVLLayerConfig(module,
			               i,
			               pConfig->ovl_config[i].source,
			               pConfig->ovl_config[i].fmt,
			               pConfig->ovl_config[i].addr,
			               pConfig->ovl_config[i].src_x,
			               pConfig->ovl_config[i].src_y,
			               pConfig->ovl_config[i].src_pitch,
			               pConfig->ovl_config[i].dst_x,
			               pConfig->ovl_config[i].dst_y,
			               pConfig->ovl_config[i].dst_w,
			               pConfig->ovl_config[i].dst_h,
			               pConfig->ovl_config[i].keyEn,
			               pConfig->ovl_config[i].key,
			               pConfig->ovl_config[i].aen,
			               pConfig->ovl_config[i].alpha,
			               handle);
		} else {
#if 0
			DDPDBG("module %d, layer=%d disable\n", module, pConfig->ovl_config[i].layer);
			OVLLayerConfig(module,
			               i,
			               OVL_LAYER_SOURCE_MEM,   // data source (0=memory)
			               eRGB888, //fmt
			               1, // addr
			               0,  // x
			               0,  // y
			               0, //pitch, pixel number
			               0,  // x
			               0,  // y
			               0, // width
			               0, // height
			               0,  //color key
			               0,  //color key
			               0, // alpha enable
			               0, // alpha
			               handle);
#endif
		}
	}
	return 0;
}

DDP_MODULE_DRIVER ddp_driver_ovl0 = {
	.module          = DISP_MODULE_OVL0,
	.init            = OVLInit,
	.deinit          = OVLDeInit,
	.config          = OVLConfig_l,
	.start           = OVLStart,
	.trigger         = NULL,
	.stop            = OVLStop,
	.reset           = NULL,
	.power_on        = OVLClockOn,
	.power_off       = OVLClockOff,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};

DDP_MODULE_DRIVER ddp_driver_ovl1 = {
	.module          = DISP_MODULE_OVL1,
	.init            = OVLInit,
	.deinit          = OVLDeInit,
	.config          = OVLConfig_l,
	.start           = OVLStart,
	.trigger         = NULL,
	.stop            = OVLStop,
	.reset           = NULL,
	.power_on        = OVLClockOn,
	.power_off       = OVLClockOff,
	.is_idle         = NULL,
	.is_busy         = NULL,
	.dump_info       = NULL,
	.bypass          = NULL,
	.build_cmdq      = NULL,
	.set_lcm_utils   = NULL,
};

