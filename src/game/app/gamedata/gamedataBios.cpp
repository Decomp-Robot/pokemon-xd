#include <gamedata.h>

void gamedataAttestBiosSetLangareaId(GamedataAttest *attest, u8 value)
{
    if (attest != NULL) {
        attest->langareaId = value;
    }
}

void gamedataAttestBiosSetAreaId(GamedataAttest *attest, u8 value)
{
    if (attest != NULL) {
        attest->areaId = value;
    }
}

void gamedataAttestBiosSetGenId(GamedataAttest *attest, u8 value)
{
    if (attest != NULL) {
        attest->generationId = value;
    }
}

void gamedataAttestBiosSetVerId(GamedataAttest *attest, u8 value)
{
    if (attest != NULL) {
        attest->versionId = value;
    }
}

u8 gamedataAttestBiosGetLangareaId(GamedataAttest *attest)
{
    if (attest == NULL) {
        return 0;
    }
    return attest->langareaId;
}

u8 gamedataAttestBiosGetAreaId(GamedataAttest *attest)
{
    if (attest == NULL) {
        return 0;
    }
    return attest->areaId;
}

u8 gamedataAttestBiosGetGenId(GamedataAttest *attest)
{
    if (attest == NULL) {
        return 0;
    }
    return attest->generationId;
}

u8 gamedataAttestBiosGetVerId(GamedataAttest *attest)
{
    if (attest == NULL) {
        return 0;
    }
    return attest->versionId;
}

void gamedataAttestBiosCopy(GamedataAttest *destination, GamedataAttest *source)
{
    if (destination != NULL && source != NULL) {
        *destination = *source;
    }
}

void gamedataBiosSetGamedataAtttestPtr(GamedataSave *gamedata, GamedataAttest *attest)
{
    if (gamedata == NULL || attest == NULL) {
        return;
    }
    gamedata->attest = *attest;
}

GamedataAttest *gamedataBiosGetGamedataAtttestPtr(GamedataSave *gamedata)
{
    if (gamedata == NULL) {
        return NULL;
    }
    return &gamedata->attest;
}
