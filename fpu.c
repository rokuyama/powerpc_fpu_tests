#include <stdio.h>
#include <stdlib.h>

#include "fpu.h"

#define	TEST_FPSCR(func, expected, ignored)				\
    test_fpscr(func, expected, ignored, #func)

static void
test_fpscr(uint32_t (*func)(void), uint32_t expected, uint32_t ignored,
    const char *name)
{
	uint32_t mask, result;

	mask = 0xffffffffU & ~ignored;

//	set_fpscr(false, FPSCR_MCRFS_MASK | FPSCR_FR | FPSCR_FI | FPSCR_FPRF);
	set_fpscr(false, 0xffffffff);
	set_fpscr(true, FPSCR_ENABLE_MASK);
	set_msr(false, MSR_FE_MASK);

	result = func();
	if ((result & mask) != (expected & mask)) {
		printf("%s: FAILED\n", name);
		printf("\texpected 0x%08x (0x%08x)\n",
		    expected, expected & mask);
		printf("\tresult   0x%08x (0x%08x)\n",
		    result, result & mask);
	} else {
		printf("%s: passed 0x%08x (0x%08x)\n",
		    name, result, result & mask);
	}
}

#define	TEST_RD(func, expected)	test_rd(func, expected, #func)

static void
test_rd(double (*func)(void), double expected, const char *name)
{
	double result;

//	set_fpscr(false, FPSCR_MCRFS_MASK | FPSCR_FR | FPSCR_FI | FPSCR_FPRF);
	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);
	set_msr(false, MSR_FE_MASK);

	result = func();
	if (result != expected) {
		printf("%s: FAILED %f != %f\n", name, result, expected);
	} else {
		printf("%s: passed %f\n", name, result);
	}
}

static uint32_t
raise_zx(void)
{
	volatile double a = atoi("1"), b = atoi("0"), d;
	fp fpscr;

	asm __volatile (
		"fdiv	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a), [b] "f" (b)
	);

	return fpscr.word[1];
}

