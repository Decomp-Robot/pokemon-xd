#include <gamedata.h>
#include <savedata.h>

void *gamedataGetStatus(void *gamedata, u16 field)
{
    if (field == 0 || field >= GAMEDATA_STATUS_LIMIT) {
        return NULL;
    }
    if (gamedata == NULL) {
        void *savedata = savedataGetStatus(NULL, SAVE_DATA_IMAGE);
        if (savedata == NULL) {
            return NULL;
        }
        gamedata = savedataGetStatus(savedata, SAVE_DATA_GAME);
        if (gamedata == NULL) {
            return NULL;
        }
    }
    GamedataAttest *attest = gamedataBiosGetGamedataAtttestPtr((GamedataSave *)gamedata);
    if (attest == NULL) {
        return NULL;
    }
    switch (field) {
    case GAMEDATA_STATUS_ATTEST:
        return attest;
    case GAMEDATA_STATUS_ORIGIN:
        return (void *)gamedataAttestBiosGetVerId(attest);
    case GAMEDATA_STATUS_GENERATION:
        return (void *)gamedataAttestBiosGetGenId(attest);
    case GAMEDATA_STATUS_REGION:
        return (void *)gamedataAttestBiosGetAreaId(attest);
    case GAMEDATA_STATUS_LANGUAGE:
        return (void *)gamedataAttestBiosGetLangareaId(attest);
    default:
        return NULL;
    }
}

void gamedataSetStatus(void *gamedata, u16 field, u32 status)
{
    if (field == 0 || field >= GAMEDATA_STATUS_LIMIT) {
        return;
    }
    if (gamedata == NULL) {
        void *savedata = savedataGetStatus(NULL, SAVE_DATA_IMAGE);
        if (savedata == NULL) {
            return;
        }
        gamedata = savedataGetStatus(savedata, SAVE_DATA_GAME);
        if (gamedata == NULL) {
            return;
        }
    }
    GamedataAttest *attest = gamedataBiosGetGamedataAtttestPtr((GamedataSave *)gamedata);
    if (attest == NULL) {
        return;
    }
    switch (field) {
    case GAMEDATA_STATUS_ATTEST:
        gamedataBiosSetGamedataAtttestPtr((GamedataSave *)gamedata, (GamedataAttest *)status);
        break;
    case GAMEDATA_STATUS_ORIGIN:
        gamedataAttestBiosSetVerId(attest, (u8)status);
        break;
    case GAMEDATA_STATUS_GENERATION:
        gamedataAttestBiosSetGenId(attest, (u8)status);
        break;
    case GAMEDATA_STATUS_REGION:
        gamedataAttestBiosSetAreaId(attest, (u8)status);
        break;
    case GAMEDATA_STATUS_LANGUAGE:
        gamedataAttestBiosSetLangareaId(attest, (u8)status);
        break;
    }
}

void gamedataAttestInit(GamedataAttest *attest)
{
    if (attest != NULL) {
        gamedataSetStatus(attest, GAMEDATA_STATUS_ORIGIN, 0);
        gamedataSetStatus(attest, GAMEDATA_STATUS_GENERATION, 0);
        gamedataSetStatus(attest, GAMEDATA_STATUS_REGION, 0);
        gamedataSetStatus(attest, GAMEDATA_STATUS_LANGUAGE, 0);
    }
}

u8 gamedataAttestCheckValid(GamedataAttest *attest)
{
    if (attest == NULL) {
        return 0;
    }
    if ((s32)gamedataGetStatus(attest, GAMEDATA_STATUS_ORIGIN) == 0) {
        return 0;
    }
    if ((s32)gamedataGetStatus(attest, GAMEDATA_STATUS_GENERATION) == 0) {
        return 0;
    }
    if ((s32)gamedataGetStatus(attest, GAMEDATA_STATUS_REGION) == 0) {
        return 0;
    }
    return (s32)gamedataGetStatus(attest, GAMEDATA_STATUS_LANGUAGE) != 0;
}

