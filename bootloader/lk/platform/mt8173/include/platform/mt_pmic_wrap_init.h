#ifndef __PMIC_WRAP_REGS_H__
#define __PMIC_WRAP_REGS_H__
#include <debug.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_gpt.h>
#include <platform/mt_irq.h>
#include <sys/types.h>
#include <platform/sync_write.h>

//---start ---external API--------------------------------------------------
S32 pwrap_read( U32  adr, U32 *rdata );
S32 pwrap_write( U32  adr, U32  wdata );
S32 pwrap_wacs2( U32  write, U32  adr, U32  wdata, U32 *rdata );
//#define MACH_FPGA
#ifdef MACH_FPGA
extern S32 pwrap_init_lk ( void );
int  pwrap_init_for_early_porting(void);
#endif
//---end ---external API----------------------------------------------------

static BOOL _pwrap_timeout_ns (U64 start_time_ns, U64 timeout_time_ns);
static U64 _pwrap_get_current_time(void);
static U64 _pwrap_time2ns (U64 time_us);
static S32 pwrap_read_nochk( U32  adr, U32 *rdata );
static S32 pwrap_write_nochk( U32  adr, U32  wdata );
static S32 _pwrap_wacs2_nochk( U32 write, U32 adr, U32 wdata, U32 *rdata );
//---end--internal API--------------------------------------------------

#define DEW_BASE  (0xBC00)
//-------macro for pmic register--------------------------------
#define PMIC_BASE (0x0000)
#define PMIC_WRP_CKPDN            (PMIC_BASE+0x011A) //0x0056
#define PMIC_WRP_RST_CON          (PMIC_BASE+0x0120)//0x005C
#define PMIC_TOP_CKCON2           (PMIC_BASE+0x012A)
#define PMIC_TOP_CKCON3           (PMIC_BASE+0x01D4)
//-------macro for timeout setting--------------------------------
/******************************************************************************
global variable and  sys interface
******************************************************************************/
#define TIMEOUT_RESET       0xFF        //us
#define TIMEOUT_READ        0xFF        //us
#define TIMEOUT_WAIT_IDLE   0xFF        //us

#define WACS2       (1 << 4)
#define CKSYS_BASE (0x10000000)
#define CLK_SPI_CK_26M  0x7
#define CLK_CFG_5_CLR                       (CKSYS_BASE+0x098)

//-------macro for spi clock config--------------------------------
#define CLK_CFG_8                       (CKSYS_BASE+0x100) //8173
#define WDT_SWSYSRST                    (TOPRGU_BASE+0x18) //6589

