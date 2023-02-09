
#define IS_BIG_ENDIAN (!(union { uint16_t u16; unsigned char c; }){ .u16 = 1 }.c)
// Constants for the magic field of the mach_header
#define	MH_MAGIC	0xfeedface	/* the mach magic number */
#define MH_CIGAM	0xcefaedfe	/* NXSwapInt(MH_MAGIC) */
#define MH_MAGIC_64 0xfeedfacf /* the 64-bit mach magic number */
#define MH_CIGAM_64 0xcffaedfe /* NXSwapInt(MH_MAGIC_64) */

//Capability bits used in the definition of cpu_type.
#define CPU_ARCH_MASK           0xff000000      /* mask for architecture bits */
#define CPU_ARCH_ABI64          0x01000000      /* 64 bit ABI */
#define CPU_ARCH_ABI64_32       0x02000000      /* ABI for 64-bit hardware with 32-bit types; LP32 */
// Constants for the cputype field of the mach_header
#define CPU_TYPE_ANY            ((cpu_type_t) -1)
#define CPU_TYPE_VAX            ((cpu_type_t) 1)
#define CPU_TYPE_MC680x0        ((cpu_type_t) 6)
#define CPU_TYPE_X86            ((cpu_type_t) 7)
#define CPU_TYPE_I386           CPU_TYPE_X86            /* compatibility */
#define CPU_TYPE_X86_64         (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_TYPE_MIPS			((cpu_type_t) 8)
#define CPU_TYPE_MC98000        ((cpu_type_t) 10)
#define CPU_TYPE_HPPA           ((cpu_type_t) 11)
#define CPU_TYPE_ARM            ((cpu_type_t) 12)
#define CPU_TYPE_ARM64          (CPU_TYPE_ARM | CPU_ARCH_ABI64)
#define CPU_TYPE_ARM64_32       (CPU_TYPE_ARM | CPU_ARCH_ABI64_32)
#define CPU_TYPE_MC88000        ((cpu_type_t) 13)
#define CPU_TYPE_SPARC          ((cpu_type_t) 14)
#define CPU_TYPE_I860           ((cpu_type_t) 15)
#define CPU_TYPE_ALPHA			((cpu_type_t) 16)
#define CPU_TYPE_POWERPC        ((cpu_type_t) 18)
#define CPU_TYPE_POWERPC64      (CPU_TYPE_POWERPC | CPU_ARCH_ABI64)

//Capability bits used in the definition of cpu_subtype.
#define CPU_SUBTYPE_MASK        0xff000000      /* mask for feature flags */
#define CPU_SUBTYPE_LIB64       0x80000000      /* 64 bit libraries */
#define CPU_SUBTYPE_PTRAUTH_ABI 0x80000000      /* pointer authentication with versioned ABI */
//	X86 subtypes.
#define CPU_SUBTYPE_X86_ALL             ((cpu_subtype_t)3)
#define CPU_SUBTYPE_X86_64_ALL          ((cpu_subtype_t)3)
#define CPU_SUBTYPE_X86_ARCH1           ((cpu_subtype_t)4)
#define CPU_SUBTYPE_X86_64_H            ((cpu_subtype_t)8)      /* Haswell feature subset */
#define CPU_THREADTYPE_INTEL_HTT        ((cpu_threadtype_t) 1)
// ARM subtypes.
#define CPU_SUBTYPE_ARM_V7              ((cpu_subtype_t) 9)
#define CPU_SUBTYPE_ARM_V8              ((cpu_subtype_t) 13)
//ARM64 subtypes
#define CPU_SUBTYPE_ARM64_ALL           ((cpu_subtype_t) 0)
#define CPU_SUBTYPE_ARM64_V8            ((cpu_subtype_t) 1)
#define CPU_SUBTYPE_ARM64E              ((cpu_subtype_t) 2)

