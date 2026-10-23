extern "C" {
#include <dolphin/types.h>
}
#include <dolphin/mtx.h>
#include <dolphin/gx/GXStruct.h>
#include <types.h>
#include <GSgfx.h>
#include <AppScript.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <menuUseItem.h>
#include <menuScript.h>
#include <undertaker.h>
#include <mailMain.h>
#include <evolution.h>
#include <pokemonDB.h>
#include <GSpart.h>
#include <menuPokemon.h>
#include <hero.h>
#include <savedata.h>
#include <gamedata.h>
#include <GSflag.h>
#include <sound.h>
#include <GSlog.h>
#include <GSres.h>
#include <GSmodel.h>
#include <floor.h>
#include <fade.h>
#include <script.h>
#include <GSinput.h>
#include <GSthread.h>
#include <GSmsg.h>
#include <esaba.h>
#include <pokemonRelive.h>
#include <people.h>
#include <darkPokemon.h>
#include <fightEncount.h>
#include <FightResult.h>
#include <wazaSequence.h>
#include <sodateya.h>
#include <light.h>
#include <wazaDB.h>
#include <pokecolo.h>

static inline char* GetString(GSscript::tigaVariant*, char*);

static inline GSscript::tigaWaza* GetWaza(GSscript::tigaVariant* v)
{
    if (v->kind == GSscript::VariantWaza)
        return v->waza;
    GSscript::tigaErrMessage("waza not access");
    return 0;
}

static inline void WaitWaza(GSscript::tigaWaza* waza)
{
    while (WazaSequence_IsActive(waza->wazaSequence))
        GSthreadSwitch();
}

static inline char* GetString(GSscript::tigaVariant* value, char* buffer)
{
    if (value->kind == GSscript::VariantString)
        return value->string;
    return value->getCnvString(buffer);
}

static inline void SetArray(GSscript::tigaVariant* value, GSscript::tigaArrayFixed* array)
{
    value->kind = GSscript::VariantArray;
    value->array = array;
}

static inline void FreeMapResource(int resourceId)
{
    GSresFreeResource(floorGetCurrentGroupID(), resourceId);
}

static inline GSresID* GetResource(GSscript::tigaVariant* value)
{
    if (value->kind == GSscript::VariantResource)
        return value->resource;
    GSscript::tigaErrMessage("people not access");
    return 0;
}
static inline void FreeMapResource(GSresID* resource)
{
    GSresFreeResource(floorGetCurrentGroupID(), resource->resourceId);
}
static inline float VectorDistance(GSvec* first, GSvec* second)
{
    return PSVECDistance((const Vec*)first, (const Vec*)second);
}

static inline GSresID* FindResource(GSresID* resource)
{
    long index = GSscript::Tiga::m_cpCurrentObject->mObjectManager.getResourceIdx(resource);
    GSresID* entry;
    if (index == -1) {
        entry = GSscript::Tiga::m_cpCurrentObject->mObjectManager.newResource();
        if (entry) {
            entry->groupId = resource->groupId;
            entry->resourceId = resource->resourceId;
        }
    } else {
        entry = GSscript::Tiga::m_cpCurrentObject->mObjectManager.getResource(index);
    }
    return entry;
}
static inline void SetResource(GSscript::tigaVariant* value, GSresID* resource)
{
    value->kind = GSscript::VariantResource;
    value->resource = resource;
    if (value->kind != GSscript::VariantInteger && value->kind != GSscript::VariantSingle)
        GSscript::Tiga::m_cpCurrentObject->mCellMemory->dupVariant(value);
}
static inline void SetSingle(GSscript::tigaVariant* value, float single)
{
    value->kind = GSscript::VariantSingle;
    value->single = single;
}

static inline Pokemon* GetPokemon(GSscript::tigaVariant* value)
{
    if (value->kind == GSscript::VariantPokemon)
        return (Pokemon*)value->object;
    GSscript::tigaErrMessage("pokemon not access");
    return 0;
}


extern "C" void appScriptPlayME_Thread(u8, int, int, int);
extern "C" int appScriptCommonCall(int);
int motorFun(u8, u8, unsigned long, u8, bool);
void _appScriptModelSetVisibility(GSmodel*, void*);
static const f32 appScriptDegreesToRadians = 0.017453292f;
static const u32 ScriptThreadPriority = 128;
static const u32 ScriptThreadFlags = 1;
static const GSinputDevice ScriptController = GS_INPUT_PORT_1;

enum ScriptObjectKind {
    ScriptCamera = 33,
    ScriptPeople = 35,
    ScriptPokemon = 37,
    ScriptFloor = 38,
    ScriptTask = 39,
    ScriptMenu = 40,
    ScriptFade = 41,
    ScriptFight = 42,
    ScriptHero = 43,
    ScriptLight = 44,
    ScriptModel = 45,
    ScriptCol = 46,
    ScriptSound = 47,
    ScriptPad = 48,
    ScriptMail = 49,
    ScriptDirection = 50,
    ScriptTv = 51,
    ScriptSodateya = 52,
    ScriptWaza = 58,
    ScriptDarkPokemon = 59,
    ScriptEsaba = 60
};
enum ScriptGlobalFunction {
    ScriptFlagOn = 129,
    ScriptFlagOff = 130,
    ScriptFlagSet = 131,
    ScriptFlagTest = 132,
    ScriptFlagGet = 133,
    ScriptPrintf = 136,
    ScriptRandom = 137,
    ScriptDarkPokemonInformation = 138,
    ScriptAndFlagTest = 139,
    ScriptOrFlagTest = 140,
    ScriptCommonCall = 142,
    ScriptDisplayScriptInfo = 143,
    ScriptSetHumanCollision = 145,
    ScriptGetHumanCollision = 146,
    ScriptSetTreasureSearchDistance = 147,
    ScriptMessageString = 148,
    ScriptTreasureResource = 149,
    ScriptPokemonNameId = 150,
    ScriptArrayInteger = 151,
    ScriptIsHiddenWaza = 152,
    ScriptVectorDistance = 153,
    ScriptFreeResource = 154,
    ScriptGlowStart = 155,
    ScriptGlowEnd = 156,
    ScriptLensFlare = 157,
    ScriptResource = 158,
    ScriptVsyncRate = 159,
    ScriptCountryCode = 160,
    ScriptLanguage = 161
};

GSscript::Tiga* scriptCreateAppScript()
{
    return new AppScript;
}