#define PMIC_WRAP_MUX_SEL               (PWRAP_BASE+0x0)
#define PMIC_WRAP_WRAP_EN               (PWRAP_BASE+0x4)
#define PMIC_WRAP_DIO_EN                (PWRAP_BASE+0x8)
#define PMIC_WRAP_SIDLY                 (PWRAP_BASE+0xC)
#define PMIC_WRAP_RDDMY                 (PWRAP_BASE+0x10)
#define PMIC_WRAP_SI_CK_CON             (PWRAP_BASE+0x14)
#define PMIC_WRAP_CSHEXT_WRITE          (PWRAP_BASE+0x18)
#define PMIC_WRAP_CSHEXT_READ           (PWRAP_BASE+0x1C)
#define PMIC_WRAP_CSLEXT_START          (PWRAP_BASE+0x20)
#define PMIC_WRAP_CSLEXT_END            (PWRAP_BASE+0x24)
#define PMIC_WRAP_STAUPD_PRD            (PWRAP_BASE+0x28)
#define PMIC_WRAP_STAUPD_GRPEN          (PWRAP_BASE+0x2C)
#define PMIC_WRAP_STAUPD_MAN_TRIG       (PWRAP_BASE+0x40)
#define PMIC_WRAP_STAUPD_STA            (PWRAP_BASE+0x44)
#define PMIC_WRAP_WRAP_STA              (PWRAP_BASE+0x48)
#define PMIC_WRAP_HARB_INIT             (PWRAP_BASE+0x4C)
#define PMIC_WRAP_HARB_HPRIO            (PWRAP_BASE+0x50)
#define PMIC_WRAP_HIPRIO_ARB_EN         (PWRAP_BASE+0x54)
#define PMIC_WRAP_HARB_STA0             (PWRAP_BASE+0x58)
#define PMIC_WRAP_HARB_STA1             (PWRAP_BASE+0x5C)
#define PMIC_WRAP_MAN_EN                (PWRAP_BASE+0x60)
#define PMIC_WRAP_MAN_CMD               (PWRAP_BASE+0x64)
#define PMIC_WRAP_MAN_RDATA             (PWRAP_BASE+0x68)
#define PMIC_WRAP_MAN_VLDCLR            (PWRAP_BASE+0x6C)
#define PMIC_WRAP_WACS0_EN              (PWRAP_BASE+0x70)
#define PMIC_WRAP_INIT_DONE0            (PWRAP_BASE+0x74)
#define PMIC_WRAP_WACS0_CMD             (PWRAP_BASE+0x78)
#define PMIC_WRAP_WACS0_RDATA           (PWRAP_BASE+0x7C)
#define PMIC_WRAP_WACS0_VLDCLR          (PWRAP_BASE+0x80)
#define PMIC_WRAP_WACS1_EN              (PWRAP_BASE+0x84)
#define PMIC_WRAP_INIT_DONE1            (PWRAP_BASE+0x88)
#define PMIC_WRAP_WACS1_CMD             (PWRAP_BASE+0x8C)
#define PMIC_WRAP_WACS1_RDATA           (PWRAP_BASE+0x90)
#define PMIC_WRAP_WACS1_VLDCLR          (PWRAP_BASE+0x94)
#define PMIC_WRAP_WACS2_EN              (PWRAP_BASE+0x98)
#define PMIC_WRAP_INIT_DONE2            (PWRAP_BASE+0x9C)
#define PMIC_WRAP_WACS2_CMD             (PWRAP_BASE+0xA0)
#define PMIC_WRAP_WACS2_RDATA           (PWRAP_BASE+0xA4)
#define PMIC_WRAP_WACS2_VLDCLR          (PWRAP_BASE+0xA8)
#define PMIC_WRAP_INT_EN                (PWRAP_BASE+0xAC)
#define PMIC_WRAP_INT_FLG_RAW           (PWRAP_BASE+0xB0)
#define PMIC_WRAP_INT_FLG               (PWRAP_BASE+0xB4)
#define PMIC_WRAP_INT_CLR               (PWRAP_BASE+0xB8)
#define PMIC_WRAP_SIG_ADR               (PWRAP_BASE+0xBC)
#define PMIC_WRAP_SIG_MODE              (PWRAP_BASE+0xC0)
#define PMIC_WRAP_SIG_VALUE             (PWRAP_BASE+0xC4)
#define PMIC_WRAP_SIG_ERRVAL            (PWRAP_BASE+0xC8)
#define PMIC_WRAP_CRC_EN                (PWRAP_BASE+0xCC)
#define PMIC_WRAP_TIMER_EN              (PWRAP_BASE+0xD0)
#define PMIC_WRAP_TIMER_STA             (PWRAP_BASE+0xD4)
#define PMIC_WRAP_WDT_UNIT              (PWRAP_BASE+0xD8)
#define PMIC_WRAP_WDT_SRC_EN            (PWRAP_BASE+0xDC)
#define PMIC_WRAP_WDT_FLG               (PWRAP_BASE+0xE0)
#define PMIC_WRAP_DEBUG_INT_SEL         (PWRAP_BASE+0xE4)
#define PMIC_WRAP_DVFS_ADR0             (PWRAP_BASE+0xE8)
#define PMIC_WRAP_DVFS_WDATA0           (PWRAP_BASE+0xEC)
#define PMIC_WRAP_DVFS_ADR1             (PWRAP_BASE+0xF0)
#define PMIC_WRAP_DVFS_WDATA1           (PWRAP_BASE+0xF4)
#define PMIC_WRAP_DVFS_ADR2             (PWRAP_BASE+0xF8)
#define PMIC_WRAP_DVFS_WDATA2           (PWRAP_BASE+0xFC)
#define PMIC_WRAP_DVFS_ADR3             (PWRAP_BASE+0x100)
#define PMIC_WRAP_DVFS_WDATA3           (PWRAP_BASE+0x104)
#define PMIC_WRAP_DVFS_ADR4             (PWRAP_BASE+0x108)
#define PMIC_WRAP_DVFS_WDATA4           (PWRAP_BASE+0x10C)
#define PMIC_WRAP_DVFS_ADR5             (PWRAP_BASE+0x110)
#define PMIC_WRAP_DVFS_WDATA5           (PWRAP_BASE+0x114)
#define PMIC_WRAP_DVFS_ADR6             (PWRAP_BASE+0x118)
#define PMIC_WRAP_DVFS_WDATA6           (PWRAP_BASE+0x11C)
#define PMIC_WRAP_DVFS_ADR7             (PWRAP_BASE+0x120)
#define PMIC_WRAP_DVFS_WDATA7           (PWRAP_BASE+0x124)
#define PMIC_WRAP_SPMINF_STA            (PWRAP_BASE+0x128)
#define PMIC_WRAP_CIPHER_KEY_SEL        (PWRAP_BASE+0x12C)
#define PMIC_WRAP_CIPHER_IV_SEL         (PWRAP_BASE+0x130)
#define PMIC_WRAP_CIPHER_EN             (PWRAP_BASE+0x134)
#define PMIC_WRAP_CIPHER_RDY            (PWRAP_BASE+0x138)
#define PMIC_WRAP_CIPHER_MODE           (PWRAP_BASE+0x13C)
#define PMIC_WRAP_CIPHER_SWRST          (PWRAP_BASE+0x140)
#define PMIC_WRAP_DCM_EN                (PWRAP_BASE+0x144)
#define PMIC_WRAP_DCM_DBC_PRD           (PWRAP_BASE+0x148)


