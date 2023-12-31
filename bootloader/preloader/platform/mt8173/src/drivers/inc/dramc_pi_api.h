#ifndef _DRAMC_PI_API_H
#define _DRAMC_PI_API_H

#include "dramc_register.h"
#include "emi.h"
/***********************************************************************/
/*              Includes                                               */
/***********************************************************************/

/***********************************************************************/
/*              Constant Define                                        */
/***********************************************************************/

// MEMPLL reference clock input config (from SYSPLL)
#define fcMEMPLL_52M_REF_IN
//#define fcMEMPLL_26M_REF_IN

// MEMPLL BW config
#define fcMEMPLL_0p5M_BW
//#define fcMEMPLL_1M_BW

#define fcWAVEFORM_MEASURE
//#define fcMEMPLL_TPTN_MON_EN

//MEMPLL config
#ifdef fcMEMPLL_26M_REF_IN
 #define Fin 26   // ref clock input in MHz
#elif defined fcMEMPLL_52M_REF_IN
 #define Fin 52
#else
 #define Fin 52
#endif

#define PREDIV 1   // pre-divider
#define POSDIV 1   // post-divider

// SSC / SYSPLL
//====== Config ======
#ifdef fcMEMPLL_26M_REF_IN
 #define SSC_SYSPLL_N 60
 #define DMSS_DIV 0xf
#elif defined fcMEMPLL_52M_REF_IN
 #define SSC_SYSPLL_N 56
 #define DMSS_DIV 0x7
#else
 #define SSC_SYSPLL_N 60
 #define DMSS_DIV 0xf
#endif

//init
#define DEFAULT_TEST2_1_CAL 0x55000000   // pattern0 and base address for test engine when we do calibration
// for testing, to separate TA4-3 address for running simultaneously
//#define DEFAULT_TEST2_1_CAL 0x55010000   // pattern0 and base address for test engine when we do calibration
#define DEFAULT_TEST2_2_CAL 0xaa000400   // pattern1 and offset address for test engine when we  do calibraion
#define DEFAULT_TEST2_1_DQSIEN 0x55000000   // pattern0 and base address for test engine when we do dqs gating window
#define DEFAULT_TEST2_2_DQSIEN 0xaa000010   // pattern1 and offset address for test engine when we  do dqs gating window
#define DEFAULT_GOLD_DQSIEN 0x20202020   // gold pattern for dqsien compare
#define DEFAULT_BL_TYPE BL_TYPE_8
#define DEFAULT_DATA_WIDTH DATA_WIDTH_32BIT
#define DEFAULT_DRAM_MODE MODE_2X

#define DEFAULT_MR1_VALUE_DDR3 0x00002000
#define DEFAULT_MR2_VALUE_LP3 0x001c0002

// timeout for TE2: (CMP_CPT_POLLING_PERIOD X MAX_CMP_CPT_WAIT_LOOP) 
// complete flag
// for testing
//#define CMP_CPT_POLLING_PERIOD 1000
#define CMP_CPT_POLLING_PERIOD 10
#define MAX_CMP_CPT_WAIT_LOOP 10000   // max loop

// jitter meter for PLL phase calibration
#define fcJMETER_COUNT 1024
#define fcJMETER_WAIT_DONE_US (fcJMETER_COUNT/Fin+10)    // 10us for more margin

// gating window
//==========================
#define fcNEW_GATING_FINETUNE_LIMIT   // to filter fake passed window
#if defined(fcNEW_GATING_FINETUNE_LIMIT) && defined(DDR_FT_LOAD_BOARD)
 #define fcNEW_GATING_FINETUNE_LIMIT_2   // improved version for very long fake passed window in MT6595 FT (only for FT) 
#endif