AppScript::AppScript()
{
    memset(mWaza, 0, sizeof(mWaza));
}

AppScript::~AppScript()
{
}

void AppScript::mapping()
{
    memset(mWaza, 0, sizeof(mWaza));
}

void AppScript::unmapping()
{
}

GSscript::tigaWaza* AppScript::allocWazaPtr()
{
    for (GSscript::tigaWaza* waza = mWaza; waza != mWaza + sizeof(mWaza) / sizeof(mWaza[0]); ++waza) {
        if (waza->sequence == 0) {
            waza->sequence = 0;
            waza->wazaSequence = 0;
            return waza;
        }
    }
    return 0;
}

void AppScript::freeWazaPtr(GSscript::tigaWaza* waza)
{
    if (waza == 0)
        return;
    for (GSscript::tigaWaza* slot = mWaza; slot != mWaza + sizeof(mWaza) / sizeof(mWaza[0]); ++slot) {
        if (slot == waza) {
            waza->sequence = 0;
            waza->wazaSequence = 0;
        }
    }
}

void AppScript::unload()
{
    for (GSscript::tigaWaza* waza = mWaza; waza != mWaza + sizeof(mWaza) / sizeof(mWaza[0]); ++waza) {
        if (waza->wazaSequence != 0 || waza->sequence != 0) {
            if (waza->wazaSequence != 0) {
                WazaSequence_Stop(waza->wazaSequence);
                WazaSequence_Free(waza->wazaSequence);
            }
            if (waza->sequence != 0)
                Sequence_Free(waza->sequence);
            --mLiveWaza;
            waza->sequence = 0;
            waza->wazaSequence = 0;
        }
    }
}

int AppScript::execExtendMethod(int objectKind, int methodId, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    int status = 0;
    switch (objectKind) {
    case ScriptPeople: status = cmpPeople(methodId, args, result); break;
    case ScriptCamera: status = cmpCamera(methodId, args, result); break;
    case ScriptPokemon: status = cmpPokemon(methodId, args, result); break;
    case ScriptFloor: status = cmpFloor(methodId, args, result); break;
    case ScriptTask: status = cmpTask(methodId, args, result); break;
    case ScriptMenu: status = cmpMenu(methodId, args, result); break;
    case ScriptFade: status = cmpFade(methodId, args, result); break;
    case ScriptFight: status = cmpFight(methodId, args, result); break;
    case ScriptHero: status = cmpHero(methodId, args, result); break;
    case ScriptLight: status = cmpLight(methodId, args, result); break;
    case ScriptModel: status = cmpModel(methodId, args, result); break;
    case ScriptCol: status = cmpCol(methodId, args, result); break;
    case ScriptSound: status = cmpSound(methodId, args, result); break;
    case ScriptPad: status = cmpPad(methodId, args, result); break;
    case ScriptMail: status = cmpMail(methodId, args, result); break;
    case ScriptDirection: status = cmpDirection(methodId, args, result); break;
    case ScriptTv: status = cmpTv(methodId, args, result); break;
    case ScriptSodateya: status = cmpSodateya(methodId, args, result); break;
    case ScriptWaza: status = cmpWaza(methodId, args, result); break;
    case ScriptDarkPokemon: status = cmpDarkPokemon(methodId, args, result); break;
    case ScriptEsaba: status = cmpEsaba(methodId, args, result); break;
    }
    return status;
}

int AppScript::execExtendGlobalFunction(int functionId, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    GSresID resource;
    switch (functionId) {
    case ScriptFlagOn: {
        u32 flag = (args)->getInteger();
        if (flag != 0 && flag <= flag_data_number->count)
            GSflagOn(flag);
        break;
    }
    case ScriptFlagOff: {
        u32 flag = (args)->getInteger();
        if (flag != 0 && flag <= flag_data_number->count)
            GSflagOff(flag);
        break;
    }
    case ScriptFlagSet: {
        u32 flag = (args)->getInteger();
        long value = (args + 1)->getInteger();
        if (flag != 0 && flag <= flag_data_number->count)
            GSflagSet(flag, value);
        break;
    }
    case ScriptFlagTest: {
        u32 flag = (args)->getInteger();
        if (flag != 0 && flag <= flag_data_number->count)
            result->setInteger((u8)GSflagTest(flag));
        break;
    }
    case ScriptFlagGet: {
        u32 flag = (args)->getInteger();
        if (flag != 0 && flag <= flag_data_number->count)
            result->setInteger(GSflagGet(flag));
        break;
    }
    case ScriptPrintf:
        fncPrintf(args);
        break;
    case ScriptRandom:
        result->setInteger(scriptGetRandom((args)->getInteger()));
        break;
    case ScriptDarkPokemonInformation: {
        long pokemonId = (args)->getInteger();
        long information = (args + 1)->getInteger();
        darkPokemonBiosSetInfomation(pokemonId, information);
        break;
    }
    case ScriptAndFlagTest:
        result->setInteger(andFlagTest(args));
        break;
    case ScriptOrFlagTest:
        result->setInteger(orFlagTest(args));
        break;
    case ScriptCommonCall:
        commonCall(args, result);
        break;
    case ScriptDisplayScriptInfo:
        floorSetDispScriptInfo((args)->getInteger() != 0);
        break;
    case ScriptSetHumanCollision:
        floorSetHumanColision((args)->getInteger() != 0);
        break;
    case ScriptGetHumanCollision:
        result->setInteger(floorGetHumanColision());
        break;
    case ScriptSetTreasureSearchDistance: {
        long treasureId = (args)->getInteger();
        float distance = (args + 1)->getSingle();
        peopleSetTresureSearchDistance(treasureId, distance);
        break;
    }
    case ScriptMessageString: {
        u16* message = GSmsgGetStringPtr((args)->getInteger());
        result->kind = GSscript::VariantFunctionPointer;
        result->object = message;
        break;
    }
    case ScriptTreasureResource: {
        resource.groupId = floorGetCurrentGroupID();
        resource.resourceId = floorTresureGetResID((args)->getInteger());
        SetResource(result, FindResource(&resource));
        break;
    }
    case ScriptPokemonNameId:
        result->setInteger(pokemonDB_GetName((args)->getInteger()));
        break;
    case ScriptArrayInteger: {
        result->setInteger(((args)->getArray()->get((args + 1)->getInteger()))->getInteger());
        break;
    }
    case ScriptIsHiddenWaza:
        result->setInteger(wazaDB_GetHidenFlag((args)->getInteger()));
        break;
    case ScriptVectorDistance: {
        SetSingle(result, VectorDistance((args)->getVector(), (args + 1)->getVector()));
        break;
    }
    case ScriptFreeResource:
        if (args->kind == GSscript::VariantResource) {
            GSresID* resourceEntry = GetResource(args);
            FreeMapResource(resourceEntry);
        } else if (args->kind == GSscript::VariantInteger) {
            long resourceId = (args)->getInteger();
            FreeMapResource(resourceId);
        }
        break;
    case ScriptGlowStart:
        scriptGlowStart((args)->getSingle(), (u8)(args + 1)->getInteger(), (bool)(args + 2)->getInteger(),
                        (bool)(args + 3)->getInteger(), (bool)(args + 4)->getInteger(), (args + 5)->getInteger());
        break;
    case ScriptGlowEnd:
        scriptGlowEnd();
        break;
    case ScriptLensFlare:
        scriptSetLensFlare((args)->getInteger(), (bool)(args + 1)->getSingle(), (args + 2)->getSingle(),
                           (args + 3)->getSingle(), (args + 4)->getSingle());
        break;
    case ScriptResource: {
        resource.groupId = floorGetCurrentGroupID();
        resource.resourceId = (args)->getInteger();
        SetResource(result, FindResource(&resource));
        break;
    }
    case ScriptVsyncRate:
        SetSingle(result, GSgfxVideoGetVsyncRate());
        break;
    case ScriptCountryCode:
        result->setInteger(pokecoloGetCountryCode());
        break;
    case ScriptLanguage:
        result->setInteger(pokecoloGetLanguage());
        break;
    }
    return 0;
}

