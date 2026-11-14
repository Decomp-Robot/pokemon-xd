#include <gamedata.h>

void gamedataBiosSetZukanFLag(GamedataPlayData *playdata, u16 index, u16 value)
{
    if (playdata != NULL) {
        if (index > GAMEDATA_ZUKAN_FLAG_BITS) {
            return;
        }
        u16 mask = 1 << index;
        if (value == 1) {
            playdata->zukanFlags |= mask;
        } else {
            playdata->zukanFlags &= ~mask;
        }
    }
}

u8 gamedataBiosGetZukanFlag(GamedataPlayData *playdata, u16 index)
{
    if (playdata != NULL) {
        if (index > GAMEDATA_ZUKAN_FLAG_BITS) {
            return 0;
        }
        u16 mask = 1 << index;
        mask &= playdata->zukanFlags;
        return mask != 0;
    }
    return 0;
}

GamedataPlayData *gamedatasaveBiosGetplaydata(GamedataSave *save)
{
    if (save == NULL) {
        return NULL;
    }
    return &save->playdata;
}

void gamedatasaveBiosSetSysHeadChecksum(GamedataSaveMetadata *metadata, u32 checksum)
{
    if (metadata != NULL) {
        metadata->sysHeadChecksum = checksum;
    }
}

u32 gamedatasaveBiosGetSysHeadChecksum(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->sysHeadChecksum;
    }
    return 0;
}

void gamedatasaveBiosSetParChecksum(GamedataSaveMetadata *metadata, u8 checksum)
{
    if (metadata != NULL) {
        metadata->parChecksum = checksum;
    }
}

u8 gamedatasaveBiosGetParChecksum(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->parChecksum;
    }
    return 0;
}

void gamedatasaveBiosSetOptionAudio(GamedataSaveMetadata *metadata, u32 value)
{
    if (metadata != NULL) {
        metadata->optionAudioWord = value;
    }
}

u32 gamedatasaveBiosGetOptionAudioWord(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->optionAudioWord;
    }
    return 0;
}

void gamedatasaveBiosSetOptionAudioByte(GamedataSaveMetadata *metadata, u8 value)
{
    if (metadata != NULL) {
        metadata->optionAudio = value;
    }
}

void gamedatasaveBiosSetOptionNoVibration(GamedataSaveMetadata *metadata, u8 value)
{
    if (metadata != NULL) {
        metadata->optionNoVibration = value;
    }
}

void gamedatasaveBiosSetFloorposindex(GamedataSaveMetadata *metadata, u8 index)
{
    if (metadata != NULL) {
        metadata->floorposindex = index;
    }
}

void gamedatasaveBiosSetPrevfloorid(GamedataSaveMetadata *metadata, u32 floorid)
{
    if (metadata != NULL) {
        metadata->prevfloorid = floorid;
    }
}

void gamedatasaveBiosSetPlaytime(GamedataSaveMetadata *metadata, f64 playtime)
{
    if (metadata != NULL) {
        metadata->playtime = playtime;
    }
}

void gamedatasaveBiosSetFloorid(GamedataSaveMetadata *metadata, u32 floorid)
{
    if (metadata != NULL) {
        metadata->floorid = floorid;
    }
}

void gamedatasaveBiosSetSavecount(GamedataSaveMetadata *metadata, u32 count)
{
    if (metadata != NULL) {
        metadata->savecount = count;
    }
}

void gamedatasaveBiosSetMemcardID(GamedataSaveMetadata *metadata, u64 memcardID)
{
    if (metadata != NULL) {
        metadata->memcardID = memcardID;
    }
}

void gamedatasaveBiosSetSavernd(GamedataSaveMetadata *metadata, u32 value)
{
    if (metadata != NULL) {
        metadata->savernd = value;
    }
}

u8 gamedatasaveBiosGetOptionAudio(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->optionAudio;
    }
    return 0;
}

u8 gamedatasaveBiosGetOptionNoVibration(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->optionNoVibration;
    }
    return 0;
}

u8 gamedatasaveBiosGetFloorposindex(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->floorposindex;
    }
    return 0;
}

u32 gamedatasaveBiosGetPrevfloorid(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->prevfloorid;
    }
    return 0;
}

f64 gamedatasaveBiosGetPlaytime(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->playtime;
    }
    return 0.0;
}

u32 gamedatasaveBiosGetFloorid(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->floorid;
    }
    return 0;
}

u32 gamedatasaveBiosGetSavecount(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->savecount;
    }
    return 0;
}

u64 gamedatasaveBiosGetMemcardID(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->memcardID;
    }
    return 0;
}

u32 gamedatasaveBiosGetSavernd(GamedataSaveMetadata *metadata)
{
    if (metadata != NULL) {
        return metadata->savernd;
    }
    return 0;
}

void gamedatasaveBiosSetPtr(GamedataSave *save, GamedataSaveMetadata *metadata)
{
    if (save == NULL || metadata == NULL) {
        return;
    }
    save->metadata = *metadata;
}

GamedataSaveMetadata *gamedatasaveBiosGetPtr(GamedataSave *save)
{
    if (save == NULL) {
        return NULL;
    }
    return &save->metadata;
}

void gamedatasaveInit(GamedataSave *save)
{
    GamedataSaveMetadata *metadata = gamedatasaveBiosGetPtr(save);
    gamedatasaveBiosSetSavecount(metadata, 0);
}
