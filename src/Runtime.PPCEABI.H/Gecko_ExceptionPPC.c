/*
 * Exception fragment registration for dynamically loaded code.
 */

/* ------------------------------------------------------------------ */
/* Exception fragments                                                      */
/* ------------------------------------------------------------------ */

typedef struct ExceptionTableInitInfo ExceptionTableInitInfo;

typedef struct {
    ExceptionTableInitInfo *exception_info;
    char *toc;
    int active;
} FragmentInfo;

static FragmentInfo fragmentinfo[1];

/*
 * Number of exception fragments that can be registered at once.  The table
 * is what the registration walks, so its own size carries the count.
 */
#define FRAGMENT_COUNT (sizeof(fragmentinfo) / sizeof(fragmentinfo[0]))
/* Unregister the exception tables of a loaded fragment. */
void __unregister_fragment(int frag) {
    FragmentInfo *info;

    if (frag < 0) {
        return;
    }
    if (frag >= 1) {
        return;
    }
    info = &fragmentinfo[frag];
    info->exception_info = 0;
    info->toc = 0;
    info->active = 0;
}

/* Register exception tables and the fragment's TOC. */
int __register_fragment(ExceptionTableInitInfo *exception_info, char *toc) {
    unsigned int i;
    FragmentInfo *info;

    for (i = 0; i < FRAGMENT_COUNT; i++) {
        info = &fragmentinfo[i];
        if (info->active == 0) {
            info->exception_info = exception_info;
            info->toc = toc;
            info->active = 1;
            return 0;
        }
    }
    return -1;
}
