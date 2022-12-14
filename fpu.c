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
	}
#if 0
	else {
		printf("%s: passed 0x%08x (0x%08x)\n",
		    name, result, result & mask);
	}
#endif
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
	}
#if 0
	else {
		printf("%s: passed %f\n", name, result);
	}
#endif
}

static uint32_t
raise_zx(void)
{
	volatile double a = atoi("1"), b = atoi("0"), d;
	fp fpscr;

	set_fpscr(false, FPSCR_ENABLE_MASK);

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
set_frfi(void)
{
	fp fpscr;

	asm __volatile (
		"mtfsb1	13;"
		"mtfsb1	14;"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
	);

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
	volatile double a = atoi("1"), b = atoi("0"), d;
	fp fpscr;

	d = a / b;

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

	set_fpscr(false, FPSCR_ENABLE_MASK);

	asm __volatile (
		"fmul	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a), [b] "f" (b)
	);

	return fpscr.word[1];
}

static uint32_t
raise_sox(void)
{
	double tmpa, tmpb, tmpd;
	volatile float a = 1.0e30, b = 1.0e30, d = 0.0;
	volatile float *ap = &a, *bp = &b, *dp = &d;
	fp fpscr;

	set_fpscr(false, FPSCR_ENABLE_MASK);

	asm __volatile (
		"lfs	%[a],0(%[ap]);"
		"lfs	%[b],0(%[bp]);"
		"fmuls	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		"stfs	%[d],0(%[dp]);"
		: [a] "=f" (tmpa), [b] "=f" (tmpb), [d] "=f" (tmpd),
		  [fpscr] "=f" (fpscr.fp)
		: [ap] "b" (ap), [bp] "b" (bp), [dp] "b" (dp)
	);

	printf("%s: %e (0x%08x)\n", __func__, d, *(uint32_t *)dp);

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
mtfsf_fprf1(void)
{
	fp a, b, fpscr;

	a.word[0] = 0;
	a.word[1] = FPSCR_FPRF;
	b.word[0] = 0;
	b.word[1] = 0;
	asm __volatile (
		"mtfsf	0xff,%[a];"
		"mtfsf	0x10,%[b];"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp), [b] "f" (b.fp)
	);

	return fpscr.word[1];
}

static uint32_t
mtfsf_fprf2(void)
{
	fp a, b, fpscr;

	a.word[0] = 0;
	a.word[1] = FPSCR_FPRF;
	b.word[0] = 0;
	b.word[1] = 0;
	asm __volatile (
		"mtfsf	0xff,%[a];"
		"mtfsf	0x08,%[b];"
		"mffs	%[fpscr];"
		: [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp), [b] "f" (b.fp)
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

#if 0
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
#endif

static uint32_t
arith_snan(void)
{
	fp a;
	volatile double b = 1.0, d;
	fp fpscr;

	set_fpscr(false, FPSCR_ENABLE_MASK);

	a.bin = FP_SNAN;
	asm __volatile (
		"fadd	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp), [b] "f" (b)
	);

	return fpscr.word[1];
}

static uint32_t
arith_qnan(void)
{
	fp a;
	volatile double b = 1.0, d;
	fp fpscr;

	set_fpscr(false, FPSCR_ENABLE_MASK);

	a.bin = FP_QNAN;
	asm __volatile (
		"fadd	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp), [b] "f" (b)
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

//	printf("%s: passed\n", __func__);
}

static const char *
str_rn(int rn)
{
	switch (rn) {
	case 0:
		return "RN";
	case 1:
		return "RZ";
	case 2:
		return "RP";
	case 3:
		return "RM";
	}
	return NULL;
}

static void
fctiw(double val, int exp_rn, int exp_rz, int exp_rp, int exp_rm)
{
	volatile double b, d;
	fp fpscr;
	int rn, exp[4], i;
	int *ip = &i;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	exp[0] = exp_rn;
	exp[1] = exp_rz;
	exp[2] = exp_rp;
	exp[3] = exp_rm;

	for (rn = 0; rn < 4; rn++) {
		set_fpscr(false, 0xffffffff);
		set_fpscr(false, FPSCR_ENABLE_MASK);
		set_fpscr(true, rn);
		i = 0;
		fpscr.bin = 0;
		b = val;
		asm __volatile (
			"fctiw	%[d],%[b];"
			"mffs	%[fpscr];"
			"stfiwx	%[d],0,%[ip];"
			: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
			: [b] "f" (b), [ip] "r" (ip)
			: "memory"
		);
		if (i != exp[rn])
			printf("%s: FAILED %f (%s) -> "
			    "%d (0x%08x) != %d (0x%08x)\n",
			    __func__, val, str_rn(rn),
			    i, i, exp[rn], exp[rn]);
#if 0
		else
			printf("%s: PASSED %f (%s) -> "
			    "%d (0x%08x)\n",
			    __func__, val, str_rn(rn), i, i);
#endif
#if 0
		printf("%s: %f -> fpscr 0x%08x\n",
		    __func__, val, fpscr.word[1]);
#endif

#define	QNAN_MASK	0xfffffffcU
#define	SNAN_MASK	QNAN_MASK
#define	INF_MASK	QNAN_MASK
#define	QNAN_VAL	(FPSCR_FX | FPSCR_VX | FPSCR_VXCVI)
#define	SNAN_VAL	(FPSCR_VXSNAN | QNAN_VAL)
#define	INF_VAL		QNAN_VAL

		fp v;
		v.fp = val;
		if (FP_ISQNAN(v.bin)) {
			if ((fpscr.word[1] & QNAN_MASK) != QNAN_VAL)
				printf("%s: SNaN FAILED FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#if 0
			else
				printf("%s: SNaN passed FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#endif
		}
		if (FP_ISSNAN(v.bin)) {
			if ((fpscr.word[1] & SNAN_MASK) != SNAN_VAL)
				printf("%s: QNaN FAILED FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#if 0
			else
				printf("%s: QNaN passed FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#endif
		}
		if (FP_ISINF(v.bin)) {
			if ((fpscr.word[1] & INF_MASK) != INF_VAL)
				printf("%s: INF FAILED FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#if 0
			else
				printf("%s: INF passed FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#endif
		}

		set_fpscr(false, 0xffffffff);
		set_fpscr(false, FPSCR_ENABLE_MASK);
		set_fpscr(true, rn);
		i = 0;
		b = val;
		asm __volatile (
			"fctiwz	%[d],%[b];"
			"mffs	%[fpscr];"
			"stfiwx	%[d],0,%[ip];"
			: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
			: [b] "f" (b), [ip] "r" (ip)
			: "memory"
		);
		if (i != exp_rz)
			printf("%s: FAILED %f (z:%s) -> "
			    "%d (0x%08x) != %d (0x%08x)\n",
			    __func__, val, str_rn(rn),
			    i, i, exp_rz, exp_rz);
#if 0
		else
			printf("%s: PASSED %f (z:%s) -> "
			    "%d (0x%08x)\n",
			    __func__, val, str_rn(rn), i, i);
#endif
#if 0
		printf("%sz: %f -> fpscr 0x%08x\n",
		    __func__, val, fpscr.word[1]);
#endif

		v.fp = val;
		if (FP_ISQNAN(v.bin)) {
			if ((fpscr.word[1] & QNAN_MASK) != QNAN_VAL)
				printf("%s: SNaN FAILED FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#if 0
			else
				printf("%s: SNaN passed FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#endif
		}
		if (FP_ISSNAN(v.bin)) {
			if ((fpscr.word[1] & SNAN_MASK) != SNAN_VAL)
				printf("%s: QNaN FAILED FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#if 0
			else
				printf("%s: QNaN passed FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#endif
		}
		if (FP_ISINF(v.bin)) {
			if ((fpscr.word[1] & INF_MASK) != INF_VAL)
				printf("%s: INF FAILED FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#if 0
			else
				printf("%s: INF passed FPSCR %08x\n",
				    __func__, fpscr.word[1]);
#endif
		}
	}
#undef	QNAN_MASK
#undef	SNAN_MASK
#undef	INF_MASK
#undef	QNAN_VAL
#undef	SNAN_VAL
#undef	INF_VAL
}

static void
fctid(double val, int64_t exp_rn, int64_t exp_rz, int64_t exp_rp, int64_t exp_rm)
{
	volatile double b, d;
	fp fpscr;
	int64_t rn, exp[4], i;
	int64_t *ip = &i;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);
	set_msr(false, MSR_FE_MASK);

	exp[0] = exp_rn;
	exp[1] = exp_rz;
	exp[2] = exp_rp;
	exp[3] = exp_rm;

	for (rn = 0; rn < 4; rn++) {
		set_fpscr(false, 0xffffffff);
		set_fpscr(false, FPSCR_ENABLE_MASK);
		set_fpscr(true, rn);
		i = 0;
		fpscr.bin = 0;
		b = val;
		asm __volatile (
			"fctid	%[d],%[b];"
			"mffs	%[fpscr];"
			"stfd	%[d],0(%[ip]);"
			: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
			: [b] "f" (b), [ip] "b" (ip)
			: "memory"
		);
		if (i != exp[rn])
			printf("%s: FAILED %f (%s) -> "
			    "%lld (0x%016llx) != %lld (0x%016llx)\n",
			    __func__, val, str_rn(rn),
			    i, i, exp[rn], exp[rn]);
#if 0
		else
			printf("%s: PASSED %f (%s) -> "
			    "%lld (0x%016llx)\n",
			    __func__, val, str_rn(rn), i, i);
#endif
#if 0
		printf("%s: %f -> fpscr 0x%08x\n",
		    __func__, val, fpscr.word[1]);
#endif

		set_fpscr(false, 0xffffffff);
		set_fpscr(false, FPSCR_ENABLE_MASK);
		set_fpscr(true, rn);
		i = 0;
		fpscr.bin = 0;
		b = val;
		asm __volatile (
			"fctidz	%[d],%[b];"
			"mffs	%[fpscr];"
			"stfd	%[d],0(%[ip]);"
			: [d] "=f" (d), [fpscr] "=f" (fpscr.fp)
			: [b] "f" (b), [ip] "b" (ip)
			: "memory"
		);
		if (i != exp_rz)
			printf("%s: FAILED %f (z:%s) -> "
			    "%lld (0x%016llx) != %lld (0x%016llx)\n",
			    __func__, val, str_rn(rn),
			    i, i, exp_rz, exp_rz);
#if 0
		else
			printf("%s: PASSED %f (z:%s) -> "
			    "%lld (0x%016llx)\n",
			    __func__, val, str_rn(rn), i, i);
#endif
#if 0
		printf("%sz: %f -> fpscr 0x%08x\n",
		    __func__, val, fpscr.word[1]);
#endif
	}
}

static void
round_double(void)
{
	volatile fp a, b, d, fpscr;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);
	set_msr(false, MSR_FE_MASK);

	a.bin = __BIT(53);
	b.fp  = 0.5;
	d.bin = 0;
	fpscr.bin = 0;
	__asm volatile (
		"fmul	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d.fp), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp), [b] "f" (b.fp)
		: "memory"
	);
	printf("%s: %e (0x%016llx) : 0x%08x\n", __func__,
	    d.fp, d.bin, fpscr.word[1]);

	set_fpscr(true, RN_RZ);

	a.fp = 1.0e300;
	b.fp = 1.0e300;
	d.bin = 0;
	fpscr.bin = 0;
	__asm volatile (
		"fmul	%[d],%[a],%[b];"
		"mffs	%[fpscr];"
		: [d] "=f" (d.fp), [fpscr] "=f" (fpscr.fp)
		: [a] "f" (a.fp), [b] "f" (b.fp)
		: "memory"
	);
	printf("%s: %e (0x%016llx) : 0x%08x\n", __func__,
	    d.fp, d.bin, fpscr.word[1]);
}

#if 0
static void
fres(uint32_t rn, uint64_t b)
{
	volatile double d, a = 1.0;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_fpscr(true, rn);
	set_msr(false, MSR_FE_MASK);

#if 1
	__asm volatile (
		"fres	%[d],%[b];"
		: [d] "=f" (d)
		: [b] "f" (b)
	);
#else
	__asm volatile (
		"fdivs	%[d],%[a],%[b];"
		: [d] "=f" (d)
		: [a] "f" (a), [b] "f" (b)
	);
#endif
	printf("%s: 1 / %e (0x%016llx) ~ %e (0x%016llx)\n", __func__,
	    *(double *)&b, b, d, *(uint64_t *)&d);
}

static void
frsqrte(uint32_t rn, uint64_t b)
{
	volatile double d, a = 1.0;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_fpscr(true, rn);
	set_msr(false, MSR_FE_MASK);

	__asm volatile (
		"frsqrte %[d],%[b];"
		: [d] "=f" (d)
		: [b] "f" (b)
	);
	printf("%s: 1 / sqrt(%e) [1 / sqrt(0x%016llx)] ~\n"
	    "\t%e (0x%016llx)\n", __func__,
	    *(double *)&b, b, d, *(uint64_t *)&d);
}
#endif

#if 0
static void
fmadd(double a, double b, double c)
{
	volatile double d;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	__asm volatile(
		"fmadd	%[d],%[a],%[b],%[c];"
		: [d] "=f" (d)
		: [a] "f" (a), [b] "f" (b), [c] "f" (c)
	);
	printf("%s: %e x %e + %e = %e\n", __func__, a, c, b, d);
}

static void
fmsub(double a, double b, double c)
{
	volatile double d;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	__asm volatile(
		"fmsub	%[d],%[a],%[b],%[c];"
		: [d] "=f" (d)
		: [a] "f" (a), [b] "f" (b), [c] "f" (c)
	);
	printf("%s: %e x %e - %e = %e\n", __func__, a, c, b, d);
}

static void
fnmadd(double a, double b, double c)
{
	volatile double d;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	__asm volatile(
		"fnmadd	%[d],%[a],%[b],%[c];"
		: [d] "=f" (d)
		: [a] "f" (a), [b] "f" (b), [c] "f" (c)
	);
	printf("%s: -(%e x %e + %e) = %e\n", __func__, a, c, b, d);
}

static void
fnmsub(double a, double b, double c)
{
	volatile double d;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	__asm volatile(
		"fnmsub	%[d],%[a],%[b],%[c];"
		: [d] "=f" (d)
		: [a] "f" (a), [b] "f" (b), [c] "f" (c)
	);
	printf("%s: -(%e x %e - %e) = %e\n", __func__, a, c, b, d);
}
#endif

#define	TEST_2OP_D(op)							\
static void								\
op(uint64_t v, uint64_t e, int exp_fpscr)				\
{									\
	volatile fp val, exp, ret, fpscr;				\
									\
	set_fpscr(false, 0xffffffff);					\
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */		\
	set_msr(false, MSR_FE_MASK);					\
									\
	val.bin = v;							\
	exp.bin = e;							\
	fpscr.bin = 0;							\
	asm __volatile (						\
		#op							\
		"	%[ret],%[val];"					\
		"mffs	%[fpscr];"					\
		: [ret] "=f" (ret.fp), [fpscr] "=f" (fpscr.fp)		\
		: [val] "f" (val.fp)					\
		: "memory"						\
	);								\
	if (ret.bin != exp.bin) {					\
		printf("%s: %f (0x%016llx) val: FAILED\n", __func__,	\
		    val.fp, val.bin);					\
		printf("\t%f (0x%016llx) !=\n", ret.fp, ret.bin);	\
		printf("\t%f (0x%016llx)\n", exp.fp, exp.bin);		\
	}								\
	if (fpscr.word[1] != exp_fpscr) {				\
		printf("%s: %f (0x%016llx) fpscr: FAILED\n", __func__,	\
		    val.fp, val.bin);					\
		printf("\t0x%08x !=\n", fpscr.word[1]);			\
		printf("\t0x%08x\n", exp_fpscr);			\
	}								\
}

TEST_2OP_D(fsqrt)
TEST_2OP_D(frsp)
TEST_2OP_D(frin)

TEST_2OP_D(fre)
TEST_2OP_D(fres)	/* XXX TEST_2OP_S */
TEST_2OP_D(frsqrte)
TEST_2OP_D(frsqrtes)	/* XXX TEST_2OP_S */

#define	TEST_3OP_D(op)							\
static void								\
op(uint64_t a, uint64_t b, uint64_t e, int exp_fpscr)			\
{									\
	volatile fp vala, valb, exp, ret, fpscr;			\
									\
	set_fpscr(false, 0xffffffff);					\
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */		\
	set_msr(false, MSR_FE_MASK);					\
									\
	vala.bin = a;							\
	valb.bin = b;							\
	exp.bin = e;							\
	fpscr.bin = 0;							\
	asm __volatile (						\
		#op							\
		"	%[ret],%[a],%[b];"				\
		"mffs	%[fpscr];"					\
		: [ret] "=f" (ret.fp), [fpscr] "=f" (fpscr.fp)		\
		: [a] "f" (vala.fp), [b] "f" (valb.fp)			\
		: "memory"						\
	);								\
	if (ret.bin != exp.bin) {					\
		printf("%s: %f (0x%016llx) %f (0x%016llx) val: FAILED\n", \
		    __func__, vala.fp, vala.bin, valb.fp, valb.bin);	\
		printf("\t%f (0x%016llx) !=\n", ret.fp, ret.bin);	\
		printf("\t%f (0x%016llx)\n", exp.fp, exp.bin);		\
	}								\
	if (fpscr.word[1] != exp_fpscr) {				\
		printf("%s: %f (0x%016llx) %f (0x%016llx) fpscr: FAILED\n", \
		    __func__, vala.fp, vala.bin, valb.fp, valb.bin);	\
		printf("\t0x%08x !=\n", fpscr.word[1]);			\
		printf("\t0x%08x\n", exp_fpscr);			\
	}								\
}

TEST_3OP_D(fadd)
TEST_3OP_D(fsub)
TEST_3OP_D(fsubs)
TEST_3OP_D(fmul)
TEST_3OP_D(fdiv)

#define	TEST_4OP_D(op)							\
static void								\
op(uint64_t a, uint64_t b, uint64_t c, uint64_t e, int exp_fpscr)	\
{									\
	volatile fp vala, valb, valc, exp, ret, fpscr;			\
									\
	set_fpscr(false, 0xffffffff);					\
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */		\
	set_msr(false, MSR_FE_MASK);					\
									\
	vala.bin = a;							\
	valb.bin = b;							\
	valc.bin = c;							\
	exp.bin = e;							\
	fpscr.bin = 0;							\
	asm __volatile (						\
		#op							\
		"	%[ret],%[a],%[b],%[c];"				\
		"mffs	%[fpscr];"					\
		: [ret] "=f" (ret.fp), [fpscr] "=f" (fpscr.fp)		\
		: [a] "f" (vala.fp), [b] "f" (valb.fp), [c] "f" (valc.fp) \
		: "memory"						\
	);								\
	if (ret.bin != exp.bin) {					\
		printf("%s: %f (0x%016llx) %f (0x%016llx) %f (0x%016llx) " \
		    "val: FAILED\n", 					\
		    __func__, vala.fp, vala.bin, valb.fp, valb.bin,	\
		    valc.fp, valc.bin);					\
		printf("\t%f (0x%016llx) !=\n", ret.fp, ret.bin);	\
		printf("\t%f (0x%016llx)\n", exp.fp, exp.bin);		\
	}								\
	if (fpscr.word[1] != exp_fpscr) {				\
		printf("%s: %f (0x%016llx) %f (0x%016llx) %f (0x%016llx) " \
		    "fpscr: FAILED\n", 					\
		    __func__, vala.fp, vala.bin, valb.fp, valb.bin,	\
		    valc.fp, valc.bin);					\
		printf("\t0x%08x !=\n", fpscr.word[1]);			\
		printf("\t0x%08x\n", exp_fpscr);			\
	}								\
}

TEST_4OP_D(fmadd)
TEST_4OP_D(fnmadd)
TEST_4OP_D(fnmsub)

static void
fcfid(int64_t i)
{
	volatile fp d, b;
	uint64_t *p = &i;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	b.bin = 0;
	d.bin = 0;
	__asm volatile(
		"lfd	%[b],0(%[p]);"
		"fcfid	%[d],%[b];"
		: [b] "=f" (b.fp), [d] "=f" (d.fp)
		: [p] "b" (p)
	);
	printf("%s: %lld (0x%016llx) -> %e (0x%016llx)\n", __func__,
	    i, i, d.fp, d.bin);
}

#define	TEST_3OP_D_RN(op)						\
static void								\
op ##_rn(uint32_t rn, uint64_t a, uint64_t b, uint64_t e, uint32_t e_f)	\
{									\
	volatile fp d, fpscr;						\
									\
	e_f |= rn;							\
									\
	set_fpscr(false, 0xffffffff);					\
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */		\
	set_fpscr(true, rn);						\
	set_msr(false, MSR_FE_MASK);					\
									\
	__asm volatile(							\
		#op							\
		"	%[d],%[a],%[b];"				\
		"mffs	%[fpscr];"					\
		: [d] "=f" (d), [fpscr] "=f" (fpscr)			\
		: [a] "f" (a), [b] "f" (b)				\
	);								\
	if (d.bin != e)							\
		printf("%s: FAILED val %e (0x%016llx) != %e (0x%016llx)\n", \
		    __func__, d.fp, d.bin, *(double *)&e, e);		\
	if (fpscr.word[1] != e_f)					\
		printf("%s: FAILED fpscr 0x%08x != 0x%08x\n",		\
		    __func__, fpscr.word[1], e_f);			\
}

TEST_3OP_D_RN(fadd)
TEST_3OP_D_RN(fmul)
TEST_3OP_D_RN(fdiv)

#define	FCMP_TEST(op)							\
static void								\
op(uint64_t a, uint64_t b, uint32_t ecc, uint32_t efpscr)		\
{									\
	volatile fp fpscr;						\
	uint32_t orig_cr, cr;						\
									\
	efpscr |= ecc;							\
	ecc >>= (4 * 3);						\
									\
	set_fpscr(false, 0xffffffff);					\
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */		\
	set_msr(false, MSR_FE_MASK);					\
									\
	__asm volatile (						\
		"mfcr	%[orig_cr];"					\
		#op							\
		"	7,%[a],%[b];"					\
		"mffs	%[fpscr];"					\
		"mfcr	%[cr];"						\
		: [orig_cr] "=r" (orig_cr),				\
			[fpscr] "=f" (fpscr), [cr] "=r" (cr)		\
		: [a] "f" (a), [b] "f" (b)				\
		: "cr7"							\
	);								\
	ecc |= (orig_cr & ~0xf);					\
	if (cr != ecc)							\
		printf("%s: FAILED %e (0x%016llx) vs %e (0x%016llx):\n"	\
		    "\tcr 0x%08x != 0x%08x\n", __func__,		\
		    *(double *)&a, a, *(double *)&b, b, cr, ecc);	\
	if (fpscr.word[1] != efpscr)					\
		printf("%s: FAILED %e (0x%016llx) vs %e (0x%016llx):\n"	\
		    "\tfpscr 0x%08x != 0x%08x\n", __func__,		\
		    *(double *)&a, a, *(double *)&b, b,			\
		    fpscr.word[1], efpscr);				\
}

FCMP_TEST(fcmpu)
FCMP_TEST(fcmpo)

static void
stfs(uint64_t val, uint32_t exp)
{
	fp fp;
	fp.bin = val;
	uint32_t ret, *p = &ret;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	__asm volatile(
		"stfs %[val],0(%[p]);"
		:
		: [val] "f" (val), [p] "b" (p)
		:
	);
	if (ret != exp)
		printf("%s: %e (0x%016llx) FAILED\n"
		    "\t%e (0x%08x) != %e (0x%08x)\n", __func__, fp.fp, fp.bin,
		    *(float *)&ret, ret, *(float *)&exp, exp);
}

static void
lfs(uint32_t val, uint64_t exp)
{
	sfp sfp;
	sfp.bin = val;
	uint64_t ret;
	uint32_t *p = &sfp.bin;

	set_fpscr(false, 0xffffffff);
	set_fpscr(false, FPSCR_ENABLE_MASK);	/* XXXRO */
	set_msr(false, MSR_FE_MASK);

	__asm volatile(
		"lfs %[ret],0(%[p]);"
		: [ret] "=&f" (ret)
		: [p] "b" (p)
		:
	);
	if (ret != exp)
		printf("%s: %e (0x%08x) FAILED\n"
		    "\t%e (0x%016llx) != %e (0x%016llx)\n", __func__,
		    sfp.sfp, sfp.bin,
		    *(double *)&ret, ret, *(double *)&exp, exp);
}

int
main(void)
{
	fp fp;

	/* expected values were taken from MPC8245 (603e) */

	TEST_FPSCR(raise_zx,	0x84005000, 0);
	TEST_FPSCR(raise_ox,	0x92065000, 0);
	TEST_FPSCR(raise_sox,	0x92025000, 0);
	TEST_FPSCR(set_fex,	0x000000f8, 0);
	TEST_FPSCR(set_vx,	0x000000f8, 0);
	TEST_FPSCR(set_fprf,	0x0001c0f8, 0);
	TEST_FPSCR(set_frfi,	0x000600f8, 0);
	TEST_FPSCR(set_bit31,	0x000000f9, 0);
	TEST_FPSCR(set_zx,	0xc40000f8, 0);
	TEST_FPSCR(clear_xe,	0x000000f0, 0);
	TEST_FPSCR(set_xe,	0x000000f8, 0);
	TEST_FPSCR(set_vxisi,	0xe08000f8, 0);
	TEST_FPSCR(set_vx_clear_fx,
				0x608000f8, 0);
	TEST_FPSCR(reset_vx,	0xe0c000f8, 0);
#ifdef notyet /* XXX also, not well-defined */
	TEST_FPSCR(fex,		0x440020f8, 0);	/* fix enabled mask */
#endif
	TEST_FPSCR(mtfsf,	0x90000000, 0);
	TEST_FPSCR(mtfsf_fprf1,	0x0000f000, 0);
	TEST_FPSCR(mtfsf_fprf2,	0x00010000, 0);
	TEST_FPSCR(mcrfs,	0x000000f8, 0);
	TEST_FPSCR(what_this,	0x000000f8, 0);
	TEST_FPSCR(fneg,	0x000000f8, 0);
//	TEST_FPSCR(fadd,	0x000040f8, 0);
//	TEST_FPSCR(fadds,	0x000040f8, 0);
	TEST_FPSCR(arith_snan,	0xa1011000, 0);
	TEST_FPSCR(arith_qnan,	0x00011000, 0);
	TEST_FPSCR(fsel_qnan_s,	0x000000f8, 0);

	/* XXX sfp overflow */

	TEST_RD(fsel_snan,	0.0);
	TEST_RD(fsel_qnan,	0.0);
	TEST_RD(fsel_mzero,	1.0);

	set_fx();

	fctiw(0.0, 0, 0, 0, 0);
	fctiw(0.6, 1, 0, 1, 0);
	fctiw(0.5, 0, 0, 1, 0);
	fctiw(0.4, 0, 0, 1, 0);
	fctiw(-0.6, -1, 0, 0, -1);
	fctiw(-0.5, 0, 0, 0, -1);
	fctiw(-0.4, 0, 0, 0, -1);
	fctiw(1.6, 2, 1, 2, 1);
	fctiw(1.5, 2, 1, 2, 1);
	fctiw(1.4, 1, 1, 2, 1);
	fctiw(-1.6, -2, -1, -1, -2);
	fctiw(-1.5, -2, -1, -1, -2);
	fctiw(-1.4, -1, -1, -1, -2);
	fctiw((double)__BIT(32),
	    0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff);
	fctiw(-(double)__BIT(32),
	    __BIT(31), __BIT(31), __BIT(31), __BIT(31));
	fp.bin = FP_PINF;
	fctiw(fp.fp, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff);
	fp.bin = FP_MINF;
	fctiw(fp.fp, __BIT(31), __BIT(31), __BIT(31), __BIT(31));
	fp.bin = FP_SNAN;
	fctiw(fp.fp, __BIT(31), __BIT(31), __BIT(31), __BIT(31));
	fp.bin = FP_QNAN;
	fctiw(fp.fp, __BIT(31), __BIT(31), __BIT(31), __BIT(31));

#if 0
	fctid(0.0, 0, 0, 0, 0);
	fctid(0.6, 1, 0, 1, 0);
	fctid(0.5, 0, 0, 1, 0);
	fctid(0.4, 0, 0, 1, 0);
	fctid(-0.6, -1, 0, 0, -1);
	fctid(-0.5, 0, 0, 0, -1);
	fctid(-0.4, 0, 0, 0, -1);
	fctid(1.6, 2, 1, 2, 1);
	fctid(1.5, 2, 1, 2, 1);
	fctid(1.4, 1, 1, 2, 1);
	fctid(-1.6, -2, -1, -1, -2);
	fctid(-1.5, -2, -1, -1, -2);
	fctid(-1.4, -1, -1, -1, -2);
	fctid((double)__BIT(63),
	    0x7fffffffffffffff, 0x7fffffffffffffff,
	    0x7fffffffffffffff, 0x7fffffffffffffff);
	fctid(-(double)(__BIT(63) + 1),
	    __BIT(63), __BIT(63), __BIT(63), __BIT(63));
	fp.bin = FP_PINF;
	fctid(fp.fp, 0x7fffffffffffffff, 0x7fffffffffffffff,
	    0x7fffffffffffffff, 0x7fffffffffffffff);
	fctid(fp.fp, 0x7fffffffffffffff, 0x7fffffffffffffff,
	    0x7fffffffffffffff, 0x7fffffffffffffff);
	fp.bin = FP_MINF;
	fctid(fp.fp, __BIT(63), __BIT(63), __BIT(63), __BIT(63));
	fp.bin = FP_SNAN;
	fctid(fp.fp, __BIT(63), __BIT(63), __BIT(63), __BIT(63));
	fp.bin = FP_QNAN;
	fctid(fp.fp, __BIT(63), __BIT(63), __BIT(63), __BIT(63));
	fctid((double)0x0123456700000000LL, 0x0123456700000000LL,
	    0x0123456700000000LL, 0x0123456700000000LL, 0x0123456700000000LL);
#endif

	round_double();

#if 0
	fres(RN_RZ, FP_P1);
	fres(RN_RZ, FP_P0_5);
	fres(RN_RZ, FP_PZERO);

	frsqrte(RN_RZ, FP_P1);
	frsqrte(RN_RZ, FP_P0_5);
	frsqrte(RN_RZ, FP_PZERO);
#endif

#if 0
	fmadd(1.0, 1.0, 1.0);
	fmsub(1.0, 1.0, 1.0);
	fnmadd(1.0, 1.0, 1.0);
	fnmsub(1.0, 1.0, 1.0);
#endif

#if 0
	/* Tested on G5 */
	fsqrt(FP_MINF, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSQRT | FPRF_C | FPRF_FU);
	fsqrt(FP_M1, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSQRT | FPRF_C | FPRF_FU);
	fsqrt(FP_MZERO, FP_MZERO, FPRF_C | FPRF_FE);
	fsqrt(FP_PZERO, FP_PZERO, FPRF_FE);
	fsqrt(FP_P1, FP_P1, FPRF_FG);
	fsqrt(FP_PINF, FP_PINF, FPRF_FG | FPRF_FU);
	fsqrt(FP_SNAN, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fsqrt(FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);
#endif

	frsp(FP_QNAN1, FP_QNAN1 & 0xffffffffe0000000ULL, FPRF_C | FPRF_FU);
	frsp(FP_SNAN1, FP_QNAN1 & 0xffffffffe0000000ULL,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

	frsp(FP_QNAN, FP_QNAN & 0xffffffffe0000000ULL, FPRF_C | FPRF_FU);
	frsp(FP_SNAN, FP_QNAN & 0xffffffffe0000000ULL,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

#if 0
	/* not implemented even on G5 */
	frin(0, 0, 0);
#endif

	fadd(FP_P1, FP_P1, FP_P2, FPRF_FG);
	fadd(FP_P1, FP_M1, FP_PZERO, FPRF_FE);
	fadd(FP_M1, FP_P1, FP_PZERO, FPRF_FE);
	fadd(FP_QNAN, FP_P1, FP_QNAN, FPRF_C | FPRF_FU);
	fadd(FP_P1, FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);
	fadd(FP_QNAN, FP_QNAN_CPU, FP_QNAN, FPRF_C | FPRF_FU);
	fadd(FP_QNAN_CPU, FP_QNAN, FP_QNAN_CPU, FPRF_C | FPRF_FU);
	fadd(FP_SNAN, FP_P1, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fadd(FP_QNAN_CPU, FP_SNAN, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fadd(FP_SNAN, FP_QNAN_CPU, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

	fadd(FP_PINF, FP_MINF, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXISI | FPRF_C | FPRF_FU);

	fsub(FP_P1, FP_P1, FP_PZERO, FPRF_FE);
	fsub(FP_P1, FP_M1, FP_P2, FPRF_FG);
	fsub(FP_P1, FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);

	fsubs(FP_P1, FP_QNAN, FP_QNAN & 0xffffffffe0000000ULL,
	    FPRF_C | FPRF_FU);

	fmul(FP_P1, FP_P1, FP_P1, FPRF_FG);
	fmul(FP_P1, FP_M1, FP_M1, FPRF_FL);
	fmul(FP_M1, FP_P1, FP_M1, FPRF_FL);
	fmul(FP_QNAN, FP_P1, FP_QNAN, FPRF_C | FPRF_FU);
	fmul(FP_P1, FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);
	fmul(FP_QNAN, FP_QNAN_CPU, FP_QNAN, FPRF_C | FPRF_FU);
	fmul(FP_QNAN_CPU, FP_QNAN, FP_QNAN_CPU, FPRF_C | FPRF_FU);
	fmul(FP_SNAN, FP_P1, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fmul(FP_QNAN_CPU, FP_SNAN, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fmul(FP_SNAN, FP_QNAN_CPU, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

	fmul(FP_PINF, FP_PZERO, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXIMZ | FPRF_C | FPRF_FU);

	fmul(FP_SNAN, FP_M1, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

	fdiv(FP_P1, FP_MZERO, FP_MINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FL | FPRF_FU);

	fdiv(FP_PINF, FP_PINF, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXIDI | FPRF_C | FPRF_FU);
	fdiv(FP_PZERO, FP_PZERO, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXZDZ | FPRF_C | FPRF_FU);

	fmadd(FP_QNAN_CPU, FP_QNAN1, FP_SNAN, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fmadd(FP_QNAN_CPU, FP_SNAN, FP_QNAN1, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fmadd(FP_SNAN, FP_QNAN_CPU, FP_QNAN1, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

	fnmadd(FP_P1, FP_P1, FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);
	fnmsub(FP_P1, FP_P1, FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);
	fnmadd(FP_P1, FP_P1, FP_SNAN, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fnmsub(FP_P1, FP_P1, FP_SNAN, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);

	fadd(FP_P_DEN_MIN, FP_P_DEN_MIN, FP_P_DEN_MINx2, FPRF_C | FPRF_FG);
	fsub(FP_P_DEN_MINx2, FP_P_DEN_MIN, FP_P_DEN_MIN, FPRF_C | FPRF_FG);
	fmul(FP_P_DEN_MIN, FP_P2, FP_P_DEN_MINx2, FPRF_C | FPRF_FG);
	fdiv(FP_P_DEN_MIN, FP_P0_5, FP_P_DEN_MINx2, FPRF_C | FPRF_FG);
#if 0
	fsqrt(FP_P_DEN_MIN, FP_SQRT_DEN_MIN, FPRF_FG);
	fsqrt(FP_P_DEN_MINx9, FP_SQRT_DEN_MINx3, FPRF_FG);
#endif

#if 0
	/* Even G5 does not implement this... */
	fre(FP_MINF, FP_MZERO, FPRF_C | FPRF_FE);
	fre(FP_MZERO, FP_MINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FL);
	fre(FP_PZERO, FP_PINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FG);
	fre(FP_PINF, FP_PZERO, FPRF_FE);
	fre(FP_SNAN, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fre(FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);
#endif

	fres(FP_MINF, FP_MZERO, FPRF_C | FPRF_FE);
	fres(FP_MZERO, FP_MINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FL);
	fres(FP_PZERO, FP_PINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FG);
	fres(FP_PINF, FP_PZERO, FPRF_FE);
	fres(FP_SNAN1, FP_QNAN1 & 0xffffffffe0000000ULL,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	fres(FP_QNAN1, FP_QNAN1 & 0xffffffffe0000000ULL, FPRF_C | FPRF_FU);

	frsqrte(FP_MINF, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSQRT | FPRF_C | FPRF_FU);
	frsqrte(FP_M2, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSQRT | FPRF_C | FPRF_FU);
	frsqrte(FP_MZERO, FP_MINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FL);
	frsqrte(FP_PZERO, FP_PINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FG);
	frsqrte(FP_PINF, FP_PZERO, FPRF_FE);
	frsqrte(FP_SNAN, FP_QNAN,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	frsqrte(FP_QNAN, FP_QNAN, FPRF_C | FPRF_FU);

#if 0
	/* Even G5 does not implement this... */
	frsqrtes(FP_MINF, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSQRT | FPRF_C | FPRF_FU);
	frsqrtes(FP_M2, FP_QNAN_CPU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSQRT | FPRF_C | FPRF_FU);
	frsqrtes(FP_MZERO, FP_MINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FL);
	frsqrtes(FP_PZERO, FP_PINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FU | FPRF_FG);
	frsqrtes(FP_PINF, FP_PZERO, FPRF_FE);
	frsqrtes(FP_SNAN1, FP_QNAN1 & 0xffffffffe0000000ULL,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN | FPRF_C | FPRF_FU);
	frsqrtes(FP_QNAN1, FP_QNAN1 & 0xffffffffe0000000ULL, FPRF_C | FPRF_FU);
#endif

	fmul(FP_P_DEN_MIN, FP_P0_5, FP_PZERO,
	    FPSCR_FX | FPSCR_UX | FPSCR_XX | FPSCR_FI | FPRF_FE);
	fmul(FP_P_DEN_MINx3, FP_P0_5, FP_P_DEN_MINx2 /* RN */,
	    FPSCR_FX | FPSCR_UX | FPSCR_XX | FPSCR_FR | FPSCR_FI |
	    FPRF_C | FPRF_FG);
	fmul_rn(RN_RZ, FP_P_DEN_MINx3, FP_P0_5, FP_P_DEN_MIN,
	    FPSCR_FX | FPSCR_UX | FPSCR_XX | FPSCR_FI |
	    FPRF_C | FPRF_FG);

	fmul_rn(RN_RN, FP_P_NORM_MAX, FP_P2, FP_PINF,
	    FPSCR_FX | FPSCR_OX | FPSCR_XX | FPSCR_FI | FPRF_FG | FPRF_FU);
	fmul_rn(RN_RZ, FP_P_NORM_MAX, FP_P2, FP_P_NORM_MAX,
	    FPSCR_FX | FPSCR_OX | FPSCR_XX | FPSCR_FI | FPRF_FG);
	fmul_rn(RN_RP, FP_P_NORM_MAX, FP_P2, FP_PINF,
	    FPSCR_FX | FPSCR_OX | FPSCR_XX | FPSCR_FI | FPRF_FG | FPRF_FU);
	fmul_rn(RN_RM, FP_P_NORM_MAX, FP_P2, FP_P_NORM_MAX,
	    FPSCR_FX | FPSCR_OX | FPSCR_XX | FPSCR_FI | FPRF_FG);

	fadd_rn(RN_RN, FP_P1, FP_M1, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RZ, FP_P1, FP_M1, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RP, FP_P1, FP_M1, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RM, FP_P1, FP_M1, FP_MZERO, FPRF_C | FPRF_FE);

	fadd_rn(RN_RN, FP_PZERO, FP_PZERO, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RZ, FP_PZERO, FP_PZERO, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RP, FP_PZERO, FP_PZERO, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RM, FP_PZERO, FP_PZERO, FP_PZERO, FPRF_FE);

	fadd_rn(RN_RN, FP_PZERO, FP_MZERO, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RZ, FP_PZERO, FP_MZERO, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RP, FP_PZERO, FP_MZERO, FP_PZERO, FPRF_FE);
	fadd_rn(RN_RM, FP_PZERO, FP_MZERO, FP_MZERO, FPRF_C | FPRF_FE);

	fadd_rn(RN_RN, FP_MZERO, FP_MZERO, FP_MZERO, FPRF_C | FPRF_FE);
	fadd_rn(RN_RZ, FP_MZERO, FP_MZERO, FP_MZERO, FPRF_C | FPRF_FE);
	fadd_rn(RN_RP, FP_MZERO, FP_MZERO, FP_MZERO, FPRF_C | FPRF_FE);
	fadd_rn(RN_RM, FP_MZERO, FP_MZERO, FP_MZERO, FPRF_C | FPRF_FE);

	fdiv_rn(RN_RZ, FP_P1, FP_MZERO, FP_MINF,
	    FPSCR_FX | FPSCR_ZX | FPRF_FL | FPRF_FU);

	fcmpu(FP_P1, FP_P2, FPRF_FL, 0);
	fcmpu(FP_P1, FP_M2, FPRF_FG, 0);
	fcmpu(FP_M1, FP_M2, FPRF_FG, 0);
	fcmpu(FP_PZERO, FP_MZERO, FPRF_FE, 0);
	fcmpu(FP_PINF, FP_MINF, FPRF_FG, 0);
	fcmpu(FP_PINF, FP_PINF, FPRF_FE, 0);
	fcmpu(FP_PINF, FP_P1, FPRF_FG, 0);
	fcmpu(FP_PINF, FP_PZERO, FPRF_FG, 0);
	fcmpu(FP_MINF, FP_PZERO, FPRF_FL, 0);
	fcmpu(FP_PZERO, FP_QNAN, FPRF_FU, 0);
	fcmpu(FP_P1, FP_QNAN, FPRF_FU, 0);
	fcmpu(FP_PINF, FP_QNAN, FPRF_FU, 0);
	fcmpu(FP_QNAN, FP_QNAN, FPRF_FU, 0);
	fcmpu(FP_P1, FP_SNAN, FPRF_FU,
	    FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN);

#define	EX_NAN	(FPSCR_FX | FPSCR_VX | FPSCR_VXVC)

	fcmpo(FP_P1, FP_P2, FPRF_FL, 0);
	fcmpo(FP_P1, FP_M2, FPRF_FG, 0);
	fcmpo(FP_M1, FP_M2, FPRF_FG, 0);
	fcmpo(FP_PZERO, FP_MZERO, FPRF_FE, 0);
	fcmpo(FP_PINF, FP_MINF, FPRF_FG, 0);
	fcmpo(FP_PINF, FP_PINF, FPRF_FE, 0);
	fcmpo(FP_PINF, FP_P1, FPRF_FG, 0);
	fcmpo(FP_PINF, FP_PZERO, FPRF_FG, 0);
	fcmpo(FP_MINF, FP_PZERO, FPRF_FL, 0);
	fcmpo(FP_PZERO, FP_QNAN, FPRF_FU, EX_NAN);
	fcmpo(FP_P1, FP_QNAN, FPRF_FU, EX_NAN);
	fcmpo(FP_PINF, FP_QNAN, FPRF_FU, EX_NAN);
	fcmpo(FP_QNAN, FP_QNAN, FPRF_FU, EX_NAN);
	fcmpo(FP_P1, FP_SNAN, FPRF_FU,
	    EX_NAN | FPSCR_FX | FPSCR_VX | FPSCR_VXSNAN);

#undef	EX_NAN

#if 0
	/* Tested on G5 */
	fcfid(1);
	fcfid(-1);
	fcfid(2);
	fcfid(-2);
	fcfid(0x12345678deadbeefULL);
	fcfid(0x92345678deadbeefULL);
#endif

	stfs(FP_P1, SFP_P1);
	stfs(FP_M1, SFP_M1);
	stfs(FP_PZERO, SFP_PZERO);
	stfs(FP_MZERO, SFP_MZERO);
	stfs(FP_PINF, SFP_PINF);
	stfs(FP_MINF, SFP_MINF);
	stfs(FP_QNAN, SFP_MASK_EXP | __SHIFTOUT(FP_QNAN, __BITS(51, 29)));
	stfs(FP_QNAN | FP_SIGN,
	    SFP_SIGN | SFP_MASK_EXP | __SHIFTOUT(FP_QNAN, __BITS(51, 29)));
	stfs(__SHIFTIN(896, __BITS(62, 52)), __BIT(22));
	stfs(__SHIFTIN(874, __BITS(62, 52)), 1);
	stfs(FP_SIGN | __SHIFTIN(896, __BITS(62, 52)), SFP_SIGN | __BIT(22));
	stfs(FP_SIGN | __SHIFTIN(874, __BITS(62, 52)), SFP_SIGN | 1);

	stfs(FP_P1 | __BITS(28, 0), SFP_P1);
	stfs(FP_SIGN | FP_P1 | __BITS(28, 0), SFP_SIGN | SFP_P1);
	stfs(FP_P_NORM_MAX, FP_NORM_TO_SINGLE(FP_P_NORM_MAX));
	stfs(FP_SIGN | FP_P_NORM_MAX,
	    FP_NORM_TO_SINGLE(FP_SIGN | FP_P_NORM_MAX));

#if 0
	/* undefined, G5 seems to round them to sign|0. */
	stfs(__SHIFTIN(873, __BITS(62, 52)), 0x34800000U);
	stfs(__SHIFTIN(872, __BITS(62, 52)), 0x34000000U);
	stfs(FP_P_DEN_MIN, 0x00800000U);
	stfs(FP_P_DEN_MINx9, 0x00800000U);

	stfs(FP_SIGN | __SHIFTIN(873, __BITS(62, 52)), SFP_SIGN | 0x34800000U);
	stfs(FP_SIGN | __SHIFTIN(872, __BITS(62, 52)), SFP_SIGN | 0x34000000U);
	stfs(FP_SIGN | FP_P_DEN_MIN, SFP_SIGN | 0x00800000U);
	stfs(FP_SIGN | FP_P_DEN_MINx9, SFP_SIGN | 0x00800000U);
#endif

	lfs(SFP_P1, FP_P1);
	lfs(SFP_M1, FP_M1);
	lfs(SFP_PZERO, FP_PZERO);
	lfs(SFP_MZERO, FP_MZERO);
	lfs(SFP_PINF, FP_PINF);
	lfs(SFP_MINF, FP_MINF);
	lfs(SFP_QNAN,
	    FP_MASK_EXP | (__SHIFTOUT(SFP_QNAN, SFP_MASK_FRAC) << (51 - 22)));
	lfs(SFP_SIGN | SFP_QNAN,
	    FP_SIGN | FP_MASK_EXP |
	    (__SHIFTOUT(SFP_QNAN, SFP_MASK_FRAC) << (51 - 22)));
	lfs(SFP_P_DEN_MIN, __SHIFTIN(- 126 - 23 + 1023, FP_MASK_EXP));
	lfs(SFP_SIGN | SFP_P_DEN_MIN,
	    FP_SIGN | __SHIFTIN(- 126 - 23 + 1023, FP_MASK_EXP));

	return 0;
}