int AppScript::getExtendVariable(GSscript::tigaVariant* value, int variable, int index)
{
    enum {
        FirstPeopleVariable = 128,
        LastPeopleVariable = 288,
        FirstArrayVariable = 512,
        LastArrayVariable = 767
    };
    if (variable >= FirstPeopleVariable && variable <= LastPeopleVariable) {
        value->setPeople(&mPeople[variable - FirstPeopleVariable]);
        return 1;
    }
    if (variable >= FirstArrayVariable && variable <= LastArrayVariable) {
        int array = variable - FirstArrayVariable;
        if (array >= 0 && array < mArrayCount) {
            SetArray(value, mArrays[array]);
        } else {
            (value)->setInteger(0);
        }
        return 1;
    }
    return 0;
}

int AppScript::setExtendVariable(GSscript::tigaVariant* value, int variable, int index)
{
    return 0;
}

int AppScript::fncPrintf(GSscript::tigaVariant* args)
{
    char buffer[256] = {0};
    int length = 0;
    int argument = 1;
    char* format = GetString(args, mStringBuffer);
    for (; *format; ++format) {
        int character = *format;
        if (character == '%') {
            ++format;
            if (*format == 'd' || *format == 'D') {
                if (args[argument].kind == GSscript::VariantInteger)
                    length = sprintf(buffer, "%s%d", buffer, (&args[argument])->getInteger());
                else {
                    GSlogWrite("script error line[%d] : printf argument %d error \n", GSscript::Tiga::m_cpCurrentObject->mCurrentThread->lineNumber, argument);
                    return 0;
                }
            } else if (*format == 'f' || *format == 'F') {
                if (args[argument].kind == GSscript::VariantSingle)
                    length = sprintf(buffer, "%s%f", buffer, (&args[argument])->getSingle());
                else {
                    GSlogWrite("script error line[%d] : printf argument %d error \n", GSscript::Tiga::m_cpCurrentObject->mCurrentThread->lineNumber, argument);
                    return 0;
                }
            } else if (*format == 's' || *format == 'S') {
                if (args[argument].kind == GSscript::VariantString)
                    length = sprintf(buffer, "%s%s", buffer, GetString(&args[argument], mStringBuffer));
                else {
                    GSlogWrite("script error line[%d] : printf argument %d error \n", GSscript::Tiga::m_cpCurrentObject->mCurrentThread->lineNumber, argument);
                    return 0;
                }
            } else if (*format == 'c' || *format == 'C') {
                if (args[argument].kind == GSscript::VariantString)
                    length = sprintf(buffer, "%s%c", buffer, GetString(&args[argument], mStringBuffer));
                else {
                    GSlogWrite("script error line[%d] : printf argument %d error \n", GSscript::Tiga::m_cpCurrentObject->mCurrentThread->lineNumber, argument);
                    return 0;
                }
            } else if (*format == 'v' || *format == 'V') {
                if (args[argument].kind == GSscript::VariantVector) {
                    GSvec* vector = (&args[argument])->getVector();
                    length = sprintf(buffer, "%s x:%f y:%f z:%f", buffer, vector->x, vector->y, vector->z);
                } else {
                    GSlogWrite("script error line[%d] : printf argument %d error \n", GSscript::Tiga::m_cpCurrentObject->mCurrentThread->lineNumber, argument);
                    return 0;
                }
            } else if (*format == 'p' || *format == 'P') {
                if (args[argument].kind == GSscript::VariantPeople) {
                    GSresID* people = (&args[argument])->getPeople();
                    length = sprintf(buffer, "%s grpID:%d resID:%d", buffer, people->groupId, people->resourceId);
                } else {
                    GSlogWrite("script error line[%d] : printf argument %d error \n", GSscript::Tiga::m_cpCurrentObject->mCurrentThread->lineNumber, argument);
                    return 0;
                }
            }
            ++argument;
        } else if (character == '\\') {
            ++format;
            if (*format == 'n') {
                sprintf(buffer, "%s\n", buffer);
                GSlogWrite(buffer);
                for (int i = 0; i < 256; ++i)
                    buffer[i] = 0;
                length = 0;
            } else if (*format == 't') {
                length = sprintf(buffer, "%s\t", buffer);
            }
        } else {
            buffer[length] = character;
            ++length;
        }
    }
    if (buffer[length] != '\n' && length != 0) {
        buffer[length] = '\n';
        GSlogWrite(buffer);
    }
    return 0;
}

int AppScript::andFlagTest(GSscript::tigaVariant* args)
{
    int argument = 1;
    u8 matched = 1;
    char* tests = GetString(&args[0], mStringBuffer);
    for (; *tests; ++tests) {
        if (*tests == '0') {
            if ((u8)GSflagTest((&args[argument])->getInteger()) == 1)
                matched = 0;
            ++argument;
        } else if (*tests == '1') {
            if ((u8)GSflagTest((&args[argument])->getInteger()) == 0)
                matched = 0;
            ++argument;
        } else {
            return -1;
        }
    }
    return matched;
}

