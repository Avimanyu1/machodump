#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <mach/machine.h>
#include <mach/vm_prot.h>
#include <mach/machine/thread_status.h>
#include <architecture/byte_order.h>
#include <mach-o/loader.h>
#include <stdbool.h>
#include <stdlib.h>
//#include "tuibox.h"


#define MACHODUMP_VERSION 0.01

#define IS_BIG_ENDIAN (!(union { uint16_t u16; unsigned char c; }){ .u16 = 1 }.c)

#define isObject(filetype) ((filetype&0x00000001))
#define isExecute(filetype) ((filetype&0x00000002))
#define isFVMLIB(filetype) ((filetype&0x00000003))
#define isCore(filetype) ((filetype&0x00000004))
#define isPreLoad(filetype) ((filetype&0x00000005))
#define isDylib(filetype) ((filetype&0x00000006))
#define isDyLinkEdit(filetype) ((filetype&0x00000007))
#define isBundle(filetype) ((filetype&0x00000008))
#define isDynStub(filetype) ((filetype&0x00000009))
#define isdbg(filetype) ((filetype&0x0000000A))
#define isKEXT(filetype) ((filetype&0x0000000B))
#define isSet(filetype) ((filetype&0x0000000C))

#define hasNoUnDef(filetype) ((filetype&0x1))
#define hasIncrLink(filetype) ((filetype&0x2))
#define willDyldLink(filetype) ((filetype&0x4))
#define willBindAtLoad(filetype) ((filetype&0x8))
#define hasPrebound(filetype) ((filetype&0x10))
#define isSplit(filetype) ((filetype&0x20))
#define willLazyInit(filetype) ((filetype&0x40))
#define isTwoLevel(filetype) ((filetype&0x80))
#define isFlat(filetype) ((filetype&0x100))
#define hasnoMultiDef(filetype) ((filetype&0x200))
#define noFixPrebind(filetype) ((filetype&0x400))
#define isPrebindable(filetype) ((filetype&0x800))
#define modsBounds(filetype) ((filetype&0x1000))
#define subsectionDivis(filetype) ((filetype&0x2000))
#define canonicalized(filetype) ((filetype&0x4000))
#define hasWeakDefs(filetype) ((filetype&0x8000))
#define hasWeakSymbols(filetype) ((filetype&0x10000))
#define hasStackExec(filetype) ((filetype&0x20000))
#define rootUse(filetype) ((filetype&0x40000))
#define issetuidSafe(filetype) ((filetype&0x80000))
#define hasNoReExpDyl(filetype) ((filetype&0x100000))
#define isPIE(filetype) ((filetype&0x200000))
#define isDeadStrip(filetype) ((filetype&0x400000))
#define hasTLV_Descr(filetype) ((filetype&0x800000))
#define hasNoHeapExec(filetype) ((filetype&0x1000000))
#define isAppExt(filetype) ((filetype&0x02000000))
#define isNLIST_OOS_W_DI(filetype) ((filetype&0x04000000))
#define isSIMSupport(filetype) ((filetype&0x08000000))
#define isDylibCached(filetype) ((filetype&0x80000000))


typedef struct mach_header_64 mheader64;

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


void printHelp(void)
{
	puts("Usage: ./machodump [-f file] [-hvc]");
	puts("h:\tPrint MachO header");
	puts("c:\tPrint MachO load commands");
	puts("v:\tPrint MachODump version");
	puts("H:\tPrint help menu");
	return;
}
char *printMagic(int magic)
{
	switch (magic) {
		case MH_MAGIC:				return "MH_MAGIC (32 bit) MachO";
		case MH_MAGIC_64:			return "MH_MAGIC_64 (64 bit) MachO";
		case MH_CIGAM:				return "MH_CIGAM (32 bit) MachO";
		case MH_CIGAM_64:			return "MH_CIGAM_64 (64 bit) MachO";
		default: 					return "Invalid Magic Number Detected!";
	}
}

char *printCPUType(int cputype)
{
	switch (cputype) {
		case CPU_TYPE_X86:		return "x86";
		case CPU_TYPE_X86_64:	return "x86_64";
		case CPU_TYPE_ARM:		return "ARM";
		case CPU_TYPE_ARM64_32:	return "ARM64_32";
		case CPU_TYPE_ARM64:	return "ARM64";
		default:				return "What even is this?";
	}
}
char *printFiletype(int filetype)
{
	switch (filetype) {
		case MH_OBJECT: return "RELOC OBJ\t";
		case MH_EXECUTE: return "EXEC\t";
		case MH_FVMLIB: return "FVM LIB\t";
		case MH_CORE: return "CORE\t";
		case MH_PRELOAD: return "PRELOAD\t";
		case MH_DYLIB: return "DYLIB\t";
		case MH_DYLINKER: return "DYLINKER\t";
		case MH_BUNDLE: return "BUNDLE\t";
		case MH_DYLIB_STUB: return "DYLINB STUB\t";
		case MH_DSYM: return "DSYM\t";
		case MH_KEXT_BUNDLE: return "KEXT\t";
		case MH_FILESET: return "FILESET\t";
		default: return "Invalid Filetype!";
	}
}

