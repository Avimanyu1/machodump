#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/machine.h>
#include <mach/vm_prot.h>
#include "defs.h"

void printFlags(int);
void printMagic(int);
void printFiletype(int);
void printCPUType(int);
void *alloc_segcom(void);
void *alloc_seccom(void);
void printCmd(int cmd);

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
	printf("\n CPU Type:\t%x\t",header.cputype);
	printCPUType(header.cputype);
	printf("\n CPU Subtype:\t%x",header.cpusubtype);
	printf("\n Filetype:\t%x",header.filetype);
	printFiletype(header.filetype);
	printf("\n Number of LC:\t%d",header.ncmds);
	printf("\n LC_SIZE:\t%x [%d]",header.sizeofcmds,header.sizeofcmds);
	printf("\n Flags:\t%x",header.flags);
	printFlags(header.flags);
	printf("\n Reserved:\t%x\n",header.reserved);
	puts("Load Commands :");
	struct load_command* load_commands = malloc(sizeof(struct load_command) * header.ncmds);
	struct segment_command_64* segment_commands = malloc(sizeof(struct segment_command_64) * header.ncmds);
    
//  for (size_t offset = 0,cnt = 0; cnt < header.ncmds ; cnt++) {
//      fread(&load_commands[cnt], sizeof(struct load_command), 1, stream);
//		fseek(stream, -(sizeof(struct load_command)), SEEK_CUR);
//		offset = load_commands[cnt].cmdsize;
//		printCmd(load_commands[cnt].cmd);
//		printf("\t%d\n",load_commands[cnt].cmdsize);
//		fseek(stream, offset, SEEK_CUR);
//  }
	fseek(stream, sizeof(struct mach_header_64), SEEK_SET );
	for (size_t offset = 0,cnt = 0; cnt < header.ncmds ; cnt++) {
		fread(&segment_commands[cnt], sizeof(struct segment_command_64), 1, stream);
		fseek(stream, -(sizeof(struct segment_command_64)), SEEK_CUR);
		offset = segment_commands[cnt].cmdsize;
		printCmd(segment_commands[cnt].cmd);
		printf("\t%d\t",segment_commands[cnt].cmdsize);
		if (*segment_commands[cnt].segname == 95 && *(segment_commands[cnt].segname+1) == 95)
			printf("%s\n",segment_commands[cnt].segname);
		else puts("");
		fseek(stream, offset, SEEK_CUR);

	}
}