#define DQS_GW_COARSE_START 10//22//18
#define DQS_GW_COARSE_END	40//32//29
#define DQS_GW_COARSE_STEP	1
#define DQS_GW_COARSE_MAX	((DQS_GW_COARSE_END-DQS_GW_COARSE_START)/DQS_GW_COARSE_STEP+1)
#define DQS_GW_FINE_START 0
#define DQS_GW_FINE_END 127
#define DQS_GW_FINE_STEP 8
#define DQS_GW_FINE_MAX ((DQS_GW_FINE_END-DQS_GW_FINE_START)/DQS_GW_FINE_STEP+1)
#define DQS_GW_LEN_PER_COARSE_ELEMENT 32   // depend on dqs_gw[] type. U32 currently
#define DQS_GW_LEN_PER_COARSE	(CEIL_A_OVER_B(DQS_GW_FINE_MAX, DQS_GW_LEN_PER_COARSE_ELEMENT))   // dqs_gw[] length per coarse tune. each bit represents one test result 
#define DQS_GW_LEN	(DQS_GW_COARSE_MAX*DQS_GW_LEN_PER_COARSE)   // dqs_gw[] total length

#define DQS_GW_TE_OFFSET 0x10
#define DQS_GW_GOLD_COUNTER_16BIT 0x40404040   // 16*16/4=64
#define DQS_GW_GOLD_COUNTER_32BIT 0x20202020

// common
#define DQ_DATA_WIDTH 32   // define max support bus width in the system (to allocate array size)
#define DQS_BIT_NUMBER 8
#define DQS_NUMBER (DQ_DATA_WIDTH /DQS_BIT_NUMBER)

// RX DQ/DQS
#define MAX_RX_DQSDLY_TAPS 64   	// 0x018, May set back to 64 if no need.
#define MAX_RX_DQDLY_TAPS 16      // 0x210~0x22c, 0~15 delay tap

// DATLAT
#ifdef E2_MODIFICATION
#define DATLAT_TAP_NUMBER 26   // DATLAT[4:0] = {0xf0[25] 0x0e4[4] 0x07c[6:4]}
#else
#define DATLAT_TAP_NUMBER 22   // DATLAT[4:0] = {0xf0[25] 0x0e4[4] 0x07c[6:4]}
#endif

// TX DQ/DQS
#define FIRST_TX_DQ_DELAY 0   // first step to DQ delay
#define FIRST_TX_DQS_DELAY 0   // first step to DQS delay
#define MAX_TX_DQDLY_TAPS 16   // max DQ TAP number
#define MAX_TX_DQSDLY_TAPS 16   // max DQS TAP number

// DRAM Driving
#define MAX_DRAM_DRIV_SET_LPDDR2 6   // LPDDR2 driving setting steps
#define MAX_DRAM_DRIV_SET_DDR3 2     // DDR3 driving setting steps
#define DEFAULT_LPDDR2_DRIV_MR3_VALUE 0x00020003   // LPDDR2 MR3 default value
#define DEFAULT_DDR3_DRIV_MR1_VALUE 0x00002000   // DDR3 MR1 default value

#ifdef DDR_FT_LOAD_BOARD
//Step
#define FLAG_PLLPHASE_CALIBRATION		0
#define FLAG_PLLGPPHASE_CALIBRATION	1
#define FLAG_IMPEDANCE_CALIBRATION		2
#define FLAG_CA_CALIBRATION				3
#define FLAG_WL_CALIBRATION			4
#define FLAG_GATING_CALIBRATION		5
#define FLAG_RX_CALIBRATION				6
#define FLAG_DATLAT_CALIBRATION		7
#define FLAG_TX_CALIBRATION				8
//Error Type
#define FLAG_CALIBRATION_PASS			0
#define FLAG_WINDOW_TOO_SMALL			1
#define FLAG_WINDOW_TOO_BIG			2
#define FLAG_CALIBRATION_FAIL			3
//Channel (the same as DRAM_CHANNEL_T)

//Complete flag
#define FLAG_NOT_COMPLETE_OR_FAIL		0
#define FLAG_COMPLETE_AND_PASS			1

