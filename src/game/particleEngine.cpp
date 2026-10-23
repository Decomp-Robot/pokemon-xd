// Static initializer of the particle engine module, run once at startup
// through the .ctors table. The engine keeps its state in statically
// initialized globals, so the runtime hook registers no extra startup work.
extern "C" void particleEngineStaticInit(void)
{
}

// .ctors entry holding the initializer address (SDK _ctors[] convention).
extern "C" __declspec(section ".ctors") void (*const particleEngineStaticInit_ctor)(void) =
    particleEngineStaticInit;
