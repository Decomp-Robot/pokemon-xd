#include <battleGrid.h>
#include <fight.h>
#include <fightFloor.h>
#include <fightOutPokemon.h>
#include <fightSeq.h>
#include <fightTrainer.h>
#include <fightTrainerStatus.h>
#include <GSmodel.h>
#include <Pokemon.h>
#include <sequence.h>
#include <string.h>
#include <wazaSequence.h>

// Trainer data the battle floor event plays its own sequence with.
enum { FIGHT_GSEVENT_DATA_ID = 64 };

// One snapshot of a battle floor sequence: the record that plays it, the data
// it was loaded from and the pose it was showing, followed by the grid slot it
// has to be put back into.
struct FightGSFloorTrainerRecord {
    FIGHT_TRAINER *mTrainer;        // 0x00: trainer that plays the sequence.
    u16 mDataId;                    // 0x04
    u8 mFlags;                      // 0x06
    bool mFlipped;                  // 0x07
    ModelSequence **mSlot;          // 0x08: grid slot that held the sequence.
    GSmodelState mState;            // 0x0C
};

struct FightGSFloorPokemonRecord {
    FightOutPokemon *mPokemon;      // 0x00: deployed Pokemon that plays the sequence.
    u16 mDataId;                    // 0x04
    u8 mFlags;                      // 0x06
    bool mRare;                     // 0x07
    bool mFlipped;                  // 0x08
    u8 _09[3];
    u32 mRnd;                       // 0x0C
    ModelSequence **mSlot;          // 0x10: grid slot that held the sequence.
    GSmodelState mState;            // 0x14
};

// Header of the block fightGSfloorPushData writes in front of the grid snapshot.
struct FightGSFloorHeader {
    u16 mTrainerNum;                // 0x00
    u16 mPokemonNum;                // 0x02
    u16 mHasEvent;                  // 0x04
    u8 _06[2];
};

// State carried through one traversal of the battle floor sequences.
struct FightGSFloorPass {
    bool mSnapshot;                 // 0x00: write a record for every sequence.
    bool mFree;                     // 0x01: release every sequence that is walked.
    u16 mCount;                     // 0x02: sequences walked so far.
    battleGrid *mGrid;              // 0x04: grid the sequences standing on it are saved from.
    u8 *mRecord;                    // 0x08: record the next snapshot is written to.
};

#ifdef __cplusplus
extern "C" {
#endif

void fightGSfloorFree(void);
s32 fightGSfloorGetPushDataSize(void);
void fightGSfloorPushData(void *data);
void fightGSfloorPopData(void *data);

#ifdef __cplusplus
}
#endif

u8 _fightGSfloorTrainerEvt(void *unused, u16 index, void *argument);
u8 _fightGSfloorPokemonCB(void *pokemon, u16 index, void *argument);
u8 _fightGSfloorTrainerCB(void *trainer, u16 index, void *argument);
ModelSequence **_fightGSfloorFindBGSeqPtr(battleGrid *grid, u32 sequence, bool trainerOnly);

// Releases every battle floor sequence: the trainers that fought, the event
// scene when the floor was an encounter that snatches Pokemon, and the Pokemon
// that were deployed. The grid and the camera go back to their idle state.
void fightGSfloorFree(void)
{
    FightGSFloorPass pass;

    memset(&pass, 0, sizeof(pass));
    pass.mFree = true;

    fightFloorLoopValidFightTrainer(0, _fightGSfloorTrainerCB, &pass, 0);
    if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
        _fightGSfloorTrainerEvt(0, 0, &pass);
    }
    fightFloorLoopValidFightOutPokemon(0, _fightGSfloorPokemonCB, &pass, 0);

    battleGridReset();
    fightMainAllCamera(1, 0);
}

// Size of the block that holds the grid snapshot and one record per sequence.
s32 fightGSfloorGetPushDataSize(void)
{
    FightGSFloorPass pass;
    u16 trainerNum;
    u16 pokemonNum;

    pass.mSnapshot = false;
    pass.mFree = false;
    pass.mCount = 0;
    pass.mRecord = 0;

    fightFloorLoopValidFightTrainer(0, _fightGSfloorTrainerCB, &pass, 0);
    trainerNum = pass.mCount;
    pass.mCount = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightGSfloorPokemonCB, &pass, 0);
    pokemonNum = pass.mCount;

    if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
        trainerNum++;
    }

    return sizeof(FightGSFloorHeader) + sizeof(battleGrid)
        + trainerNum * sizeof(FightGSFloorTrainerRecord)
        + pokemonNum * sizeof(FightGSFloorPokemonRecord);
}