void printHeader(mheader64 *header)
{
	printf("Mach Header->");
	printf("\nMagic:\t\t%8x\t%s %s",header->magic,printMagic(header->magic),IS_BIG_ENDIAN ? "BE" : "LE");
	printf("\nCPU type:\t0x%x\t%s",header->cputype,printCPUType(header->cputype));
	printf("\nSubtype:\t0x%x",header->cpusubtype);
	printf("\nFiletype:\t0x%x\t%s",header->filetype,printFiletype(header->filetype));
	printf("\nLC Count:\t%d",header->ncmds);
	printf("\nLC Size:\t%x [%d]",header->sizeofcmds,header->sizeofcmds);
	printf("\nFlags:\t\t0x%08x",header->flags);
	printFlags(header->flags);
	printf("\nReserved:\t%x\n",header->reserved);
}

char *printCmd(int cmd)
{
	switch (cmd) {
		case 0x1:return "LC_SEGMENT";
		case 0x2:return "LC_SYMTAB";
		case 0x3:return "LC_SYMSEG";
		case 0x4:return "LC_THREAD";
		case 0x5:return "LC_UNIXTHREAD";
		case 0x6:return "LC_LOADFVMLIB";
		case 0x7:return "LC_IDFVMLIBLC_IDENT";
		case 0x8:return "LC_IDENT";
		case 0x9:return "LC_FVMFILE";
		case 0xa:return "LC_PREPAGE";
		case 0xb:return "LC_DYSYMTAB";
		case 0xc:return "LC_LOAD_DYLIB";
		case 0xd:return "LC_ID_DYLIB";
		case 0xe:return "LC_LOAD_DYLINKER";
		case 0xf:return "LC_ID_DYLINKER";
		case 0x10:return "LC_PREBOUND_DYLIB";
		case 0x11:return "LC_ROUTINES";
		case 0x12:return "LC_SUB_FRAMEWORK";
		case 0x13:return "LC_SUB_UMBRELLA";
		case 0x14:return "LC_SUB_CLIENT";
		case 0x15:return "LC_SUB_LIBRARY";
		case 0x16:return "LC_TWOLEVEL_HINTS";
		case 0x17:return "LC_PREBIND_CKSUM";
		case (0x18 | LC_REQ_DYLD):return "LC_LOAD_WEAK_DYLIB";
		case 0x19:return "LC_SEGMENT_64";
		case 0x1a:return "LC_ROUTINES_64";
		case 0x1b:return "LC_UUID";
		case (0x1c | LC_REQ_DYLD):return "LC_RPATH";
		case 0x1d:return "LC_CODE_SIGNATURE";
		case 0x1e:return "LC_SEGMENT_SPLIT_INFO";
		case (0x1f | LC_REQ_DYLD):return "LC_REEXPORT_DYLIB";
		case 0x20:return "LC_LAZY_LOAD_DYLIB";
		case 0x21:return "LC_ENCRYPTION_INFO";
		case 0x22:return "LC_DYLD_INFO";
		case (0x22|LC_REQ_DYLD):return "LC_DYLD_INFO_ONLY";
		case (0x23 | LC_REQ_DYLD):return "LC_LOAD_UPWARD_DYLIB";
		case 0x24:return "LC_VERSION_MIN_MACOSX";
		case 0x25:return "LC_VERSION_MIN_IPHONEOS";
		case 0x26:return "LC_FUNCTION_STARTS";
		case 0x27:return "LC_DYLD_ENVIRONMENT";
		case (0x28|LC_REQ_DYLD):return "LC_MAIN";
		case 0x29:return "LC_DATA_IN_CODE";
		case 0x2a:return "LC_SOURCE_VERSION";
		case 0x2b:return "LC_DYLIB_CODE_SIGN_DRS";
		case 0x2c:return "LC_ENCRYPTION_INFO_64";
		case 0x2d:return "LC_LINKER_OPTION";
		case 0x2e:return "LC_LINKER_OPTIMIZATION_HINT";
		case 0x2f:return "LC_VERSION_MIN_TVOS";
		case 0x30:return "LC_VERSION_MIN_WATCHOS";
		case 0x31:return "LC_NOTE";
		case 0x32:return "LC_BUILD_VERSION";
		case (0x33 | LC_REQ_DYLD):return "LC_DYLD_EXPORTS_TRIE";
		case (0x34 | LC_REQ_DYLD):return "LC_DYLD_CHAINED_FIXUPS";
		case (0x35 | LC_REQ_DYLD):return "LC_FILESET_ENTRY";
		default: return "Error : No such Load Command exists!";
	}
}

