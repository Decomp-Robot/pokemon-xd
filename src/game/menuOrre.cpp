extern "C" void menuOrreStaticInit(void)
{
}

extern "C" __declspec(section ".ctors") void (*const menuOrreStaticInit_ctor)(void) =
    menuOrreStaticInit;
