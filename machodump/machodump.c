#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/machine.h>
#include <mach/vm_prot.h>
#include "defs.h"
#include <string.h>

void printFlags(int);
void printMagic(int);
void printFiletype(int);
void printCPUType(int);
void *alloc_segcom(void);
void *alloc_seccom(void);
char *printCmd(int);
char *printSectionType(int);

static int is64 = 0;

struct mach_header {
	uint32_t		magic;		/* mach magic number identifier */
	cpu_type_t		cputype;	/* cpu specifier */
	cpu_subtype_t	cpusubtype;	/* machine specifier */
	uint32_t		filetype;	/* type of file */
	uint32_t		ncmds;		/* number of load commands */
	uint32_t		sizeofcmds;	/* the size of all the load commands */
	uint32_t		flags;		/* flags */
};
struct mach_header_64 {
	uint32_t		magic;		/* mach magic number identifier */
	cpu_type_t		cputype;	/* cpu specifier */
	cpu_subtype_t	cpusubtype;	/* machine specifier */
	uint32_t		filetype;	/* type of file */
	uint32_t		ncmds;		/* number of load commands */
	uint32_t		sizeofcmds;	/* the size of all the load commands */
	uint32_t		flags;		/* flags */
	uint32_t		reserved;	/* reserved */
};

struct segment_command {
	uint32_t 	 cmd;
	uint32_t 	 cmdsize;
	char     	 segname[16];
	uint32_t 	 vmaddr;
	uint32_t 	 vmsize;
	uint32_t 	 fileoff;
	uint32_t 	 filesize;
	vm_prot_t	 maxprot;
	vm_prot_t 	initprot;
	uint32_t 	 nsects;
	uint32_t 	 flags;
};

struct segment_command_64 { /* for 64-bit architectures */
	uint32_t	cmd;		/* LC_SEGMENT_64 */
	uint32_t	cmdsize;	/* includes sizeof section_64 structs */
	char		segname[16];	/* segment name */
	uint64_t	vmaddr;		/* memory address of this segment */
	uint64_t	vmsize;		/* memory size of this segment */
	uint64_t	fileoff;	/* file offset of this segment */
	uint64_t	filesize;	/* amount to map from the file */
	vm_prot_t	maxprot;	/* maximum VM protection */
	vm_prot_t	initprot;	/* initial VM protection */
	uint32_t	nsects;		/* number of sections in segment */
	uint32_t	flags;		/* flags */
};

struct section { /* for 32-bit architectures */
    char        sectname[16];    /* name of this section */
    char        segname[16];    /* segment this section goes in */
    uint32_t    addr;        /* memory address of this section */
    uint32_t    size;        /* size in bytes of this section */
    uint32_t    offset;        /* file offset of this section */
    uint32_t    align;        /* section alignment (power of 2) */
    uint32_t    reloff;        /* file offset of relocation entries */
    uint32_t    nreloc;        /* number of relocation entries */
    uint32_t    flags;        /* flags (section type and attributes)*/
    uint32_t    reserved1;    /* reserved (for offset or index) */
    uint32_t    reserved2;    /* reserved (for count or sizeof) */
};

struct section_64 { /* for 64-bit architectures */
    char        sectname[16];    /* name of this section */
    char        segname[16];    /* segment this section goes in */
    uint64_t    addr;        /* memory address of this section */
    uint64_t    size;        /* size in bytes of this section */
    uint32_t    offset;        /* file offset of this section */
    uint32_t    align;        /* section alignment (power of 2) */
    uint32_t    reloff;        /* file offset of relocation entries */
    uint32_t    nreloc;        /* number of relocation entries */
    uint32_t    flags;        /* flags (section type and attributes)*/
    uint32_t    reserved1;    /* reserved (for offset or index) */
    uint32_t    reserved2;    /* reserved (for count or sizeof) */
    uint32_t    reserved3;    /* reserved */
};

struct load_command {
	uint32_t cmd;		/* type of load command */
	uint32_t cmdsize;	/* total size of command in bytes */
};