void *loadLCommands(FILE *file,mheader64 *header)
{
	fseek(file, sizeof(mheader64), SEEK_SET);
	void *load_commands = malloc(header->sizeofcmds);
	if (load_commands == NULL) perror("Failed to buffer load commands");
	fread(load_commands, header->sizeofcmds, 1, file);
	return load_commands;
}

void printLCommands(void *file,mheader64 *header)
{
	struct load_command *load_command = file;
	for (size_t LC_cnt = 0; LC_cnt < header->ncmds; LC_cnt++) {
		printf("%3zu|\t0x%8x\t%27s\t%5x[%6d]\t%s\n",
			LC_cnt+1,load_command->cmd,
			printCmd(load_command->cmd),
			load_command->cmdsize,
			load_command->cmdsize,
			(load_command->cmd == LC_SEGMENT_64) ? ((struct segment_command_64 *)load_command)->segname : ""
		);
		load_command = (uint64_t)load_command + load_command->cmdsize;
	}
}
void printLCSegment(void *file,mheader64 *header)
{
	struct load_command *load_command = file;
	for (size_t LC_cnt = 0; LC_cnt < header->ncmds; LC_cnt++) {
		if (load_command->cmd == LC_SEGMENT_64) {
			printf("\n%3zu|\t0x%8x\t%27s\t%5x[%6d]\t%14s\nvmaddr: 0x%012llx\tvmsz: 0x%010llx\tfilesz: 0x%5llx[%6llu]\tnsect: %u\n\n",
				LC_cnt+1,
				load_command->cmd,
				printCmd(load_command->cmd),
				load_command->cmdsize,
				load_command->cmdsize,
				((struct segment_command_64 *)load_command)->segname,
				((struct segment_command_64 *)load_command)->vmaddr,
				((struct segment_command_64 *)load_command)->vmsize,
				((struct segment_command_64 *)load_command)->filesize,
				((struct segment_command_64 *)load_command)->filesize,
				((struct segment_command_64 *)load_command)->nsects
			);
			for (size_t sec_cnt = 0; sec_cnt < ((struct segment_command_64 *)load_command)->nsects ; sec_cnt++) {
				struct section_64 *sec_ptr = (uint64_t)load_command + sizeof(struct segment_command_64) + (sec_cnt * sizeof(struct section_64));
				printf("\t\t%24s\tvmaddr: 0x%012x\tvmsz: 0x%010llx\talign: 2^%d\tnreloc: %d\n",
					sec_ptr->sectname,
					sec_ptr->addr,
					sec_ptr->size,
					sec_ptr->align,
					sec_ptr->nreloc
					
				);
			}
		}
		load_command = (uint64_t)load_command + load_command->cmdsize;
	}
}

mheader64 *parseMHeader(FILE *file)
{
	mheader64 *magic = malloc(sizeof(mheader64));
	fseek(file, 0, SEEK_SET);
	fread(magic, sizeof(mheader64), 1, file);
	return magic;
}

int main(int argc, char *argv[])
{
	unsigned int opt;
	char		*file	= NULL;
	FILE 		*handle;
	mheader64	*header	= NULL;
	void *load_commands	= NULL;
	bool givenFile = false;
	
	while ((opt = getopt(argc, argv, "f:Hhvcas")) != -1) {
		switch (opt) {
			case 'a':
			case 'f':
				{
					if (optarg) {
						file = optarg;
						givenFile = true;
						handle = fopen(file, "r");
					} else {
						perror("Failed to retrieve file\n");
					}
					break;
				}
			case 'v':
				{
					printf("MachODump version : %1.2f\n",MACHODUMP_VERSION);
					break;
				}
			case 'h':
				{
					if (givenFile) {
						header = parseMHeader(handle);
						printHeader(header);
					} else {
						perror("Failed to provide file\n");
					}
					break;
				}
			case 'c':
				{
					if (givenFile) {
						if (header == NULL)	header = parseMHeader(handle);
						if (load_commands == NULL) load_commands = loadLCommands(handle, header);
						printf("\t%8s\t%27s\t\t%11s\t%10s\n",
						"cmd",
						"cmdtype",
						"cmdsize 0x[0d]",
						"segname"
						);
						printLCommands(load_commands, header);
					} else {
						perror("Failed to provide file\n");
					}
					break;
				}
			case 's':
				{
					if (givenFile) {
						if (header == NULL)	header = parseMHeader(handle);
						if (load_commands == NULL) load_commands = loadLCommands(handle, header);
						printLCSegment(load_commands, header);
					} else {
						perror("Failed to provide file\n");
					}
					break;
				}
			case 'H':
				{
					printHelp();
					break;
				}
			default:
				{
					printf("-%c is an unrecognised command!\nUsage: ./machodump [-f file] [-hvcH]\n",opt);
				}
		}
	}
}