//-----macro for wrapper  regsister--------------------------------------------------------
#define GET_STAUPD_DLE_CNT(x)        ((x>>0)  & 0x00000007)
#define GET_STAUPD_ALE_CNT(x)        ((x>>3)  & 0x00000007)
#define GET_STAUPD_FSM(x)            ((x>>6)  & 0x00000007)
#define GET_WRAP_CH_DLE_RESTCNT(x)   ((x>>0)  & 0x00000007)
#define GET_WRAP_CH_ALE_RESTCNT(x)   ((x>>3)  & 0x00000003)
#define GET_WRAP_AG_DLE_RESTCNT(x)   ((x>>5)  & 0x00000003)
#define GET_WRAP_CH_W(x)             ((x>>7)  & 0x00000001)
#define GET_WRAP_CH_REQ(x)           ((x>>8)  & 0x00000001)
#define GET_AG_WRAP_W(x)             ((x>>9)  & 0x00000001)
#define GET_AG_WRAP_REQ(x)           ((x>>10) & 0x00000001)
#define GET_WRAP_FSM(x)              ((x>>11) & 0x0000000f)
#define GET_HARB_WRAP_WDATA(x)       ((x>>0)  & 0x0000ffff)
#define GET_HARB_WRAP_ADR(x)         ((x>>16) & 0x00007fff)
#define GET_HARB_WRAP_REQ(x)         ((x>>31) & 0x00000001)
#define GET_HARB_DLE_EMPTY(x)        ((x>>0)  & 0x00000001)
#define GET_HARB_DLE_FULL(x)         ((x>>1)  & 0x00000001)
#define GET_HARB_VLD(x)              ((x>>2)  & 0x00000001)
#define GET_HARB_DLE_OWN(x)          ((x>>3)  & 0x00000007)
#define GET_HARB_OWN(x)              ((x>>6)  & 0x00000007)
#define GET_HARB_DLE_RESTCNT(x)      ((x>>9)  & 0x00000007)
#define GET_AG_HARB_REQ(x)           ((x>>12) & 0x0000003f)
#define GET_HARB_WRAP_W(x)           ((x>>18) & 0x00000001)
#define GET_HARB_WRAP_REQ0(x)        ((x>>19) & 0x00000001)
#define GET_SPI_WDATA(x)             ((x>>0)  & 0x000000ff)
#define GET_SPI_OP(x)                ((x>>8)  & 0x0000001f)
#define GET_SPI_W(x)                 ((x>>13) & 0x00000001)
#define GET_MAN_RDATA(x)             ((x>>0)  & 0x000000ff)
#define GET_MAN_FSM(x)               ((x>>8)  & 0x00000007)
#define GET_MAN_REQ(x)               ((x>>11) & 0x00000001)
#define GET_WACS0_WDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS0_ADR(x)             ((x>>16) & 0x00007fff)
#define GET_WACS0_WRITE(x)           ((x>>31) & 0x00000001)
#define GET_WACS0_RDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS0_FSM(x)             ((x>>16) & 0x00000007)
#define GET_WACS0_REQ(x)             ((x>>19) & 0x00000001)
#define GET_SYNC_IDLE0(x)            ((x>>20) & 0x00000001)
#define GET_INIT_DONE0(x)            ((x>>21) & 0x00000001)
#define GET_SYS_IDLE0(x)             ((x>>22) & 0x00000001)
#define GET_WACS0_FIFO_FILLCNT(x)    ((x>>24) & 0x0000000f)
#define GET_WACS0_FIFO_FREECNT(x)    ((x>>28) & 0x0000000f)
#define GET_WACS1_WDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS1_ADR(x)             ((x>>16) & 0x00007fff)
#define GET_WACS1_WRITE(x)           ((x>>31) & 0x00000001)
#define GET_WACS1_RDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS1_FSM(x)             ((x>>16) & 0x00000007)
#define GET_WACS1_REQ(x)             ((x>>19) & 0x00000001)
#define GET_SYNC_IDLE1(x)            ((x>>20) & 0x00000001)
#define GET_INIT_DONE1(x)            ((x>>21) & 0x00000001)
#define GET_SYS_IDLE1(x)             ((x>>22) & 0x00000001)
#define GET_WACS1_FIFO_FILLCNT(x)    ((x>>24) & 0x0000000f)
#define GET_WACS1_FIFO_FREECNT(x)    ((x>>28) & 0x0000000f)
#define GET_WACS2_WDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS2_ADR(x)             ((x>>16) & 0x00007fff)
#define GET_WACS2_WRITE(x)           ((x>>31) & 0x00000001)
#define GET_WACS2_RDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS2_FSM(x)             ((x>>16) & 0x00000007)
#define GET_WACS2_REQ(x)             ((x>>19) & 0x00000001)
#define GET_SYNC_IDLE2(x)            ((x>>20) & 0x00000001)
#define GET_INIT_DONE2(x)            ((x>>21) & 0x00000001)
#define GET_SYS_IDLE2(x)             ((x>>22) & 0x00000001)
#define GET_WACS2_FIFO_FILLCNT(x)    ((x>>24) & 0x0000000f)
#define GET_WACS2_FIFO_FREECNT(x)    ((x>>28) & 0x0000000f)

