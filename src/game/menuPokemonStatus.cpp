// Static initializer of the menuPokemonStatus module, run once at startup
// through the .ctors table.
extern "C" void menuPokemonStatusStaticInit(void)
{
}

// .ctors entry holding the initializer address (SDK _ctors[] convention).
extern "C" __declspec(section ".ctors") void (*const menuPokemonStatusStaticInit_ctor)(void) =
    menuPokemonStatusStaticInit;
