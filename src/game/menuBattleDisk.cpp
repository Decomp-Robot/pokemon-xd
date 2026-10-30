#include <GSthread.h>
#include <menuPdaSub.h>
#include <menuSub.h>
#include <zokusei.h>
#include <deckDataVirtualBios.h>
#include <winSprite.h>
#include <window.h>

#include <hero.h>
#include <Pokemon.h>
#include <PokemonStatus.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <FightResult.h>
#include <menu.h>
#include <menuDataBios.h>
#include <menuBattleDisk.h>
#include <flashsystem.h>
#include <menuItemBios.h>
#include <itemDataBios.h>
#include <item.h>
#include <itemBios.h>
#include <waza.h>
#include <pokemonBios.h>
#include <fightEncountBios.h>
#include <fightTrainer.h>
#include <fight.h>
#include <fightEncount.h>
#include <heroItem.h>
#include <menuFace.h>
#include <menuOffScreen.h>
#include <winMsg.h>
#include <string.h>
#include <sprite.h>
#include <floorManager.h>
#include <floor.h>
#include <time.h>
#include <pokemonDataBios.h>

/* Cursor-magnet search limits. The vertical scan starts wide and widens each
 * pass, capped at the sprite field; the horizontal scan is capped wider
 * still because rows are further apart than columns. */
static const s32 kCursorLimitUpDown = 480;
static const s32 kCursorLimitLeftRight = 640;

enum PokemonMoveIconID {
    kMoveEmpty = 0,
    kShadowMoveFirst = 0x164,
    kMoveFaceIcon = 0x176
};
static const long kMoveFaceTexture = 0x7B;

enum BattleDiskMenuID {
    kMenuBattleDiskPokemonInfo = 0xEC,
    kSequencePokemonClose = 0x17B,
    kSequenceEnemyInfoOpen = 0x1E6,
    kSequenceEnemyInfoClose = 0x1EA,
    kSequenceAllyInfoOpen = 0x1EE,
    kSequenceAllyInfoClose = 0x1F2,
    kAllyInfoWindowX = 0x131,
    kItemPokemonPrimaryType = 0x78B,
    kItemPokemonSecondaryType = 0x78C,
    kItemPokemonNickname = 0x791,
    kItemPokemonSex = 0x792,
    kItemPokemonLevel = 0x793,
    kItemPokemonPhysicalAttack = 0x79D,
    kItemPokemonPhysicalDefense = 0x79E,
    kItemPokemonSpecialAttack = 0x79F,
    kItemPokemonSpecialDefense = 0x7A0,
    kItemPokemonSpeed = 0x7A1,
    kItemPokemonHeldItem = 0x7A3,
    kItemPokemonCurrentHP = 0x7FA,
    kItemBattleDiskExplanation = 0x74B,
    kItemBattleDiskTitle = 0x74C,
    kItemEnemyTrainerName = 0x74D,
    kItemAllyTrainerName = 0x74E,
    kItemResultBackground = 0x7F2,
    kItemResultAnimation = 0x7F3,
    kControlText = 0x37,
    kControlNumber = 0x34,
    kControlHeldItem = 0x2D,
    kMessageSexMale = 0x9C5,
    kMessageSexFemale = 0x9C6,
    kMessageTextAtPosition = 0x4277,
    kMessageTextAtOrigin = 0x4276,
    kMessageLevelOrHP = 0x426F,
    kMessageStatNumber = 0x4287,
    kMessageHeldItemName = 0x3AE7,
    kMessageNoHeldItem = 0x3C1A,
    kBattleDiskItemBase = 0x215,
    kTextureResultBackgroundSpecial = 0x303,
    kTextureResultBackgroundDefault = 0x305,
    kTextureResultAnimationPrimary = 0x314,
    kTextureResultAnimationAlternate = 0x315,
    kTextureResultAnimationDefault = 0x316,
    kFlashResultPrimary = 0x1F333000,
    kFlashResultAlternate = 0x1F343000,
    kFlashResultDefault = 0x1F353000,
    kItemAllyFace1 = 0x733,
    kItemAllyFace2 = 0x734,
    kItemAllyFace3 = 0x735,
    kItemAllyFace4 = 0x736,
    kItemAllyFace5 = 0x737,
    kItemAllyFace6 = 0x738,
    kItemAllySlotIndicator1 = 0x739,
    kItemAllySlotIndicator2 = 0x73A,
    kItemAllySlotIndicator3 = 0x73B,
    kItemAllySlotIndicator4 = 0x73C,
    kItemAllySlotIndicator5 = 0x73D,
    kItemAllySlotIndicator6 = 0x73E,
    kItemEnemyFace1 = 0x73F,
    kItemEnemyFace2 = 0x740,
    kItemEnemyFace3 = 0x741,
    kItemEnemyFace4 = 0x742,
    kItemEnemyFace5 = 0x743,
    kItemEnemyFace6 = 0x744,
    kItemEnemySlotIndicator1 = 0x745,
    kItemEnemySlotIndicator2 = 0x746,
    kItemEnemySlotIndicator3 = 0x747,
    kItemEnemySlotIndicator4 = 0x748,
    kItemEnemySlotIndicator5 = 0x749,
    kItemEnemySlotIndicator6 = 0x74A,
    kItemMoveType1 = 0x78D,
    kItemMoveType2 = 0x78E,
    kItemMoveType3 = 0x78F,
    kItemMoveType4 = 0x790,
    kItemMoveName1 = 0x794,
    kItemMoveName2 = 0x795,
    kItemMoveName3 = 0x796,
    kItemMoveName4 = 0x797,
    kItemDefaultResultDetail1 = 0x877,
    kItemDefaultResultDetail2 = 0x878,
    kItemDefaultResultDetail3 = 0x879,
    kItemDefaultResultDetail4 = 0x87A,
    kItemDefaultResultDetail5 = 0x87B,
    kItemDefaultResultDetail6 = 0x87C,
    kItemDefaultResultDetail7 = 0x87D,
    kItemDefaultResultDetail8 = 0x87E,
};
static const u32 kMessageWhiteRGB = 0xFFFFFF00;

enum BattleDiskResult {
    kBattleResultAlternateWithoutDiskText = 4,
    kBattleResultAlternateOther = 7
};

enum WindowSequencePhase {
    kWindowOpening = 0,
    kWindowWaiting = 1,
    kWindowActive = 2,
    kWindowClosing = 3
};

enum PokemonInfoFormat {
    kMoveStatusName = 1,
    kMoveStatusType = 3,
    kLevelHPNumberXOffset = 0x24,
    kPokemonSexMale = 0,
    kPokemonSexFemale = 1,
    kPokemonSexNone = 2
};



extern "C" {

void menuBattleDiskPokemonCursorMove(tagWINDOW_WORK* window, u8 move);

}

/* C++ entry points the hooks share with the other menus. */
void dispFace(unsigned long id, GSvec2* pos, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite,
              Pokemon* pokemon);




#pragma force_active on

/* Wait for the information window to settle before moving the pokemon cursor. */
extern "C" long menuBattleDiskPokemonCursor(tagWINDOW_WORK* window)
{
    if (winSeqCheckMove(kMenuBattleDiskPokemonInfo) != 0) {
        return 0;
    }
    menuBattleDiskPokemonCursorMove(window, 1);
    return 0;
}

/* Move the cursor to the adjacent pokemon in the requested direction. */