// FT window size criterion (need to be updated)
#define PLL_PHCALIB_BOUND				24 // skew difference (00~h'17), from simulation
#define PLL_GPPHCALIB_BOUND			56 // skew difference (00~h'37), from simulation
#define CA_TRAINING_BOUND				5
#define RXWIN_CALIB_BOUND				10
#define DATLAT_CALIB_BOUND				1
#define TXWIN_CALIB_BOUND				7
#endif

/***********************************************************************/
/*              Defines                                                */
/***********************************************************************/
#define ENABLE  1
#define DISABLE 0

typedef enum
{
    DRAM_OK = 0, // OK
    DRAM_FAIL    // FAIL
} DRAM_STATUS_T; // DRAM status type

typedef enum
{
    CHANNEL_A = 0,    
    CHANNEL_B,    
} DRAM_CHANNEL_T;

typedef enum
{
    TYPE_mDDR = 1,
    TYPE_LPDDR2, 
    TYPE_LPDDR3,
    TYPE_PCDDR3
} DRAM_DRAM_TYPE_T;

typedef enum
{
    DATA_WIDTH_16BIT = 16,
    DATA_WIDTH_32BIT = 32
} DRAM_DATA_WIDTH_T;

// for A60808 DDR3
typedef enum
{
    PCB_LOC_ASIDE = 0,
    PCB_LOC_BSIDE
} DRAM_PCB_LOC_T;

typedef enum
{
    MODE_1X = 0,
    MODE_2X
} DRAM_DRAM_MODE_T;

typedef enum
{
    PACKAGE_SBS = 0,
    PACKAGE_POP
} DRAM_PACKAGE_T;

typedef enum
{
    TE_OP_WRITE_READ_CHECK = 0,
    TE_OP_READ_CHECK
} DRAM_TE_OP_T;

typedef enum
{
    TEST_ISI_PATTERN = 0,
    TEST_AUDIO_PATTERN,
    TEST_TA1_SIMPLE,
    TEST_TESTPAT4,
    TEST_TESTPAT4_3, 
    TEST_XTALK_PATTERN,
    TEST_MIX_PATTERN
} DRAM_TEST_PATTERN_T;

typedef enum
{
    BL_TYPE_4 = 0,
    BL_TYPE_8
} DRAM_BL_TYPE_T;

typedef enum
{
    DLINE_0 = 0,
    DLINE_1,
    DLINE_TOGGLE    
} PLL_PHASE_CAL_STATUS_T;

typedef enum
{
    TA43_OP_STOP,
    TA43_OP_CLEAR,
    TA43_OP_RUN,
    TA43_OP_RUNQUIET,
    TA43_OP_UNKNOWN,
} DRAM_TA43_OP_TYPE_T;

// used for record last test pattern in TA
typedef enum
{
    TA_PATTERN_IDLE,
    TA_PATTERN_TA43,
    TA_PATTERN_TA4,
    TA_PATTERN_UNKNOWM,
} DRAM_TA_PATTERN_T;

typedef enum
{
    DMA_OP_PURE_READ,
    DMA_OP_PURE_WRITE,
    DMA_OP_READ_WRITE,
} DRAM_DMA_OP_T;

////////////////////////////
typedef struct _DRAMC_CTX_T
{
    DRAM_CHANNEL_T channel;
    DRAM_DRAM_TYPE_T dram_type;
    DRAM_PACKAGE_T package;
    DRAM_DATA_WIDTH_T data_width;
    U32 test2_1;
    U32 test2_2;
    DRAM_TEST_PATTERN_T test_pattern;
    U16 frequency;
    U16 frequency_low;
    U8 fglow_freq_write_en;
    U8 ssc_en;
    //U8 dynamicODT;
    U8 en_4bitMux;  
} DRAMC_CTX_T;

typedef struct _RXDQS_PERBIT_DLY_T
{
    S8 first_dqdly_pass;
    S8 last_dqdly_pass;
    S8 first_dqsdly_pass;
    S8 last_dqsdly_pass;
    S8 best_first_dqdly_pass;
    S8 best_last_dqdly_pass;
    S8 best_first_dqsdly_pass;
    S8 best_last_dqsdly_pass;    
    U8 best_dqdly;
    U8 best_dqsdly;
} RXDQS_PERBIT_DLY_T;