void printCmd(int cmd)
{
	switch (cmd) {
		case 0x1:
			{
				printf("LC_SEGMENT");
				break;
			}
		case 0x2:
			{
				printf("LC_SYMTAB");
				break;
			}
			
		case 0x3:
			{
				printf("LC_SYMSEG");
				break;
			}
			
		case 0x4:
			{
				printf("LC_THREAD");
				break;
			}
			
		case 0x5:
			{
				printf("LC_UNIXTHREAD");
				break;
			}
			
		case 0x6:
			{
				printf("LC_LOADFVMLIB");
				break;
			}
			
		case 0x7:
			{
				printf("LC_IDFVMLIBLC_IDENT");
				break;
			}
			
		case 0x8:
			{
				printf("LC_IDENT");
				break;
			}
			
		case 0x9:
			{
				printf("LC_FVMFILE");
				break;
			}
			
		case 0xa:
			{
				printf("LC_PREPAGE");
				break;
			}
			
		case 0xb:
			{
				printf("LC_DYSYMTAB");
				break;
			}
			
		case 0xc:
			{
				printf("LC_LOAD_DYLIB");
				break;
			}
			
		case 0xd:
			{
				printf("LC_ID_DYLIB");
				break;
			}
			
		case 0xe:
			{
				printf("LC_LOAD_DYLINKER");
				break;
			}
			
		case 0xf:
			{
				printf("LC_ID_DYLINKER");
				break;
			}
			
		case 0x10:
			{
				printf("LC_PREBOUND_DYLIB");
				break;
			}
			
		case 0x11:
			{
				printf("LC_ROUTINES");
				break;
			}
			
		case 0x12:
			{
				printf("LC_SUB_FRAMEWORK");
				break;
			}
			
		case 0x13:
			{
				printf("LC_SUB_UMBRELLA");
				break;
			}
		case 0x14:
			{
				printf("LC_SUB_CLIENT");
				break;
			}
		case 0x15:
			{
				printf("LC_SUB_LIBRARY");
				break;
			}
			
		case 0x16:
			{
				printf("LC_TWOLEVEL_HINTS");
				break;
			}
			
		case 0x17:
			{
				printf("LC_PREBIND_CKSUM");
				break;
			}
			
		case (0x18 | LC_REQ_DYLD):
			{
				printf("LC_LOAD_WEAK_DYLIB");
				break;
			}
			
		case 0x19:
			{
				printf("LC_SEGMENT_64");
				break;
			}
			
		case 0x1a:
			{
				printf("LC_ROUTINES_64");
				break;
			}
			
		case 0x1b:
			{
				printf("LC_UUID");
				break;
			}
			
		case (0x1c | LC_REQ_DYLD):
			{
				printf("LC_RPATH");
				break;
			}
			
		case 0x1d:
			{
				printf("LC_CODE_SIGNATURE");
				break;
			}
			
		case 0x1e:
			{
				printf("LC_SEGMENT_SPLIT_INFO");
				break;
			}
			
		case (0x1f | LC_REQ_DYLD):
			{
				printf("LC_REEXPORT_DYLIB");
				break;
			}
			
		case 0x20:
			{
				printf("LC_LAZY_LOAD_DYLIB");
				break;
			}
			
		case 0x21:
			{
				printf("LC_ENCRYPTION_INFO");
				break;
			}
			
		case 0x22:
			{
				printf("LC_DYLD_INFO");
				break;
			}
		case (0x22|LC_REQ_DYLD):
			{
				printf("LC_DYLD_INFO_ONLY");
				break;
			}
		case (0x23 | LC_REQ_DYLD):
			{
				printf("LC_LOAD_UPWARD_DYLIB");
				break;
			}
			
		case 0x24:
			{
				printf("LC_VERSION_MIN_MACOSX");
				break;
			}
			
		case 0x25:
			{
				printf("LC_VERSION_MIN_IPHONEOS");
				break;
			}
			
		case 0x26:
			{
				printf("LC_FUNCTION_STARTS");
				break;
			}			
		case 0x27:
			{
				printf("LC_DYLD_ENVIRONMENT");
				break;
			}
			
		case (0x28|LC_REQ_DYLD):
			{
				printf("LC_MAIN");
				break;
			}
			
		case 0x29:
			{
				printf("LC_DATA_IN_CODE");
				break;
			}
			
		case 0x2a:
			{
				printf("LC_SOURCE_VERSION");
				break;
			}
			
		case 0x2b:
			{
				printf("LC_DYLIB_CODE_SIGN_DRS");
				break;
			}			
		case 0x2c:
			{
				printf("LC_ENCRYPTION_INFO_64");
				break;
			}
			
		case 0x2d:
			{
				printf("LC_LINKER_OPTION");
				break;
			}
			
		case 0x2e:
			{
				printf("LC_LINKER_OPTIMIZATION_HINT");
				break;
			}
			
		case 0x2f:
			{
				printf("LC_VERSION_MIN_TVOS");
				break;
			}			
		case 0x30:
			{
				printf("LC_VERSION_MIN_WATCHOS");
				break;
			}
			
		case 0x31:
			{
				printf("LC_NOTE");
				break;
			}
			
		case 0x32:
			{
				printf("LC_BUILD_VERSION");
				break;
			}			
		case (0x33 | LC_REQ_DYLD):
			{
				printf("LC_DYLD_EXPORTS_TRIE");
				break;
			}
			
		case (0x34 | LC_REQ_DYLD):
			{
				printf("LC_DYLD_CHAINED_FIXUPS");
				break;
			}			
		case (0x35 | LC_REQ_DYLD):
			{
				printf("LC_FILESET_ENTRY");
				break;
			}
		default: {
			printf("Error : No such Load Command exists!");
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
