/* symbols just to satisfy the linker*/
#define TEXT_STUB(name) void name(void) {}
#define DATA_STUB(name) int name;

// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_start)

// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(__ArenaHi)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(__ArenaLo)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(__init_registers)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_ctors)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_dtors)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_bss_init_info)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_rom_copy_info)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_stack_addr)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(_stack_end)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(powf)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(rand)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(srand)
// UNUSED: the game never calls this; dead-stripped at link.
TEXT_STUB(atan2f)
