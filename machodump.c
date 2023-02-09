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
	struct mach_header_64 header = {0};
	fread(&header, sizeof(struct mach_header_64), 1, stream);
	
	printf("\n Magic Number:\t%X\t",header.magic);
	printMagic(header.magic);
	printf("\n CPU Type:\t%d\t",header.cputype);
	printCPUType(header.cputype);
	printf("\n CPU Subtype:\t%d",header.cpusubtype);
	printf("\n Filetype:\t%X",header.filetype);
	printFiletype(header.filetype);
	printf("\n Number of LC:\t%d",header.ncmds);
	printf("\n LC_SIZE:\t%X [%d]",header.sizeofcmds,header.sizeofcmds);
	printf("\n Flags:\t%X",header.flags);
	printFlags(header.flags);
	printf("\n Reserved:\t%X",header.reserved);
	
	struct load_command* load_commands = malloc(sizeof(load_commands) * header.ncmds);
	struct load_command *tmp = load_commands;
	void *actual_commands[50] = {0};
	size_t offset = 0;
	for (size_t cnt = 0; cnt < header.ncmds; cnt++) {
		fread(tmp++, (sizeof(struct load_command)), 1, stream);
		puts("fread for lc header");
		struct load_command lc = load_commands[cnt];
		actual_commands[cnt] = malloc(lc.cmdsize);
		puts("malloc");
		fread((actual_commands+cnt), lc.cmdsize, 1, stream);
		puts("fread for lc");
		offset += lc.cmdsize;
		printf("%d",lc.cmd);
	}
};

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