int AppScript::orFlagTest(GSscript::tigaVariant* args)
{
    int argument = 1;
    u8 matched = 0;
    char* tests = GetString(&args[0], mStringBuffer);
    for (; *tests; ++tests) {
        if (*tests == '0') {
            if ((u8)GSflagTest((&args[argument])->getInteger()) == 0)
                matched = 1;
            ++argument;
        } else if (*tests == '1') {
            if ((u8)GSflagTest((&args[argument])->getInteger()) == 1)
                matched = 1;
            ++argument;
        } else {
            return -1;
        }
    }
    return matched;
}

int AppScript::commonCall(GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    long script = args[0].getInteger();
    long count = (&args[1])->getInteger();
    floorGetCommonScript()->runParamsWait(script, &args[2], count, 0);
    return 1;
}

int AppScript::cmpPokemon(int cmd, GSscript::tigaVariant* args,
                          GSscript::tigaVariant* result)
{
    enum {
        PlaySpeciesVoice = 16, ForgetWaza, CopyWaza, PlayVoice,
        GetWazaCount, GetCatchBall, GetName,
        IsDarkPokemon = 24, GetWazaId, GetHp, GetDarkPoints, GetSpecies,
        IsLegend, GetFriend, GetSpeciesName, GetItem, GetCatchTrainerRnd,
        SetWaza, RememberWaza, CanLearnTutorWaza, OpenWazaLearn,
        OpenWazaLearnList
    };
    Pokemon* pokemon;
    if (cmd != PlaySpeciesVoice) {
        if (args->kind != GSscript::VariantPokemon) {
            result->setInteger(0);
            return 1;
        }
        pokemon = GetPokemon(args);
    }
    switch (cmd) {
    case PlaySpeciesVoice:
        scriptPlayPokemonNakigoe((u16)(args + 1)->getInteger());
        break;
    case PlayVoice:
        pokemonPlayVoice(pokemon);
        break;
    case CopyWaza: {
        long destination = (args + 1)->getInteger();
        long source = (args + 2)->getInteger();
        pokemonWazaCopy(pokemon, (u16)destination, (u16)source);
        break;
    }
    case ForgetWaza:
        pokemonWazaForget(pokemon, (args + 1)->getInteger());
        break;
    case GetWazaCount:
        result->setInteger(pokemonGetWazaCount(pokemon));
        break;
    case GetCatchBall:
        result->setInteger(pokemonBiosGetCatchBallId(pokemon));
        break;
    case GetName: {
        u16* name = pokemon_GetName(pokemon);
        result->kind = GSscript::VariantFunctionPointer;
        result->object = name;
        break;
    }
    case IsDarkPokemon:
        result->setInteger(pokemonBiosGetDarkFlag(pokemon));
        break;
    case GetWazaId:
        result->setInteger(pokemonBiosGetPokemonWazaDataId(pokemon, (u16)(args + 1)->getInteger()));
        break;
    case GetHp:
        result->setInteger(pokemonBiosGetHp(pokemon));
        break;
    case GetDarkPoints:
        result->setInteger(pokemonBiosGetDp(pokemon));
        break;
    case GetSpecies:
        result->setInteger(pokemonBiosGetPokemonDataId(pokemon));
        break;
    case IsLegend:
        result->setInteger(pokemon->isLegend());
        break;
    case GetFriend:
        result->setInteger(pokemonBiosGetFriend(pokemon));
        break;
    case GetSpeciesName:
        result->setInteger(pokemonDB_GetName(pokemonBiosGetPokemonDataId(pokemon)));
        break;
    case GetItem:
        result->setInteger(pokemonBiosGetItemDataId(pokemon));
        break;
    case GetCatchTrainerRnd:
        result->setInteger(pokemonBiosGetCatchTrainerRnd(pokemon));
        break;
    case SetWaza: {
        long wazaId = (args + 1)->getInteger();
        bool replace = (args + 2)->getInteger() != 0;
        result->setInteger(pokemonSetWazaStatus(pokemon, (u16)wazaId, replace));
        break;
    }
    case RememberWaza:
        result->setInteger(scriptPokemonRememberWaza(pokemon, (args + 1)->getInteger()));
        break;
    case CanLearnTutorWaza:
        result->setInteger(pokemonDB_GetTeachWaza(pokemonBiosGetPokemonDataId(pokemon),
            (u16)scriptPokemonTeachWazaIndex((args + 1)->getInteger())));
        break;
    case OpenWazaLearn:
        result->setInteger(menuPokemonStatusOpenWazaLearn(pokemon, (u16)(args + 1)->getInteger()));
        break;
    case OpenWazaLearnList:
        result->setInteger(menuPokemonStatusOpenWazaLearnList(pokemon, (u16)(args + 1)->getInteger()));
        break;
    }
    return 0;
}

int AppScript::cmpTask(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { CallInteger = 16, ExecIntegerThread, CallIntegerByName,
           ExecIntegerByName, GetIntegerResult, WaitSeconds };
    switch (cmd) {
    case CallInteger: {
        long scriptId = args[1].getInteger();
        long parameter1 = args[2].getInteger();
        long parameter2 = args[3].getInteger();
        long parameter3 = args[4].getInteger();
        long parameter4 = args[5].getInteger();
        floorExecScriptInteger(scriptId, parameter1, parameter2, parameter3, parameter4);
        break;
    }
    case ExecIntegerThread: {
        long scriptId = args[1].getInteger();
        long parameter1 = args[2].getInteger();
        long parameter2 = args[3].getInteger();
        long parameter3 = args[4].getInteger();
        long parameter4 = args[5].getInteger();
        floorExecScriptIntegerThread(scriptId, parameter1, parameter2, parameter3, parameter4);
        break;
    }
    case CallIntegerByName: {
        char* name = GetString(&args[1], mStringBuffer);
        long parameter1 = args[2].getInteger();
        long parameter2 = args[3].getInteger();
        long parameter3 = args[4].getInteger();
        long parameter4 = args[5].getInteger();

        floorExecScriptIntegerByName(name, parameter1, parameter2, parameter3, parameter4);
        break;
    }
    case ExecIntegerByName: {
        char* name = GetString(&args[1], mStringBuffer);
        long parameter1 = args[2].getInteger();
        long parameter2 = args[3].getInteger();
        long parameter3 = args[4].getInteger();
        long parameter4 = args[5].getInteger();

        floorExecScriptIntegerByNameThread(name, parameter1, parameter2, parameter3, parameter4);
        break;
    }
    case GetIntegerResult:
        result->setInteger(floorGetScriptResult());
        break;
    case WaitSeconds:
        scriptWaitTime(args[1].getSingle());
        break;
    }
    return 0;
}

