#include <types.h>
#include <AppScript.h>
#include <GSscript.h>

/* The script engine addresses the game's interface screens through this
   dispatcher; every method below is one script menu command. */

#include <winMsg.h>
#include <menu.h>
#include <menuSub.h>
#include <msgctrl.h>
#include <GSmsg.h>
#include <GSthread.h>
#include <menuPokemon.h>
#include <menuPokemonChange.h>
#include <menuPocket2.h>
#include <menuPokemonStatus.h>
#include <menuNameEntry.h>
#include <menuOrre.h>
#include <menuScript.h>
#include <script.h>
#include <menuShop.h>
#include <menuPda.h>
#include <menuPanel.h>
#include <menuMewWaza.h>
#include <people.h>

// Values returned to the script interpreter: continue the script, or stop
// this pass through the interpreter.
enum MenuMethodResult { MENU_METHOD_CONTINUE = 0, MENU_METHOD_STOP_PASS = 1 };

// Reads the Pokemon a message substitution refers to.
static inline Pokemon* GetPokemon(GSscript::tigaVariant* value)
{
    if (value->kind == GSscript::VariantPokemon)
        return (Pokemon*)value->object;
    GSscript::tigaErrMessage("pokemon not access");
    return 0;
}

// Reads a message-text substitution, converting other kinds into the script
// string buffer.
static inline const void* GetMessageText(GSscript::tigaVariant* value, char* buffer)
{
    if (value->kind == GSscript::VariantFunctionPointer)
        return value->object;
    return value->getCnvString(buffer);
}

enum { MENU_SELECT_ENTRY_CAPACITY = 128 };

const u8 NUMBER_INPUT_ACCEPTED = 1;