//macro for staupd sta fsm
#define STAUPD_FSM_IDLE               (0x00)
#define STAUPD_FSM_REQ                (0x02)
#define STAUPD_FSM_WFDLE              (0x04) //wait for dle,wait for read data done,

//macro for MAN_RDATA  FSM
#define MAN_FSM_NO_REQ             (0x00)
#define MAN_FSM_IDLE               (0x00)
#define MAN_FSM_REQ                (0x02)
#define MAN_FSM_WFDLE              (0x04) //wait for dle,wait for read data done,
#define MAN_FSM_WFVLDCLR           (0x06)

//macro for WACS_FSM
#define WACS_FSM_IDLE               (0x00)
#define WACS_FSM_REQ                (0x02)
#define WACS_FSM_WFDLE              (0x04) //wait for dle,wait for read data done,
#define WACS_FSM_WFVLDCLR           (0x06) //finish read data , wait for valid flag clearing
#define WACS_INIT_DONE              (0x01)
#define WACS_SYNC_IDLE              (0x01)
#define WACS_SYNC_BUSY              (0x00)

//-----macro for wrapper bridge regsister--------------------------------------------------------
// APB Module peri_pwrap_bridge

#define PERI_PWRAP_BRIDGE_IARB_INIT         (PERI_PWRAP_BRIDGE_BASE+0x0)
#define PERI_PWRAP_BRIDGE_IORD_ARB_EN       (PERI_PWRAP_BRIDGE_BASE+0x4)
#define PERI_PWRAP_BRIDGE_IARB_STA0         (PERI_PWRAP_BRIDGE_BASE+0x8)
#define PERI_PWRAP_BRIDGE_IARB_STA1         (PERI_PWRAP_BRIDGE_BASE+0xC)
#define PERI_PWRAP_BRIDGE_WACS3_EN          (PERI_PWRAP_BRIDGE_BASE+0x10)
#define PERI_PWRAP_BRIDGE_INIT_DONE3        (PERI_PWRAP_BRIDGE_BASE+0x14)
#define PERI_PWRAP_BRIDGE_WACS3_CMD         (PERI_PWRAP_BRIDGE_BASE+0x18)
#define PERI_PWRAP_BRIDGE_WACS3_RDATA       (PERI_PWRAP_BRIDGE_BASE+0x1C)
#define PERI_PWRAP_BRIDGE_WACS3_VLDCLR      (PERI_PWRAP_BRIDGE_BASE+0x20)
#define PERI_PWRAP_BRIDGE_WACS4_EN          (PERI_PWRAP_BRIDGE_BASE+0x24)
#define PERI_PWRAP_BRIDGE_INIT_DONE4        (PERI_PWRAP_BRIDGE_BASE+0x28)
#define PERI_PWRAP_BRIDGE_WACS4_CMD         (PERI_PWRAP_BRIDGE_BASE+0x2C)
#define PERI_PWRAP_BRIDGE_WACS4_RDATA       (PERI_PWRAP_BRIDGE_BASE+0x30)
#define PERI_PWRAP_BRIDGE_WACS4_VLDCLR      (PERI_PWRAP_BRIDGE_BASE+0x34)
#define PERI_PWRAP_BRIDGE_INT_EN            (PERI_PWRAP_BRIDGE_BASE+0x38)
#define PERI_PWRAP_BRIDGE_INT_FLG_RAW       (PERI_PWRAP_BRIDGE_BASE+0x3C)
#define PERI_PWRAP_BRIDGE_INT_FLG           (PERI_PWRAP_BRIDGE_BASE+0x40)
#define PERI_PWRAP_BRIDGE_INT_CLR           (PERI_PWRAP_BRIDGE_BASE+0x44)
#define PERI_PWRAP_BRIDGE_TIMER_EN          (PERI_PWRAP_BRIDGE_BASE+0x48)
#define PERI_PWRAP_BRIDGE_TIMER_STA         (PERI_PWRAP_BRIDGE_BASE+0x4C)
#define PERI_PWRAP_BRIDGE_WDT_UNIT          (PERI_PWRAP_BRIDGE_BASE+0x50)
#define PERI_PWRAP_BRIDGE_WDT_SRC_EN        (PERI_PWRAP_BRIDGE_BASE+0x54)
#define PERI_PWRAP_BRIDGE_WDT_FLG           (PERI_PWRAP_BRIDGE_BASE+0x58)
#define PERI_PWRAP_BRIDGE_DEBUG_INT_SEL     (PERI_PWRAP_BRIDGE_BASE+0x5C)