extern "C" void menuBattleDiskPokemonCursorMove(tagWINDOW_WORK* window, u8 move)
{
    enum {
        kDirectionUp = 1,
        kDirectionDown = 2,
        kDirectionLeft = 4,
        kDirectionRight = 8,
        kDirectionMask = 15,
        kMenuFirstItemOffset = 6,
        kHorizontalRadiusStep = 16,
        kVerticalRadiusStep = 12,
        kMinimumHorizontalSeparation = 30
    };

    struct CursorTraversal {
        tagMENU_ITEM_DATA* current;
        tagMENU_ITEM_DATA* origin;
    };

    CursorTraversal traversal;
    u32 repeatKeys;
    u16 heldKeys;
    char* menuData;
    s32 searchUp;
    s32 searchDown;
    s32 searchLeft;
    s32 searchRight;
    signed char selectableIndex;
    s32 closestDistance;
    s32 horizontalRadius;
    s32 verticalRadius;

    if (window == 0) {
        return;
    }
    menuData = (char*)menuDataBiosGetPtr(window->mMenuID);
    traversal.current = windowGetCursorToItem(window);
    if (traversal.current == 0) {
        window->mCursor.y = 0;
        return;
    }
    horizontalRadius = kHorizontalRadiusStep;
    verticalRadius = kVerticalRadiusStep;
    repeatKeys = windowGetKeyInfo()->mKeyRepeat;
    heldKeys = windowGetKeyInfo()->mKeyOn;
    repeatKeys |= move != 0 ? ((heldKeys & kDirectionMask) != 0 ? 0 : windowGetKeyInfo2()->mKeyRepeat) : 0;
    searchUp = (u16)repeatKeys & kDirectionUp;
    searchDown = (u16)repeatKeys & kDirectionDown;
    searchLeft = (u16)repeatKeys & kDirectionLeft;
    searchRight = (u16)repeatKeys & kDirectionRight;
    for (;;) {
        traversal.origin = traversal.current;
        selectableIndex = 0;
        traversal.current = menuItemBiosGetPtr(window->mMenuID, *(s16*)(menuData + kMenuFirstItemOffset));
        if (searchUp) {
            closestDistance = kCursorLimitUpDown;
            for (;;) {
                if (traversal.current->mShownFlag) {
                    s32 dy = traversal.origin->mOffsetA - traversal.current->mOffsetA;
                    if (dy > 0) {
                        s32 dx = traversal.origin->mMessageX - traversal.current->mMessageX;
                        if (dx < 0) {
                            dx = -dx;
                        }
                        if (dx < horizontalRadius && closestDistance > dy) {
                            window->mCursor.y = selectableIndex;
                            closestDistance = dy;
                        }
                    }
                    selectableIndex++;
                }
                if (traversal.current->mChainEndFlag) {
                    break;
                }
                traversal.current = menuItemBiosGetPtr(window->mMenuID, traversal.current->mNextID);
            }
        } else if (searchDown) {
            closestDistance = kCursorLimitUpDown;
            for (;;) {
                if (traversal.current->mShownFlag) {
                    s32 dy = traversal.current->mOffsetA - traversal.origin->mOffsetA;
                    if (dy > 0) {
                        s32 dx = traversal.origin->mMessageX - traversal.current->mMessageX;
                        if (dx < 0) {
                            dx = -dx;
                        }
                        if (dx < horizontalRadius && closestDistance > dy) {
                            window->mCursor.y = selectableIndex;
                            closestDistance = dy;
                        }
                    }
                    selectableIndex++;
                }
                if (traversal.current->mChainEndFlag) {
                    break;
                }
                traversal.current = menuItemBiosGetPtr(window->mMenuID, traversal.current->mNextID);
            }
        } else if (searchLeft) {
            closestDistance = kCursorLimitLeftRight;
            for (;;) {
                if (traversal.current->mShownFlag) {
                    s32 dx = traversal.origin->mMessageX - traversal.current->mMessageX;
                    if (dx > kMinimumHorizontalSeparation) {
                        s32 dy = traversal.origin->mOffsetA - traversal.current->mOffsetA;
                        if (dy < 0) {
                            dy = -dy;
                        }
                        if (dy < verticalRadius && closestDistance > dx) {
                            window->mCursor.y = selectableIndex;
                            closestDistance = dx;
                        }
                    }
                    selectableIndex++;
                }
                if (traversal.current->mChainEndFlag) {
                    break;
                }
                traversal.current = menuItemBiosGetPtr(window->mMenuID, traversal.current->mNextID);
            }
        } else if (searchRight) {
            closestDistance = kCursorLimitLeftRight;
            for (;;) {
                if (traversal.current->mShownFlag) {
                    s32 dx = traversal.current->mMessageX - traversal.origin->mMessageX;
                    if (dx > kMinimumHorizontalSeparation) {
                        s32 dy = traversal.origin->mOffsetA - traversal.current->mOffsetA;
                        if (dy < 0) {
                            dy = -dy;
                        }
                        if (dy < verticalRadius && closestDistance > dx) {
                            window->mCursor.y = selectableIndex;
                            closestDistance = dx;
                        }
                    }
                    selectableIndex++;
                }
                if (traversal.current->mChainEndFlag) {
                    break;
                }
                traversal.current = menuItemBiosGetPtr(window->mMenuID, traversal.current->mNextID);
            }
        }
        if (window->cursorTo != window->mCursor.y) {
            break;
        }
        traversal.current = traversal.origin;
        horizontalRadius += kHorizontalRadiusStep;
        verticalRadius += kVerticalRadiusStep;
        if (horizontalRadius >= kCursorLimitLeftRight) {
            horizontalRadius = kCursorLimitLeftRight;
        }
        if (verticalRadius >= kCursorLimitUpDown) {
            verticalRadius = kCursorLimitUpDown;
        }
        if (horizontalRadius == kCursorLimitLeftRight && verticalRadius == kCursorLimitUpDown) {
            break;
        }
    }
}