int AppScript::cmpFade(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { Set = 16, Check };
    switch (cmd) {
    case Set: {
        long fade = args[1].getInteger();
        float duration = args[2].getSingle();
        fadeSet((u16)fade, duration);
        break;
    }
    case Check:
        result->setInteger(fadeCheck(args[1].getInteger() != 0));
        break;
    }
    return 0;
}

int AppScript::cmpFight(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { ScriptEncounter = 16, Encounter, GetResult, PlaySound,
           PlayEnvironmentSound, EncounterByShadowStatus, GetEncounter,
           SetEncounterId, GetEncounterId, GetMessage, SetMessage,
           GetEncounterParameter, GetEncounterParameterB, GetEncounterParameterC,
           SetEncounterParameters, GetSwapMode, GetSwapDeck, SetSwap,
           GetSwapDeckAgain, SetEncounterMode, GetEncounterOption,
           SetEncounterOption, GetEncounterOptionB, SetEncounterOptionB,
           RecoverAnnihilation, GetEncounterValue, SetEncounterValue };
    switch (cmd) {
    case ScriptEncounter: {
        result->setInteger(scriptEncount((u16)args[1].getInteger(), args[2].getInteger(), args[3].getInteger()));
        break;
    }
    case Encounter:
        result->setInteger(fightEncount((u16)args[1].getInteger()));
        break;
    case GetResult:
        result->setInteger(fightGetFightResultId());
        break;
    case PlaySound:
        fightSoundOpenPlay(args[1].getInteger());
        break;
    case PlayEnvironmentSound:
        fightSoundOpenPlayEnv(args[1].getInteger());
        break;
    case EncounterByShadowStatus: {
        DarkPokemon* pokemon = darkPokemonGetDarkPokemon((u16)args[1].getInteger());
        if (pokemon->getStatus() > 2)
            fightEncount((u16)args[2].getInteger());
        else
            fightEncount((u16)args[3].getInteger());
        break;
    }
    case GetEncounter:
        result->object = fightEncountGetStrObj();
        break;
    case SetEncounterId: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setField((u16)args[1].getInteger());
        break;
    }
    case GetEncounterId:
        result->setInteger(fightEncountGetStrObj()->getField());
        break;
    case GetMessage: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        result->setInteger(encounter->getMsg((u8)args[1].getInteger()));
        break;
    }
    case SetMessage: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        long index = args[1].getInteger();
        long message = args[2].getInteger();
        encounter->setMsg((u8)index, (u16)message);
        break;
    }
    case GetEncounterParameter:
        result->setInteger(fightEncountGetStrObj()->getAddMode());
        break;
    case GetEncounterParameterB:
        result->setInteger(fightEncountGetStrObj()->getAddPlace());
        break;
    case GetEncounterParameterC:
        result->setInteger(fightEncountGetStrObj()->getAddID());
        break;
    case SetEncounterParameters: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setAddMode((u8)args[1].getInteger());
        encounter->setAddPlace((u8)args[2].getInteger());
        encounter->setAddID((u8)args[3].getInteger());
        break;
    }
    case GetSwapMode:
        result->setInteger(fightEncountGetStrObj()->getSwapMode());
        break;
    case GetSwapDeck:
        result->setInteger(fightEncountGetStrObj()->getSwapDeckNO());
        break;
    case SetSwap: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setSwapMode((u8)args[1].getInteger());
        encounter->setSwapDeckNO((u8)args[2].getInteger());
        break;
    }
    case GetSwapDeckAgain:
        result->setInteger(fightEncountGetStrObj()->getSwapDeckNO());
        break;
    case SetEncounterMode: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setFightMode((u8)args[1].getInteger());
        break;
    }
    case GetEncounterOption:
        result->setInteger(fightEncountGetStrObj()->getDestroyFlag());
        break;
    case SetEncounterOption: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setDestroyFlag((u8)args[1].getInteger());
        break;
    }
    case GetEncounterOptionB:
        result->setInteger(fightEncountGetStrObj()->getTrainerKind());
        break;
    case SetEncounterOptionB: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setTrainerKind((u8)args[1].getInteger());
        break;
    }
    case RecoverAnnihilation:
        fightEncountAnnihilationRecovery();
        break;
    case GetEncounterValue:
        result->setInteger(fightEncountGetStrObj()->getRoundNo());
        break;
    case SetEncounterValue: {
        FIGHT_ENCOUNT* encounter = fightEncountGetStrObj();
        if (encounter->getDirtyFlag() == true)
            encounter->clear();
        encounter->setRoundNo(args[1].getInteger());
        break;
    }
    }
    return 0;
}

int AppScript::cmpLight(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { SetDisplay = 16, SetMotion, StopMotion, WaitMotion, SetType,
           SetPosition, SetColor, GetPosition };
    switch (cmd) {
    case SetDisplay: {
        long lightId = args[1].getInteger();
        long display = args[2].getInteger();
        scriptLightSetDisp(lightId, display);
        break;
    }
    case SetMotion: {
        long lightId = args[1].getInteger();
        long motion = args[2].getInteger();
        long mode = args[3].getInteger();
        long parameter = args[4].getInteger();
        scriptLightSetMotion(lightId, motion, mode, parameter);
        break;
    }
    case StopMotion:
        scriptLightStopMotion(args[1].getInteger());
        break;
    case WaitMotion: {
        long lightId = args[1].getInteger();
        long motion = args[2].getInteger();
        result->setInteger(scriptLighgtWaitMotion(lightId, motion));
        break;
    }
    case SetType: {
        GSlight* light = lightGetLight(args[1].getInteger());
        long type = args[2].getInteger();
        GSlightSetType(light, type);
        break;
    }
    case SetPosition: {
        GSlight* light = lightGetLight(args[1].getInteger());
        GSlightSetPosition(light, (args + 2)->getVector());
        break;
    }
    case SetColor: {
        GSlight* light = lightGetLight(args[1].getInteger());
        GXColor color;
        color.r = (u8)args[2].getInteger();
        color.g = (u8)args[3].getInteger();
        color.b = (u8)args[4].getInteger();
        color.a = (u8)args[5].getInteger();
        GSlightSetColor(light, color);
        break;
    }
    case GetPosition: {
        GSlight* light = lightGetLight(args[1].getInteger());
        GSvec position;
        GSlightGetPosition(light, &position);
        setRpVec(&position);
        break;
    }
    }
    return 0;
}

