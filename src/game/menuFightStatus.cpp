#include <string.h>
#include <math.h>
#include <menu.h>
#include <menuFight.h>
#include <menuFace.h>
#include <menuDataBios.h>
#include <menuSeqBios.h>
#include <deck.h>
#include <fightFloor.h>
#include <fightSide.h>
#include <fightPokemon.h>
#include <menuCBBios.h>
#include <GSmsg.h>
#include <GSprim.h>
#include <Pokemon.h>
#include <darkPokemon.h>
#include <app/fight/fightKind.h>
#include <menuFightStatus.h>
#include <menuFightStatusInternal.h>
#include <menuIds.h>
#include <menuPokemon.h>
#include <msgctrl.h>
#include <time.h>
#include <window.h>
#include <winSprite.h>
#include <GSsndPlayback.h>
#include <menuPokemonMessages.h>
#include <gamedata.h>

enum StatusItem {
    STATUS_ITEM_TARGET_MARK = 208,
    STATUS_ITEM_TARGET_SUBBAR_MARK = 209,
    STATUS_ITEM_FACE = 210,
    STATUS_ITEM_NORMAL_PANEL = 211,
    STATUS_ITEM_NORMAL_SUBBAR_PANEL = 212,
    STATUS_ITEM_SHADOW_PANEL = 213,
    STATUS_ITEM_SHADOW_SUBBAR_PANEL = 214,
    STATUS_ITEM_HYPER_PANEL = 215,
    STATUS_ITEM_HYPER_SUBBAR_PANEL = 216,
    STATUS_ITEM_HP_GAUGE = 218,
    STATUS_ITEM_EXP_GAUGE_BASE = 219,
    STATUS_ITEM_EXP_GAUGE = 220,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_5 = 221,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_4 = 222,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_3 = 223,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_2 = 224,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_1 = 225,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_5 = 226,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_4 = 227,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_3 = 228,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_2 = 229,
    STATUS_ITEM_SHADOW_GAUGE_SEGMENT_1 = 230,
    STATUS_ITEM_NAME = 231,
    STATUS_ITEM_HP_TEXT = 232,
    STATUS_ITEM_LEVEL_TEXT = 233,
    STATUS_ITEM_LEVEL_LABEL = 234,
    STATUS_ITEM_JOUTAI = 235,
    STATUS_ITEM_TARGET_MARK_MIRRORED = 236,
    STATUS_ITEM_FACE_MIRRORED = 237,
    STATUS_ITEM_NORMAL_PANEL_MIRRORED = 238,
    STATUS_ITEM_SHADOW_PANEL_MIRRORED = 239,
    STATUS_ITEM_HYPER_PANEL_MIRRORED = 240,
    STATUS_ITEM_HP_GAUGE_MIRRORED = 242,
    STATUS_ITEM_NAME_MIRRORED = 243,
    STATUS_ITEM_HP_TEXT_MIRRORED = 244,
    STATUS_ITEM_LEVEL_TEXT_MIRRORED = 245,
    STATUS_ITEM_ZUKAN_MARK = 246,
    STATUS_ITEM_LEVEL_LABEL_MIRRORED = 247,
    STATUS_ITEM_JOUTAI_MIRRORED = 248,
};

enum MenuFightTargetItem {
    TARGET_ITEM_LEFT = 312,
    TARGET_ITEM_DOWN = 313,
    TARGET_ITEM_UP = 314,
    TARGET_ITEM_RIGHT = 315
};

// Fill color of the HP gauge, packed as RGBA bytes. The bar is colored with
// the red and green channels only; the window contributes the alpha byte.
enum StatusHpColor {
    STATUS_HP_COLOR_RED = 128 << 24,
    STATUS_HP_COLOR_YELLOW = (100 << 24) | (100 << 16),
    STATUS_HP_COLOR_GREEN = 128 << 16,
};

// White the status panel is written and marked in. Text takes its alpha byte
// from the window, the target mark sets the whole word and then subtracts the
// flash from its green and blue channels, tinting it red as it pulses.
enum StatusColor {
    STATUS_COLOR_TEXT_WHITE = (255 << 24) | (255 << 16) | (255 << 8),
    STATUS_COLOR_MARK_WHITE = (255 << 24) | (255 << 16) | (255 << 8) | 255,
};

enum StatusSprite {
    STATUS_SPRITE_HP_GREEN = 144,
    STATUS_SPRITE_SHADOW_GAUGE = 145,
    STATUS_SPRITE_EXP_GAUGE = 146,
    STATUS_SPRITE_HP_YELLOW = 159,
    STATUS_SPRITE_HP_RED = 160,
};