extern "C" long menuBattleDiskPokemonCtrl(tagWINDOW_WORK* window)
{
    switch (window->mState) {
    case kWindowOpening:
    case kWindowWaiting:
    case kWindowActive:
        break;
    case kWindowClosing:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, kSequencePokemonClose);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

/* Position the information window beside the selected ally or enemy. */
extern "C" long menuBattleDiskPokemonInfoCtrl(tagWINDOW_WORK* window)
{
    switch (window->mState) {
    case kWindowOpening: {
        s32 selectedIndex;
        if (window->mFlag != 0) {
            break;
        }
        selectedIndex = gBDisk.mSelectedPokemonIndex;
        if (selectedIndex < heroGetPokeomCount(&gBDisk.mHeroes[0])) {
            winSeqSetMenu(window->mMenuID, kSequenceAllyInfoOpen);
        } else {
            winSeqSetMenu(window->mMenuID, kSequenceEnemyInfoOpen);
        }
        window->mFlag = 1;
        break;
    }
    case kWindowActive: {
        s32 selectedIndex;
        if (gBDisk.mCurrentX != gBDisk.mRequestedX) {
            gBDisk.mCurrentX = gBDisk.mRequestedX;
        }
        if (gBDisk.mCurrentY != gBDisk.mRequestedY) {
            gBDisk.mCurrentY = gBDisk.mRequestedY;
        }
        if (winSeqCheckMove(kMenuBattleDiskPokemonInfo) == 0) {
            selectedIndex = gBDisk.mSelectedPokemonIndex;
            if (selectedIndex < heroGetPokeomCount(&gBDisk.mHeroes[0])) {
                gBDisk.mRequestedX = kAllyInfoWindowX;
                gBDisk.mRequestedY = 0;
            } else {
                gBDisk.mRequestedX = 0;
                gBDisk.mRequestedY = 0;
            }
            menuSetPosition(kMenuBattleDiskPokemonInfo, gBDisk.mCurrentX, gBDisk.mCurrentY);
        }
        break;
    }
    case kWindowClosing: {
        s32 selectedIndex;
        if (window->mFlag != 0) {
            break;
        }
        selectedIndex = gBDisk.mSelectedPokemonIndex;
        if (selectedIndex < heroGetPokeomCount(&gBDisk.mHeroes[0])) {
            winSeqSetMenu(window->mMenuID, kSequenceAllyInfoClose);
        } else {
            winSeqSetMenu(window->mMenuID, kSequenceEnemyInfoClose);
        }
        window->mFlag = 1;
        break;
    }
    }
    return 0;
}

/* Draw the selected pokemon's status, move and held-item information. */
extern "C" long menuBattleDiskPokemonInfoDraw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    s32 moveSlot = 0;
    s32 moveID;
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);
    unsigned long count = heroGetPokeomCount(&gBDisk.mHeroes[0]);
    Hero* target;
    s32 slot = gBDisk.mSelectedPokemonIndex;
    if (slot < count) {
        target = &gBDisk.mHeroes[0];
    } else {
        target = &gBDisk.mHeroes[1];
        slot = slot - count;
    }
    Pokemon* pokemon = target->getPokemon(slot);
    switch (sprite->mItemID) {
    case kItemPokemonNickname:
        msgctrlSetValue(kControlText, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_NICKNAME, 0));
        GSmsgPrintRect(layer, 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtPosition);
        break;
    case kItemPokemonSex: {
        u8 sex = menuSubGetPokemonSexForDisp(pokemon);
        unsigned long msg;
        switch (sex) {
        case kPokemonSexMale:
            msg = kMessageSexMale;
            break;
        case kPokemonSexFemale:
            msg = kMessageSexFemale;
            break;
        case kPokemonSexNone:
        default:
            msg = 0;
            break;
        }
        if (msg != 0) {
            msgctrlSetValue(kControlText, (unsigned long)GSmsgGetStringPtr(msg));
            GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtOrigin);
        }
        break;
    }
    case kItemPokemonLevel:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_LEVEL, 0));
        GSmsgPrintRect(layer, kLevelHPNumberXOffset, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageLevelOrHP);
        break;
    case kItemPokemonCurrentHP:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_CURRENT_HP, 0));
        GSmsgPrintRect(layer, kLevelHPNumberXOffset, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageLevelOrHP);
        break;
    case kItemPokemonPrimaryType: {
        PokemonData* stats = pokemon->getPokemonDataPtr();
        u16 kind = zokuseiGetPokemonSpriteID(stats, 0);
        if (kind != 0) {
            windowDrawSprite(0, 0, window, kind, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case kItemPokemonSecondaryType: {
        u8 first = pokemonDataBiosGetZokuseiDataId(
            pokemon->getPokemonDataPtr(), 0);
        u8 second = pokemonDataBiosGetZokuseiDataId(
            pokemon->getPokemonDataPtr(), 1);
        if (first != second) {
            u16 kind = zokuseiGetPokemonSpriteID(
                pokemon->getPokemonDataPtr(), 1);
            windowDrawSprite(0, 0, window, kind, 0, winSpriteGetLayerID(window, sprite));
        }
        break;
    }
    case kItemMoveType4:
        moveSlot = 1;
        /* fall through */
    case kItemMoveType3:
        moveSlot++;
        /* fall through */
    case kItemMoveType2:
        moveSlot++;
        /* fall through */
    case kItemMoveType1: {
        moveID = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE_ID, moveSlot);
        switch (moveID) {
        case kMoveEmpty:
        case kShadowMoveFirst + 0:
        case kShadowMoveFirst + 1:
        case kShadowMoveFirst + 2:
        case kShadowMoveFirst + 3:
        case kShadowMoveFirst + 4:
        case kShadowMoveFirst + 5:
        case kShadowMoveFirst + 6:
        case kShadowMoveFirst + 7:
        case kShadowMoveFirst + 8:
        case kShadowMoveFirst + 9:
        case kShadowMoveFirst + 10:
        case kShadowMoveFirst + 11:
        case kShadowMoveFirst + 12:
        case kShadowMoveFirst + 13:
        case kShadowMoveFirst + 14:
        case kShadowMoveFirst + 15:
        case kShadowMoveFirst + 16:
        case kShadowMoveFirst + 17:
            break;
        case kMoveFaceIcon:
            windowDrawSprite(0, 0, window, kMoveFaceTexture, 0, winSpriteGetLayerID(window, sprite));
            break;
        default:
            windowDrawSprite(0, 0, window,
                zokuseiBiosGetSpriteID(wazaGetStatus(0, moveID, kMoveStatusType, 0)), 0,
                winSpriteGetLayerID(window, sprite));
            break;
        }
        break;
    }
    case kItemMoveName4:
        moveSlot = 1;
        /* fall through */
    case kItemMoveName3:
        moveSlot++;
        /* fall through */
    case kItemMoveName2:
        moveSlot++;
        /* fall through */
    case kItemMoveName1: {
        u32 moveID = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_MOVE_ID, moveSlot);
        if (moveID != kMoveEmpty && wazaGetStatus(0, moveID, kMoveStatusName, 0) != 0) {
            msgctrlSetValue(kControlText,
                            (unsigned long)GSmsgGetStringPtr(wazaGetStatus(0, moveID, kMoveStatusName, 0)));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                        window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtPosition);
        }
        break;
    }
    case kItemPokemonPhysicalAttack:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_PHYSICAL_ATTACK, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageStatNumber);
        break;
    case kItemPokemonPhysicalDefense:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_PHYSICAL_DEFENSE, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageStatNumber);
        break;
    case kItemPokemonSpecialAttack:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIAL_ATTACK, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageStatNumber);
        break;
    case kItemPokemonSpecialDefense:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPECIAL_DEFENSE, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageStatNumber);
        break;
    case kItemPokemonSpeed:
        msgctrlSetValue(kControlNumber, pokemonGetStatus(pokemon, 0, POKEMON_STATUS_SPEED, 0));
        GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageStatNumber);
        break;
    case kItemPokemonHeldItem: {
        if (pokemonGetSoubiItemDataId(pokemon) != 0) {
            msgctrlSetValue(kControlHeldItem, pokemonGetSoubiItemDataId(pokemon));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                        window->mRgba.mColorA | kMessageWhiteRGB, kMessageHeldItemName);
        } else {
            msgctrlSetValue(kControlText, (unsigned long)GSmsgGetStringPtr(kMessageNoHeldItem));
            GSmsgPrintRect(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                        window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtPosition);
        }
        break;
    }
    }
    return 0;
}

/* Draw both parties and the Battle Disk result presentation. */
extern "C" long menuBattleDiskSpriteDraw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    GSvec2 pos;
    pos.x = 0.0f;
    pos.y = 0.0f;
    s32 slot = 0;
    switch (sprite->mItemID) {
    case kItemAllyFace6:
        slot = 1;
        /* fall through */
    case kItemAllyFace5:
        slot++;
        /* fall through */
    case kItemAllyFace4:
        slot++;
        /* fall through */
    case kItemAllyFace3:
        slot++;
        /* fall through */
    case kItemAllyFace2:
        slot++;
        /* fall through */
    case kItemAllyFace1:
        if (heroGetPokeomCount(&gBDisk.mHeroes[0]) > slot) {
            dispFace(slot, &pos, window, sprite,
                     heroBiosGetPokemonPtr(&gBDisk.mHeroes[0], slot));
        }
        break;
    case kItemEnemyFace6:
        slot = 1;
        /* fall through */
    case kItemEnemyFace5:
        slot++;
        /* fall through */
    case kItemEnemyFace4:
        slot++;
        /* fall through */
    case kItemEnemyFace3:
        slot++;
        /* fall through */
    case kItemEnemyFace2:
        slot++;
        /* fall through */
    case kItemEnemyFace1:
        if (heroGetPokeomCount(&gBDisk.mHeroes[1]) > slot) {
            dispFace(slot, &pos, window, sprite,
                     heroBiosGetPokemonPtr(&gBDisk.mHeroes[1], slot));
        }
        break;
    case kItemAllySlotIndicator6:
        slot = 1;
        /* fall through */
    case kItemAllySlotIndicator5:
        slot++;
        /* fall through */
    case kItemAllySlotIndicator4:
        slot++;
        /* fall through */
    case kItemAllySlotIndicator3:
        slot++;
        /* fall through */
    case kItemAllySlotIndicator2:
        slot++;
        /* fall through */
    case kItemAllySlotIndicator1:
        winSpriteSetDisp(sprite, 0);
        if (slot < heroGetPokeomCount(&gBDisk.mHeroes[0])) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case kItemEnemySlotIndicator6:
        slot = 1;
        /* fall through */
    case kItemEnemySlotIndicator5:
        slot++;
        /* fall through */
    case kItemEnemySlotIndicator4:
        slot++;
        /* fall through */
    case kItemEnemySlotIndicator3:
        slot++;
        /* fall through */
    case kItemEnemySlotIndicator2:
        slot++;
        /* fall through */
    case kItemEnemySlotIndicator1:
        winSpriteSetDisp(sprite, 0);
        if (slot < heroGetPokeomCount(&gBDisk.mHeroes[1])) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case kItemBattleDiskExplanation:
        sprite->mMsgID = 0;
        switch (gBDisk.mBattleResult) {
        case kFightResultHostVictory:
        case kFightResultOtherSideVictory:
            sprite->mMsgID = deckDataVirtualDB_GetExplanetion2(gBDisk.mBattleDiskID);
            break;
        }
        break;
    case kItemBattleDiskTitle:
        sprite->mMsgID = 0;
        switch (gBDisk.mBattleResult) {
        case kFightResultHostVictory:
        case kFightResultOtherSideVictory:
            sprite->mMsgID = itemDataBiosGetName(itemDataBiosGetPtr(gBDisk.mBattleDiskID + kBattleDiskItemBase));
            break;
        default:
            sprite->mMsgID = deckDataVirtualDB_GetTitle(gBDisk.mBattleDiskID);
            break;
        }
        break;
    case kItemEnemyTrainerName:
        msgctrlSetValue(kControlText, (unsigned long)heroBiosGetNamePtr(&gBDisk.mHeroes[1]));
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtOrigin);
        break;
    case kItemAllyTrainerName:
        msgctrlSetValue(kControlText, (unsigned long)heroBiosGetNamePtr(&gBDisk.mHeroes[0]));
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0,
                    window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtOrigin);
        break;
    case kItemResultBackground:
        switch (gBDisk.mBattleResult) {
        case kFightResultHostVictory:
        case kFightResultOtherSideVictory:
            windowDrawSprite(0, 0, window, kTextureResultBackgroundSpecial, 0, winSpriteGetLayerID(window, sprite));
            break;
        default:
            windowDrawSprite(0, 0, window, kTextureResultBackgroundDefault, 0, winSpriteGetLayerID(window, sprite));
            break;
        }
        break;
    case kItemResultAnimation:
        switch (gBDisk.mBattleResult) {
        case kFightResultHostVictory:
            windowDrawSprite(0, 0, window, kTextureResultAnimationPrimary, 0, winSpriteGetLayerID(window, sprite));
            gBDisk.mResultAnimationFrame = (u16)FlashSystemGetNowFrame(kFlashResultPrimary);
            gBDisk.mResultAnimationEndFrame = (u16)FlashSystemGetEndFrame(kFlashResultPrimary);
            break;
        case kFightResultOtherSideVictory:
        case kBattleResultAlternateWithoutDiskText:
        case kBattleResultAlternateOther:
            windowDrawSprite(0, 0, window, kTextureResultAnimationAlternate, 0, winSpriteGetLayerID(window, sprite));
            gBDisk.mResultAnimationFrame = (u16)FlashSystemGetNowFrame(kFlashResultAlternate);
            gBDisk.mResultAnimationEndFrame = (u16)FlashSystemGetEndFrame(kFlashResultAlternate);
            break;
        default:
            windowDrawSprite(0, 0, window, kTextureResultAnimationDefault, 0, winSpriteGetLayerID(window, sprite));
            gBDisk.mResultAnimationFrame = (u16)FlashSystemGetNowFrame(kFlashResultDefault);
            gBDisk.mResultAnimationEndFrame = (u16)FlashSystemGetEndFrame(kFlashResultDefault);
            break;
        }
        /* fall through */
    case kItemDefaultResultDetail1:
    case kItemDefaultResultDetail2:
    case kItemDefaultResultDetail3:
    case kItemDefaultResultDetail4:
    case kItemDefaultResultDetail5:
    case kItemDefaultResultDetail6:
    case kItemDefaultResultDetail7:
    case kItemDefaultResultDetail8:
        switch (gBDisk.mBattleResult) {
        case kFightResultHostVictory:
        case kFightResultOtherSideVictory:
        case kBattleResultAlternateWithoutDiskText:
        case kBattleResultAlternateOther:
            winSpriteSetDisp(sprite, 0);
            break;
        default:
            winSpriteSetDisp(sprite, 1);
            break;
        }
        break;
    }
    return 0;
}