void _appScriptModelSetVisibility(GSmodel* model, void* visibility)
{
    GSmodelSetVisibility(model, (bool)visibility);
}

int AppScript::cmpModel(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { Open = 16, Close, SetDisplay, SetMotion, StopMotion, WaitMotion,
           Attach, Detach, SetParticle, DestroyParticleGenerators, GetPartPosition,
           AddPosition, SetAllVisibility, SetMotionRate, SetBoundCheck,
           SetEndedMotion, SetPartParticle, SetFractionalFrames, RecalculateBound,
           UseRootJointAnimation };
    switch (cmd) {
    case Open:
        result->setInteger(scriptModelOpen(args[1].getInteger()));
        break;
    case Close:
        scriptModelClose(args[1].getInteger());
        break;
    case SetDisplay: {
        long modelId = args[1].getInteger();
        long display = args[2].getInteger();
        scriptModelSetDisp(modelId, display);
        break;
    }
    case SetMotion: {
        long modelId = args[1].getInteger();
        long motion = args[2].getInteger();
        long mode = args[3].getInteger();
        long parameter = args[4].getInteger();
        scriptModelSetMotion(modelId, motion, mode, parameter);
        break;
    }
    case StopMotion:
        scriptModelStopMotion(args[1].getInteger());
        break;
    case WaitMotion: {
        long modelId = args[1].getInteger();
        long motion = args[2].getInteger();
        result->setInteger(scriptModelWaitMotion(modelId, motion));
        break;
    }
    case Attach: {
        long modelId = args[1].getInteger();
        long parentId = args[2].getInteger();
        long part = args[3].getInteger();
        result->setInteger(scriptModelAttach(modelId, parentId, part));
        break;
    }
    case Detach:
        result->setInteger(scriptModelDetach(args[1].getInteger()));
        break;
    case SetParticle: {
        long modelId = args[1].getInteger();
        long particle = args[2].getInteger();
        long mode = args[3].getInteger();
        scriptModelSetParticle(modelId, particle, (u8)mode);
        break;
    }
    case DestroyParticleGenerators: {
        long modelId = args[1].getInteger();
        long enabled = args[2].getInteger();
        scriptModelDeleteParticle(modelId, enabled);
        break;
    }
    case GetPartPosition: {
        long modelId = args[1].getInteger();
        GSmodel* model = (GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelId);
        if (!model)
            return 0;
        long partId = args[2].getInteger();
        GSpart* part = GSmodelGetPart(model, partId);
        GSvec position;
        GSpartGetTransform(part, &position, 0, 0);
        GSpartFree(part);
        setRpVec(&position);
        break;
    }
    case AddPosition: {
        long modelId = args[1].getInteger();
        GSmodel* model = (GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelId);
        if (!model)
            return 0;
        GSmodelAddPosition(model, (args + 2)->getVector());
        break;
    }
    case SetAllVisibility:
        GSmodelIterateList(_appScriptModelSetVisibility, (void*)args[1].getInteger());
        break;
    case SetMotionRate: {
        long modelId = args[1].getInteger();
        long motion = args[2].getInteger();
        long mode = args[3].getInteger();
        float rate = args[4].getSingle();
        long parameter = args[5].getInteger();
        scriptModelSetMotionRate(modelId, motion, mode, rate, parameter);
        break;
    }
    case SetBoundCheck: {
        long modelId = args[1].getInteger();
        GSmodel* model = (GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelId);
        long enabled = args[2].getInteger();
        GSmodelSetBoundCheck(model, (bool)enabled);
        break;
    }
    case SetEndedMotion: {
        long modelId = args[1].getInteger();
        long motion = args[2].getInteger();
        long enabled = args[3].getInteger();
        result->setInteger(scriptModelSetEndedMotion(modelId, motion, enabled));
        break;
    }
    case SetPartParticle: {
        long modelId = args[1].getInteger();
        long part = args[2].getInteger();
        long particle = args[3].getInteger();
        scriptModelPartPaticle(modelId, part, particle);
        break;
    }
    case SetFractionalFrames: {
        long modelId = args[1].getInteger();
        long enabled = args[2].getInteger();
        scriptModelSetFractionalFramesFlag(modelId, enabled);
        break;
    }
    case RecalculateBound: {
        long modelId = args[1].getInteger();
        GSmodelRecalculateBound((GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelId));
        break;
    }
    case UseRootJointAnimation: {
        long modelId = args[1].getInteger();
        GSmodelUseRootJointAnimation((GSmodel*)GSresGetResource(floorGetCurrentGroupID(), modelId), true);
        break;
    }
    }
    return 0;
}

int AppScript::cmpCol(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant*)
{
    enum { SetCollision = 16, LoadWarpIndex, SetWarpCollision };
    switch (cmd) {
    case SetCollision: {
        long collision = args[1].getInteger();
        long enabled = args[2].getInteger();
        scriptSetCol(collision, enabled);
        break;
    }
    case LoadWarpIndex:
        scriptGetEventColID();
        break;
    case SetWarpCollision:
        scriptSetEventCol(args[1].getInteger());
        break;
    }
    return 0;
}

