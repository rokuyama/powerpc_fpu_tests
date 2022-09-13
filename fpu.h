#include <sys/types.h>
#include <sys/cdefs.h>

#include <stdbool.h>

#include <stdio.h>

/* Double-Precision in FPR */

#define	FP_MASK_SIGN	__BIT(63)	/* 0x8000000000000000ULL */
#define	FP_MASK_EXP	__BITS(52, 62)	/* 0x7ff0000000000000ULL */
#define	FP_MASK_FRAC	__BITS(0, 51)	/* 0x000fffffffffffffULL */

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
#define	FP_M1		(FP_SIGN | FP_P1)

#define	FP_P0_5		0x3fe0000000000000ULL
#define	FP_M0_5		(FP_SIGN | FP_P0_5)

#define	FP_P2		0x4000000000000000ULL
#define	FP_M2		(FP_SIGN | FP_P2)

#define	FP_P1_DELTA	0x3ff0000000000001ULL
#define	FP_M1_DELTA	(FP_SIGN | FP_P1_DELTA)

#define	FP_P_DEN_MIN	0x0000000000000001ULL
#define	FP_M_DEN_MIN	(FP_SIGN | FP_P_DEN_MIN)

#define	FP_P_DEN_MINx2	0x0000000000000002ULL
#define	FP_M_DEN_MINx2	(FP_SIGN | FP_P_DEN_MIN)

#define	FP_P_DEN_MINx3	0x0000000000000003ULL
#define	FP_M_DEN_MINx3	(FP_SIGN | FP_P_DEN_MIN)

#define	FP_P_DEN_MINx9	0x0000000000000009ULL
#define	FP_M_DEN_MINx9	(FP_SIGN | FP_P_DEN_MIN)

#define	FP_SQRT_DEN_MIN		0x1e60000000000000ULL
#define	FP_SQRT_DEN_MINx3	0x1e78000000000000ULL

#define	FP_P_NORM_MAX	0x7fefffffffffffffULL
#define	FP_M_NORM_MAX	(FP_SIGN | FP_P_NORM_MAX)

#define	FP_ISNAN(u64)							\
    (((u64) & FP_MASK_EXP) == FP_MASK_EXP && ((u64) & FP_MASK_FRAC) != 0)
#define	FP_ISQNAN(u64)	(FP_ISNAN(u64) && ((u64) & FP_QNAN_BIT) != 0)
#define	FP_ISSNAN(u64)	(FP_ISNAN(u64) && ((u64) & FP_QNAN_BIT) == 0)

#define	FP_ISINF(u64)							\
    (((u64) & FP_MASK_EXP) == FP_MASK_EXP && ((u64) & FP_MASK_FRAC) == 0)

/* Single-Precision in memory */

#define	SFP_MASK_SIGN		__BIT(31)	/* 0x80000000U */
#define	SFP_SIGN		SFP_MASK_SIGN
#define	SFP_MASK_EXP		__BITS(30, 23)	/* 0x7f800000U */
#define	SFP_MASK_FRAC		__BITS(22, 0)	/* 0x007fffffU */

#define	SFP_PZERO		0U
#define	SFP_MZERO		(SFP_SIGN | SFP_PZERO)

#define	SFP_PINF		SFP_MASK_EXP
#define	SFP_MINF		(SFP_SIGN | SFP_PINF)

#if 0
#define	FP_SNAN		(FP_MASK_EXP | 0x0007deadbeef1234)	/* XXX */
#define	FP_SNAN1	(FP_MASK_EXP | 0x0007ffffffffffff)	/* XXX */
#define	FP_QNAN_BIT	__BIT(51)
#define	FP_QNAN_CPU	(FP_MASK_EXP | FP_QNAN_BIT)
#define	FP_QNAN		(FP_SNAN | FP_QNAN_BIT)
#define	FP_QNAN1	(FP_SNAN1 | FP_QNAN_BIT)
#endif

#define	SFP_SNAN		(SFP_MASK_EXP | 0x003deadbU)
#define	SFP_SNAN1		(SFP_MASK_EXP | 0x003fffffU)
#define	SFP_QNAN_BIT		__BIT(22)
#define	SFP_QNAN_CPU		(SFP_MASK_EXP | SFP_QNAN_BIT)
#define	SFP_QNAN		(SFP_SNAN | SFP_QNAN_BIT)
#define	SFP_QNAN1		(SFP_SNAN1 | SFP_QNAN_BIT)

#define	SFP_P1			0x3f800000U
#define	SFP_M1			(SFP_SIGN | SFP_P1)

#define	SFP_P0_5		0x3f000000U
#define	SFP_M0_5		(SFP_SIGN | SFP_P0_5)

#define	SFP_P2			0x40000000U
#define	SFP_M2			(SFP_SIGN | SFP_P2)

#define	SFP_P1_DELTA		0x3f800001U
#define	SFP_M1_DELTA		(SFP_SIGN | SFP_P1_DELTA)

#define	SFP_P_DEN_MIN		0x00000001U
#define	SFP_M_DEN_MIN		(SFP_SIGN | SFP_P_DEN_MIN)

#define	SFP_P_DEN_MINx2		0x00000002U
#define	SFP_M_DEN_MINx2		(SFP_SIGN | SFP_P_DEN_MIN)

#define	SFP_P_DEN_MINx3		0x00000003U
#define	SFP_M_DEN_MINx3		(SFP_SIGN | SFP_P_DEN_MIN)

#define	SFP_P_DEN_MINx9		0x00000009U
#define	SFP_M_DEN_MINx9		(SFP_SIGN | SFP_P_DEN_MIN)

#define	SFP_SQRT_DEN_MINx2	0x1a800000U

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