typedef struct _TXDQS_PERBIT_DLY_T
{
    S8 first_dqdly_pass;
    S8 last_dqdly_pass;
    S8 first_dqsdly_pass;
    S8 last_dqsdly_pass;
    S8 best_first_dqdly_pass;
    S8 best_last_dqdly_pass;
    S8 best_first_dqsdly_pass;
    S8 best_last_dqsdly_pass;        
    U8 best_dqdly;
    U8 best_dqsdly;
} TXDQS_PERBIT_DLY_T;

#ifdef DRAMC_DEBUG_TIME
#define DRAMC_CHANNEL	2
typedef struct _DRAMC_DEBUG_TIME
{
    U32 u4Total_set_emi;
    U32 u4Total_dram_init;
    U32 u4Total_dram_calib;
    U32 u4Total_dram_autodetect;	
	U32 u4Total_dram_logsave;	
    U32 u4Dramc_calib_SW_Impedance[DRAMC_CHANNEL];	
    U32 u4Dramc_calib_Mempll[DRAMC_CHANNEL];	
    U32 u4Dramc_calib_CA_Training[DRAMC_CHANNEL];	
    U32 u4Dramc_calib_Write_Leveling[DRAMC_CHANNEL];
    U32 u4Dramc_calib_Gating_DQS[DRAMC_CHANNEL];
    U32 u4Dramc_calib_DATLAT[DRAMC_CHANNEL];
    U32 u4Dramc_calib_Clkduty[DRAMC_CHANNEL];
    U32 u4Dramc_calib_RXPerbit[DRAMC_CHANNEL];
    U32 u4Dramc_calib_TXPerbit[DRAMC_CHANNEL];
} Dramc_debug_time_struct;

#endif

#ifdef DRAMC_DEBUG_CALIB_LOG
#define DRAMC_CALIB_HEADER	0x81735a5a
#define DRAMC_CALIB_FREQ_NUM 30
#define DRAMC_CALIB_REG_NUM  50

typedef enum {
    DRAMC_CALIB_FIRST_BUFFER    = 0,
    DRAMC_CALIB_SECOND_BUFFER,
    DRAMC_CALIB_THIRD_BUFFER,
	
	DRAMC_CALIB_BUFFER_MAX
} dramc_calib_buffer;

typedef enum {
    STEP_DRAM_INIT           	= 0,
    STEP_DRAM_CALIB_SW          = 1,		
    STEP_DRAM_CALIB_CA       	= 2,
    STEP_DRAM_CALIB_WL          = 3,
    STEP_DRAM_CALIB_GW          = 4,
    STEP_DRAM_CALIB_DATLAT      = 5,
    STEP_DRAM_CALIB_DUTY    	= 6,
    STEP_DRAM_CALIB_RX    		= 7,
    STEP_DRAM_CALIB_TX          = 8,
	STEP_DRAM_CALIB_DONE		= 9,
   	
    STEP_MAX
} dramc_calib_step;

typedef struct _DRAMC_DEBUG_REG
{
    UINT32  addr;	//register offset
    UINT32  value0;	//register value A
    UINT32  value1;	//register value B
} DRAMC_debug_dramcalib_reg;

typedef struct _DRAMC_DEBUG_CA_WIN
{
    UINT8  CA_Win;	//CA window
    UINT8  CLK_Win;	//CLK window
    UINT8  shift;	//CA delay
    INT8   center;	//CLK delay
} DRAMC_debug_dramcalib_CA_win;

typedef struct _DRAMC_DEBUG_DQ_WIN
{
    UINT8  setuptime;	//DQ
    UINT8  holdtime;	//DQS
    UINT8  window;		//window size
    INT8   offset;		//+/- offset 
} DRAMC_debug_dramcalib_win;