static long gBattleDiskListItemIDs[8] = {
    0x6F9, 0x6FB, 0x6FD, 0x6FF, 0x701, 0x703, 0x705, 0x707
};

static long gBattleDiskPanelItemIDs[15] = {
    0x76F, 0x770, 0x771, 0x772, 0x773, 0x774, 0x775, 0x776, 0x777, 0x778, 0x7B6, 0x7B7, 0x7B8, 0x7B9, 0x7BA
};

static s16 gBattleDiskPanelRects[14 * 4] = {
    216, 6, 42, 42,
    0, 0, -264, 54,
    19, 7, 133, 18,
    30, 29, 95, 18,
    126, 29, 80, 18,
    154, 7, 19, 18,
    9, 29, 20, 15,
    6, 6, 42, 42,
    0, 0, 264, 54,
    64, 7, 133, 18,
    75, 29, 95, 18,
    171, 29, 80, 18,
    199, 7, 19, 18,
    54, 29, 20, 15,
};
enum BattleDiskPanelLayout {
    kAllyPanelRect = 0,
    kEnemyPanelRect = 7,
    kPanelFace = 0,
    kPanelBackground = 1,
    kPanelNickname = 2,
    kPanelLevel = 3,
    kPanelHP = 4,
    kPanelSex = 5,
    kPanelItem = 6,
    kPanelBackgroundTexture = 0x2FC,
    kPanelHeldItemTexture = 0x14B,
    kPanelLevelLabel = 0x42D4,
    kPanelHPLabel = 0x42D5,
    kPanelLevelNumberX = 24,
    kPanelHPNumberX = 28,
    kPokemonPartySize = 6
};




enum BattleDiskPanelPosition {
    kPanelAllyFirst = 0,
    kPanelAllySecondDouble = 1,
    kPanelAllySecondSingle = 2,
    kPanelAllyThird = 3,
    kPanelAllyFourth = 4,
    kPanelAllyFifth = 5,
    kPanelAllySixth = 6,
    kPanelEnemyFirstDefault = 7,
    kPanelEnemyFirstDoubleSingle = 8,
    kPanelEnemySecondDouble = 9,
    kPanelEnemySecondSingle = 10,
    kPanelEnemyThird = 11,
    kPanelEnemyFourth = 12,
    kPanelEnemyFifth = 13,
    kPanelEnemySixth = 14,
};

enum BattleDiskSiblingItemID {
    kItemContentsSingleBattleLayout = 0x753,
    kItemContentsDoubleBattleLayout1 = 0x754,
    kItemContentsDoubleBattleLayout2 = 0x755,
    kItemContentsDoubleBattleLayout3 = 0x756,
    kItemAllyPanelFirst = 0x760,
    kItemAllyPanelSecondDouble = 0x761,
    kItemAllyPanelSecondSingle = 0x762,
    kItemAllyPanelThird = 0x763,
    kItemAllyPanelFourth = 0x764,
    kItemAllyPanelFifth = 0x765,
    kItemAllyPanelSixth = 0x766,
    kItemEnemyPanelFirstDefault = 0x767,
    kItemEnemyPanelFirstDoubleSingle = 0x768,
    kItemEnemyPanelSecondDouble = 0x769,
    kItemEnemyPanelSecondSingle = 0x76A,
    kItemEnemyPanelThird = 0x76B,
    kItemEnemyPanelFourth = 0x76C,
    kItemEnemyPanelFifth = 0x76D,
    kItemEnemyPanelSixth = 0x76E,
    kItemContentsBattleDiskExplanation = 0x7BB,
    kItemContentsFocusMessage = 0x7BC,
    kItemContentsActiveDecoration = 0x7BD,
    kItemContentsInactiveDecoration = 0x7BE,
    kItemDiskListScrollDown = 0x6EE,
    kItemDiskListScrollUp = 0x6EF,
    kItemDiskListSelection1 = 0x6F8,
    kItemDiskListSelection2 = 0x6FA,
    kItemDiskListSelection3 = 0x6FC,
    kItemDiskListSelection4 = 0x6FE,
    kItemDiskListSelection5 = 0x700,
    kItemDiskListSelection6 = 0x702,
    kItemDiskListSelection7 = 0x704,
    kItemDiskListSelection8 = 0x706,
    kItemDiskListNames = 0x708,
    kItemSelectedDiskBattleTitle = 0x709,
    kItemDiskChoice1 = 0x714,
    kItemDiskChoice2 = 0x716,
    kItemDiskChoice3 = 0x718,
    kItemSelectedDiskTitle = 0x727,
    kItemSelectedDiskDescription = 0x729,
    kItemDiskListScissor = 0x7B5,
    kItemDiskListScrollBar = 0x856,
    kMessageContentsInactive = 0xE2E2,
    kMessageContentsActive = 0xE300,
};

long getPanelIdx(unsigned long panel, unsigned char battleCount);
void drawPkmPanel(unsigned long panel, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);

#pragma force_active reset