int main(int argc, char *argv[]) {
	if (argc < 2 || argc > 3)
		{
			puts("Invalid options passed!\nUsage : ./macho <file>");
			return 1;
		}
	FILE *stream = fopen(argv[1], "r");
	if (stream == NULL)
		{
		fprintf(stderr,"\nError opening the file\n");
		_exit(1);
		}
	fseek(stream,0,SEEK_SET);
	struct mach_header_64 header;
	fread(&header, sizeof(struct mach_header_64), 1, stream);
	puts("Mach Header :");
	printf("\n Magic Number:\t%x\t",header.magic);
	printMagic(header.magic);
	printf("\n CPU Type:\t0x%x\t",header.cputype);
	printCPUType(header.cputype);
	printf("\n CPU Subtype:\t0x%x",header.cpusubtype);
	printf("\n Filetype:\t0x%x",header.filetype);
	printFiletype(header.filetype);
	printf("\n Number of LC:\t%d",header.ncmds);
	printf("\n LC_SIZE:\t%x [%d]",header.sizeofcmds,header.sizeofcmds);
	printf("\n Flags:\t0x%x",header.flags);
	printFlags(header.flags);
	printf("\n Reserved:\t%x\n",header.reserved);
	puts("Load Commands :");
	
	struct segment_command_64* segment_commands = malloc(sizeof(struct segment_command_64) * header.ncmds);
	memset(segment_commands, 0, sizeof(struct segment_command_64) * header.ncmds);
    unsigned int section_cmd_cnt = 0;
	
	for (size_t offset,segcnt = 0; segcnt < header.ncmds ; segcnt++) {
		fread(&segment_commands[segcnt], sizeof(struct segment_command_64), 1, stream);
		offset = segment_commands[segcnt].cmdsize;
		printf("%lu",segcnt);
		printf("%24s",printCmd(segment_commands[segcnt].cmd));
		printf("\t%d\t",segment_commands[segcnt].cmdsize);
		if (segment_commands[segcnt].cmd == LC_SEGMENT_64)
			{
				printf("%16s\t",segment_commands[segcnt].segname);
				printf("vmadr: 0x%012llx\tvmsz: 0x%012llx\t floff: 0x%04llx\tflsz: 0x%6llx\tnsect: %d\n",
					segment_commands[segcnt].vmaddr,
					segment_commands[segcnt].vmsize,
					segment_commands[segcnt].fileoff,
					segment_commands[segcnt].filesize,
					segment_commands[segcnt].nsects);
				section_cmd_cnt += segment_commands[segcnt].nsects;
			}
		else puts("");
		
		fseek(stream, offset-sizeof(struct segment_command_64), SEEK_CUR);
	}
	printf("%d",section_cmd_cnt);
	fseek(stream, sizeof(struct mach_header_64), SEEK_SET);
	struct section_64* section_commands = malloc(sizeof(section_commands) * section_cmd_cnt);
	
	for (size_t offset,seccount = 0,segcnt = 0; segcnt < header.ncmds ; segcnt++) {
		fseek(stream, sizeof(struct segment_command_64), SEEK_CUR);
		offset = segment_commands[segcnt].cmdsize;
		if (segment_commands[segcnt].nsects) {
			fread(&section_commands[seccount], sizeof(struct section_64), segment_commands[segcnt].nsects, stream);
			seccount += segment_commands[segcnt].nsects;
			fseek(stream, -(sizeof(struct section_64) * segment_commands[segcnt].nsects), SEEK_CUR);
		}
		fseek(stream, offset-sizeof(struct segment_command_64), SEEK_CUR);
	}
	puts("Segment - Section \t\t\t\t\t[addr\t\tsz\toffs\talign\treloff\tnreloc\tflags]");
	for (size_t nsect = 0; nsect < section_cmd_cnt; nsect++) {
		printf("%lu",nsect+1);
		printf("%16s\t<==\t%16s\t[0x%012llx 0x%08llx 0x%04x 0x%04x 0x%04x 0x%04x 0x%08x] %s\n",
			section_commands[nsect].segname,
			section_commands[nsect].sectname,
			section_commands[nsect].addr,
			section_commands[nsect].size,
			section_commands[nsect].offset,
			section_commands[nsect].align,
			section_commands[nsect].reloff,
			section_commands[nsect].nreloc,
			section_commands[nsect].flags,
			printSectionType(section_commands[nsect].flags)
		);
	}
}

