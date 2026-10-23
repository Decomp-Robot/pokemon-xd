// Static initializer of the menuTop module. The .ctors table entry makes the
// C++ runtime run it once at startup (__init_cpp); the module itself
// registers no startup work.
extern "C" void menuTopStaticInit(void)
{
}

// .ctors entry holding the initializer address (SDK _ctors[] convention).
extern "C" __declspec(section ".ctors") void (*const menuTopStaticInit_ctor)(void) =
    menuTopStaticInit;