extern "C" long BattleDiskContentsDraw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    unsigned long panel = 0;
    switch (sprite->mItemID) {
    case 0x751: case 0x752: break;
    case kItemContentsSingleBattleLayout:
        if (gBDisk.mBattlePokemonCount == 1) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemContentsDoubleBattleLayout1: case kItemContentsDoubleBattleLayout2:
        if (gBDisk.mBattlePokemonCount == 2) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemContentsDoubleBattleLayout3:
        if (gBDisk.mBattlePokemonCount == 2) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemEnemyPanelSixth: panel++;
    case kItemEnemyPanelFifth: panel++;
    case kItemEnemyPanelFourth: panel++;
    case kItemEnemyPanelThird: panel++;
    case kItemEnemyPanelSecondSingle: panel++;
    case kItemEnemyPanelSecondDouble: panel++;
    case kItemEnemyPanelFirstDoubleSingle: panel++;
    case kItemEnemyPanelFirstDefault: panel++;
    case kItemAllyPanelSixth: panel++;
    case kItemAllyPanelFifth: panel++;
    case kItemAllyPanelFourth: panel++;
    case kItemAllyPanelThird: panel++;
    case kItemAllyPanelSecondSingle: panel++;
    case kItemAllyPanelSecondDouble: panel++;
    case kItemAllyPanelFirst:
        if (getPanelIdx(panel,gBDisk.mBattlePokemonCount)>=0)
            drawPkmPanel(getPanelIdx(panel,gBDisk.mBattlePokemonCount),window,sprite);
        break;

    case 0x779:
    case 0x77A:
    case 0x77B:
    case 0x77C:
    case 0x77D:
    case 0x77E:
    case 0x77F:
    case 0x780:
    case 0x781:
    case 0x782:
    case 0x783:
    case 0x784:
    case 0x785:
    case 0x786:
    case 0x787:
    case 0x788:
    case 0x789:
    case 0x78A:
    case kItemPokemonPrimaryType:
    case kItemPokemonSecondaryType:
    case kItemMoveType1:
    case kItemMoveType2:
    case kItemMoveType3:
    case kItemMoveType4:
    case kItemPokemonNickname:
    case kItemPokemonSex:
    case kItemPokemonLevel:
    case kItemMoveName1:
    case kItemMoveName2:
    case kItemMoveName3:
    case kItemMoveName4:
    case 0x798:
    case 0x799:
    case 0x79A:
    case 0x79B:
    case 0x79C:
    case kItemPokemonPhysicalAttack:
    case kItemPokemonPhysicalDefense:
    case kItemPokemonSpecialAttack:
    case kItemPokemonSpecialDefense:
    case kItemPokemonSpeed:
    case 0x7A2:
    case kItemPokemonHeldItem:
    case 0x7A4:
    case 0x7A5:
    case 0x7A6:
    case 0x7A7:
    case 0x7A8:
    case 0x7A9:
    case 0x7AA:
    case 0x7AB:
    case 0x7AC:
    case 0x7AD:
    case 0x7AE:
    case 0x7AF:
    case 0x7B0:
    case 0x7B1:
    case 0x7B2:
    case 0x7B3:
    case 0x7B4:
    case kItemDiskListScissor: break;
    case kItemContentsBattleDiskExplanation: sprite->mMsgID=deckDataVirtualDB_GetExplanetion2(gBDisk.mBattleDiskID); break;
    case kItemContentsFocusMessage:
        if (window->mMenuID!=windowGetActiveID()) sprite->mMsgID=kMessageContentsInactive;
        else sprite->mMsgID=kMessageContentsActive;
        break;
    case kItemContentsActiveDecoration:
        if (window->mMenuID==windowGetActiveID()) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemContentsInactiveDecoration:
        if (window->mMenuID!=windowGetActiveID()) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    }
    return 0;
}

long getPanelIdx(unsigned long panel, unsigned char battleCount)
{
    long slot=-1;
    switch(panel) {
    case kPanelAllyFirst: slot=0; break;
    case kPanelAllySecondDouble: if(battleCount==2) slot=1; break;
    case kPanelAllySecondSingle: if(battleCount==1) slot=1; break;
    case kPanelAllyThird: slot=2; break;
    case kPanelAllyFourth: slot=3; break;
    case kPanelAllyFifth: slot=4; break;
    case kPanelAllySixth: slot=5; break;
    case kPanelEnemyFirstDefault:
        if(battleCount==2) {
            long enemyCount=heroGetPokeomCount(&gBDisk.mHeroes[1]);
            if(enemyCount==1) break;
        }
        slot=6;
        break;
    case kPanelEnemyFirstDoubleSingle:
        if(battleCount==2) {
            long enemyCount=heroGetPokeomCount(&gBDisk.mHeroes[1]);
            if(enemyCount==1) slot=6;
        }
        break;
    case kPanelEnemySecondDouble:
        if(battleCount==2) {
            long enemyCount=heroGetPokeomCount(&gBDisk.mHeroes[1]);
            if(enemyCount!=1) slot=7;
        }
        break;
    case kPanelEnemySecondSingle: if(battleCount==1) slot=7; break;
    case kPanelEnemyThird: slot=8; break;
    case kPanelEnemyFourth: slot=9; break;
    case kPanelEnemyFifth: slot=10; break;
    case kPanelEnemySixth: slot=11; break;
    }
    return slot;
}

void drawPkmPanel(unsigned long panel, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    Hero* hero;
    s16 slideX;
    Pokemon* pokemon;
    s32 rect;
    unsigned long slot;
    GSgfxLayerID layer;
    GSvec2 pos;
    layer = winSpriteGetLayerID(window, sprite);
    slot = panel;
    if (panel < kPokemonPartySize) {
        hero = &gBDisk.mHeroes[0];
        rect = kAllyPanelRect;
        if (gBDisk.mPreviewElapsedSec < 0.2f) {
            slideX = -256.0f + 1280.0f * gBDisk.mPreviewElapsedSec;
        } else {
            slideX = 0;
        }
    } else {
        slot = panel - kPokemonPartySize;
        hero = &gBDisk.mHeroes[1];
        rect = kEnemyPanelRect;
        if (gBDisk.mPreviewElapsedSec < 0.2f) {
            slideX = 256.0f - 1280.0f * gBDisk.mPreviewElapsedSec;
        } else {
            slideX = 0;
        }
    }
    if (heroGetPokeomCount(hero) > slot) {
        pos.x = slideX + gBattleDiskPanelRects[(rect + kPanelFace) * 4 + 0];
        pos.y = gBattleDiskPanelRects[(rect + kPanelFace) * 4 + 1];
        pokemon = heroBiosGetPokemonPtr(hero, slot);
        if (pokemon) {
            u32 nickname = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_NICKNAME, 0);
            u32 hp = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_CURRENT_HP, 0);
            u32 level = pokemonGetStatus(pokemon, 0, POKEMON_STATUS_LEVEL, 0);
            dispFace(panel, &pos, window, sprite, pokemon);
            windowDrawSprite2(
                slideX + gBattleDiskPanelRects[(rect + kPanelBackground) * 4 + 0],
                gBattleDiskPanelRects[(rect + kPanelBackground) * 4 + 1],
                gBattleDiskPanelRects[(rect + kPanelBackground) * 4 + 2],
                gBattleDiskPanelRects[(rect + kPanelBackground) * 4 + 3],
                window->mRgba.mColorA | kMessageWhiteRGB, window, kPanelBackgroundTexture, 0, layer);
            msgctrlSetValue(kControlText, nickname);
            GSgfxLayerID textLayer = layer;
            GSmsgPrintRect(textLayer, slideX + gBattleDiskPanelRects[(rect + kPanelNickname) * 4 + 0],
                           gBattleDiskPanelRects[(rect + kPanelNickname) * 4 + 1],
                           sprite->mWidth, sprite->mHeight,
                           window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtPosition);
            msgctrlSetValue(kControlText, (unsigned long)GSmsgGetStringPtr(kPanelLevelLabel));
            GSmsgPrintRect(textLayer, slideX + gBattleDiskPanelRects[(rect + kPanelLevel) * 4 + 0],
                           gBattleDiskPanelRects[(rect + kPanelLevel) * 4 + 1],
                           sprite->mWidth, sprite->mHeight,
                           window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtOrigin);
            msgctrlSetValue(kControlNumber, level);
            GSmsgPrintRect(textLayer, slideX + gBattleDiskPanelRects[(rect + kPanelLevel) * 4 + 0] + kPanelLevelNumberX,
                           gBattleDiskPanelRects[(rect + kPanelLevel) * 4 + 1],
                           sprite->mWidth, sprite->mHeight,
                           window->mRgba.mColorA | kMessageWhiteRGB, kMessageLevelOrHP);
            msgctrlSetValue(kControlText, (unsigned long)GSmsgGetStringPtr(kPanelHPLabel));
            GSmsgPrintRect(textLayer, slideX + gBattleDiskPanelRects[(rect + kPanelHP) * 4 + 0],
                           gBattleDiskPanelRects[(rect + kPanelHP) * 4 + 1],
                           sprite->mWidth, sprite->mHeight,
                           window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtOrigin);
            msgctrlSetValue(kControlNumber, hp);
            GSmsgPrintRect(textLayer, slideX + gBattleDiskPanelRects[(rect + kPanelHP) * 4 + 0] + kPanelHPNumberX,
                           gBattleDiskPanelRects[(rect + kPanelHP) * 4 + 1],
                           sprite->mWidth, sprite->mHeight,
                           window->mRgba.mColorA | kMessageWhiteRGB, kMessageLevelOrHP);
            u8 sex = menuSubGetPokemonSexForDisp(pokemon);
            u32 sexMessage;
            switch (sex) {
            case kPokemonSexMale: sexMessage = kMessageSexMale; break;
            case kPokemonSexFemale: sexMessage = kMessageSexFemale; break;
            case kPokemonSexNone:
            default: sexMessage = 0; break;
            }
            if (sexMessage) {
                msgctrlSetValue(kControlText, (unsigned long)GSmsgGetStringPtr(sexMessage));
                GSmsgPrint2(textLayer, slideX + gBattleDiskPanelRects[(rect + kPanelSex) * 4 + 0],
                            gBattleDiskPanelRects[(rect + kPanelSex) * 4 + 1],
                            window->mRgba.mColorA | kMessageWhiteRGB, kMessageTextAtOrigin);
            }
            if (pokemonGetSoubiItemDataId(pokemon)) {
                windowDrawSprite2(
                    slideX + gBattleDiskPanelRects[(rect + kPanelItem) * 4 + 0],
                    gBattleDiskPanelRects[(rect + kPanelItem) * 4 + 1],
                    gBattleDiskPanelRects[(rect + kPanelItem) * 4 + 2],
                    gBattleDiskPanelRects[(rect + kPanelItem) * 4 + 3],
                    window->mRgba.mColorA | kMessageWhiteRGB, window, kPanelHeldItemTexture, 0, layer);
            }
        }
    }
}