int AppScript::cmpSound(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    const long SoundVolumeMask = 0xff;
    enum { OpenPlay = 16, OpenPlayEnvironment, Stop, StopEnvironment,
           CreateEmitter, CreateListener, SetVolume, ScriptOpenPlayME,
           SetOriginalVolume, OpenPlayFade, OpenPlayEnvironmentFade,
           GetBGMId, MaskMessageBGM, SetMessageBGMFadeSpeed, SetMessageBGMVolumeRate,
           PlayWait, PlayME, PlayMEThread };
    switch (cmd) {
    case OpenPlay:
        result->setInteger((u8)soundOpenPlay(args[1].getInteger(), args[2].getInteger(),
                                             (args[3].getInteger() & SoundVolumeMask)));
        break;
    case OpenPlayEnvironment:
        result->setInteger((u8)soundOpenPlayEnv(args[1].getInteger(), args[2].getInteger(),
                                                (args[3].getInteger() & SoundVolumeMask)));
        break;
    case Stop:
        result->setInteger((u8)soundStop(args[1].getInteger(), args[2].getInteger()));
        break;
    case StopEnvironment:
        result->setInteger((u8)soundStopEnv(args[1].getInteger()));
        break;
    case CreateEmitter:
        result->setInteger((u8)soundCreateEmitter(args[1].getInteger(), args[2].getSingle(),
                                                  args[3].getSingle(), args[4].getSingle()));
        break;
    case CreateListener:
        result->setInteger((u8)soundCreateListener());
        break;
    case SetVolume:
        result->setInteger((u8)soundSetVolume(args[1].getInteger(), args[2].getInteger()));
        break;
    case ScriptOpenPlayME:
        result->setInteger(scriptSoundOpenPlayME(args[1].getInteger(), args[2].getInteger(),
                                                 (u8)args[3].getInteger()));
        break;
    case SetOriginalVolume:
        result->setInteger((u8)soundSetVolumeID(args[1].getInteger(), args[2].getInteger(),
                                                args[3].getInteger()));
        break;
    case OpenPlayFade:
        result->setInteger((u8)soundOpenPlayFadeOutIn(args[1].getInteger(), args[2].getInteger(),
                                                      args[3].getInteger(), (args[4].getInteger() & SoundVolumeMask)));
        break;
    case OpenPlayEnvironmentFade:
        result->setInteger((u8)soundOpenPlayEnvFadeOutIn(args[1].getInteger(), args[2].getInteger(),
                                                         args[3].getInteger(), (args[4].getInteger() & SoundVolumeMask)));
        break;
    case GetBGMId:
        result->setInteger(soundGetBGMID());
        break;
    case MaskMessageBGM:
        peopleTalkMessageBgmMask(args[1].getInteger());
        break;
    case SetMessageBGMFadeSpeed:
        peopleTalkMessageBgmSetFadeSpeed(args[1].getSingle(), args[2].getSingle());
        break;
    case SetMessageBGMVolumeRate:
        peopleTalkMessageBgmSetVolumeRate(args[1].getSingle());
        break;
    case PlayWait:
        soundPlayWait(args[1].getInteger());
        break;
    case PlayME:
        result->setInteger((u8)soundPlayME(args[1].getInteger(), args[2].getInteger(),
                                           (args[3].getInteger() & SoundVolumeMask)));
        break;
    case PlayMEThread: {
        GSthread* thread = floorCreateThread(ScriptThreadPriority, 0, ScriptThreadFlags,
                                            (void (*)())appScriptPlayME_Thread);
        GSthreadSetArgs(thread, 4, args[1].getInteger(), args[2].getInteger(),
                        args[3].getInteger(), args[4].getInteger());
        break;
    }
    }
    return 0;
}

int motorFun(u8 motor, u8 strength, unsigned long duration, u8 mode, bool enabled)
{
    unsigned long remaining = duration;
    if (!enabled)
        return 0;
    while (remaining) {
        GSinputMotorStart(ScriptController, motor, strength, duration, mode);
        GSthreadSwitch();
        --remaining;
    }
    return 0;
}

int AppScript::cmpPad(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { IsButtonPush = 16, IsMoveButtonPush, IsTriggerPush, StartMotor, GetButtonsPressed };
    switch (cmd) {
    case IsButtonPush: {
        result->setInteger(scriptIsButtonPush());
        break;
    }
    case IsMoveButtonPush: {
        result->setInteger(scriptIsMoveButtonPush());
        break;
    }
    case IsTriggerPush: {
        result->setInteger(scriptIsTrigerPush());
        break;
    }
    case StartMotor: {
        const long MotorDisabledOption = 9;
        if ((long)gamedatasaveGetStatus(0, MotorDisabledOption) == 0) {
            GSthread* thread = floorCreateThread(ScriptThreadPriority, 0, ScriptThreadFlags,
                                                (void (*)())motorFun);
            long motor = args[1].getInteger();
            long strength = args[2].getInteger();
            long duration = args[3].getInteger();
            long mode = args[4].getInteger();
            GSthreadSetArgs(thread, 5, motor, strength, duration, mode, 1);
        }
        break;
    }
    case GetButtonsPressed: {
        result->setInteger(GSinputButtonsPressed(ScriptController));
        break;
    }
    }
    return 0;
}

int AppScript::cmpMail(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { Send = 16, ClearMemoFlag, Receive, MailAction, ReceiveAndSend };
    switch (cmd) {
    case Send: {
        mailMainSendByScrpt((u16)args[1].getInteger());
        break;
    }
    case ClearMemoFlag: {
        result->setInteger(scriptSetMemoFlag());
        break;
    }
    case Receive: {
        menuScriptReciveMail();
        break;
    }
    case MailAction: {
        menuScriptReadMail(args[1].getInteger());
        break;
    }
    case ReceiveAndSend: {
        menuScriptReciveMail();
        mailMainSendByScrpt((u16)args[1].getInteger());
        menuScriptReadMail(args[1].getInteger());
        break;
    }
    }
    return 0;
}

int AppScript::cmpDirection(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { Evolve = 16, SetReliveParameters, Relive, DisplayShrineParticle,
           UseCologne, GetMiraboPlace, RemoveMirabo };
    switch (cmd) {
    case Evolve:
        evolution();
        break;
    case SetReliveParameters: {
        long mode = args[1].getInteger();
        long parameter = args[2].getInteger();
        long wait = args[3].getInteger();
        preReliveSetParameter(mode, parameter, wait);
        break;
    }
    case Relive:
        reliveMain();
        break;
    case DisplayShrineParticle:
        preReliveDisplayHokoraParticle();
        break;
    case UseCologne: {
        long pokemon = args[1].getInteger();
        long item = args[2].getInteger();
        result->setInteger(menuUseItemScriptCologn(pokemon, item));
        break;
    }
    case GetMiraboPlace:
        result->setInteger(underTakerGetMiraboPlace());
        break;
    case RemoveMirabo:
        underTakerMiraboDisAppear();
        break;
    }
    return 0;
}

int AppScript::cmpTv(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { Control = 16, ControlScreen };
    switch (cmd) {
    case Control:
        result->setInteger(controlTV(args[1].getInteger()));
        break;
    case ControlScreen:
        result->setInteger(controlTVScreen());
        break;
    }
    return 0;
}