static uint32_t
set_fex(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	1;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
set_vx(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	2;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
set_fprf(void)
{
	fp fpscr;

#if 1
	asm __volatile (
		"mtfsb1	15;"
		"mtfsb1	16;"
		"mtfsb1	17;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);
#else
	asm __volatile (
		"mtfsb1	15;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);
	printf("%s 0x%08x\n", __func__, fpscr.word[1]);

	asm __volatile (
		"mtfsb1	16;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);
	printf("%s 0x%08x\n", __func__, fpscr.word[1]);

	asm __volatile (
		"mtfsb1	17;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);
	printf("%s 0x%08x\n", __func__, fpscr.word[1]);
#endif

	return fpscr.word[1];
}

static uint32_t
set_bit31(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	31;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
set_zx(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	5;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
set_vxisi(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	8;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
set_vx_clear_fx(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	8;"
		"mtfsb0 0;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
reset_vx(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	8;"
		"mtfsb0 0;"
		"mtfsb1 9;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
clear_xe(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb0	28;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
set_xe(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb0	28;"
		"mtfsb1	28;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

	return fpscr.word[1];
}

static uint32_t
fex(void)
{
	fp fpscr;

	(void)raise_zx();

	asm __volatile(
		"mtfsb0	0;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);
	return fpscr.word[1];
}

static uint32_t
raise_ox(void)
{
	volatile double a = 1.0e300, b = 1.0e300, d;
	fp fpscr;

	asm __volatile (
		"fmul	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a), [b] "f" (b)
	);

	return fpscr.word[1];
}

static uint32_t
mtfsf(void)
{
	fp a, fpscr;

	a.word[0] = 0;
	a.word[1] = 0x90000000;
	asm __volatile (
		"mtfsf	0xff,%[a];"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp)
	);

	return fpscr.word[1];
}

static uint32_t
mcrfs(void)
{
	fp a, fpscr;

	a.word[0] = 0;
	a.word[1] = 0x900000f8;
	asm __volatile (
		"mtfsf	0xff,%[a];"
		"mcrfs	7,0;"
		"mcrfs	7,6;"
		"mcrfs	7,7;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp)
	);

	return fpscr.word[1];
}

static uint32_t
fneg(void)
{
	volatile double a = 1.0, d;
	fp fpscr;

	asm __volatile (
		"fneg	%[d],%[a];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a)
	);

	return fpscr.word[1];
}

static uint32_t
what_this(void)
{

	set_fpscr(true, FPSCR_VX);
	set_fpscr(false, FPSCR_MCRFS_MASK | FPSCR_FR | FPSCR_FI | FPSCR_FPRF);
	return fneg();
}

static uint32_t
fadd(void)
{
	volatile double a = 1.0, b = 1.0, d;
	fp fpscr;

	asm __volatile (
		"fadd	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a), [b] "f" (b)
	);

	return fpscr.word[1];
}

static uint32_t
fsel_qnan_s(void)
{
	volatile double c = 1.0, b = 0.0, d;
	fp a, fpscr;

	a.bin = FP_QNAN;
	asm __volatile (
		"fsel	%[d],%[a],%[c],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a), [c] "f" (c), [b] "f" (b)
	);

	return fpscr.word[1];
}

static double
fsel(uint64_t bin)
{
	volatile double c = 1.0, b = 0.0, d;
	volatile fp a;

	a.bin = bin;
	asm __volatile (
		"fsel	%[d],%[a],%[c],%[b];"
		: [d] "=f" (d)
		: [a] "f" (a.fp), [c] "f" (c), [b] "f" (b)
	);

	return d;
}

static double
fsel_snan(void)
{

	return fsel(FP_SNAN);
}

static double
fsel_qnan(void)
{

	return fsel(FP_QNAN);
}

static double
fsel_mzero(void)
{

	return fsel(FP_MZERO);
}

static void
set_fx(void)
{

	set_fpscr(false, FPSCR_MCRFS_MASK | FPSCR_FR | FPSCR_FI | FPSCR_FPRF);
	set_fpscr(true, FPSCR_ENABLE_MASK);
	set_msr(false, MSR_FE_MASK);

	set_fpscr(true, FPSCR_FX);

	printf("%s: passed\n", __func__);
}

int
main(void)
{

	/* expected values were taken from MPC8245 (603e) */

	TEST_FPSCR(raise_zx,	0xc40020f8, FPSCR_FPRF);
	TEST_FPSCR(raise_ox,	0xd20640f8, FPSCR_FPRF);
	TEST_FPSCR(set_fex,	0x000000f8, 0);
	TEST_FPSCR(set_vx,	0x000000f8, 0);
	TEST_FPSCR(set_fprf,	0x0001c0f8, 0);
	TEST_FPSCR(set_bit31,	0x000000f9, 0);
	TEST_FPSCR(set_zx,	0xc40000f8, 0);
	TEST_FPSCR(clear_xe,	0x000000f0, 0);
	TEST_FPSCR(set_xe,	0x000000f8, 0);
	TEST_FPSCR(set_vxisi,	0xe08000f8, 0);
	TEST_FPSCR(set_vx_clear_fx,
				0x608000f8, 0);
	TEST_FPSCR(reset_vx,	0xe0c000f8, 0);
	TEST_FPSCR(fex,		0x440000f8, FPSCR_FPRF);
	TEST_FPSCR(mtfsf,	0x90000000, 0);
	TEST_FPSCR(mcrfs,	0x000000f8, 0);
	TEST_FPSCR(what_this,	0x000000f8, 0);
	TEST_FPSCR(fneg,	0x000000f8, 0);
/*NG*/	TEST_FPSCR(fadd,	0x000040f8, 0);
	TEST_FPSCR(fsel_qnan_s,	0x000000f8, FPSCR_FPRF);

	/* XXX sfp overflow */

	TEST_RD(fsel_snan,	0.0);
	TEST_RD(fsel_qnan,	0.0);
	TEST_RD(fsel_mzero,	1.0);

	set_fx();

	return 0;
}