// Constants for the filetype field of the mach_header
#define	MH_OBJECT	0x1		/* relocatable object file */
#define isObject(filetype) ((filetype&0x00000001))
#define	MH_EXECUTE	0x2		/* demand paged executable file */
#define isExecute(filetype) ((filetype&0x00000002))
#define	MH_FVMLIB	0x3		/* fixed VM shared library file */
#define isFVMLIB(filetype) ((filetype&0x00000003))
#define	MH_CORE		0x4		/* core file */
#define isCore(filetype) ((filetype&0x00000004))
#define	MH_PRELOAD	0x5		/* preloaded executable file */
#define isPreLoad(filetype) ((filetype&0x00000005))
#define	MH_DYLIB	0x6		/* dynamically bound shared library */
#define isDylib(filetype) ((filetype&0x00000006))
#define	MH_DYLINKER	0x7		/* dynamic link editor */
#define isDyLinkEdit(filetype) ((filetype&0x00000007))
#define	MH_BUNDLE	0x8		/* dynamically bound bundle file */
#define isBundle(filetype) ((filetype&0x00000008))
#define	MH_DYLIB_STUB	0x9		/* shared library stub for static */
#define isDynStub(filetype) ((filetype&0x00000009))
/*  linking only, no section contents */
#define	MH_DSYM		0xa		/* companion file with only debug */
#define isdbg(filetype) ((filetype&0x0000000A))
/*  sections */
#define	MH_KEXT_BUNDLE	0xb		/* x86_64 kexts */
#define isKEXT(filetype) ((filetype&0x0000000B))
#define	MH_FILESET	0xc		/* set of mach-o's */
#define isSet(filetype) ((filetype&0x0000000C))

// Constants for the flags field of the mach_header */
#define	MH_NOUNDEFS		0x1	
#define hasNoUnDef(filetype) ((filetype&0x1))
#define	MH_INCRLINK		0x2
#define hasIncrLink(filetype) ((filetype&0x2))
#define MH_DYLDLINK		0x4
#define willDyldLink(filetype) ((filetype&0x4))
#define MH_BINDATLOAD	0x8	
#define willBindAtLoad(filetype) ((filetype&0x8))
#define MH_PREBOUND		0x10
#define hasPrebound(filetype) ((filetype&0x10))
#define MH_SPLIT_SEGS	0x20
#define isSplit(filetype) ((filetype&0x20))
#define MH_LAZY_INIT	0x40
#define willLazyInit(filetype) ((filetype&0x40))
#define MH_TWOLEVEL		0x80
#define isTwoLevel(filetype) ((filetype&0x80))
#define MH_FORCE_FLAT	0x100
#define isFlat(filetype) ((filetype&0x100))
#define MH_NOMULTIDEFS	0x200
#define hasnoMultiDef(filetype) ((filetype&0x200))
#define MH_NOFIXPREBINDING 0x400
#define noFixPrebind(filetype) ((filetype&0x400))
#define MH_PREBINDABLE  0x800
#define isPrebindable(filetype) ((filetype&0x800))
#define MH_ALLMODSBOUND 0x1000
#define modsBounds(filetype) ((filetype&0x1000))
#define MH_SUBSECTIONS_VIA_SYMBOLS 0x2000
#define subsectionDivis(filetype) ((filetype&0x2000))
#define MH_CANONICAL    0x4000
#define canonicalized(filetype) ((filetype&0x4000))
#define MH_WEAK_DEFINES	0x8000
#define hasWeakDefs(filetype) ((filetype&0x8000))
#define MH_BINDS_TO_WEAK 0x10000
#define hasWeakSymbols(filetype) ((filetype&0x10000))
#define MH_ALLOW_STACK_EXECUTION 0x20000
#define hasStackExec(filetype) ((filetype&0x20000))
#define MH_ROOT_SAFE	 0x40000
#define rootUse(filetype) ((filetype&0x40000))
#define MH_SETUID_SAFE	 0x80000
#define issetuidSafe(filetype) ((filetype&0x80000))
#define MH_NO_REEXPORTED_DYLIBS 0x100000
#define hasNoReExpDyl(filetype) ((filetype&0x100000))
#define	MH_PIE			0x200000
#define isPIE(filetype) ((filetype&0x200000))
#define	MH_DEAD_STRIPPABLE_DYLIB 0x400000
#define isDeadStrip(filetype) ((filetype&0x400000))
#define MH_HAS_TLV_DESCRIPTORS	 0x800000
#define hasTLV_Descr(filetype) ((filetype&0x800000))
#define MH_NO_HEAP_EXECUTION 0x1000000
#define hasNoHeapExec(filetype) ((filetype&0x1000000))
#define MH_APP_EXTENSION_SAFE 0x02000000
#define isAppExt(filetype) ((filetype&0x02000000))
#define	MH_NLIST_OUTOFSYNC_WITH_DYLDINFO 0x04000000
#define isNLIST_OOS_W_DI(filetype) ((filetype&0x04000000))
#define	MH_SIM_SUPPORT 		0x08000000
#define isSIMSupport(filetype) ((filetype&0x08000000))
#define MH_DYLIB_IN_CACHE 	0x80000000
#define isDylibCached(filetype) ((filetype&0x80000000))