//-----macro for wrapper bridge regsister--------------------------------------------------------

#define GET_WACS3_RDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS3_FSM(x)             ((x>>16) & 0x00000007)
#define GET_WACS3_REQ(x)             ((x>>19) & 0x00000001)
#define GET_SYNC_IDLE3(x)            ((x>>20) & 0x00000001)
#define GET_INIT_DONE3(x)            ((x>>21) & 0x00000001)
#define GET_WACS4_RDATA(x)           ((x>>0)  & 0x0000ffff)
#define GET_WACS4_FSM(x)             ((x>>16) & 0x00000007)
#define GET_WACS4_REQ(x)             ((x>>19) & 0x00000001)
#define GET_SYNC_IDLE4(x)            ((x>>20) & 0x00000001)
#define GET_INIT_DONE4(x)            ((x>>21) & 0x00000001)


//-----macro for dewrapper regsister--------------------------------------------------------
#define DEW_EVENT_OUT_EN   (DEW_BASE+0x0)
#define DEW_DIO_EN         (DEW_BASE+0x2)
#define DEW_EVENT_SRC_EN   (DEW_BASE+0x4)
#define DEW_EVENT_SRC      (DEW_BASE+0x6)
#define DEW_EVENT_FLAG     (DEW_BASE+0x8)
#define DEW_READ_TEST      (DEW_BASE+0xA)
#define DEW_WRITE_TEST     (DEW_BASE+0xC)
#define DEW_CRC_EN         (DEW_BASE+0xE)
#define DEW_CRC_VAL        (DEW_BASE+0x10)
#define DEW_MON_GRP_SEL    (DEW_BASE+0x12)
#define DEW_MON_FLAG_SEL   (DEW_BASE+0x14)
#define DEW_EVENT_TEST     (DEW_BASE+0x16)
#define DEW_CIPHER_KEY_SEL (DEW_BASE+0x18)
#define DEW_CIPHER_IV_SEL  (DEW_BASE+0x1A)
#define DEW_CIPHER_LOAD    (DEW_BASE+0x1C)
#define DEW_CIPHER_START   (DEW_BASE+0x1E)
#define DEW_CIPHER_RDY     (DEW_BASE+0x20)
#define DEW_CIPHER_MODE    (DEW_BASE+0x22)
#define DEW_CIPHER_SWRST   (DEW_BASE+0x24)
#define DEW_CIPHER_IV0     (DEW_BASE+0x26)
#define DEW_CIPHER_IV1     (DEW_BASE+0x28)
#define DEW_CIPHER_IV2     (DEW_BASE+0x2A)
#define DEW_CIPHER_IV3     (DEW_BASE+0x2C)
#define DEW_CIPHER_IV4     (DEW_BASE+0x2E)
#define DEW_CIPHER_IV5     (DEW_BASE+0x30)
//-----macro for dewrapper defaule value-------------------------------------------------------
#define DEFAULT_VALUE_READ_TEST      0x5aa5
#define WRITE_TEST_VALUE             0xa55a