typedef struct _DRAMC_DEBUG_GW
{
    UINT32  coase;	//coase tune
    UINT32  fine;	//fine tune
    UINT32  step[DQS_GW_COARSE_MAX];//10 ~ 40
    UINT32  value[DQS_GW_COARSE_MAX];//10 ~ 40
} DRAMC_debug_dramcalib_gw;

typedef struct _DRAMC_DEBUG_DATLAT
{
    UINT32  first;
    UINT32  best;
    UINT32  total;
	UINT32  start;
	UINT32  step[DATLAT_TAP_NUMBER];//26
    UINT32  value[DATLAT_TAP_NUMBER];//26
} DRAMC_debug_dramcalib_DATLAT;

typedef struct _DRAMC_DEBUG_STEP
{
    UINT32  result0;	//CPU R/W test result
    UINT32  result1;	//test agent result
} DRAMC_debug_dramcalib_step;

typedef struct _DRAMC_DEBUG_CALIB
{
	UINT32 u4Header;				//4bytes : 8173 5a5a
	UINT32 u4LogVersion;			//0x00 first , 0x01 pass, 0x02 fail
	UINT32 u4PreloaderVersion[4];	//16bytes : 20141222-101010
	UINT32 u4ChipID;				//CPU ID
	UINT32 u4DeviceID[4];			//Board ID
	UINT32 u4DramVendorID;			//DRAM Vendor ID	
	UINT32 u4DramFreq;				//mempll init
	UINT32 u4Vcore;					//Vcore
	UINT32 u4Vmem;					//Vmem
	UINT32 u4DramDrv;				//DRAM driving
	UINT32 u4RankNum;				//Rank Number
	UINT32 u4Step;					//Step n rank
	UINT32 u4DramRankSize[4];       //before memory test
	UINT32 u4FullMemoryTest[4];		//0x00000001 =>  [0] 00 NA 01 PASS 02 FAIL,  [1]Error code
	DRAMC_debug_dramcalib_gw     sDramCalibGW[4];//cha rank0/cha rank1/chb rank0/chb rank1
	DRAMC_debug_dramcalib_DATLAT sDramCalibDATLAT[4];//cha rank0/cha rank1/chb rank0/chb rank1
	DRAMC_debug_dramcalib_CA_win sDramCalibWinA_CA[10];//CA win size
	DRAMC_debug_dramcalib_CA_win sDramCalibWinB_CA[10];//CA win size
	DRAMC_debug_dramcalib_win  sDramCalibWinA_RX[32];//calibration win size
	DRAMC_debug_dramcalib_win  sDramCalibWinB_RX[32];//calibration win size
	DRAMC_debug_dramcalib_win  sDramCalibWinA_TX[32];//calibration win size
	DRAMC_debug_dramcalib_win  sDramCalibWinB_TX[32];//calibration win size
	DRAMC_debug_dramcalib_step sDramCalibStepA[STEP_MAX];//calibration test result
	DRAMC_debug_dramcalib_step sDramCalibStepB[STEP_MAX];//calibration test	result
	DRAMC_debug_dramcalib_reg  sDramCalibReg[DRAMC_CALIB_FREQ_NUM + DRAMC_CALIB_REG_NUM + 1];//Register + MR2
} DRAMC_debug_calib;

extern DRAMC_debug_calib gs_dramc_calib;
extern UINT32 dramc_calib_step_init(DRAMC_CTX_T *p);
extern UINT32 dramc_calib_step_test(DRAMC_CTX_T *p, UINT32 index);
extern UINT32 dramc_calib_step_save(DRAMC_debug_calib *p, UINT32 index);
extern UINT32 dramc_calib_step_dump(DRAMC_debug_calib *p);

#endif
extern unsigned int dramc_actiming_freq_check();

