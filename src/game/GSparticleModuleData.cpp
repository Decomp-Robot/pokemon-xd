#include <GSparticle.h>

/* Module data of the particle layer: the render module descriptor the module
 * list is handed and, behind it, the texts the loader logs about the files it
 * is given. GSparticleLoad reads the texts through the descriptor and
 * GSparticleInit copies the descriptor out of the block, so the two are one
 * object, stored here in the order the loader reads it. */
extern const _GSparticleModuleData GSparticleRenderModule = {
    {
        0,
        _particlePreUpdate,
        _particleRender,
        0,
        _particlePostUpdate,
        0,
        GSGFX_RENDER_MODULE_SIGNATURE,
    },
    /* Changing the Z draw flag of a generator is not carried over to the
     * engine yet, so this text is never logged. */
    "GSparticle: changing the Z draw flag of a generator currently has no effect\n",
    "GSparticle: ***************************************************************************\n",
    "GSparticle: ************* version 0 .gpt files are no longer supported ****************\n",
    "GSparticle: unknown version\n",
    "GSparticle: version mismatch between system(%d) and ptcl-file(%d)",
};