f32 menuStatusTime;
f32 menuStatusOldTime;
static f32 statusPercentageOf(f32 full, f32 percent);
static inline void statusVertexPosition(s16 x, s16 y)
{
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y;
}

#pragma dont_inline on
void menuUpdateStatusTime(void)
{
    f32 time = timeGetLastFrameSec();
    if (time != menuStatusOldTime) {
        menuStatusOldTime = time;
        menuStatusTime += time;
    }
}

extern "C" s32 menuFightStatusCtrl(tagWINDOW_WORK *window)
{
    MENU_FIGHT_STATUS_ANIM *anim = (MENU_FIGHT_STATUS_ANIM *)windowGetFreeWork(window);
    u32 fightSeqPlaying = 0;

    if (window->mState == WINDOW_STATE_INIT) {
        MENU_POKEMON_STATUS *status =
            (MENU_POKEMON_STATUS *)windowAllocMemory(window, sizeof(MENU_POKEMON_STATUS));
        if (status != NULL) {
            memcpy(status, (void *)window->mArgs[0], sizeof(MENU_POKEMON_STATUS));
            if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_BINGO)) status->mHideSubbar = 1;
            if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_VIRTUAL_TUTORIAL)) status->mHideSubbar = 1;
            if (fightKind_CheckFightKind(FIGHT_BATTLE_KIND_BATTLE_DISK)) status->mHideSubbar = 1;
            if (menuFightGetStatus() != 0) menuFightSetSeq();
        }
    } else if (window->mState == WINDOW_STATE_WAITING) {
        if (menuFightGetStatus() != 0) {
            if (window->mSeq.mData == menuSeqBiosGetPtr(MENU_SEQ_FIGHT_STATUS_LEFT_DOWN_IN)) fightSeqPlaying = 1;
            if (window->mSeq.mData == menuSeqBiosGetPtr(MENU_SEQ_FIGHT_STATUS_UP_RIGHT_IN)) fightSeqPlaying = 1;
            if (fightSeqPlaying) menuFightSetStatus(0);
        }
    }

    windowGetAllocPtr(window);
    if (anim->hpFrames != 0) {
        anim->hpElapsed += (s32)(60.0f * timeGetLastFrameSec());
        if (anim->hpElapsed > anim->hpFrames) anim->hpFrames = 0;
    }
    if (anim->expFrames != 0) {
        anim->expElapsed += (s32)(60.0f * timeGetLastFrameSec());
        if (anim->expElapsed > anim->expFrames) {
            anim->expFrames = 0;
            GSsndStopNow(MENU_SE_EXP_GAUGE_ANIM);
        }
    }
    menuUpdateStatusTime();
    return 0;
}

extern "C" void menuFightDrawCheckDark(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_POKEMON_STATUS *status = (MENU_POKEMON_STATUS *)windowGetAllocPtr(window);
    s16 itemID = sprite->mItemID;
    bool disp = true;
    u8 hideSubbar = status->mHideSubbar;

    switch (itemID) {
    case STATUS_ITEM_NORMAL_PANEL:
    case STATUS_ITEM_NORMAL_SUBBAR_PANEL:
    case STATUS_ITEM_EXP_GAUGE_BASE:
    case STATUS_ITEM_EXP_GAUGE:
    case STATUS_ITEM_NORMAL_PANEL_MIRRORED:
        if (status->mBallType != POKEMON_BALL_TYPE_NORMAL) {
            disp = false;
        } else {
            disp = true;
        }
        break;
    case STATUS_ITEM_SHADOW_PANEL:
    case STATUS_ITEM_SHADOW_SUBBAR_PANEL:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_5:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_4:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_3:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_2:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_1:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_5:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_4:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_3:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_2:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_1:
    case STATUS_ITEM_SHADOW_PANEL_MIRRORED:
        if (status->mBallType != POKEMON_BALL_TYPE_NORMAL) {
            disp = true;
        } else {
            disp = false;
        }
        break;
    }
    switch (itemID) {
    case STATUS_ITEM_NORMAL_SUBBAR_PANEL:
    case STATUS_ITEM_SHADOW_SUBBAR_PANEL:
    case STATUS_ITEM_EXP_GAUGE_BASE:
    case STATUS_ITEM_EXP_GAUGE:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_5:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_4:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_3:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_2:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_BASE_1:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_5:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_4:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_3:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_2:
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_1:
        if (hideSubbar == 1) {
            disp = false;
        }
        break;
    }
    winSpriteSetDisp(sprite, disp);
}

