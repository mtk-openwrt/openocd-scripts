// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 */

#include <common.h>
#include <asm/sections.h>
#include <asm/cacheops.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/addrspace.h>
#include <asm/system.h>
#include <asm/cm.h>

static void enable_cca_override(void)
{
	u32 val;

	val = readl(mips_cm_base() + GCR_BASE);
	val &= ~CCA_DEFAULT_OVERRIDE_VALUE_MASK;
	val |= CCA_DEFAULT_OVERRIDE_ENABLE | \
	       (2 << CCA_DEFAULT_OVERRIDE_VALUE_SHIFT);
	writel(val, mips_cm_base() + GCR_BASE);
}

static void disable_cca_override(void)
{
	u32 val;

	val = readl(mips_cm_base() + GCR_BASE);
	val &= ~(CCA_DEFAULT_OVERRIDE_VALUE_MASK |
		 CCA_DEFAULT_OVERRIDE_ENABLE);
	writel(val, mips_cm_base() + GCR_BASE);
}

/* A simple function to initialize MT7621's cache */
void _start(void)
{
	ulong lsize = CONFIG_SYS_DCACHE_LINE_SIZE;
	u32 val;
	ulong addr;

	/* Set KSEG0 to uncached */
	val = read_c0_config();
	val &= ~CONF_CM_CMASK;
	val |= CONF_CM_UNCACHED;
	write_c0_config(val);
	ehb();

	/* Enable CCA override. Set to uncached */
	enable_cca_override();

	/* Initialize L1 I-Cache */
	write_c0_taglo(0);
	write_c0_taghi(0);

	for (addr = 0; addr < CONFIG_SYS_ICACHE_SIZE; addr += lsize)
		mips_cache(INDEX_STORE_TAG_I, (void *) addr);

	/* Initialize L1 D-Cache */
	write_c0_dtaglo(0);
	__write_32bit_c0_register($29, 2, 0); /* dtaghi */

	for (addr = 0; addr < CONFIG_SYS_DCACHE_SIZE; addr += lsize)
		mips_cache(INDEX_STORE_TAG_D, (void *) addr);

	/* Initialize L2 Cache */
	write_c0_staglo(0);
	__write_32bit_c0_register($29, 4, 0); /* staghi */

	for (addr = 0; addr < (256 << 10); addr += lsize)
		mips_cache(INDEX_STORE_TAG_SD, (void *) addr);

	/* Dsiable CCA override */
	disable_cca_override();

	/* Set KSEG0 to non-coherent cached (important!) */
	val = read_c0_config();
	val &= ~CONF_CM_CMASK;
	val |= CONF_CM_CACHABLE_NONCOHERENT;
	write_c0_config(val);
	ehb();

	/* Again, invalidate L1 D-Cache */
	for (addr = 0; addr < CONFIG_SYS_DCACHE_SIZE; addr += lsize)
		mips_cache(INDEX_WRITEBACK_INV_D, (void *) addr);

	/* Invalidate L1 I-Cache */
	for (addr = 0; addr < CONFIG_SYS_ICACHE_SIZE; addr += lsize)
		mips_cache(INDEX_INVALIDATE_I, (void *) addr);

	/* Disable L2 cache bypass */
	val = read_c0_config2();
	val &= ~MIPS_CONF_IMPL;
	write_c0_config2(val);
	ehb();

	/* Set KSEG0 to coherent cached */
	val = read_c0_config();
	val &= ~CONF_CM_CMASK;
	val |= CONF_CM_CACHABLE_COW;
	write_c0_config(val);
	ehb();

	asm volatile ("sdbbp");

	while (1);
}