void dispFace(unsigned long panel, GSvec2* pos, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, Pokemon* pokemon)
{
    unsigned long faceSpriteID = pokemonDataBiosGetStatusFaceMenuSpriteId(pokemon->getPokemonDataPtr(),0);
    tagSPRITE_WORK backup;
    memcpy(&backup,sprite,sizeof(backup));
    sprite->mHeight=42;
    sprite->mWidth=42;
    if(panel<6) sprite->mWidth *= -1;
    menuFaceDispFromID2(window,sprite,(u16)faceSpriteID,pos);
    memcpy(sprite,&backup,sizeof(backup));
}

Pokemon* loadPkmFace(void* arg)
{
    Pokemon* pokemon=0;
    unsigned long* index=(unsigned long*)arg;
    if(*index<12) {
        if(*index<6) {
            if(*index<heroGetPokeomCount(&gBDisk.mHeroes[0]))
                pokemon=heroBiosGetPokemonPtr(&gBDisk.mHeroes[0],*index);
            else *index=6;
        }
        if(*index>=6) {
            unsigned long slot=(u16)(*index%6);
            if(slot<heroGetPokeomCount(&gBDisk.mHeroes[1]))
                pokemon=heroBiosGetPokemonPtr(&gBDisk.mHeroes[1],slot);
        }
    }
    (*index)++;
    return pokemon;
}

extern "C" long BattleDiskItemDraw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    s16 row=0;
    switch(sprite->mItemID) {
    case 0x6F9: case 0x6FB: case 0x6FD: case 0x6FF: case 0x701: case 0x703: case 0x705: case 0x707: case 0x70A: break;
    case kItemDiskListScrollUp:
        if(gBDisk.mFirstDiskRow!=0) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemDiskListScrollDown:
        if(gBDisk.mFirstDiskRow<gBDisk.mDiskCount-8) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemDiskListNames:
        if(gBDisk.mScrollTargetY<gBDisk.mScrollY) gBDisk.mScrollY-=6;
        else if(gBDisk.mScrollTargetY>gBDisk.mScrollY) gBDisk.mScrollY+=6;
        {
            tagMENU_ITEM_DATA* item=menuItemBiosGetPtr(window->mMenuID,kItemDiskListScissor);
            spriteSetScissor(winSpriteGetLayerID(window,sprite),item->mMessageX-sprite->mX,
                             item->mOffsetA-sprite->mY,item->mWidth,item->mHeight);
            for(long i=0;i<gBDisk.mDiskCount;i++) {
                s16 y=gBDisk.mScrollY+i*30;
                if(y>-64 && y<480) {
                    GSmsgPrint2(winSpriteGetLayerID(window,sprite),0,y,
                                window->mRgba.mColorA|kMessageWhiteRGB,
                                gBDisk.mDiskEntries[i].mItemNameMessage);
                    if(deckDataVirtualDB_GetFlagData(gBDisk.mDiskEntries[i].mItemID-0x215)) {
                        windowDrawSprite2(178,(s16)(y+3),22,13,
                            window->mRgba.mColorA|kMessageWhiteRGB,window,0x300,0,
                            winSpriteGetLayerID(window,sprite));
                    }
                }
            }
            spriteClearScissor(winSpriteGetLayerID(window,sprite));
        }
        break;
    case kItemSelectedDiskBattleTitle:
        sprite->mMsgID=gBDisk.mDiskEntries[gBDisk.mSelectedDiskRow+gBDisk.mFirstDiskRow].mBattleTitleMessage;
        break;
    case kItemDiskListSelection8: row++;
    case kItemDiskListSelection7: row++;
    case kItemDiskListSelection6: row++;
    case kItemDiskListSelection5: row++;
    case kItemDiskListSelection4: row++;
    case kItemDiskListSelection3: row++;
    case kItemDiskListSelection2: row++;
    case kItemDiskListSelection1:
        if(gBDisk.mSelectedDiskRow==row) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemDiskChoice3: row++;
    case kItemDiskChoice2: row++;
    case kItemDiskChoice1:
        if(window->mCursor.y==row) winSpriteSetDisp(sprite,1);
        else winSpriteSetDisp(sprite,0);
        break;
    case kItemSelectedDiskTitle: sprite->mMsgID=deckDataVirtualDB_GetTitle(gBDisk.mBattleDiskID); break;
    case kItemSelectedDiskDescription: sprite->mMsgID=deckDataVirtualDB_GetExplanetion1(gBDisk.mBattleDiskID); break;
    case kItemDiskListScrollBar:
        {
            GSivec2 pos;
            pos.x=window->mCursor.x;
            pos.y=window->mCursor.y;
            pos.x=gBDisk.mFirstDiskRow;
            menuPdaSubScrollBar(window,sprite,window->mMenuID,kItemDiskListScrollBar,0xF06422FF,
                                gBDisk.mDiskCount,8,&pos);
        }
        break;
    }
    return 0;
}

extern "C" long menuBdisk_selectL_Btn(tagWINDOW_WORK* window)
{
    u16 keys=windowGetKeyRepeat();
    gBDisk.mSelectedDiskRow=window->mCursor.y;
    s16 firstRow=gBDisk.mFirstDiskRow;
    if(keys&2) {
        if(gBDisk.mScrollY==gBDisk.mScrollTargetY && window->mCursor.y==7 && firstRow<gBDisk.mDiskCount-8) {
            gBDisk.mFirstDiskRow++;
            gBDisk.mScrollTargetY=gBDisk.mFirstDiskRow*-30;
            menuPlaySe(window->mMenuID,1);
        }
    } else if(keys&1) {
        if(gBDisk.mScrollY==gBDisk.mScrollTargetY && window->mCursor.y==0 && firstRow!=0) {
            gBDisk.mFirstDiskRow--;
            gBDisk.mScrollTargetY=gBDisk.mFirstDiskRow*-30;
            menuPlaySe(window->mMenuID,1);
        }
    } else if(keys&4) {
        gBDisk.mFirstDiskRow-=8;
        if(gBDisk.mFirstDiskRow<0) gBDisk.mFirstDiskRow=gBDisk.mSelectedDiskRow=window->mCursor.y=0;
        if(firstRow!=gBDisk.mFirstDiskRow) menuPlaySe(window->mMenuID,6);
        gBDisk.mScrollY=gBDisk.mScrollTargetY=gBDisk.mFirstDiskRow*-30;
    } else if(keys&8) {
        if(gBDisk.mDiskCount<8) {
            gBDisk.mFirstDiskRow=0;
            gBDisk.mSelectedDiskRow=window->mCursor.y=gBDisk.mDiskCount-1;
        } else {
            gBDisk.mFirstDiskRow+=8;
            if(gBDisk.mDiskCount-8<gBDisk.mFirstDiskRow) {
                gBDisk.mFirstDiskRow=gBDisk.mDiskCount-8;
                gBDisk.mSelectedDiskRow=window->mCursor.y=7;
            }
        }
        if(firstRow!=gBDisk.mFirstDiskRow) menuPlaySe(window->mMenuID,6);
        gBDisk.mScrollY=gBDisk.mScrollTargetY=gBDisk.mFirstDiskRow*-30;
    }
    menuCursorNormal(window,1);
    return 0;
}