// Snapshots the grid and the sequences standing on it into the block that
// fightGSfloorGetPushDataSize sized.
void fightGSfloorPushData(void *data)
{
    u16 trainerNum;
    u16 pokemonNum;
    FightGSFloorHeader *header = (FightGSFloorHeader *)data;
    battleGrid *grid = (battleGrid *)(header + 1);
    FightGSFloorPass pass;

    memcpy(grid, battleGridGetPtr(), sizeof(battleGrid));

    pass.mSnapshot = true;
    pass.mFree = false;
    pass.mCount = 0;
    pass.mGrid = grid;
    pass.mRecord = (u8 *)grid + sizeof(battleGrid);

    fightFloorLoopValidFightTrainer(0, _fightGSfloorTrainerCB, &pass, 0);
    trainerNum = pass.mCount;

    if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
        _fightGSfloorTrainerEvt(0, 0, &pass);
    }

    pass.mCount = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightGSfloorPokemonCB, &pass, 0);
    pokemonNum = pass.mCount;

    memset(header, 0, sizeof(FightGSFloorHeader));
    header->mTrainerNum = trainerNum;
    header->mPokemonNum = pokemonNum;
    if (fightFloorIsNowEncountSnatchOnly(0) == 1) {
        header->mHasEvent = 1;
    }
}

// Rebuilds every battle floor sequence from the snapshot block, restores the
// pose it was showing and puts it back into the grid slot it came from.
void fightGSfloorPopData(void *data)
{
    FightGSFloorHeader *header = (FightGSFloorHeader *)data;
    u16 trainerNum = header->mTrainerNum;
    u16 pokemonNum = header->mPokemonNum;
    u16 hasEvent = header->mHasEvent;
    battleGrid *grid = (battleGrid *)(header + 1);
    u8 *record = (u8 *)grid + sizeof(battleGrid);
    FightGSFloorPokemonRecord *prec;
    _GSmodel *model;
    ModelSequence *sequence;

    GSvec transform;

    while (trainerNum--) {
        FightGSFloorTrainerRecord *entry = (FightGSFloorTrainerRecord *)record;

        sequence = Sequence_LoadTrainer(entry->mDataId);
        entry->mTrainer->mSequence = sequence;
        *entry->mSlot = sequence;
        model = Sequence_GetGSmodel((u32)sequence);
        GSmodelPopState(model, &entry->mState);
        GSmodelGetPosition(model, &transform);
        Sequence_SetPosition(sequence, &transform);
        GSmodelGetRotation(model, &transform);
        Sequence_SetRotation(sequence, &transform);
        GSmodelGetScale(model, &transform);
        Sequence_SetScale(sequence, &transform);
        Sequence_SetFlipped(sequence, entry->mFlipped);
        Sequence_SetFlags(sequence, entry->mFlags);
        Sequence_SetVisibility(sequence, GSmodelGetVisibility(model));
        record += sizeof(FightGSFloorTrainerRecord);
    }

    if (hasEvent) {
        FightGSFloorTrainerRecord *entry = (FightGSFloorTrainerRecord *)record;

        sequence = Sequence_LoadTrainer(FIGHT_GSEVENT_DATA_ID);
        fightEvtSetSequence(sequence);
        model = Sequence_GetGSmodel((u32)sequence);
        GSmodelPopState(model, &entry->mState);
        GSmodelGetPosition(model, &transform);
        Sequence_SetPosition(sequence, &transform);
        GSmodelGetRotation(model, &transform);
        Sequence_SetRotation(sequence, &transform);
        GSmodelGetScale(model, &transform);
        Sequence_SetScale(sequence, &transform);
        Sequence_SetFlipped(sequence, entry->mFlipped);
        Sequence_SetFlags(sequence, entry->mFlags);
        Sequence_SetVisibility(sequence, GSmodelGetVisibility(model));
        record += sizeof(FightGSFloorTrainerRecord);
    }

    prec = (FightGSFloorPokemonRecord *)record;
    while (pokemonNum--) {
        sequence = Sequence_LoadPokemon(prec->mDataId, prec->mRnd, prec->mRare);
        prec->mPokemon->mSequence = sequence;
        *prec->mSlot = sequence;
        model = Sequence_GetGSmodel((u32)sequence);
        GSmodelPopState(model, &prec->mState);
        GSmodelGetPosition(model, &transform);
        Sequence_SetPosition(sequence, &transform);
        GSmodelGetRotation(model, &transform);
        Sequence_SetRotation(sequence, &transform);
        GSmodelGetScale(model, &transform);
        Sequence_SetScale(sequence, &transform);
        Sequence_SetFlipped(sequence, prec->mFlipped);
        Sequence_SetFlags(sequence, prec->mFlags);
        Sequence_SetVisibility(sequence, GSmodelGetVisibility(model));
        prec++;
    }

    memcpy(battleGridGetPtr(), grid, sizeof(battleGrid));
    fightMainAllCamera(1, 0);
    fightFloorSetShadow();
}