int AppScript::cmpMenu(int methodId, GSscript::tigaVariant* args, GSscript::tigaVariant* result)
{
    // Message IDs for the selection window choices.
    u32 messageIDs[MENU_SELECT_ENTRY_CAPACITY];

    enum MenuCommand {
        MENU_COMMAND_OpenFieldMessage = 16,
        MENU_COMMAND_OpenFieldMessageWithSound = 17,
        MENU_COMMAND_CloseFieldMessage = 18,
        MENU_COMMAND_OpenFightMessage = 19,
        MENU_COMMAND_CloseFightMessage = 20,
        MENU_COMMAND_OpenYesNoField = 21,
        MENU_COMMAND_OpenYesNo = 22,
        MENU_COMMAND_OpenColosseumEnter = 23,
        MENU_COMMAND_Open = 24,
        MENU_COMMAND_Close = 25,
        MENU_COMMAND_GetCursor = 26,
        MENU_COMMAND_SetCursor = 27,
        MENU_COMMAND_SetMessageValue = 28,
        MENU_COMMAND_OpenMessageSelection = 29,
        MENU_COMMAND_SelectPartySlot = 32,
        MENU_COMMAND_SelectPartySlotAlt = 33,
        MENU_COMMAND_RunPokemonStatus = 34,
        MENU_COMMAND_OpenPokemonMoveList = 35,
        MENU_COMMAND_OpenNameEntry = 36,
        MENU_COMMAND_RunNameEntry = 37,
        MENU_COMMAND_OpenShop = 39,
        MENU_COMMAND_OpenPda = 41,
        MENU_COMMAND_OpenPokemonExchange = 42,
        MENU_COMMAND_OpenItemMenu = 50,
        MENU_COMMAND_OpenNumberInput = 56,
        MENU_COMMAND_OpenTutorial = 57,
        MENU_COMMAND_OpenEevee = 58,
        MENU_COMMAND_OpenPositionedSelection = 59,
        MENU_COMMAND_OpenWorldMap = 60,
        MENU_COMMAND_OpenUnderTakerMenu = 61,
        MENU_COMMAND_CloseUnderTakerMenu = 62,
        MENU_COMMAND_OpenUsohachi = 63,
        MENU_COMMAND_SelectMeijinMove = 64,
        MENU_COMMAND_SelectBasanMove = 65,
        MENU_COMMAND_SetMoveLearnedFlag = 66,
        MENU_COMMAND_OpenMoney = 67,
        MENU_COMMAND_CloseMoney = 68,
        MENU_COMMAND_CheckBasanMove = 69,
        MENU_COMMAND_OpenCoupon = 70,
        MENU_COMMAND_CloseCoupon = 71,
        MENU_COMMAND_OpenOrreColosseum = 72,
        MENU_COMMAND_OpenUsohachi2 = 73,
        MENU_COMMAND_GetOrreColosseumResult = 74,
        MENU_COMMAND_AddPdaFunction = 75,
        MENU_COMMAND_AddPocketItemScript = 76,
        MENU_COMMAND_RunMewMoveSequence = 77,
        MENU_COMMAND_GetMewMove = 78,
        MENU_COMMAND_OpenMoveTutor = 79,
    };

    switch (methodId) {
    // Field message window.
    case MENU_COMMAND_OpenFieldMessage:
        result->setInteger(winMsgOpenField(args[1].getInteger(), args[2].getInteger(),
                                           args[3].getInteger()));
        break;
    case MENU_COMMAND_OpenFieldMessageWithSound:
        result->setInteger(winMsgOpenFieldWithSE(args[1].getInteger(), args[2].getInteger(),
                                                 args[3].getInteger(),
                                                 args[4].getInteger()));
        break;
    case MENU_COMMAND_CloseFieldMessage:
        winMsgCloseField(args[1].getInteger());
        break;
    case MENU_COMMAND_OpenFightMessage:
        result->setInteger(winMsgOpenFight(args[1].getInteger(), args[2].getInteger(),
                                           args[3].getInteger()));
        break;
    case MENU_COMMAND_CloseFightMessage:
        winMsgCloseFight(args[1].getInteger());
        break;
    // Selections and menus.
    case MENU_COMMAND_OpenYesNoField:
        result->setInteger(menuSubOpenYesNoField());
        break;
    case MENU_COMMAND_OpenYesNo:
        result->setInteger(menuSubOpenYesNo(args[1].getInteger(), args[2].getInteger(),
                                            args[3].getInteger(), args[4].getInteger()));
        break;
    case MENU_COMMAND_OpenColosseumEnter:
        result->setInteger(menuSubOpenColosseumEnter(args[1].getInteger()));
        break;
    case MENU_COMMAND_Open:
        result->setInteger(menuOpen(args[1].getInteger(), args[2].getInteger() != 0));
        break;
    case MENU_COMMAND_Close:
        menuClose(args[1].getInteger());
        break;
    case MENU_COMMAND_GetCursor:
        result->setInteger(menuGetCursor(args[1].getInteger()));
        break;
    case MENU_COMMAND_SetCursor:
        result->setInteger(menuSetCursor(args[1].getInteger(), args[2].getInteger()));
        break;
    // Message substitution values.
    case MENU_COMMAND_SetMessageValue:
        if (args[1].getInteger() == MSGCTRL_VALUE_MESSAGE_STRING)
            msgctrlSetValue(MSGCTRL_VALUE_MESSAGE_STRING, (unsigned long)GSmsgGetStringPtr(args[2].getInteger()));
        else if (args[2].kind == GSscript::VariantPokemon)
            msgctrlSetValue(args[1].getInteger(), (unsigned long)GetPokemon(&args[2]));
        else if (args[2].kind == GSscript::VariantInteger)
            msgctrlSetValue(args[1].getInteger(), args[2].getInteger());
        else if (args[2].kind == GSscript::VariantFunctionPointer)
            msgctrlSetValue(args[1].getInteger(), (unsigned long)GetMessageText(&args[2], mStringBuffer));
        else if (args[2].kind == GSscript::VariantSingle)
            msgctrlSetValue(args[1].getInteger(), args[2].getSingle());
        break;
    // Selection list built from an array of message ids.
    case MENU_COMMAND_OpenMessageSelection: {
        if (args[2].getInteger() >= MENU_SELECT_ENTRY_CAPACITY) {
            result->setInteger(0);
            return MENU_METHOD_STOP_PASS;
        }
        for (int i = 0; args[2].getInteger() > i; i++)
            messageIDs[i] = args[1].getArray()->get(i)->getInteger();
        result->setInteger(menuSubOpenSelect2(SELECT_WINDOW_NORMAL, messageIDs, args[2].getInteger(),
                                              args[3].getInteger(), args[4].getInteger(),
                                              args[5].getInteger(), args[2].getInteger(), true));
        break;
    }
    // Party, status, name entry and shop screens.
    case MENU_COMMAND_SelectPartySlot:
        result->setInteger(menuPokemonOpen(PARTY_MODE_SELECT_SLOT, 0, 0));
        break;
    case MENU_COMMAND_SelectPartySlotAlt:
        result->setInteger(menuPokemonOpen(PARTY_MODE_SELECT_SLOT, 0, 0));
        break;
    case MENU_COMMAND_RunPokemonStatus:
        result->setInteger(menuPokemonStatus());
        break;
    case MENU_COMMAND_OpenPokemonMoveList:
        result->setInteger(menuPokemonStatusOpenWazaList(args[1].getInteger()));
        break;
    case MENU_COMMAND_OpenNameEntry:
        result->setInteger(menuNameEntryOpen(args[1].getInteger(), args[2].getInteger()));
        break;
    case MENU_COMMAND_RunNameEntry:
        menuNameEntry();
        break;
    case MENU_COMMAND_OpenShop:
        peopleBlock();
        menuShopOpen(args[1].getInteger());
        peopleUnblock();
        break;
    case MENU_COMMAND_OpenPda:
        menuPda();
        break;
    // Do not serialize this script thread while the party menu is open.
    case MENU_COMMAND_OpenPokemonExchange:
        mCurrentThread->setSaveFlag(false);
        menuPokemonChange();
        break;
    case MENU_COMMAND_OpenItemMenu:
        scriptOpenItemMenu();
        break;
    // Number entry; a cancelled prompt reports -1.
    case MENU_COMMAND_OpenNumberInput: {
        u32 value;

        if (menuSubOpenNumberInputDigit(args[1].getInteger(), &value) == NUMBER_INPUT_ACCEPTED)
            result->setInteger(value);
        else
            result->setInteger(-1);
        break;
    }
    case MENU_COMMAND_OpenTutorial:
        result->setInteger(menuScriptTutorial());
        break;
    case MENU_COMMAND_OpenEevee:
        result->setInteger(menuScriptEevee());
        break;
    // Selection list with an explicit window position and cursor.
    case MENU_COMMAND_OpenPositionedSelection: {
        if (args[3].getInteger() >= MENU_SELECT_ENTRY_CAPACITY) {
            result->setInteger(0);
            return MENU_METHOD_STOP_PASS;
        }
        for (int i = 0; args[3].getInteger() > i; i++)
            messageIDs[i] = args[2].getArray()->get(i)->getInteger();
        result->setInteger(menuSubOpenSelect2(args[1].getInteger(), messageIDs,
                                              args[3].getInteger(), args[4].getInteger(),
                                              args[5].getInteger(), args[6].getInteger(),
                                              args[7].getInteger(), args[8].getInteger()));
        break;
    }
    // Tutorial, world map and move teaching interfaces.
    case MENU_COMMAND_OpenWorldMap:
        GSthreadSwitch();
        menuScriptOpenWorldMap(args[1].getInteger());
        break;
    case MENU_COMMAND_OpenUnderTakerMenu:
        menuIntUnderTakerMenuOpen();
        break;
    case MENU_COMMAND_CloseUnderTakerMenu:
        menuIntUnderTakerMenuClose();
        break;
    case MENU_COMMAND_OpenUsohachi:
        menuScriptUsohachi();
        break;
    case MENU_COMMAND_SelectMeijinMove:
        result->setInteger(menuScriptWazaMeijin(args[1].getInteger()));
        break;
    case MENU_COMMAND_SelectBasanMove: {
        bool tutorFlag;

        GSthreadSwitch();
        result->setInteger(menuScriptWazaBasan(&tutorFlag));
        break;
    }
    case MENU_COMMAND_SetMoveLearnedFlag:
        GSthreadSwitch();
        menuScriptSetWazaOboetaFlag(args[1].getInteger());
        break;
    case MENU_COMMAND_OpenMoney:
        menuScriptMoneyOpen(args[1].getInteger(), args[2].getInteger());
        break;
    case MENU_COMMAND_CloseMoney:
        menuScriptMoneyClose();
        break;
    case MENU_COMMAND_CheckBasanMove:
        result->setInteger(menuScriptWazaBasanCheck());
        break;
    case MENU_COMMAND_OpenCoupon:
        menuScriptCouponOpen((s8)args[1].getInteger(), (s8)args[2].getInteger());
        break;
    case MENU_COMMAND_CloseCoupon:
        menuScriptCouponClose();
        break;
    // Orre Colosseum, pocket items and Mew's move.
    case MENU_COMMAND_OpenOrreColosseum:
        menuOrreColosseum(args[1].getInteger());
        break;
    case MENU_COMMAND_OpenUsohachi2:
        menuScriptUsohachi2();
        break;
    case MENU_COMMAND_GetOrreColosseumResult:
        result->setInteger(menuOrreColosseumGetResult());
        break;
    case MENU_COMMAND_AddPdaFunction:
        menuScriptPdaAddFunc(args[1].getInteger());
        break;
    case MENU_COMMAND_AddPocketItemScript:
        menuPocket2AddItemScript(args[1].getInteger(), args[2].getInteger());
        break;
    case MENU_COMMAND_RunMewMoveSequence:
        result->setInteger(mewWazaSequence());
        break;
    case MENU_COMMAND_GetMewMove:
        result->setInteger(mewWazaGet(args[1].getInteger()));
        break;
    // Open the party screen on the move the game picked for teaching.
    case MENU_COMMAND_OpenMoveTutor:
        result->setInteger(menuPokemonOpen(PARTY_MODE_MOVE_TUTOR, scriptPokemonTeachWazaIndex(args[1].getInteger()), 0));
        break;
    }
    return MENU_METHOD_CONTINUE;
}
