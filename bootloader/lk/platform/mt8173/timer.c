/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <sys/types.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <platform/timer.h>

#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_gpt.h>
#include <platform/mt_irq.h>

#include <kernel/thread.h>

static time_t system_time = 0;
static volatile time_t ticks = 0;
static time_t tick_interval;

static platform_timer_callback time_callback;
static void *callback_arg;

#define AP_PERI_GLOBALCON_PDN0 (PERICFG_BASE+0x10)

static enum handler_return timer_irq(void *arg)
{
	ticks += tick_interval;
	return time_callback(callback_arg, ticks);
}

void lk_scheduler(void)
{
	//static enum handler_return ret;

	/* ack GPT5 irq */
	DRV_WriteReg32(GPT_IRQACK_REG, 0x10);
	DRV_WriteReg32(GPT5_CON_REG, GPT_CLEAR);
	DRV_WriteReg32(GPT5_CON_REG, GPT_DISABLE);

	timer_irq(0);

	/*
	 * CAUTION! The de-assert signal to GIC might delay serveral clocks.
	 * Here must have enough delay to make sure the GPT signal had arrived GIC.
	 */
	/* ack GIC irq */
	mt_irq_ack(MT_GPT_IRQ_ID);

	/* enable GPT5 */
	DRV_WriteReg32(GPT5_CON_REG, GPT_ENABLE | GPT_MODE4_ONE_SHOT);

}

status_t platform_set_periodic_timer(platform_timer_callback callback, void *arg, time_t interval)
{
	time_callback = callback;
	tick_interval = interval;
	callback_arg = arg;

	DRV_WriteReg32(GPT_IRQEN_REG, 0);
	DRV_WriteReg32(GPT_IRQACK_REG, 0x3f);

	mt_irq_set_sens(MT_GPT_IRQ_ID, MT65xx_LEVEL_SENSITIVE);
	mt_irq_set_polarity(MT_GPT_IRQ_ID, MT65xx_POLARITY_LOW);

	DRV_WriteReg32(GPT5_CON_REG, 0x02);
	DRV_WriteReg32(GPT_IRQACK_REG, 0x10);
	DRV_WriteReg32(GPT5_CLK_REG, 0x0f);

	DRV_WriteReg32(GPT5_COMPARE_REG, interval * 203125 / 1000);
	DRV_WriteReg32(GPT_IRQEN_REG, 0x10);

	mt_irq_unmask(MT_GPT_IRQ_ID);

	DRV_WriteReg32(GPT5_CON_REG, GPT_ENABLE | GPT_MODE4_ONE_SHOT);

	return NO_ERROR;
}

time_t current_time(void)
{
	return ticks;
}

static void gpt_power_on (bool bPowerOn)
{
	if (!bPowerOn) {
		DRV_SetReg32(AP_PERI_GLOBALCON_PDN0, 1<<13);
	} else {
		DRV_ClrReg32(AP_PERI_GLOBALCON_PDN0, 1<<13);
	}
}

void platform_early_init_timer(void)
{
	//dprintf(SPEW, "platform_early_init_timer\n");

	gpt_power_on(1);
}