char *printSectionType(int flag)
{
	switch (flag & SECTION_TYPE) {
		case 0x0:
		{
			return "S_REGULAR";
		}
		case 0x1:
			{
				return "S_ZEROFILL";
			}
		case 0x2:
			{
				return "S_CSTRING_LITERALS";
			}
			
		case 0x3:
			{
				return "S_4BYTE_LITERALS";
			}
			
		case 0x4:
			{
				return "S_8BYTE_LITERALS";
			}
			
		case 0x5:
			{
				return "S_LITERAL_POINTERS";
			}
			
		case 0x6:
			{
				return "S_NON_LAZY_SYMBOL_POINTERS";
				break;
			}
			
		case 0x7:
			{
				return "S_LAZY_SYMBOL_POINTERS";
			}
			
		case 0x8:
			{
				return "S_SYMBOL_STUBS";
			}
			
		case 0x9:
			{
				return "S_MOD_INIT_FUNC_POINTERS";
			}
			
		case 0xa:
			{
				return "S_MOD_TERM_FUNC_POINTERS";
			}
			
		case 0xb:
			{
				return "S_COALESCED";
			}
			
		case 0xc:
			{
				return "S_GB_ZEROFILL";
			}
			
		case 0xd:
			{
				return "S_INTERPOSING";
			}
			
		case 0xe:
			{
				return "S_16BYTE_LITERALS";
			}
			
		case 0xf:
			{
				return "S_DTRACE_DOF";
			}
			
		case 0x10:
			{
				return "S_LAZY_DYLIB_SYMBOL_POINTERS";
			}
			
		case 0x11:
			{
				return "S_THREAD_LOCAL_REGULAR";
			}
			
		case 0x12:
			{
				return "S_THREAD_LOCAL_ZEROFILL";
			}
			
		case 0x13:
			{
				return "S_THREAD_LOCAL_VARIABLES";
			}
			
		case 0x14:
			{
				return "S_THREAD_LOCAL_VARIABLE_POINTERS";
			}
			
		case 0x15:
			{
				return "S_THREAD_LOCAL_INIT_FUNCTION_POINTERS";
			}
			
		case 0x16:
			{
				return "S_INIT_FUNC_OFFSETS";
			}
			
		default: return "Unknown Section type!";
	}
}
char *printCmd(int cmd)
{
	switch (cmd) {
		case 0x1:
			{
				return "LC_SEGMENT";
			}
		case 0x2:
			{
				return "LC_SYMTAB";
			}
			
		case 0x3:
			{
				return "LC_SYMSEG";
			}
			
		case 0x4:
			{
				return "LC_THREAD";
			}
			
		case 0x5:
			{
				return "LC_UNIXTHREAD";
			}
			
		case 0x6:
			{
				return "LC_LOADFVMLIB";
				break;
			}
			
		case 0x7:
			{
				return "LC_IDFVMLIBLC_IDENT";
			}
			
		case 0x8:
			{
				return "LC_IDENT";
			}
			
		case 0x9:
			{
				return "LC_FVMFILE";
			}
			
		case 0xa:
			{
				return "LC_PREPAGE";
			}
			
		case 0xb:
			{
				return "LC_DYSYMTAB";
			}
			
		case 0xc:
			{
				return "LC_LOAD_DYLIB";
			}
			
		case 0xd:
			{
				return "LC_ID_DYLIB";
			}
			
		case 0xe:
			{
				return "LC_LOAD_DYLINKER";
			}
			
		case 0xf:
			{
				return "LC_ID_DYLINKER";
			}
			
		case 0x10:
			{
				return "LC_PREBOUND_DYLIB";
			}
			
		case 0x11:
			{
				return "LC_ROUTINES";
			}
			
		case 0x12:
			{
				return "LC_SUB_FRAMEWORK";
			}
			
		case 0x13:
			{
				return "LC_SUB_UMBRELLA";
			}
		case 0x14:
			{
				return "LC_SUB_CLIENT";
			}
		case 0x15:
			{
				return "LC_SUB_LIBRARY";
			}
			
		case 0x16:
			{
				return "LC_TWOLEVEL_HINTS";
			}
			
		case 0x17:
			{
				return "LC_PREBIND_CKSUM";
			}
			
		case (0x18 | LC_REQ_DYLD):
			{
				return "LC_LOAD_WEAK_DYLIB";
			}
			
		case 0x19:
			{
				return "LC_SEGMENT_64";
			}
			
		case 0x1a:
			{
				return "LC_ROUTINES_64";
			}
			
		case 0x1b:
			{
				return "LC_UUID";
			}
			
		case (0x1c | LC_REQ_DYLD):
			{
				return "LC_RPATH";
			}
			
		case 0x1d:
			{
				return "LC_CODE_SIGNATURE";
			}
			
		case 0x1e:
			{
				return "LC_SEGMENT_SPLIT_INFO";
			}
			
		case (0x1f | LC_REQ_DYLD):
			{
				return "LC_REEXPORT_DYLIB";
			}
			
		case 0x20:
			{
				return "LC_LAZY_LOAD_DYLIB";
			}
			
		case 0x21:
			{
				return "LC_ENCRYPTION_INFO";
			}
			
		case 0x22:
			{
				return "LC_DYLD_INFO";
			}
		case (0x22|LC_REQ_DYLD):
			{
				return "LC_DYLD_INFO_ONLY";
			}
		case (0x23 | LC_REQ_DYLD):
			{
				return "LC_LOAD_UPWARD_DYLIB";
			}
			
		case 0x24:
			{
				return "LC_VERSION_MIN_MACOSX";
			}
			
		case 0x25:
			{
				return "LC_VERSION_MIN_IPHONEOS";
			}
			
		case 0x26:
			{
				return "LC_FUNCTION_STARTS";
			}
		case 0x27:
			{
				return "LC_DYLD_ENVIRONMENT";
			}
			
		case (0x28|LC_REQ_DYLD):
			{
				return "LC_MAIN";
			}
			
		case 0x29:
			{
				return "LC_DATA_IN_CODE";
			}
			
		case 0x2a:
			{
				return "LC_SOURCE_VERSION";
			}
			
		case 0x2b:
			{
				return "LC_DYLIB_CODE_SIGN_DRS";
			}
		case 0x2c:
			{
				return "LC_ENCRYPTION_INFO_64";
			}
			
		case 0x2d:
			{
				return "LC_LINKER_OPTION";
			}
			
		case 0x2e:
			{
				return "LC_LINKER_OPTIMIZATION_HINT";
			}
			
		case 0x2f:
			{
				return "LC_VERSION_MIN_TVOS";
			}
		case 0x30:
			{
				return "LC_VERSION_MIN_WATCHOS";
			}
			
		case 0x31:
			{
				return "LC_NOTE";
			}
			
		case 0x32:
			{
				return "LC_BUILD_VERSION";
			}			
		case (0x33 | LC_REQ_DYLD):
			{
				return "LC_DYLD_EXPORTS_TRIE";
			}
			
		case (0x34 | LC_REQ_DYLD):
			{
				return "LC_DYLD_CHAINED_FIXUPS";
			}			
		case (0x35 | LC_REQ_DYLD):
			{
				return "LC_FILESET_ENTRY";
			}
		default: {
			return "Error : No such Load Command exists!";
		}
	}
}
void printFlags(int flags)
{
	puts("");
	if (hasNoUnDef(flags)) {
		printf("NOUNDEF\t");
	}
	if (hasIncrLink(flags)) {
		printf("INCRLINK\t");
	}
	if (willDyldLink(flags)) {
		printf("DYLDLINK\t");
	}
	if (willBindAtLoad(flags)) {
		printf("BINDATLOAD\t");
	}
	if (hasPrebound(flags)) {
		printf("PREBOUND\t");
	}
	if (isSplit(flags)) {
		printf("SPLITSEGS\t");
	}
	if (willLazyInit(flags)) {
		printf("LAZYINIT\t");
	}
	if (isTwoLevel(flags)) {
		printf("TWOLEVEL\t");
	}
	if (isFlat(flags)) {
		printf("FORCEFLAT\t");
	}
	if (hasnoMultiDef(flags)) {
		printf("NOMULTIDEF\t");
	}
	if (noFixPrebind(flags)) {
		printf("NOFIXPREBIND\t");
	}
	if (isPrebindable(flags)) {
		printf("PREBINDABLE\t");
	}
	if (modsBounds(flags)) {
		printf("ALLMODSBOUND\t");
	}
	if (canonicalized(flags)) {
		printf("CANONICAL\t");
	}
	if (hasWeakDefs(flags)) {
		printf("WEAKDEF\t");
	}
	if (hasWeakSymbols(flags)) {
		printf("WEAKBIND\t");
	}
	if (hasStackExec(flags)) {
		printf("STACKEXEC\t");
	}
	if (rootUse(flags)) {
		printf("ROOTSAFE\t");
	}
	if (issetuidSafe(flags)) {
		printf("SETUIDSAFE\t");
	}
	if (hasNoReExpDyl(flags)) {
		printf("NOREXPDYLIB\t");
	}
	if (isPIE(flags)) {
		printf("PIE\t");
	}
	if (isDeadStrip(flags)) {
		printf("DEADSTRIPDYL\t");
	}
	if (hasTLV_Descr(flags)) {
		printf("HASTLVDESCR\t");
	}
	if (hasNoHeapExec(flags)) {
		printf("NOHEAPEXEC\t");
	}
	if (isAppExt(flags)) {
		printf("APPEXTUSE\t");
	}
	if (isNLIST_OOS_W_DI(flags)) {
		printf("NLISTUNSYNCWDINFO");
	}
	if (isSIMSupport(flags)) {
		printf("SIMSUPP");
	}
	if (isDylibCached(flags)) {
		printf("DYLIBINCACHE");
	}
}

