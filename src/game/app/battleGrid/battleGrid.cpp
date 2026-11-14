#include <battleGrid.h>
#include <ModelSequence.h>
#include <string.h>

extern "C" {
battleGrid gBattleGrid;
bool gSavedVisibilityCodes[12];
bool gBackupVisibilityCodes[12];
s32 g_max_model_size;
}

static bool savedVisibilityValid;
static bool backupVisibilityValid;

static inline ModelSequence *battleGridGetModel(NullSequence *sequence) {
    if (sequence == NULL) return NULL;
    ModelSequence *model;
    if (sequence->isModel == 1) model = (ModelSequence *)sequence;
    else model = NULL;
    return model;
}

extern "C" battleGrid *battleGridGetPtr(void) { return &gBattleGrid; }

extern "C" u16 battleGridGetNumPokemonsForTrainer(NullSequence *sequence) {
    battleGridPosition *position = battleGridGetPtr()->position;
    ModelSequence *trainer = battleGridGetModel(sequence);
    if (trainer == NULL) return 0;
    for (u16 i = 0; i < 4; ++i, ++position)
        if (position->trainerSequence == trainer)
            return position->pokemonNum;
    return 0;
}

extern "C" void battleGridRestoreModelVisibilityFlags(void) {
    battleGridPosition *position = battleGridGetPtr()->position;
    u16 i, j;
    u16 index = 0;
    if (backupVisibilityValid) {
        for (i = 0; i < 4; ++i, ++position) {
            if (position->trainerSequence != NULL)
                position->trainerSequence->NullSequence::SetVisibility(gBackupVisibilityCodes[index]);
            ++index;
            for (j = 0; j < 2; ++j) {
                if (position->pokemonSequence[j] != NULL)
                    position->pokemonSequence[j]->NullSequence::SetVisibility(gBackupVisibilityCodes[index]);
                ++index;
            }
        }
        backupVisibilityValid = false;
        memcpy(gSavedVisibilityCodes, gBackupVisibilityCodes, sizeof(gSavedVisibilityCodes));
    }
}

extern "C" void battleGridBackupModelVisibilityFlags(void) {
    battleGridPosition *position = battleGridGetPtr()->position;
    u16 i, j;
    u16 index = 0;
    memset(gBackupVisibilityCodes, 0, sizeof(gBackupVisibilityCodes));
    for (i = 0; i < 4; ++i, ++position) {
        if (position->trainerSequence != NULL)
            gBackupVisibilityCodes[index] = position->trainerSequence->visible;
        ++index;
        for (j = 0; j < 2; ++j) {
            if (position->pokemonSequence[j] != NULL)
                gBackupVisibilityCodes[index] = position->pokemonSequence[j]->visible;
            ++index;
        }
    }
    backupVisibilityValid = true;
}

extern "C" void battleGridResetModelVisibilityFlags(void) {
    battleGridPosition *position = battleGridGetPtr()->position;
    u16 i, j;
    u16 index = 0;
    if (savedVisibilityValid) {
        for (i = 0; i < 4; ++i, ++position) {
            if (position->trainerSequence != NULL)
                position->trainerSequence->NullSequence::SetVisibility(gSavedVisibilityCodes[index]);
            ++index;
            for (j = 0; j < 2; ++j) {
                if (position->pokemonSequence[j] != NULL)
                    position->pokemonSequence[j]->NullSequence::SetVisibility(gSavedVisibilityCodes[index]);
                ++index;
            }
        }
        savedVisibilityValid = false;
    }
}

extern "C" void battleGridStoreModelVisibilityFlags(void) {
    battleGridPosition *position = battleGridGetPtr()->position;
    u16 i, j;
    u16 index = 0;
    memset(gSavedVisibilityCodes, 0, sizeof(gSavedVisibilityCodes));
    for (i = 0; i < 4; ++i, ++position) {
        if (position->trainerSequence != NULL)
            gSavedVisibilityCodes[index] = position->trainerSequence->visible;
        ++index;
        for (j = 0; j < 2; ++j) {
            if (position->pokemonSequence[j] != NULL)
                gSavedVisibilityCodes[index] = position->pokemonSequence[j]->visible;
            ++index;
        }
    }
    savedVisibilityValid = true;
}

extern "C" void battleGridHideModelsExcept(NullSequence *sequence) {
    battleGridPosition *position = battleGridGetPtr()->position;
    u16 i, j;
    u16 index = 0;
    ModelSequence *keep = battleGridGetModel(sequence);
    battleGridResetModelVisibilityFlags();
    memset(gSavedVisibilityCodes, 0, sizeof(gSavedVisibilityCodes));
    for (i = 0; i < 4; ++i, ++position) {
        if (position->trainerSequence != NULL) {
            gSavedVisibilityCodes[index] = position->trainerSequence->visible;
            if (position->trainerSequence != keep) position->trainerSequence->SetVisibility(false);
        }
        ++index;
        for (j = 0; j < 2; ++j) {
            if (position->pokemonSequence[j] != NULL) {
                gSavedVisibilityCodes[index] = position->pokemonSequence[j]->visible;
                if (position->pokemonSequence[j] != keep) position->pokemonSequence[j]->SetVisibility(false);
            }
            ++index;
        }
    }
    savedVisibilityValid = true;
}

extern "C" void battleGridReset(void) {
    memset(&gBattleGrid, 0, sizeof(gBattleGrid));
    g_max_model_size = 0;
    savedVisibilityValid = false;
}

