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


#define MACHODUMP_VERSION 0.01

#define IS_BIG_ENDIAN (!(union { uint16_t u16; unsigned char c; }){ .u16 = 1 }.c)

typedef struct mach_header_64 mheader64;

void printFlags(int flags)
{
	puts("");
	if (flags & MH_NOUNDEFS)							printf("NOUNDEF\t");
	if (flags & MH_INCRLINK)							printf("INCRLINK\t");
	if (flags & MH_DYLDLINK)							printf("DYLDLINK\t");
	if (flags & MH_BINDATLOAD)						printf("BINDATLOAD\t");
	if (flags & MH_PREBOUND)							printf("PREBOUND\t");
	if (flags & MH_SPLIT_SEGS)						printf("SPLITSEGS\t");
	if (flags & MH_LAZY_INIT)						printf("LAZYINIT\t");
	if (flags & MH_TWOLEVEL)							printf("TWOLEVEL\t");
	if (flags & MH_FORCE_FLAT)						printf("FORCEFLAT\t");
	if (flags & MH_NOMULTIDEFS)						printf("NOMULTIDEF\t");
	if (flags & MH_NOFIXPREBINDING)					printf("NOFIXPREBIND\t");
	if (flags & MH_PREBINDABLE)						printf("PREBINDABLE\t");
	if (flags & MH_ALLMODSBOUND)					printf("ALLMODSBOUND\t");
	if (flags & MH_CANONICAL)						printf("CANONICAL\t");
	if (flags & MH_WEAK_DEFINES)					printf("WEAKDEF\t");
	if (flags & MH_BINDS_TO_WEAK)					printf("WEAKBIND\t");
	if (flags & MH_ALLOW_STACK_EXECUTION)			printf("STACKEXEC\t");
	if (flags & MH_ROOT_SAFE)						printf("ROOTSAFE\t");
	if (flags & MH_SETUID_SAFE)						printf("SETUIDSAFE\t");
	if (flags & MH_NO_REEXPORTED_DYLIBS)			printf("NOREXPDYLIB\t");
	if (flags & MH_PIE)								printf("PIE\t");
	if (flags & MH_DEAD_STRIPPABLE_DYLIB)			printf("DEADSTRIPDYL\t");
	if (flags & MH_HAS_TLV_DESCRIPTORS)				printf("HASTLVDESCR\t");
	if (flags & MH_NO_HEAP_EXECUTION)				printf("NOHEAPEXEC\t");
	if (flags & MH_APP_EXTENSION_SAFE)				printf("APPEXTUSE\t");
	if (flags & MH_NLIST_OUTOFSYNC_WITH_DYLDINFO)	printf("NLISTUNSYNCWDINFO");
	if (flags & MH_SIM_SUPPORT)						printf("SIMSUPP");
	if (flags & MH_DYLIB_IN_CACHE)					printf("DYLIBINCACHE");
	
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
		case MH_OBJECT:			return "RELOC OBJ\t";
		case MH_EXECUTE:			return "EXEC\t";
		case MH_FVMLIB:			return "FVM LIB\t";
		case MH_CORE:			return "CORE\t";
		case MH_PRELOAD:			return "PRELOAD\t";
		case MH_DYLIB:			return "DYLIB\t";
		case MH_DYLINKER:		return "DYLINKER\t";
		case MH_BUNDLE:			return "BUNDLE\t";
		case MH_DYLIB_STUB:		return "DYLINB STUB\t";
		case MH_DSYM:			return "DSYM\t";
		case MH_KEXT_BUNDLE:		return "KEXT\t";
		case MH_FILESET:			return "FILESET\t";
		default:				return "Invalid Filetype!";
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
		case LC_SEGMENT:						return "LC_SEGMENT";
		case LC_SYMTAB:						return "LC_SYMTAB";
		case LC_SYMSEG:						return "LC_SYMSEG";
		case LC_THREAD:						return "LC_THREAD";
		case LC_UNIXTHREAD:					return "LC_UNIXTHREAD";
		case LC_IDFVMLIB:					return "LC_IDFVMLIB";
		case LC_IDENT:						return "LC_IDENT";
		case LC_FVMFILE:						return "LC_FVMFILE";
		case LC_PREPAGE:						return "LC_PREPAGE";
		case LC_DYSYMTAB:					return "LC_DYSYMTAB";
		case LC_LOAD_DYLIB:					return "LC_LOAD_DYLIB";
		case LC_ID_DYLIB:					return "LC_ID_DYLIB";
		case LC_LOAD_DYLINKER:				return "LC_LOAD_DYLINKER";
		case LC_ID_DYLINKER:					return "LC_ID_DYLINKER";
		case LC_PREBOUND_DYLIB:				return "LC_PREBOUND_DYLIB";
		case LC_ROUTINES:					return "LC_ROUTINES";
		case LC_SUB_FRAMEWORK:				return "LC_SUB_FRAMEWORK";
		case LC_SUB_UMBRELLA:				return "LC_SUB_UMBRELLA";
		case LC_SUB_CLIENT:					return "LC_SUB_CLIENT";
		case LC_SUB_LIBRARY:					return "LC_SUB_LIBRARY";
		case LC_TWOLEVEL_HINTS:				return "LC_TWOLEVEL_HINTS";
		case LC_PREBIND_CKSUM:				return "LC_PREBIND_CKSUM";
		case LC_LOAD_WEAK_DYLIB:			return "LC_LOAD_WEAK_DYLIB";
		case LC_SEGMENT_64:					return "LC_SEGMENT_64";
		case LC_ROUTINES_64:					return "LC_ROUTINES_64";
		case LC_UUID:						return "LC_UUID";
		case LC_RPATH:						return "LC_RPATH";
		case LC_CODE_SIGNATURE:				return "LC_CODE_SIGNATURE";
		case LC_SEGMENT_SPLIT_INFO:			return "LC_SEGMENT_SPLIT_INFO";
		case LC_REEXPORT_DYLIB:				return "LC_REEXPORT_DYLIB";
		case LC_LAZY_LOAD_DYLIB:			return "LC_LAZY_LOAD_DYLIB";
		case LC_ENCRYPTION_INFO:			return "LC_ENCRYPTION_INFO";
		case LC_DYLD_INFO:					return "LC_DYLD_INFO";
		case LC_DYLD_INFO_ONLY:				return "LC_DYLD_INFO_ONLY";
		case LC_LOAD_UPWARD_DYLIB:			return "LC_LOAD_UPWARD_DYLIB";
		case LC_VERSION_MIN_MACOSX:			return "LC_VERSION_MIN_MACOSX";
		case LC_VERSION_MIN_IPHONEOS:		return "LC_VERSION_MIN_IPHONEOS";
		case LC_FUNCTION_STARTS:			return "LC_FUNCTION_STARTS";
		case LC_DYLD_ENVIRONMENT:			return "LC_DYLD_ENVIRONMENT";
		case LC_MAIN:						return "LC_MAIN";
		case LC_DATA_IN_CODE:				return "LC_DATA_IN_CODE";
		case LC_SOURCE_VERSION:				return "LC_SOURCE_VERSION";
		case LC_DYLIB_CODE_SIGN_DRS:		return "LC_DYLIB_CODE_SIGN_DRS";
		case LC_ENCRYPTION_INFO_64:			return "LC_ENCRYPTION_INFO_64";
		case LC_LINKER_OPTION:				return "LC_LINKER_OPTION";
		case LC_LINKER_OPTIMIZATION_HINT:	return "LC_LINKER_OPTIMIZATION_HINT";
		case LC_VERSION_MIN_TVOS:			return "LC_VERSION_MIN_TVOS";
		case LC_VERSION_MIN_WATCHOS:		return "LC_VERSION_MIN_WATCHOS";
		case LC_NOTE:						return "LC_NOTE";
		case LC_BUILD_VERSION:				return "LC_BUILD_VERSION";
		case LC_DYLD_EXPORTS_TRIE:			return "LC_DYLD_EXPORTS_TRIE";
		case LC_DYLD_CHAINED_FIXUPS:		return "LC_DYLD_CHAINED_FIXUPS";
		case LC_FILESET_ENTRY:				return "LC_FILESET_ENTRY";
		default:							return "Error : No such Load Command exists!";
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
char *printSectionType(int flag)
{
	switch (flag) {
		case S_REGULAR:								return "S_REGULAR";
		case S_ZEROFILL:								return "S_ZEROFILL";
		case S_CSTRING_LITERALS:					return "S_CSTRING_LITERALS";
		case S_4BYTE_LITERALS:						return "S_4BYTE_LITERALS";
		case S_8BYTE_LITERALS:						return "S_8BYTE_LITERALS";
		case S_LITERAL_POINTERS:					return "S_LITERAL_POINTERS";
		case S_NON_LAZY_SYMBOL_POINTERS:			return "S_NON_LAZY_SYMBOL_POINTERS";
		case S_LAZY_SYMBOL_POINTERS:				return "S_LAZY_SYMBOL_POINTERS";
		case S_SYMBOL_STUBS:							return "S_SYMBOL_STUBS";
		case S_MOD_INIT_FUNC_POINTERS:				return "S_MOD_INIT_FUNC_POINTERS";
		case S_MOD_TERM_FUNC_POINTERS:				return "S_MOD_TERM_FUNC_POINTERS";
		case S_COALESCED:							return "S_COALESCED";
		case S_GB_ZEROFILL:							return "S_GB_ZEROFILL";
		case S_INTERPOSING:							return "S_INTERPOSING";
		case S_16BYTE_LITERALS:						return "S_16BYTE_LITERALS";
		case S_DTRACE_DOF:							return "S_DTRACE_DOF";
		case S_LAZY_DYLIB_SYMBOL_POINTERS:			return "S_LAZY_DYLIB_SYMBOL_POINTERS";
		case S_THREAD_LOCAL_REGULAR:				return "S_THREAD_LOCAL_REGULAR";
		case S_THREAD_LOCAL_ZEROFILL:				return "S_THREAD_LOCAL_ZEROFILL";
		case S_THREAD_LOCAL_VARIABLES:				return "S_THREAD_LOCAL_VARIABLES";
		case S_THREAD_LOCAL_VARIABLE_POINTERS:		return "S_THREAD_LOCAL_VARIABLE_POINTERS";
		case S_THREAD_LOCAL_INIT_FUNCTION_POINTERS:	return "S_THREAD_LOCAL_INIT_FUNCTION_POINTERS";
		case S_INIT_FUNC_OFFSETS:					return "S_INIT_FUNC_OFFSETS";
		default:									return "Unknown Section Type!";
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
				printf("\t\t%3zu|%24s\t%s\n\t\t\t\tvmaddr: 0x%012llx\tvmsz: 0x%010llx\talign: 2^%d\tnreloc: %d\n",
					sec_cnt+1,
					sec_ptr->sectname,
					printSectionType(SECTION_TYPE & sec_ptr->flags),
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
	FILE 		*handle = NULL;
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
