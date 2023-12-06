#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "hook.h"

/*
jump to 64 bit address by using following logic:
    pushq low32 bits (as 64bit value) of patched function
    movl high32 bits of patched function (fill in missing 4 bytes of previously pushed value), 0x4(%rsp)
    retq
as there is no way to do following on x86_64:
    1. jump to absolute 64bit address directly
    2. push 64bit immediate value to perform ret right after to emulate jmp
No state maintainence has to be done. No registers saving/restoring etc...
*/
#pragma pack(push, 1)
static struct {
    uint8_t pushq;
    uint32_t low32;
    uint32_t movl;
    uint32_t high32;
    uint8_t retq;
} patch = {
    .pushq = 0x68,
    .movl = 0x042444C7,  // x86 little-endian
    .retq = 0xC3
};
#pragma pack(pop)

static int unprotect_page(void* addr) {
	int pagesize = sysconf(_SC_PAGE_SIZE);
	int pagemask = ~(pagesize -1);
	char* page = (char *)((size_t)addr & pagemask);
	return mprotect(page, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);
}

int hook_function(void* target, void* replace) {
	int count;

	if(unprotect_page(replace)) {
		fprintf(stderr, "Could not unprotect replace mem: %p\n", replace);
		return 1;
	}

	if(unprotect_page(target)) {
		fprintf(stderr, "Could not unprotect target mem: %p\n", target);
		return 1;
	}

	patch.low32 = (uintptr_t)replace & 0xFFFFFFFF;
	patch.high32 = ((uintptr_t)replace >> 32) & 0xFFFFFFFF;

	/* smash the target function */
	memcpy(target, &patch, sizeof patch);

	return 0;
}