s32 menuFightDrawDarkPanel(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                           Pokemon *pokemon, MENU_POKEMON_STATUS *status)
{
    s16 rightMsgWidth;
    s16 x;

    if (pokemon != NULL && pokemon->isDarkPokemon()) {
        msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)GSmsgGetStringPtr(MSG_SHADOW_LEVEL_TEXT));
        rightMsgWidth = GSmsgGetMessageSize(MSG_FIGHT_STATUS_TEXT) >> 16;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite),
                    (s16)(sprite->mWidth - rightMsgWidth), 0,
                    window->mRgba.mColorA | STATUS_COLOR_TEXT_WHITE, MSG_FIGHT_STATUS_TEXT);

        msgctrlSetValue(MSGCTRL_NUM_VALUE, deckGetDeckDarkPokemon(
                                pokemon->getDarkPokemon()->getDarkPokemonID())
                                ->getPlayerLevel());
        x = sprite->mWidth - rightMsgWidth - (GSmsgGetMessageSize(MSG_FIGHT_STATUS_NUMBER) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 1,
                    window->mRgba.mColorA | STATUS_COLOR_TEXT_WHITE, MSG_FIGHT_STATUS_NUMBER);
        return 1;
    }
    return 0;
}
#pragma dont_inline reset

extern "C" void menuFightDrawParam(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    static f32 cFlash = 0.0f;
    MENU_POKEMON_STATUS *status = (MENU_POKEMON_STATUS *)windowGetAllocPtr(window);
    MENU_FIGHT_STATUS_ANIM *anim = (MENU_FIGHT_STATUS_ANIM *)windowGetFreeWork(window);
    u32 color;
    s16 checkSubbarFlag = 0;
    u32 effectColor;
    GSgfxLayer *layer;
    menuFightDrawCheckDark(window, sprite);
    if (!winSpriteGetDisp(sprite)) return;
    color = window->mRgba.mColorA | STATUS_COLOR_TEXT_WHITE;
    switch (sprite->mItemID) {
    case STATUS_ITEM_SHADOW_PANEL:
    case STATUS_ITEM_SHADOW_SUBBAR_PANEL:
    case STATUS_ITEM_SHADOW_PANEL_MIRRORED:
        if (status->mBallType == POKEMON_BALL_TYPE_HYPER) winSpriteSetDisp(sprite, false);
        break;
    case STATUS_ITEM_HYPER_PANEL:
    case STATUS_ITEM_HYPER_SUBBAR_PANEL:
    case STATUS_ITEM_HYPER_PANEL_MIRRORED:
        if (status->mBallType == POKEMON_BALL_TYPE_HYPER) winSpriteSetDisp(sprite, true);
        else winSpriteSetDisp(sprite, false);
        break;
    case STATUS_ITEM_FACE:
    case STATUS_ITEM_FACE_MIRRORED:
        menuFaceDispFromID(window, sprite, status->mFaceId);
        break;
    case STATUS_ITEM_HP_TEXT_MIRRORED:
        checkSubbarFlag = 1;
    case STATUS_ITEM_HP_TEXT: {
        s32 floorStatus = fightFloorGetStatus(NULL, 0, FIGHT_FLOOR_STATUS_POKEMON_STATUS_MENU_SUBBAR_FLAG, 0);
        if (floorStatus && checkSubbarFlag) break;
        s16 hp;
        if (anim->hpFrames) {
            f32 value = (f32)anim->hpElapsed / anim->hpFrames * (status->mHp - anim->hpStart) + anim->hpStart;
            hp = value;
            if (!hp && value > 0.0f) hp = 1;
        } else hp = status->mHp;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 32, 0, color, MSG_PP_SLASH);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, hp);
        s16 x = 24 - (s16)(GSmsgGetMessageSize(MSG_FIGHT_STATUS_NUMBER) >> 16);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 1, color, MSG_FIGHT_STATUS_NUMBER);
        msgctrlSetValue(MSGCTRL_NUM_VALUE, status->mMaxHp);
        x = GSmsgGetMessageSize(MSG_FIGHT_STATUS_NUMBER) >> 16;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), (s16)(sprite->mWidth - x), 1, color, MSG_FIGHT_STATUS_NUMBER);
        break;
    }
    case STATUS_ITEM_LEVEL_LABEL:
    case STATUS_ITEM_LEVEL_LABEL_MIRRORED:
        winSpriteSetDisp(sprite, true);
        if (status->mHideLevelInfo) winSpriteSetDisp(sprite, false);
        break;
    case STATUS_ITEM_LEVEL_TEXT:
    case STATUS_ITEM_LEVEL_TEXT_MIRRORED: {
        if (status->mHideLevelInfo) break;
        if (!menuCBBios_CurrentlyWorkingToolBattle() && status->mBallType) {
            FIGHT_TRAINER *trainer = fightSideGetValidFightTrainerPtr(fightFloorGetValidFightSidePtr((FIGHT_FLOOR *)fightFloorGetNowPtr(), 0), 0);
            FightOutPokemon *pokemon[6];
            fightFloorGetFightTrainerFightOutPokemonPtrAry((FIGHT_FLOOR *)fightFloorGetNowPtr(), trainer, pokemon, 0, 0);
            if (window->mMenuID == FIGHT_STATUS_MENU_LEFT) {
                if (!fightTrainerIsGcHero(trainer)) {
                    fightFloorGetFightTrainerFightOutPokemonPtrAry((FIGHT_FLOOR *)fightFloorGetNowPtr(), trainer, pokemon, 1, 0);
                    if (menuFightDrawDarkPanel(window, sprite, fightOutPokemonGetPokemonPtr(pokemon[0]), status)) break;
                }
            } else if (window->mMenuID == FIGHT_STATUS_MENU_DOWN) {
                if (!fightTrainerIsGcHero(trainer)) {
                    s32 count = fightFloorGetFightTrainerFightOutPokemonPtrAry((FIGHT_FLOOR *)fightFloorGetNowPtr(), trainer, pokemon, 1, 0);
                    if (count == 1) {
                        if (menuFightDrawDarkPanel(window, sprite, fightOutPokemonGetPokemonPtr(pokemon[0]), status)) break;
                    } else {
                        if (menuFightDrawDarkPanel(window, sprite, fightOutPokemonGetPokemonPtr(pokemon[1]), status)) break;
                    }
                }
            } else if (window->mMenuID == FIGHT_STATUS_MENU_UP) {
                fightFloorGetFightTrainerFightOutPokemonPtrAry((FIGHT_FLOOR *)fightFloorGetNowPtr(), trainer, pokemon, 0, 0);
                if (menuFightDrawDarkPanel(window, sprite, fightOutPokemonGetPokemonPtr(pokemon[0]), status)) break;
            } else if (window->mMenuID == FIGHT_STATUS_MENU_RIGHT) {
                s32 count = fightFloorGetFightTrainerFightOutPokemonPtrAry((FIGHT_FLOOR *)fightFloorGetNowPtr(), trainer, pokemon, 0, 0);
                if (count == 1) {
                    if (menuFightDrawDarkPanel(window, sprite, fightOutPokemonGetPokemonPtr(pokemon[0]), status)) break;
                } else {
                    if (menuFightDrawDarkPanel(window, sprite, fightOutPokemonGetPokemonPtr(pokemon[1]), status)) break;
                }
            }
        }
        msgctrlSetValue(MSGCTRL_NUM_VALUE, status->mLevel);
        s16 width = GSmsgGetMessageSize(MSG_FIGHT_STATUS_NUMBER) >> 16;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), (s16)(sprite->mWidth - width), 1, window->mRgba.mColorA | STATUS_COLOR_TEXT_WHITE, MSG_FIGHT_STATUS_NUMBER);
        break;
    }
    case STATUS_ITEM_NAME:
    case STATUS_ITEM_NAME_MIRRORED: {
        msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)status->mName);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 1, color, MSG_TEXT_IN_RECT);
        s16 x = (GSmsgGetMessageSize(MSG_TEXT_IN_RECT) >> 16) + 4;
        MenuPokemonMessage message;
        switch (status->mSex) {
        case POKEMON_SEX_MALE: message = MSG_SEX_MALE; break;
        case POKEMON_SEX_FEMALE: message = MSG_SEX_FEMALE; break;
        case POKEMON_SEX_NONE: default: message = MSG_NONE; break;
        }
        if ((u32)message != MSG_NONE) {
            msgctrlSetValue(MSGCTRL_VALUE_STRING, (u32)GSmsgGetStringPtr(message));
            GSmsgPrint2(winSpriteGetLayerID(window, sprite), x, 0, color, MSG_FIGHT_STATUS_TEXT);
        }
        break;
    }
    case STATUS_ITEM_JOUTAI:
    case STATUS_ITEM_JOUTAI_MIRRORED:
        windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, color, window, status->mJoutaiId, 0, winSpriteGetLayerID(window, sprite));
        break;
    case STATUS_ITEM_HP_GAUGE:
    case STATUS_ITEM_HP_GAUGE_MIRRORED: {
        s16 width;
        layer = GSgfxLayerFindByID(winSpriteGetLayerID(window, sprite));
        if (layer == NULL) break;
        f32 hp;
        if (anim->hpFrames)
            hp = (f32)anim->hpElapsed / anim->hpFrames * (status->mHp - anim->hpStart) + anim->hpStart;
        else hp = status->mHp;

        f32 maxHP = status->mMaxHp;
        if (anim->hpFrames) {
            if (anim->hpStart <= 0) effectColor = 0;
            else if (anim->hpStart <= statusPercentageOf(maxHP, 20.0f)) effectColor = STATUS_HP_COLOR_RED;
            else if (anim->hpStart <= statusPercentageOf(maxHP, 50.0f)) effectColor = STATUS_HP_COLOR_YELLOW;
            else effectColor = STATUS_HP_COLOR_GREEN;
            effectColor |= window->mRgba.mColorA;
            width = (anim->hpStart * sprite->mWidth + (maxHP - 1.0f)) / maxHP;
            layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
            layer->setRenderStateAndSelect(GSgfxVFBuildTemp(GX_POS_XY, GX_S16, 0, GX_NRM_XYZ, 1, GX_RGBA8, 0, 0, 0));
            layer->beginSurface(4);
            statusVertexPosition(0, 0);
            GSgfxFifo.u32 = effectColor;
            statusVertexPosition(width, 0);
            GSgfxFifo.u32 = effectColor;
            statusVertexPosition(0, sprite->mHeight);
            GSgfxFifo.u32 = effectColor;
            statusVertexPosition(width, sprite->mHeight);
            GSgfxFifo.u32 = effectColor;
            layer->endSurface();
        }
        u16 texture;
        if (hp <= 0.0f) texture = 0;
        else if (hp <= statusPercentageOf(maxHP, 20.0f)) texture = STATUS_SPRITE_HP_RED;
        else if (hp <= statusPercentageOf(maxHP, 50.0f)) texture = STATUS_SPRITE_HP_YELLOW;
        else texture = STATUS_SPRITE_HP_GREEN;
        width = (hp * sprite->mWidth + (maxHP - 1.0f)) / maxHP;
        if (texture) windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, texture, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case STATUS_ITEM_EXP_GAUGE: {
        s32 exp;
        if (anim->expFrames) exp = (f32)anim->expElapsed / anim->expFrames * (status->mProgressValue.mExpIntoLevel - anim->expStart) + anim->expStart;
        else exp = status->mProgressValue.mExpIntoLevel;
        s32 maxEXP = status->mProgressRange.mLevelExpSpan;
        if (exp > maxEXP) exp = maxEXP;
        if (maxEXP > 0) {
            s16 width = (exp * sprite->mWidth + (maxEXP - 1)) / maxEXP;
            if (width != 0) {
                cFlash = 0.0f;
                windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, STATUS_SPRITE_EXP_GAUGE, 0, winSpriteGetLayerID(window, sprite));
            } else if (anim->expDone != 0) {
                windowDrawSprite2(0, 0, sprite->mWidth, sprite->mHeight, color, window, STATUS_SPRITE_EXP_GAUGE, 0, winSpriteGetLayerID(window, sprite));
                layer = GSgfxLayerFindByID(winSpriteGetLayerID(window, sprite));
                u8 flash = 255.0f - 254.0f * cFlash;
                layer->mPrimitiveType = GSGFX_QUADS;
                GSprimLayerLineWidth(layer, 6);
                layer->setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_XY_S16_COLOR));
                layer->beginSurface(4);
                statusVertexPosition(0, 0);
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                statusVertexPosition(sprite->mWidth, 0);
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                statusVertexPosition(sprite->mWidth, sprite->mHeight);
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                statusVertexPosition(0, sprite->mHeight);
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                GSgfxFifo.u8 = flash;
                layer->endSurface();
                cFlash += timeGetLastFrameSec();
                if (1.0 <= cFlash) { cFlash = 1.0f; anim->expDone = 0; }
            }
        }
        break;
    }
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_1: {
        f32 progress = status->mProgressValue.mPurification / status->mProgressRange.mShadowGauge;
        s16 width;
        if (progress < 0.2f) width = sprite->mWidth * (progress / 0.2f);
        else width = sprite->mWidth;
        if (width < 0) width = 0;
        windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, STATUS_SPRITE_SHADOW_GAUGE, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_2: {
        f32 progress = status->mProgressValue.mPurification / status->mProgressRange.mShadowGauge;
        s16 width;
        if (progress < 0.4f) width = sprite->mWidth * ((progress - 0.2f) / 0.2f);
        else width = sprite->mWidth;
        if (width < 0) width = 0;
        windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, STATUS_SPRITE_SHADOW_GAUGE, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_3: {
        f32 progress = status->mProgressValue.mPurification / status->mProgressRange.mShadowGauge;
        s16 width;
        if (progress < 0.6f) width = sprite->mWidth * ((progress - 0.4f) / 0.2f);
        else width = sprite->mWidth;
        if (width < 0) width = 0;
        windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, STATUS_SPRITE_SHADOW_GAUGE, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_4: {
        f32 progress = status->mProgressValue.mPurification / status->mProgressRange.mShadowGauge;
        s16 width;
        if (progress < 0.8f) width = sprite->mWidth * ((progress - 0.6f) / 0.2f);
        else width = sprite->mWidth;
        if (width < 0) width = 0;
        windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, STATUS_SPRITE_SHADOW_GAUGE, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case STATUS_ITEM_SHADOW_GAUGE_SEGMENT_5: {
        f32 progress = status->mProgressValue.mPurification / status->mProgressRange.mShadowGauge;
        s16 width;
        if (progress < 1.0f) width = sprite->mWidth * ((progress - 0.8f) / 0.2f);
        else width = sprite->mWidth;
        if (width < 0) width = 0;
        windowDrawSprite2(0, 0, width, sprite->mHeight, color, window, STATUS_SPRITE_SHADOW_GAUGE, 0, winSpriteGetLayerID(window, sprite));
        break;
    }
    case STATUS_ITEM_ZUKAN_MARK:
        winSpriteSetDisp(sprite, false);
        if (gamedataBiosGetFlag(NULL, GAMEDATASAVE_FIELD_ZUKAN_FLAG, 0)) winSpriteSetDisp(sprite, true);
        break;
    case STATUS_ITEM_TARGET_SUBBAR_MARK:
        checkSubbarFlag = 1;
    case STATUS_ITEM_TARGET_MARK:
    case STATUS_ITEM_TARGET_MARK_MIRRORED: {
        s32 selected = 0;
        winSpriteSetDisp(sprite, false);
        if (!menuCloseSync(MENU_ID_FIGHT_TARGET_SELECT, false)) break;
        MenuFightTargetItem item = (MenuFightTargetItem)menuGetCursorItemID(MENU_ID_FIGHT_TARGET_SELECT);
        switch (window->mMenuID) {
        case FIGHT_STATUS_MENU_LEFT: if (item == TARGET_ITEM_LEFT) selected = 1; break;
        case FIGHT_STATUS_MENU_DOWN: if (item == TARGET_ITEM_DOWN) selected = 1; break;
        case FIGHT_STATUS_MENU_UP: if (item == TARGET_ITEM_UP) selected = 1; break;
        case FIGHT_STATUS_MENU_RIGHT: if (item == TARGET_ITEM_RIGHT) selected = 1; break;
        }
        u32 ticks = (u32)(100.0f * menuStatusTime) % 100;
        sprite->mColor = STATUS_COLOR_MARK_WHITE;
        f32 pulse = sin(3.14f * (0.01f * ticks));
        u8 flash = pulse * 200;
        sprite->mRgba.mGreen -= flash;
        sprite->mRgba.mBlue -= flash;
        if (selected) {
            if (checkSubbarFlag) {
                s32 floorStatus = fightFloorGetStatus(NULL, 0, FIGHT_FLOOR_STATUS_POKEMON_STATUS_MENU_SUBBAR_FLAG, 0);
                if (floorStatus) winSpriteSetDisp(sprite, true);
            } else winSpriteSetDisp(sprite, true);
        }
        break;
    }
    }
}

static f32 statusPercentageOf(f32 full, f32 percent)
{
    if (percent < 0.0f) return 0.0f;
    if (percent > 100.0f) return full;
    return percent * full / 100.0f;
}