// has defined in Common/pi_def.h
/************************ Bit Process *************************/
/*
#define mcBITL(b)               (1L << (b))
#define mcBIT(b)                (1L << (b))
#define mcMASK(w)               (mcBIT(w) - 1)
#define mcMASKS(w, b)           (mcMASK(w) << (b))
#define mcFIELD(val, msk, pos)  (((val) << (pos)) & (msk))

#define mcSET_MASK(a, b)        ((a) |= (b))
#define mcCLR_MASK(a, b)        ((a) &= (~(b)))
#define mcCHK_MASK(a, b)        ((a) & (b))
//#define mcSET_BIT(a, b)         mcSET_MASK(a, mcBIT(b))
#define mcSET_BIT(a, b)         ((a) |= (1L<<(b)))
//#define mcCLR_BIT(a, b)         mcCLR_MASK(a, mcBIT(b))
#define mcCLR_BIT(a, b)         ((a) &= (~(1L<<(b))))
#define mcCHK_BIT1(a, b)        ((a) & mcBIT(b))
#define mcCHK_BITM(a, b, m)     (((a) >> (b)) & (m))
#define mcCHK_BITS(a, b, w)     mcCHK_BITM(a, b, mcMASK(w))
//#define mcTEST_BIT(a, b)        mcCHK_BITM(a, b, 1)
#define mcTEST_BIT(a, b)        mcCHK_BIT1(a, b)
#define mcCHG_BIT(a, b)         ((a) ^= mcBIT(b))

#define mcSET_FIELD0(var, val, msk, pos)    mcSET_MASK(var, mcFIELD(val, msk, pos))

#define mcSET_FIELD(var, value, mask, pos)  \
{                                           \
    mcCLR_MASK(var, mask);                  \
    mcSET_FIELD0(var, value, mask, pos);    \
}

#define mcGET_FIELD(var, mask, pos)     (((var) & (mask)) >> (pos))
*/

#define mcSET_DRAMC_REG_ADDR(offset)    ((p->channel << CH_INFO) | (offset))
#define mcSET_SYS_REG_ADDR(offset)    ((p->channel << CH_INFO) | (offset))

/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/

/***********************************************************************/
/*              Public Functions                                       */
/***********************************************************************/
// basic function