extern "C" long menuBattleDiskYesNoCtrl(tagWINDOW_WORK* window)
{
    switch(window->mState) {
    case 0:
        if(window->mFlag==0) {
            winSeqSetMenu(window->mMenuID,0x24);
            window->mFlag=1;
        }
        break;
    case 3:
        if(window->mFlag==0) {
            winSeqSetMenu(window->mMenuID,0x17B);
            window->mFlag=1;
        }
        break;
    }
    return 0;
}

extern "C" long menuBattleDiskCtrl(tagWINDOW_WORK*) { return 0; }

enum BattleDiskResource {
    kBattleDiskResourceGroup = 0x842,
    kBattleDiskPreviewResource = 0x18513400
};

extern "C" __declspec(export) void menuBattleDiskExit()
{
    fightFreeResource(kBattleDiskResourceGroup,kBattleDiskPreviewResource);
}

enum BattleDiskScreenID {
    kMenuDiskList = 0xE7,
    kMenuDiskChoice = 0xE8,
    kMenuDiskDocumentFrame = 0xE9,
    kMenuDiskBattleResult = 0xEA,
    kMenuDiskPreviewLoading = 0xEB,
    kMenuDiskPokemonPanels = 0x109,
    kMenuDocumentChoice = 0x8F,
    kBattleDiskEncount = 14,
    kFloorBattleDisk = 0x389,
    kFloorBattleDiskDocument = 0x38D,
    kItemKindBattleDisk = 7,
    kBattleDiskBrowse = 0,
    kBattleDiskFight = 1,
    kBattleDiskDocument = 2,
    kDiskChoiceCancel = 0,
    kDiskChoicePokemon = 1,
    kDiskChoiceBattle = 2,
    kKeyPokemonInfo = 0x10,
    kKeyPokemonInfoClose = 0x20,
    kKeyDiskExplanation = 0x40,
    kKeyDiskMessageClose = 0x60,
    kKeyBattleResultClose = 0x30,
    kCursorSound = 1,
    kCancelSound = 3
};


long getPanelIdx(unsigned long panel, u8 pokemonCount);
Pokemon* loadPkmFace(void* work);

static inline void clearBattleDiskPokemonPanels(void)
{
    for (s32 panel = 0; panel < 15; ++panel) {
        menuItemBiosSetSelectFlag(kMenuDiskPokemonPanels, gBattleDiskPanelItemIDs[panel], 0);
    }
}

static inline void setBattleDiskPokemonPanelFlags(void)
{
    bool selectable;
    for (s32 panel = 0; panel < 15; ++panel) {
        selectable = true;
        if (getPanelIdx(panel, gBDisk.mBattlePokemonCount) == -1) {
            selectable = false;
        } else {
            unsigned long slot = getPanelIdx(panel, gBDisk.mBattlePokemonCount);
            if (slot < 6) {
                if (heroGetPokeomCount(&gBDisk.mHeroes[0]) <= slot) {
                    selectable = false;
                }
            } else if (heroGetPokeomCount(&gBDisk.mHeroes[1]) <= slot - 6) {
                selectable = false;
            }
        }
        if (selectable) {
            switch (panel) {
            case 1:
                if (gBDisk.mBattlePokemonCount != 2) selectable = false;
                break;
            case 2:
                if (gBDisk.mBattlePokemonCount != 1) selectable = false;
                break;
            case 7:
                if (gBDisk.mBattlePokemonCount == 2) {
                    long pokemonCount = heroGetPokeomCount(&gBDisk.mHeroes[1]);
                    if (pokemonCount == 1) selectable = false;
                }
                break;
            case 8: {
                long pokemonCount;
                if (gBDisk.mBattlePokemonCount != 2 ||
                    (pokemonCount = heroGetPokeomCount(&gBDisk.mHeroes[1])) != 1) selectable = false;
                break;
            }
            case 9:
                if (menuItemBiosGetSelectFlag(kMenuDiskPokemonPanels, gBattleDiskPanelItemIDs[8])) selectable = false;
                else if (gBDisk.mBattlePokemonCount != 2) selectable = false;
                break;
            case 10:
                if (menuItemBiosGetSelectFlag(kMenuDiskPokemonPanels, gBattleDiskPanelItemIDs[8])) selectable = false;
                else if (gBDisk.mBattlePokemonCount != 1) selectable = false;
                break;
            }
        }
        menuItemBiosSetSelectFlag(kMenuDiskPokemonPanels, gBattleDiskPanelItemIDs[panel], selectable);
    }
}

