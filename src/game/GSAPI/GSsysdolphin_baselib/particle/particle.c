#include <GSAPI/GSsysdolphin_baselib/particle/particle.h>

/* Instance options of a generator. Every psSet* entry point below toggles a
 * single bit of _psGenerator::mOptionFlags; the generator reads the bits back
 * while it spawns and updates the particles of its family. The mode bits of
 * the tornado scaling and of the local offset rotation only qualify the
 * feature they belong to. */
enum {
    PS_OPTION_CHILD_GENS_ATTACHED = 0x1,
    PS_OPTION_VELOCITY_ROTATION_IN_LOCAL = 0x4,
    PS_OPTION_OFFSET_ROTATION_IN_LOCAL = 0x8,
    PS_OPTION_OFFSET_ROTATION_IN_LOCAL_MODE = 0x10,
    PS_OPTION_PARTICLE_TEX_SCALING = 0x20,
    PS_OPTION_TORNADO_SCALING = 0x40,
    PS_OPTION_TORNADO_SCALING_MODE = 0x80,
    PS_OPTION_NODE_SCALING = 0x100,
    PS_OPTION_RANDOM_VELOCITY_SCALING = 0x200,
};

void psSetRandomVelocityScaling(_psGenerator *gen, u8 enable)
{
    if (enable) {
        gen->mOptionFlags |= PS_OPTION_RANDOM_VELOCITY_SCALING;
    } else {
        gen->mOptionFlags &= ~PS_OPTION_RANDOM_VELOCITY_SCALING;
    }
}

void psSetNodeScaling(_psGenerator *gen, u8 enable)
{
    if (enable) {
        gen->mOptionFlags |= PS_OPTION_NODE_SCALING;
    } else {
        gen->mOptionFlags &= ~PS_OPTION_NODE_SCALING;
    }
}

void psSetTornadoScaling(_psGenerator *gen, u8 enable, u8 mode)
{
    if (enable) {
        gen->mOptionFlags |= PS_OPTION_TORNADO_SCALING;
    } else {
        gen->mOptionFlags &= ~PS_OPTION_TORNADO_SCALING;
    }

    if (mode) {
        gen->mOptionFlags |= PS_OPTION_TORNADO_SCALING_MODE;
    } else {
        gen->mOptionFlags &= ~PS_OPTION_TORNADO_SCALING_MODE;
    }
}

void psSetParticleTexScaling(_psGenerator *gen, u8 enable)
{
    if (enable) {
        gen->mOptionFlags |= PS_OPTION_PARTICLE_TEX_SCALING;
    } else {
        gen->mOptionFlags &= ~PS_OPTION_PARTICLE_TEX_SCALING;
    }
}

void psSetOffsetRotationInLocal(_psGenerator *gen, u8 enable, u8 mode)
{
    if (enable) {
        gen->mOptionFlags |= PS_OPTION_OFFSET_ROTATION_IN_LOCAL;

        if (mode) {
            gen->mOptionFlags |= PS_OPTION_OFFSET_ROTATION_IN_LOCAL_MODE;
        } else {
            gen->mOptionFlags &= ~PS_OPTION_OFFSET_ROTATION_IN_LOCAL_MODE;
        }
    } else {
        gen->mOptionFlags &= ~PS_OPTION_OFFSET_ROTATION_IN_LOCAL;
        gen->mOptionFlags &= ~PS_OPTION_OFFSET_ROTATION_IN_LOCAL_MODE;
    }
}

void psSetVelocityRotationInLocal(_psGenerator *gen, u8 enable)
{
    if (enable) {
        gen->mOptionFlags |= PS_OPTION_VELOCITY_ROTATION_IN_LOCAL;
    } else {
        gen->mOptionFlags &= ~PS_OPTION_VELOCITY_ROTATION_IN_LOCAL;
    }
}

void psUnlinkChildGensFromJObj(_psGenerator *gen)
{
    gen->mOptionFlags &= ~PS_OPTION_CHILD_GENS_ATTACHED;
}

void psLinkChildGensToJObj(_psGenerator *gen, _HSD_JObj *jobj)
{
    gen->mOptionFlags |= PS_OPTION_CHILD_GENS_ATTACHED;
    gen->mJObj = jobj;
}