int AppScript::cmpSodateya(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { GetLevelUpValue = 17, GetDPValue, Raise, Reclaim, Reserved,
           CalculatePrice, CheckRaising, GetPokemon };
    switch (cmd) {
    case GetLevelUpValue:
        result->setInteger(sodateyaGetPokemonLevelUpValue());
        break;
    case GetDPValue:
        result->setInteger(sodateyaGetPokemonDPValue());
        break;
    case Raise:
        sodateyaRaise(args[1].getInteger());
        break;
    case Reclaim:
        sodateyaReclaim(args[1].getInteger());
        break;
    case Reserved:
        break;
    case CalculatePrice:
        result->setInteger(sodateyaCalcPrice(args[1].getInteger()));
        break;
    case CheckRaising:
        result->setInteger(sodateyaeChekcRais());
        break;
    case GetPokemon:
        Pokemon* pokemon = sodateyaGetPokemonPtr(0);
        result->kind = GSscript::VariantPokemon;
        result->object = pokemon;
        break;
    }
    return 0;
}

int AppScript::cmpWaza(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { Allocate = 16, Start, SetPosition, SetRotation, SetScale, Wait, Free };
    switch (cmd) {
    case Allocate: {
        GSscript::tigaWaza* waza = allocWazaPtr();
        if (waza) {
            waza->sequence = Sequence_CreateNull();
            result->kind = GSscript::VariantWaza;
            result->waza = waza;
            ++mLiveWaza;
        }
        break;
    }
    case Start: {
        GSscript::tigaWaza* waza = GetWaza(args);
        if (!waza)
            break;
        if (!waza->wazaSequence) {
            long wazaId = args[1].getInteger();
            long actor = args[2].getInteger();
            long target = args[3].getInteger();
            waza->wazaSequence = Sequence_LoadWaza(waza->sequence, (u16)wazaId,
                                                 (u8)actor, (u8)target);
        }
        WazaSequence_Start(waza->wazaSequence, 0);
        if (args[4].getInteger() == 1)
            WaitWaza(waza);
        break;
    }
    case SetPosition: {
        GSscript::tigaWaza* waza = GetWaza(args);
        if (waza)
            Sequence_SetPosition(waza->sequence, (args + 1)->getVector());
        break;
    }
    case SetRotation: {
        GSscript::tigaWaza* waza = GetWaza(args);
        if (waza) {
            GSvec* rotation = (args + 1)->getVector();
            rotation->x = appScriptDegreesToRadians * rotation->x;
            rotation->y = appScriptDegreesToRadians * rotation->y;
            rotation->z = appScriptDegreesToRadians * rotation->z;
            Sequence_SetRotation(waza->sequence, rotation);
        }
        break;
    }
    case SetScale: {
        GSscript::tigaWaza* waza = GetWaza(args);
        if (waza)
            Sequence_SetScale(waza->sequence, (args + 1)->getVector());
        break;
    }
    case Wait: {
        GSscript::tigaWaza* waza = GetWaza(args);
        if (waza)
            WaitWaza(waza);
        break;
    }
    case Free: {
        GSscript::tigaWaza* waza = GetWaza(args);
        if (waza) {
            if (waza->wazaSequence)
                WazaSequence_Free(waza->wazaSequence);
            if (waza->sequence)
                Sequence_Free(waza->sequence);
            freeWazaPtr(waza);
        }
        --mLiveWaza;
        break;
    }
    }
    return 0;
}

int AppScript::cmpDarkPokemon(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { IsPurified = 16, IsSnagged, SetStatus, GetSpecies, GetStatus,
           RegisterCapture, SetPlace };
    Hero* trainer = (Hero*)savedataGetStatus(0, 2);
    DarkPokemon* pokemon = darkPokemonGetDarkPokemon((u16)args[1].getInteger());
    if (!pokemon) {
        result->setInteger(0);
        return 1;
    }
    switch (cmd) {
    case IsPurified:
        if (pokemon->getStatus() == DARK_POKEMON_STATUS_RELIVED)
            result->setInteger(1);
        else
            result->setInteger(0);
        break;
    case IsSnagged:
        if (pokemon->getStatus() >= DARK_POKEMON_SNATCHED)
            result->setInteger(1);
        else
            result->setInteger(0);
        break;
    case SetStatus:
        pokemon->setStatus((darkPokemonStatus)args[2].getInteger());
        break;
    case GetSpecies:
        result->setInteger(pokemon->getPokemonID());
        break;
    case GetStatus:
        result->setInteger(pokemon->getStatus());
        break;
    case RegisterCapture: {
        u8 order = trainer->getMeetDarkPokemonCount();
        trainer->setMeetDarkPokemonCount((u8)(order + 1));
        pokemon->setSnatchIndex(order);
        pokemon->addStatus(DARK_POKEMON_STATUS_FIGHT);
        const u16 CaptureFloor = 181;
        pokemon->setCatchFloorId(CaptureFloor);
        pokemon->setUpdate(true);
        break;
    }
    case SetPlace: {
        long place = args[2].getInteger();
        long index = args[3].getInteger();
        pokemon->setPlace((darkPokemonWhere)place, (u8)index);
        break;
    }
    }
    return 0;
}

int AppScript::cmpEsaba(int cmd, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    enum { SetFood = 16, GetPokemonId, SetFoodCount, GetFoodCount, IsRarePokemon };
    switch (cmd) {
    case SetFood: {
        GSthreadSwitch();
        result->setInteger(esabaSet(args[1].getInteger()));
        if (result->getInteger()) {
            const long FoodPlacedSound = 1398;
            soundOpenPlay(FoodPlacedSound, 0, 255);
        }
        break;
    }
    case GetPokemonId: {
        result->setInteger(esabaGetPokemonID(args[1].getInteger()));
        break;
    }
    case SetFoodCount: {
        long foodId = args[1].getInteger();
        long count = args[2].getInteger();
        esabaSetEsaNum(foodId, count);
        break;
    }
    case GetFoodCount: {
        result->setInteger(esabaGetEsaNum(args[1].getInteger()));
        break;
    }
    case IsRarePokemon: {
        result->setInteger(esabaGetPokemonIsRare(args[1].getInteger()));
    }
    }
    return 0;
}

extern "C" int appScriptCommonCall(int scriptId)
{
    GSscript::Tiga* common = floorGetCommonScript();
    return common->runParamsWait(scriptId, 0, 0, 0);
}

extern "C" void appScriptPlayME_Thread(u8 open, int soundId, int time, int volume)
{
    if (open)
        soundOpenPlayME(soundId, time, volume);
    else
        soundPlayME(soundId, time, volume);
}