extern "C" void menuBattleDiskMain(void)
{
    s32 listCursor = 0;
    s32 documentChoice;
    u8 browsing = 1;
    switch (gBDisk.mMode) {
    case kBattleDiskBrowse:
        while (browsing) {
            listCursor = gBDisk.mSelectedDiskRow;
            s32 selection = menuOpenCustom(kMenuDiskList, 0, &listCursor, 0, 1, 0, 0);
            if (selection == -1) {
                menuClose(kMenuDiskList);
                browsing = 0;
                gBDisk.mBattleDiskID = 0;
                gBDisk.mBattleResult = 0;
                continue;
            }
            gBDisk.mSelectedDiskRow = selection;
            for (s32 frame = 0; frame < 6; ++frame) {
                GSthreadSwitch();
            }
            selection += gBDisk.mFirstDiskRow;
            s32 choice = menuOpen(kMenuDiskChoice, 1);
            u8 disk = gBDisk.mDiskEntries[selection].mDiskID;
            s32 faceWork = 0;
            fightEncountDataBiosSetDiskId(fightEncountDataBiosGetPtr(kBattleDiskEncount), disk);
            fightTrainerCreateFightTrainerDataIdToHero(
                (u8)deckDataVirtualDB_GetAllyDeck(disk), 0, &gBDisk.mHeroes[0], kBattleDiskEncount);
            fightTrainerCreateFightTrainerDataIdToHero(
                (u8)deckDataVirtualDB_GetEnemyDeck(disk), 0, &gBDisk.mHeroes[1], kBattleDiskEncount);
            menuFaceLoad(0, loadPkmFace, &faceWork);
            gBDisk.mBattlePokemonCount = deckDataVirtualDB_GetPokemonNum(disk);
            gBDisk.mBattleDiskID = disk;
            clearBattleDiskPokemonPanels();
            while (menuFaceCheckLoading()) {
                GSthreadSwitch();
            }
            menuClose(kMenuDiskList);
            menuClose(kMenuDiskChoice);
            switch (choice) {
            case kDiskChoiceCancel:
                browsing = 0;
                break;
            case kDiskChoicePokemon:
                gBDisk.mPreviewElapsedSec = 0.0f;
                menuOpen(kMenuDiskPreviewLoading, 0);
                while (gBDisk.mPreviewElapsedSec < 0.2f) {
                    GSthreadSwitch();
                    gBDisk.mPreviewElapsedSec += timeGetLastFrameSec();
                }
                menuClose(kMenuDiskPreviewLoading);
                setBattleDiskPokemonPanelFlags();
                menuOpen(kMenuDiskPokemonPanels, 0);
                s32 preview = 1;
                while (preview) {
                    GSthreadSwitch();
                    if (windowGetKeyInfo()->mKeyTrg & kKeyPokemonInfo) {
                        gBDisk.mSelectedPokemonIndex = windowSearchID(kMenuDiskPokemonPanels)->mCursor.y;
                        menuOpen(kMenuBattleDiskPokemonInfo, 0);
                        for (;;) {
                            s32 selectedPokemon = gBDisk.mSelectedPokemonIndex;
                            if ((unsigned long)selectedPokemon < heroGetPokeomCount(&gBDisk.mHeroes[0])) {
                                gBDisk.mRequestedX = kAllyInfoWindowX;
                                gBDisk.mRequestedY = 0;
                            } else {
                                gBDisk.mRequestedX = 0;
                                gBDisk.mRequestedY = 0;
                            }
                            const s16 currentX = gBDisk.mCurrentX;
                            menuSetPosition(kMenuBattleDiskPokemonInfo, currentX, gBDisk.mCurrentY);
                            GSthreadSwitch();
                            if (!(windowGetKeyInfo()->mKeyTrg & kKeyPokemonInfo)) {
                                menuBattleDiskPokemonCursorMove(windowSearchID(kMenuDiskPokemonPanels), 1);
                            }
                            if (gBDisk.mSelectedPokemonIndex != windowSearchID(kMenuDiskPokemonPanels)->mCursor.y) {
                                windowCreateCursorSprite(windowSearchID(kMenuDiskPokemonPanels));
                                const s16 cursor = windowSearchID(kMenuDiskPokemonPanels)->mCursor.y;
                                windowSearchID(kMenuDiskPokemonPanels)->cursorTo = cursor;
                                gBDisk.mSelectedPokemonIndex = cursor;
                                menuPlaySe(kMenuDiskPokemonPanels, kCursorSound);
                            }
                            if (windowGetKeyInfo()->mKeyTrg & kKeyPokemonInfoClose) break;
                        }
                        menuPlaySe(kMenuDiskPokemonPanels, kCancelSound);
                        menuClose(kMenuBattleDiskPokemonInfo);
                        menuCloseSync(kMenuBattleDiskPokemonInfo, 1);
                        menuOpen(kMenuDiskPokemonPanels, 0);
                    } else if (windowGetKeyInfo()->mKeyTrg & kKeyPokemonInfoClose) {
                        preview = 0;
                    }
                }
                menuClose(kMenuDiskPokemonPanels);
                menuCloseSync(kMenuDiskPokemonPanels, 1);
                break;
            case kDiskChoiceBattle:
                break;
            }
        }
        break;
    case kBattleDiskFight:
        gBDisk.mBattleResult = 0;
        menuOpen(kMenuDiskBattleResult, 0);
        for (;;) {
            GSthreadSwitch();
            if (windowGetKeyInfo()->mKeyTrg == kKeyDiskExplanation) {
                winMsgOpen(2, deckDataVirtualDB_GetExplanetion1(gBDisk.mBattleDiskID), 1, 1);
                do {
                    GSthreadSwitch();
                } while (!(windowGetKeyInfo()->mKeyTrg & kKeyDiskMessageClose));
                winMsgClose(1);
            } else if (windowGetKeyInfo()->mKeyTrg == kKeyPokemonInfo) {
                break;
            }
        }
        gBDisk.mBattleResult = fightEncount(kBattleDiskEncount);
        menuOpen(kMenuDiskBattleResult, 0);
        GSthreadSwitch();
        while (gBDisk.mResultAnimationFrame < gBDisk.mResultAnimationEndFrame) {
            GSthreadSwitch();
        }
        while (!(windowGetKeyInfo()->mKeyTrg & kKeyBattleResultClose)) {
            GSthreadSwitch();
        }
        menuClose(kMenuDiskBattleResult);
        break;
    case kBattleDiskDocument: {
        documentChoice = 0;
        menuOpenCustom(kMenuDiskDocumentFrame, windowGetActiveID(), 0, 2, 0, 0);
        gBDisk.mDocumentChoice = menuOpenCustom(kMenuDocumentChoice, 0, &documentChoice, 0, 1, 0);
        menuCloseCustom(kMenuDocumentChoice, 0, 1);
        menuClose(kMenuDiskDocumentFrame);
        menuCloseSync(kMenuDiskDocumentFrame, 1);
        menuOffScreenSetPriority(1);
        break;
    }
    }
    GSfloorPop();
    if (gBDisk.mMode == kBattleDiskBrowse || gBDisk.mMode == kBattleDiskDocument) {
        floorSetFadeScript(0, 0);
    }
}

static inline void updateDiskListFlags()
{
    for (s32 diskIndex = 0; diskIndex < 8; diskIndex++) {
        if (diskIndex < gBDisk.mDiskCount) {
            menuItemBiosSetSelectFlag(kMenuDiskChoice, gBattleDiskListItemIDs[diskIndex], 1);
        } else {
            menuItemBiosSetSelectFlag(kMenuDiskChoice, gBattleDiskListItemIDs[diskIndex], 0);
        }
    }
}

extern "C" __declspec(export) void menuBattleDiskInit(void)
{
    u16 itemCount;
    ItemSlot* items = heroItemGetItemKindToItemAryPtr(
        Hero::getHeroPtr(), kItemKindBattleDisk, &itemCount, 0, 0, 0);
    u16 diskCount = 0;
    fightReadResource(kBattleDiskResourceGroup, kBattleDiskPreviewResource);
    for (s32 itemIndex = 0; itemIndex < itemCount; ++itemIndex, ++items) {
        ItemSlot* item = items;
        if (itemCheckValid(item)) {
            gBDisk.mDiskEntries[diskCount].mItemID = itemBiosGetItemDataId(item);
            gBDisk.mDiskEntries[diskCount].mItemNameMessage = itemDataBiosGetName(itemDataBiosGetPtr(gBDisk.mDiskEntries[diskCount].mItemID));
            gBDisk.mDiskEntries[diskCount].mItemDocumentMessage = itemDataBiosGetDoc(itemDataBiosGetPtr(gBDisk.mDiskEntries[diskCount].mItemID));
            gBDisk.mDiskEntries[diskCount].mDiskID = (u8)(gBDisk.mDiskEntries[diskCount].mItemID - kBattleDiskItemBase);
            gBDisk.mDiskEntries[diskCount].mBattleDescriptionMessage = deckDataVirtualDB_GetExplanetion1(gBDisk.mDiskEntries[diskCount].mDiskID);
            gBDisk.mDiskEntries[diskCount].mBattleDetailsMessage = deckDataVirtualDB_GetExplanetion2(gBDisk.mDiskEntries[diskCount].mDiskID);
            gBDisk.mDiskEntries[diskCount].mBattleTitleMessage = deckDataVirtualDB_GetTitle(gBDisk.mDiskEntries[diskCount].mDiskID);
            ++diskCount;
        }
    }
    gBDisk.mDiskCount = (s16)diskCount;
    updateDiskListFlags();
}

/* The battle disk menu keeps its temporary party and selection state here. */
MenuBattleDiskWork gBDisk;

extern "C" void menuBattleDiskCloseDoc(void)
{
    menuClose(kMenuDiskDocumentFrame);
}

extern "C" void menuBattleDiskOpenDoc(u8 disk)
{
    gBDisk.mBattleDiskID = disk;
    menuOpenCustom(kMenuDiskDocumentFrame, windowGetActiveID(), 0, 2, 0, 0);
}

extern "C" {

long menuBattleDiskOpenDocPush(u8 disk)
{
    gBDisk.mBattleDiskID = disk;
    gBDisk.mMode = kBattleDiskDocument;
    const f32 fadeSeconds = 8.0f / (f32)GSgfxVideoGetVsyncRate();
    menuCreateOffScreen(fadeSeconds, 50.0f);
    menuOffScreenSetPriority(0);
    menuOffScreenFadeSync(1);
    GSfloorPush(kFloorBattleDiskDocument);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    menuReleaseOffScreen(8.0f / (f32)GSgfxVideoGetVsyncRate());
    gBDisk.mMode = kBattleDiskFight;
    return gBDisk.mDocumentChoice;
}

u16 menuBattleDiskGetResult(void)
{
    return gBDisk.mBattleResult;
}

u8 menuBattleDiskGetSelectID(void)
{
    return gBDisk.mBattleDiskID;
}

void menuBattleDiskOpenFight(void)
{
    if (gBDisk.mBattleDiskID != 0) {
        gBDisk.mMode = kBattleDiskFight;
        GSfloorPush(kFloorBattleDisk);
    }
}

void menuBattleDiskOpen(void)
{
    gBDisk.mMode = kBattleDiskBrowse;
    const f32 fadeSeconds = 20.0f / (f32)GSgfxVideoGetVsyncRate();
    menuCreateOffScreen(fadeSeconds, 50.0f);
    GSfloorPush(kFloorBattleDisk);
    floorSetFadeScript(0, 0);
    floorWaitForChange();
    menuReleaseOffScreen(20.0f / (f32)GSgfxVideoGetVsyncRate());
}
}
