#include <GSAPI/GSsysdolphin_baselib/particle/particle.h>

u16 numActiveParticles;
u16 numActiveGenerators;
u16 numActivePSAppSRT;
u16 numPeakParticles;
u16 numPeakGenerators;
u16 numPeakPSAppSRT;
u16 g_ps_token;

_psCommandList **psCmdListArray[PS_NUM_BANK];
u32 psNumTexGroup[PS_NUM_BANK];

/* Instance options of a generator. Every psSet* entry point below toggles a
 * single bit of _psGenerator::mOptionFlags; the generator reads the bits back
 * while it spawns and updates the particles of its family. The mode bits of
 * the tornado scaling and of the local offset rotation only qualify the
 * feature they belong to. */
enum psGeneratorOption {
    PS_OPTION_CHILD_GENS_ATTACHED = 0x1,
    PS_OPTION_VELOCITY_ROTATION_IN_LOCAL = 0x4,
    PS_OPTION_OFFSET_ROTATION_IN_LOCAL = 0x8,
    PS_OPTION_OFFSET_ROTATION_IN_LOCAL_MODE = 0x10,
    PS_OPTION_PARTICLE_TEX_SCALING = 0x20,
    PS_OPTION_TORNADO_SCALING = 0x40,
    PS_OPTION_TORNADO_SCALING_MODE = 0x80,
    PS_OPTION_NODE_SCALING = 0x100,
    PS_OPTION_RANDOM_VELOCITY_SCALING = 0x200,
    PS_OPTION_DYNAMICS_SCALING = 0x1000,
};

void psSetToken(u16 token)
{
    g_ps_token = token;
}

void psSetParticleVisibility(_psGenerator *gen, u8 visible)
{
    _psParticle *particle;
    _psGenerator *generator;

    for (particle = _psListGetFirst(gen->mListGroup); particle != NULL;
         particle = particle->next) {
        if (particle->familyID == gen->mFamilyID) {
            if (visible) {
                particle->flags &= ~PS_FLAG_HIDDEN;
            } else {
                particle->flags |= PS_FLAG_HIDDEN;
            }
        }
    }

    for (generator = activeGenerator; generator != NULL; generator = generator->next) {
        if (generator->mFamilyID == gen->mFamilyID) {
            if (visible) {
                generator->flags &= ~PS_FLAG_HIDDEN;
            } else {
                generator->flags |= PS_FLAG_HIDDEN;
            }
        }
    }
}

void psSetGeneratorAngleRadiusScale(_psGenerator *gen, f32 *scale, u8 scaleDynamics)
{
    f32 average = (scale[0] + scale[1] + scale[2]) / 3.0F;
    int bankID = gen->mBankID;
    int index = gen->mIndex;

    switch (gen->mType & PS_EMISSION_SHAPE_MASK) {
    case PS_EMIT_RADIAL:
    case PS_EMIT_DISK_RADIAL_SPEED:
    case PS_EMIT_DISK:
        gen->mEmission.radial.azimuthStart = average * psCmdListArray[bankID][index]->emissionParameters[0];
        gen->mEmission.radial.azimuthEnd = average * psCmdListArray[bankID][index]->emissionParameters[1];
        break;
    case PS_EMIT_LINE:
        gen->mEmission.line.x = scale[0] * psCmdListArray[bankID][index]->emissionParameters[0];
        gen->mEmission.line.y = scale[1] * psCmdListArray[bankID][index]->emissionParameters[1];
        gen->mEmission.line.z = scale[2] * psCmdListArray[bankID][index]->emissionParameters[2];
        break;
    case PS_EMIT_CONE:
    case PS_EMIT_CYLINDER:
        gen->mEmission.radial.azimuthStart = average * psCmdListArray[bankID][index]->emissionParameters[0];
        gen->mEmission.radial.azimuthEnd = average * psCmdListArray[bankID][index]->emissionParameters[1];
        gen->mEmission.radial.height = average * psCmdListArray[bankID][index]->emissionParameters[2];
        break;
    case PS_EMIT_BOX:
        gen->mEmission.box.basis[0][0] = gen->mEmission.box.dimensions.x = scale[0] * psCmdListArray[bankID][index]->emissionParameters[0];
        gen->mEmission.box.basis[1][1] = gen->mEmission.box.dimensions.y = scale[1] * psCmdListArray[bankID][index]->emissionParameters[1];
        gen->mEmission.box.basis[2][2] = gen->mEmission.box.dimensions.z = scale[2] * psCmdListArray[bankID][index]->emissionParameters[2];
        break;
    case PS_EMIT_SPHERE:
        gen->mEmission.sphere.polarRange = average * psCmdListArray[bankID][index]->emissionParameters[0];
        gen->mEmission.sphere.angle = average * psCmdListArray[bankID][index]->emissionParameters[1];
        break;
    }

    gen->mEmissionRadius = average * psCmdListArray[bankID][index]->radius;
    if (scaleDynamics == 1) {
        gen->mGravity *= average;
        gen->mVelocityMultiplier *= average;
        gen->mOptionFlags |= PS_OPTION_DYNAMICS_SCALING;
    }
    gen->mEmissionScale.x = scale[0];
    gen->mEmissionScale.y = scale[1];
    gen->mEmissionScale.z = scale[2];
}

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