#ifdef COMBO_MCP
DRAM_STATUS_T DramcPreInit(DRAMC_CTX_T *p, EMI_SETTINGS* emi_set);
DRAM_STATUS_T DramcInit(DRAMC_CTX_T *p, EMI_SETTINGS* emi_set);
#else
DRAM_STATUS_T DramcPreInit(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcInit(DRAMC_CTX_T *p);
#endif
DRAMC_CTX_T *DramcCtxCreate(void);
void vDramcCtxDestroy(DRAMC_CTX_T *p);
void vDramcCtxInit(DRAMC_CTX_T *p);
DRAM_STATUS_T MemPllPreInit(DRAMC_CTX_T *p);
DRAM_STATUS_T MemPllInit(DRAMC_CTX_T *p);
void MemPllRefClkChange(U32 ncpo_value);
DRAM_STATUS_T SscEnable(DRAMC_CTX_T *p);
U32 DramOperateDataRate(DRAMC_CTX_T *p);
U32 DramcEngine1(DRAMC_CTX_T *p, U32 test2_1, U32 test2_2, S16 loopforever, U8 period);
U32 DramcEngine2(DRAMC_CTX_T *p, DRAM_TE_OP_T wr, U32 test2_1, U32 test2_2, U8 testaudpat, S16 loopforever, U8 period, U8 log2loopcount);
void DramcEnterSelfRefresh(DRAMC_CTX_T *p, U8 op);
void DramcLowFreqWrite(DRAMC_CTX_T *p);
void DramcPhyReset(DRAMC_CTX_T *p);
void DramcDiv2PhaseSync(DRAMC_CTX_T *p);
void DramcRunTimeConfig(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcRegDump(DRAMC_CTX_T *p);
void DramcTestPat4_3(DRAMC_CTX_T *p, DRAM_TA43_OP_TYPE_T type);
U32 DramcPat4_3(DRAMC_CTX_T *p);
U32 DramcDmaEngine(DRAMC_CTX_T *p, DRAM_DMA_OP_T op, U32 src_addr, U32 dst_addr, U32 trans_len, U8 burst_len, U8 check_result, U8 ChannelNum);
void DramcWorstPat_mem(DRAMC_CTX_T *p, U32 src_addr);
U32 DramcWorstPat_dma(DRAMC_CTX_T *p, U32 src_addr, U32 dst_addr, U32 loop, U8 check_result);

U32 DramcDmaEngineNoWait(DRAMC_CTX_T *p, DRAM_DMA_OP_T op, U32 src_addr, U32 dst_addr, U32 trans_len, U8 burst_len);
U32 DramcDmaWaitCompare(DRAMC_CTX_T *p, U32 src_addr, U32 dst_addr, U32 trans_len, U8 burst_len);

#ifdef DUAL_FREQ_K
void DramcSwitchFreq(DRAMC_CTX_T *p, U8 InitTime);
void DramcSaveFreqSetting(DRAMC_CTX_T *p);
void DramcDumpFreqSetting(DRAMC_CTX_T *p);
void DramcRestorePLLSetting(DRAMC_CTX_T *p);
void DramcRestoreFreqSetting(DRAMC_CTX_T *p);
#endif

void DramcRANKINCTLConfig(DRAMC_CTX_T *p);

#ifdef LOOPBACK_TEST
void DramcLoopbackTest(DRAMC_CTX_T *p);
#endif

// mandatory calibration function
DRAM_STATUS_T DramcPllPhaseCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramCPllGroupsCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcSwImpedanceCal(DRAMC_CTX_T *p, U8 apply);
DRAM_STATUS_T DramcHwImpedanceCal(DRAMC_CTX_T *p);
#ifdef COMBO_MCP
DRAM_STATUS_T DramcWriteLeveling(DRAMC_CTX_T *p, EMI_SETTINGS* emi_set);
#else
DRAM_STATUS_T DramcWriteLeveling(DRAMC_CTX_T *p);
#endif
DRAM_STATUS_T DramcRxdqsGatingCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DualRankDramcRxdqsGatingCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcRxWindowPerbitCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcDualRankRxdatlatCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcRxdatlatCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcTxWindowPerbitCal(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcCATraining(DRAMC_CTX_T *p);
void DramcClkDutyCal(DRAMC_CTX_T *p);
#ifdef SPM_CONTROL_AFTERK
void TransferToSPMControl(void);
void TransferToRegControl(void);
#endif
#ifdef SUSPEND_TEST
void Suspend_Resume(DRAMC_CTX_T *p);
#endif

// reference function
DRAM_STATUS_T DramcRxEyeScan(DRAMC_CTX_T *p);
DRAM_STATUS_T DramcTxEyeScan(DRAMC_CTX_T *p);

#ifdef DDR_FT_LOAD_BOARD
void LoadBoardGpioInit(void);
void LoadBoardShowResult(U8 step, U8 error_type, U8 channel, U8 complete);
#endif

// Global variables
#ifdef _WIN32
extern FILE *fp_A60808;
#endif

#ifdef DUAL_RANKS
extern unsigned int uiDualRank;
#endif
extern U8 CurrentRank;
extern U8 RXPERBIT_LOG_PRINT;
extern U8 GATINGDQS_LOG_PRINT;
#ifdef DRAMC_DEBUG_TIME
extern Dramc_debug_time_struct gs_Dramc_Debug_Time;
#endif

#ifdef DRAMC_DEBUG_RETRY
extern unsigned int _emi_retry;
extern void emi_retry_adjust(int nAdjust);
#endif

// API prototypes.
U8 ucDram_Register_Read(U32 u4reg_addr, U32 *pu4reg_value);
U8 ucDram_Register_Write(U32 u4reg_addr, U32 u4reg_value);
U8 ucDramC_Register_Read(U32 u4reg_addr, U32 *pu4reg_value);
U8 ucDramC_Register_Write(U32 u4reg_addr, U32 u4reg_value);
void DramcDmaEngine_Config(DRAMC_CTX_T *p);
void DramcGeneralPat_mem(DRAMC_CTX_T *p, U32 src_addr);
#endif // _PI_API_H
