#include <sys/types.h>
#include <sys/cdefs.h>

#include <stdbool.h>

#include <stdio.h>

/* Double-Precision in FPR */

#define	FP_MASK_SIGN	__BIT(63)
#define	FP_MASK_EXP	__BITS(52, 62)
#define	FP_MASK_FRAC	__BITS(0, 51)

#define	FP_SIGN		FP_MASK_SIGN

#define	FP_PZERO	0ULL
#define	FP_MZERO	(FP_SIGN | FP_PZERO)

#define	FP_PINF		FP_MASK_EXP
#define	FP_MINF		(FP_SIGN | FP_PINF)

#define	FP_SNAN		(FP_MASK_EXP | 0x0007deadbeef1234)	/* XXX */
#define	FP_SNAN1	(FP_MASK_EXP | 0x0007ffffffffffff)	/* XXX */
#define	FP_QNAN_BIT	__BIT(51)
#define	FP_QNAN_CPU	(FP_MASK_EXP | FP_QNAN_BIT)
#define	FP_QNAN		(FP_SNAN | FP_QNAN_BIT)
#define	FP_QNAN1	(FP_SNAN1 | FP_QNAN_BIT)

#define	FP_P1		0x3ff0000000000000ULL
#define	FP_M1		(FP_MASK_SIGN | FP_P1)

#define	FP_P2		0x4000000000000000ULL
#define	FP_M2		(FP_MASK_SIGN | FP_P2)

#define	FP_ISNAN(u64)							\
    (((u64) & FP_MASK_EXP) == FP_MASK_EXP && ((u64) & FP_MASK_FRAC) != 0)
#define	FP_ISQNAN(u64)	(FP_ISNAN(u64) && ((u64) & FP_QNAN_BIT) != 0)
#define	FP_ISSNAN(u64)	(FP_ISNAN(u64) && ((u64) & FP_QNAN_BIT) == 0)

#define	FP_ISINF(u64)							\
    (((u64) & FP_MASK_EXP) == FP_MASK_EXP && ((u64) & FP_MASK_FRAC) == 0)

/* Single-Precision in FPR */

#define	SFP_MASK_SIGN	FP_MASK_SIGN
#define	SFP_MASK_EXP	FP_MASK_EXP			/* XXX */
#define	SFP_MASK_FRAC	__BITS(29, 51)

#define	SFP_PZERO	0ULL
#define	SFP_MZERO	(SFP_MASK_SIGN | SFP_PZERO)

#define	SFP_PINF	SFP_MASK_EXP
#define	SFP_MINF	(SFP_MASK_SIGN | SFP_PINF)

#define	SFP_QNAN	(SFP_MASK_EXP | __BITS(29, 51)) /* XXX */
#define	SFP_SNAN	(SFP_MASK_EXP | __BITS(29, 50))	/* XXX */

/* FPSCR bits */

#define	FPSCR_FX	__BIT(31)	/* s */
#define	FPSCR_FEX	__BIT(30)	/* or */
#define	FPSCR_VX	__BIT(29)	/* or */

#define	FPSCR_OX	__BIT(28)
#define	FPSCR_UX	__BIT(27)
#define	FPSCR_ZX	__BIT(26)
#define	FPSCR_XX	__BIT(25)

#define	FPSCR_VXSNAN	__BIT(24)
#define	FPSCR_VXISI	__BIT(23)
#define	FPSCR_VXIDI	__BIT(22)
#define	FPSCR_VXZDZ	__BIT(21)
#define	FPSCR_VXIMZ	__BIT(20)
#define	FPSCR_VXVC	__BIT(19)

#define	FPSCR_FR	__BIT(18)
#define	FPSCR_FI	__BIT(17)
#define	FPSCR_FPRF	__BITS(12, 16)
#define	 FPRF_C		__BIT(16)
#define	 FPRF_FL	__BIT(15)
#define	 FPRF_FG	__BIT(14)
#define	 FPRF_FE	__BIT(13)
#define	 FPRF_FU	__BIT(12)
#define	  FPRF_QNAN	(FPRF_C | FPRF_FU)
#define	  FPRF_MINF	(FPRF_FL | FPRF_FU)
#define	  FPRF_MNOR	FPRF_FL
#define	  FPRF_MDEN	(FPRF_C | FPRF_FL)
#define	  FPRF_MZERO	(FPRF_C | FPRF_FE)
#define	  FPRF_PZERO	FPRF_FE
#define	  FPRF_PDEN	(FPRF_C | FPRF_FG)
#define	  FPRF_PNOR	FPRF_FG
#define	  FPRF_PINF	(FPRF_FG | FPRF_FU)

#define	FPSCR_RSVD	__BIT(11)

#define	FPSCR_VXSOFT	__BIT(10)
#define	FPSCR_VXSQRT	__BIT(9)
#define	FPSCR_VXCVI	__BIT(8)

#define	FPSCR_VE	__BIT(7)
#define	FPSCR_OE	__BIT(6)
#define	FPSCR_UE	__BIT(5)
#define	FPSCR_ZE	__BIT(4)
#define	FPSCR_XE	__BIT(3)

#define	FPSCR_NI	__BIT(2)

#define	FPSCR_RN	__BITS(1, 0)
#define	 RN_RN		0x0ULL
#define	 RN_RZ		0x1ULL
#define	 RN_RP		0x2ULL
#define	 RN_RM		0x3ULL

#define	FPSCR_ENABLE_MASK						\
    (FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE)

#define	FPSCR_MCRFS_MASK						\
    (									\
	FPSCR_FX     | FPSCR_OX     | 					\
	FPSCR_UX     | FPSCR_ZX     | FPSCR_XX    | FPSCR_VXSNAN | 	\
	FPSCR_VXISI  | FPSCR_VXIDI  | FPSCR_VXZDZ | FPSCR_VXIMZ  |	\
	FPSCR_VXVC   | 							\
	FPSCR_VXSOFT | FPSCR_VXSQRT | FPSCR_VXCVI			\
    )

/* MSR bits */
#define	MSR_FE0		__BIT(11)
#define	MSR_FE1		__BIT(8)
#define	MSR_FE_MASK	(MSR_FE0 | MSR_FE1)

typedef union fp {
	double fp;
	uint64_t bin;
	uint32_t word[2];
} fp;

static inline uint32_t
mfmsr(void)
{
	uint32_t msr;

	__asm volatile ("mfmsr %0" : "=r"(msr));
	return msr;
}

static inline uint32_t
mtmsr(uint32_t msr)
{

	__asm volatile ("mtmsr %0" : : "r"(msr));
}

static inline uint32_t
mffs(void)
{
	fp fpscr;

	__asm volatile("mffs %0" : "=f"(fpscr.fp));
	return fpscr.word[1];
}

static inline void
mtffs(uint32_t bits)
{
	fp fpscr;

	fpscr.word[0] = 0;
	fpscr.word[1] = bits;
	__asm __volatile("mtfsf 0xff,%0" : : "f"(fpscr.fp));
}

static inline void
set_msr(bool on, uint32_t mask)
{
	uint32_t msr;

	msr = mfmsr();
	if (on)
		msr |= mask;
	else
		msr &= ~mask;
	mtmsr(msr);
}

static inline void
set_fpscr(bool on, uint32_t mask)
{
	uint32_t fpscr;

	fpscr = mffs();
//printf("%s 0x%08x\n", __func__, fpscr);
	if (on)
		fpscr |= mask;
	else
		fpscr &= ~mask;
	mtffs(fpscr);
}