// Snapshots the event scene sequence when the floor has one.
u8 _fightGSfloorTrainerEvt(void *unused, u16 index, void *argument)
{
    FightGSFloorPass *pass = (FightGSFloorPass *)argument;
    ModelSequence *sequence = fightEvtGetSequence();

    if (sequence == 0) {
        return 1;
    }

    if (pass->mSnapshot) {
        FightGSFloorTrainerRecord *record = (FightGSFloorTrainerRecord *)pass->mRecord;

        record->mTrainer = 0;
        record->mDataId = FIGHT_GSEVENT_DATA_ID;
        record->mFlags = Sequence_GetFlags(sequence);
        record->mFlipped = Sequence_IsFlipped(sequence);
        record->mSlot = &sequence;
        GSmodelPushState(Sequence_GetGSmodel((u32)sequence), &record->mState);
        pass->mRecord += sizeof(FightGSFloorTrainerRecord);
    }

    if (pass->mFree) {
        Sequence_Free(sequence);
    }

    pass->mCount++;
    return 1;
}

// Collects the sequence of a deployed Pokemon.
u8 _fightGSfloorPokemonCB(void *pokemon, u16 index, void *argument)
{
    FightGSFloorPass *pass = (FightGSFloorPass *)argument;
    FightGSFloorPokemonRecord *record;
    ModelSequence *sequence =
        (ModelSequence *)pokemonGetStatus(pokemon, 0, POKEMON_QUERY_FIGHT_SEQUENCE, 0);
    u16 dataId;

    if (sequence == 0) {
        return 1;
    }

    dataId = Sequence_GetID(sequence);
    if (dataId == 0) {
        return 1;
    }

    if (pass->mSnapshot) {
        record = (FightGSFloorPokemonRecord *)pass->mRecord;
        record->mPokemon = (FightOutPokemon *)pokemon;
        record->mDataId = dataId;
        record->mFlags = Sequence_GetFlags(sequence);
        record->mFlipped = Sequence_IsFlipped(sequence);
        record->mRare = Sequence_IsRare(sequence);
        record->mRnd = Sequence_GetPokemonRndVal(sequence);
        record->mSlot = _fightGSfloorFindBGSeqPtr(pass->mGrid, (u32)sequence, false);
        GSmodelPushState(Sequence_GetGSmodel((u32)sequence), &record->mState);
        pass->mRecord += sizeof(FightGSFloorPokemonRecord);
    }

    if (pass->mFree) {
        Sequence_Free(sequence);
    }

    pass->mCount++;
    return 1;
}

// Collects the sequence of a trainer that is still fighting.
u8 _fightGSfloorTrainerCB(void *trainer, u16 index, void *argument)
{
    FightGSFloorPass *pass = (FightGSFloorPass *)argument;
    FightGSFloorTrainerRecord *record;
    ModelSequence *sequence = (ModelSequence *)fightTrainerGetStatus(
        (FIGHT_TRAINER *)trainer, 0, FIGHT_TRAINER_QUERY_SEQUENCE, 0);
    u16 dataId;

    if (sequence == 0) {
        return 1;
    }

    dataId = Sequence_GetID(sequence);
    if (dataId == 0) {
        return 1;
    }

    if (pass->mSnapshot) {
        record = (FightGSFloorTrainerRecord *)pass->mRecord;
        record->mTrainer = (FIGHT_TRAINER *)trainer;
        record->mDataId = dataId;
        record->mFlags = Sequence_GetFlags(sequence);
        record->mFlipped = Sequence_IsFlipped(sequence);
        record->mSlot = _fightGSfloorFindBGSeqPtr(pass->mGrid, (u32)sequence, true);
        GSmodelPushState(Sequence_GetGSmodel((u32)sequence), &record->mState);
        pass->mRecord += sizeof(FightGSFloorTrainerRecord);
    }

    if (pass->mFree) {
        Sequence_Free(sequence);
    }

    pass->mCount++;
    return 1;
}

// Finds the grid slot that holds the sequence of the given data ID, either the
// trainer slot of a position or one of its two Pokemon slots.
ModelSequence **_fightGSfloorFindBGSeqPtr(battleGrid *grid, u32 sequence, bool trainerOnly)
{
    battleGridPosition *position;
    s32 i;
    s32 j;

    if (sequence == 0) {
        return 0;
    }

    for (i = 0; i < 4; i++) {
        position = &grid->position[i];

        if (trainerOnly == 1) {
            if ((u32)position->trainerSequence == sequence) {
                return &position->trainerSequence;
            }
        } else {
            for (j = 0; j < 2; j++) {
                if ((u32)position->pokemonSequence[j] == sequence) {
                    return &position->pokemonSequence[j];
                }
            }
        }
    }

    return 0;
}