void gamedataAttestCreate(GamedataAttest *attest, u8 versionId, u8 generationId, u8 areaId, u8 langareaId)
{
    if (attest == NULL) {
        return;
    }
    if (versionId == 0) {
        return;
    }
    if (generationId == 0) {
        return;
    }
    if (areaId == 0) {
        return;
    }
    if (langareaId == 0) {
        return;
    }
    gamedataAttestInit(attest);
    gamedataSetStatus(attest, GAMEDATA_STATUS_ORIGIN, versionId);
    gamedataSetStatus(attest, GAMEDATA_STATUS_GENERATION, generationId);
    gamedataSetStatus(attest, GAMEDATA_STATUS_REGION, areaId);
    gamedataSetStatus(attest, GAMEDATA_STATUS_LANGUAGE, langareaId);
}

void gamedataBiosSetFlag(GamedataSave *save, u16 field, u16 index, u8 value)
{
    if (field == 0 || field >= GAMEDATASAVE_FIELD_LIMIT) {
        return;
    }
    if (save == NULL) {
        void *savedata = savedataGetStatus(NULL, SAVE_DATA_IMAGE);
        if (savedata == NULL) {
            return;
        }
        save = (GamedataSave *)savedataGetStatus(savedata, SAVE_DATA_GAME);
        if (save == NULL) {
            return;
        }
    }
    GamedataPlayData *playdata = gamedatasaveBiosGetplaydata(save);
    if (playdata == NULL) {
        return;
    }
    switch (field) {
    case GAMEDATASAVE_FIELD_ZUKAN_FLAG:
        gamedataBiosSetZukanFLag(playdata, index, value);
        break;
    }
}

void gamedataInit(GamedataSave *gamedata)
{
    GamedataAttest *attest = (GamedataAttest *)gamedataGetStatus(gamedata, GAMEDATA_STATUS_ATTEST);
    if (attest != NULL) {
        gamedataAttestInit(attest);
        gamedatasaveInit(gamedata);
        for (u16 index = 0; index < GAMEDATA_ZUKAN_FLAG_BITS; index++) {
            gamedataBiosSetFlag(gamedata, GAMEDATASAVE_FIELD_ZUKAN_FLAG, index, 0);
        }
    }
}

void gamedataCreate(GamedataSave *gamedata, u8 versionId, u8 generationId, u8 areaId, u8 langareaId)
{
    if (gamedata != NULL) {
        gamedataInit(gamedata);
        GamedataAttest *attest = (GamedataAttest *)gamedataGetStatus(gamedata, GAMEDATA_STATUS_ATTEST);
        if (attest != NULL) {
            gamedataAttestCreate(attest, versionId, generationId, areaId, langareaId);
        }
    }
}

unsigned long gamedatasaveGetStatus(void *gamedata, u16 field)
{
    GamedataSave *save = (GamedataSave *)gamedata;
    void *savedata = NULL;
    if (field == 0 || field >= GAMEDATASAVE_FIELD_LIMIT) {
        return 0;
    }
    if (save == NULL) {
        savedata = savedataGetStatus(NULL, SAVE_DATA_IMAGE);
        if (savedata == NULL) {
            return 0;
        }
        save = (GamedataSave *)savedataGetStatus(savedata, SAVE_DATA_GAME);
        if (save == NULL) {
            return 0;
        }
    }
    GamedataSaveMetadata *metadata = gamedatasaveBiosGetPtr(save);
    if (metadata == NULL) {
        return 0;
    }
    switch (field) {
    case GAMEDATASAVE_FIELD_METADATA:
        return (unsigned long)savedata;
    case GAMEDATASAVE_FIELD_SAVE_RANDOM:
        return gamedatasaveBiosGetSavernd(metadata);
    case GAMEDATASAVE_FIELD_SAVE_COUNT:
        return gamedatasaveBiosGetSavecount(metadata);
    case GAMEDATASAVE_FIELD_WORLD_MAP_FLOOR:
        return gamedatasaveBiosGetFloorid(metadata);
    case GAMEDATASAVE_FIELD_PLAYTIME:
        return (s32)gamedatasaveBiosGetPlaytime(metadata);
    case GAMEDATASAVE_FIELD_PREVIOUS_FLOOR:
        return gamedatasaveBiosGetPrevfloorid(metadata);
    case GAMEDATASAVE_FIELD_FLOOR_POSITION:
        return gamedatasaveBiosGetFloorposindex(metadata);
    case GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION:
        return gamedatasaveBiosGetOptionNoVibration(metadata);
    case GAMEDATASAVE_FIELD_OPTION_AUDIO_BYTE:
        return gamedatasaveBiosGetOptionAudio(metadata);
    case GAMEDATASAVE_FIELD_OPTION_AUDIO_WORD:
        return gamedatasaveBiosGetOptionAudioWord(metadata);
    case GAMEDATASAVE_FIELD_SYSTEM_CHECKSUM:
        return gamedatasaveBiosGetSysHeadChecksum(metadata);
    case GAMEDATASAVE_FIELD_PAR_CHECKSUM:
        return gamedatasaveBiosGetParChecksum(metadata);
    default:
        return 0;
    }
}