void printMagic(int magic)
{
	switch (magic) {
		case MH_MAGIC:
			{	
				printf("MH_MAGIC (32 bit) MachO");
				break;
			};
		case MH_MAGIC_64:
			{
				is64 = 1;
				printf("MH_MAGIC_64 (64 bit) MachO");
				break;
			};
		case MH_CIGAM:
			{
				printf("MH_CIGAM (32 bit) MachO\t");
				if (IS_BIG_ENDIAN) printf("Little Endian");
				else printf("Big Endian");
				break;
			};
		case MH_CIGAM_64: 
			{
				is64 = 1;
				printf("MH_CIGAM_64 (64 bit) MachO");
				if (IS_BIG_ENDIAN) printf("Little Endian");
				else printf("Big Endian");
				break;
			};
		default: printf("Invalid Magic Number Detected! %x", magic);
	}
}

void printFiletype(int filetype)
{
	puts("");
	switch (filetype) {
		case MH_OBJECT: printf("RELOC OBJ\t"); break;
		case MH_EXECUTE: printf("EXEC\t"); break;
		case MH_FVMLIB: printf("FVM LIB\t"); break;
		case MH_CORE: printf("CORE\t"); break;
		case MH_PRELOAD: printf("PRELOAD\t"); break;
		case MH_DYLIB: printf("DYLIB\t"); break;
		case MH_DYLINKER: printf("DYLINKER\t"); break;
		case MH_BUNDLE: printf("BUNDLE\t"); break;
		case MH_DYLIB_STUB: printf("DYLINB STUB\t"); break;
		case MH_DSYM: printf("DSYM\t"); break;
		case MH_KEXT_BUNDLE: printf("KEXT\t"); break;
		case MH_FILESET: printf("FILESET\t"); break;
		default: printf("Invalid Filetype!");
	}
}

void printCPUType(int cputype)
{
	switch (cputype) {
		case CPU_TYPE_X86: 
			{
				printf("x86");
				break;
			}
		case CPU_TYPE_X86_64: 
			{
				printf("x86_64");
				break;
			}
		case CPU_TYPE_ARM: 
			{
				printf("ARM");
				break;
			}
		case CPU_TYPE_ARM64_32: 
			{
				printf("ARM64_32");
				break;
			}
		case CPU_TYPE_ARM64: 
			{
				printf("ARM64");
				break;
			}
		default: printf("What even is this?");
	}
}
void *alloc_segcom(void)
{
	if (is64)
		{
			return malloc(sizeof(struct segment_command_64) * 16);
		}
	else  return malloc(sizeof(struct segment_command) * 16);
}
void *alloc_seccom(void)
{
	if (is64)
		{
			return malloc(sizeof(struct section_64));
		}
	else  return malloc(sizeof(struct section));
}
	