//-----macro for manual commnd --------------------------------------------------------
#define OP_WR    (0x1)
#define OP_CSH   (0x0)
#define OP_CSL   (0x1)
#define OP_OUTS  (0x8)
#define OP_OUTD  (0x9)
#define OP_INS   (0xC)
#define OP_IND   (0xD)

//-----macro for read/write register --------------------------------------------------------

#define WRAP_RD32(addr)            DRV_Reg32(addr)
#define WRAP_WR32(addr,val)        mt65xx_reg_sync_writel((val), (addr))

#define WRAP_SET_BIT(BS,REG)       mt65xx_reg_sync_writel((DRV_Reg32(REG) | (U32)(BS)), (REG))
#define WRAP_CLR_BIT(BS,REG)       mt65xx_reg_sync_writel((DRV_Reg32(REG) & (~(U32)(BS))), (REG))

//-----------------soft reset --------------------------------------------------------


#define INFRA_GLOBALCON_RST0               (INFRACFG_AO_BASE+0x030)

#define PWRAP_SOFT_RESET                   WRAP_SET_BIT(1<<7,INFRA_GLOBALCON_RST0)
#define PWRAP_CLEAR_SOFT_RESET_BIT         WRAP_CLR_BIT(1<<7,INFRA_GLOBALCON_RST0)
#define PERI_GLOBALCON_RST1                (PERICFG_BASE+0x004)


#define PMIC_WRAP_DEBUG

#define PWRAPTAG                "[PMIC_WRAP] "
#ifdef PMIC_WRAP_DEBUG
#define PWRAPLOG(fmt, arg...)   dprintf(CRITICAL, PWRAPTAG fmt, ##arg)
#define PWRAPFUC(fmt, arg...)   dprintf(CRITICAL, PWRAPTAG "%s\n", __FUNCTION__)
#endif
#define PWRAPERR(fmt, arg...)   dprintf(INFO, PWRAPTAG "ERROR,line=%d " fmt, __LINE__, ##arg)
#define PWRAPREG(fmt, arg...)   dprintf(INFO, PWRAPTAG fmt,##arg)

#define E_PWR_INVALID_ARG               1
#define E_PWR_INVALID_RW                2
#define E_PWR_INVALID_ADDR              3
#define E_PWR_INVALID_WDAT              4
#define E_PWR_INVALID_OP_MANUAL         5
#define E_PWR_NOT_IDLE_STATE            6
#define E_PWR_NOT_INIT_DONE             7
#define E_PWR_NOT_INIT_DONE_READ        8
#define E_PWR_WAIT_IDLE_TIMEOUT         9
#define E_PWR_WAIT_IDLE_TIMEOUT_READ    10
#define E_PWR_INIT_SIDLY_FAIL           11
#define E_PWR_RESET_TIMEOUT             12
#define E_PWR_TIMEOUT                   13


#define E_PWR_INIT_RESET_SPI            20
#define E_PWR_INIT_SIDLY                21
#define E_PWR_INIT_REG_CLOCK            22
#define E_PWR_INIT_ENABLE_PMIC          23
#define E_PWR_INIT_DIO                  24
#define E_PWR_INIT_CIPHER               25
#define E_PWR_INIT_WRITE_TEST           26
#define E_PWR_INIT_ENABLE_CRC           27
#define E_PWR_INIT_ENABLE_DEWRAP        28
#define E_PWR_INIT_ENABLE_EVENT         29

#define E_PWR_READ_TEST_FAIL            30
#define E_PWR_WRITE_TEST_FAIL           31
#define E_PWR_SWITCH_DIO                32


#endif //__PMIC_WRAP_REGS_H__