void gamedatasaveSetStatus(void *save, u16 field, unsigned long value)
{
    if (field == 0 || field >= GAMEDATASAVE_FIELD_LIMIT) {
        return;
    }
    if (save == NULL) {
        void *savedata = savedataGetStatus(NULL, SAVE_DATA_IMAGE);
        if (savedata == NULL) {
            return;
        }
        save = (GamedataSave *)savedataGetStatus(savedata, SAVE_DATA_GAME);
        if (save == NULL) {
            return;
        }
    }
    GamedataSaveMetadata *metadata = gamedatasaveBiosGetPtr((GamedataSave *)save);
    if (metadata == NULL) {
        return;
    }
    switch (field) {
    case GAMEDATASAVE_FIELD_METADATA:
        gamedatasaveBiosSetPtr((GamedataSave *)save, (GamedataSaveMetadata *)value);
        break;
    case GAMEDATASAVE_FIELD_SAVE_RANDOM:
        gamedatasaveBiosSetSavernd(metadata, value);
        break;
    case GAMEDATASAVE_FIELD_SAVE_COUNT:
        gamedatasaveBiosSetSavecount(metadata, value);
        break;
    case GAMEDATASAVE_FIELD_WORLD_MAP_FLOOR:
        gamedatasaveBiosSetFloorid(metadata, value);
        break;
    case GAMEDATASAVE_FIELD_PLAYTIME:
        gamedatasaveBiosSetPlaytime(metadata, (f32)(s32)value);
        break;
    case GAMEDATASAVE_FIELD_PREVIOUS_FLOOR:
        gamedatasaveBiosSetPrevfloorid(metadata, value);
        break;
    case GAMEDATASAVE_FIELD_FLOOR_POSITION:
        gamedatasaveBiosSetFloorposindex(metadata, (u8)value);
        break;
    case GAMEDATASAVE_FIELD_OPTION_NO_VIBRATION:
        gamedatasaveBiosSetOptionNoVibration(metadata, (u8)value);
        break;
    case GAMEDATASAVE_FIELD_OPTION_AUDIO_BYTE:
        gamedatasaveBiosSetOptionAudioByte(metadata, (u8)value);
        break;
    case GAMEDATASAVE_FIELD_OPTION_AUDIO_WORD:
        gamedatasaveBiosSetOptionAudio(metadata, value);
        break;
    case GAMEDATASAVE_FIELD_SYSTEM_CHECKSUM:
        gamedatasaveBiosSetSysHeadChecksum(metadata, value);
        break;
    case GAMEDATASAVE_FIELD_PAR_CHECKSUM:
        gamedatasaveBiosSetParChecksum(metadata, (u8)value);
        break;
    }
}

u8 gamedataBiosGetFlag(GamedataSave *save, u16 field, u16 index)
{
    if (field == 0 || field >= GAMEDATASAVE_FIELD_LIMIT) {
        return 0;
    }
    if (save == NULL) {
        void *savedata = savedataGetStatus(NULL, SAVE_DATA_IMAGE);
        if (savedata == NULL) {
            return 0;
        }
        save = (GamedataSave *)savedataGetStatus(savedata, SAVE_DATA_GAME);
        if (save == NULL) {
            return 0;
        }
    }
    GamedataPlayData *playdata = gamedatasaveBiosGetplaydata(save);
    if (playdata == NULL) {
        return 0;
    }
    switch (field) {
    case GAMEDATASAVE_FIELD_ZUKAN_FLAG:
        return gamedataBiosGetZukanFlag(playdata, index);
    default:
        return 0;
    }
}
