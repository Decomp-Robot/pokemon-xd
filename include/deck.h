#ifndef DECK_H
#define DECK_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* File-system archive groups the fight resources are read from. */
enum DeckResourceGroup {
    DECK_RESOURCE_GROUP = 2114,  /* the deck archive group */
    COMMON_RESOURCE_GROUP = 10   /* the common archive group */
};

/* The Dark Pokemon deck. */
enum DeckResourceId {
    DECK_RESOURCE_DARK_POKEMON = 0x184D3400
};

u32 deckGetTrainerCount(void);
u32 getDeckResourceID(u8 deckId);
char *getDeckMsgID(u16 trainerId, u8 deckId);

extern u32 deck_group_data_number;
extern u32 deck_group_data[];

#ifdef __cplusplus
}

/* One entry of the status determination table: the upper and lower bounds of
 * a single category share one byte, the maximum in the high nibble and the
 * minimum in the low nibble. */
struct DeckStatusDetermVal {
    u8 mMax : 4;
    u8 mMin : 4;
};

/*
 * Deck trainer AI parameters: the per-trainer rates, thresholds and decision
 * flags the deck AI reads. The accessors work on the record in place and
 * accept a null record.
 */
class DeckTrainerAI {
public:
    void setTotalCorrection(u8 correction);
    u8 getTotalCorrection() const;

    void setWazaKindParam(u8 index, u8 param);
    u8 getWazaKindParam(u8 index) const;
    void setWazaKindID(u8 index, u16 id);
    u16 getWazaKindID(u8 index) const;

    void setAttrTypeParam(u8 index, u8 param);
    u8 getAttrTypeParam(u8 index) const;
    void setAttrTypeID(u8 index, u8 id);
    u8 getAttrTypeID(u8 index) const;

    void setLastPPDetermRate(u8 rate);
    u8 getLastPPDetermRate() const;
    void setLowHPDetermRate(u8 rate);
    u8 getLowHPDetermRate() const;
    u8 getAnnihilateDetermRate() const;
    void setHitRate(u8 rate);
    u8 getHitRate() const;
    void setItemUseRate(u8 rate);
    u8 getItemUseRate() const;
    void setPokeChgRate(u8 rate);
    u8 getPokeChgRate() const;

    void setStatusDetermValTotalMin(u8 value);
    u8 getStatusDetermValTotalMin() const;
    void setStatusDetermValTotalMax(u8 value);
    u8 getStatusDetermValTotalMax() const;
    u8 getStatusDetermValMin(u8 index) const;
    u8 getStatusDetermValMax(u8 index) const;

    void setRespectPokemonCondJuge(bool value);
    bool getRespectPokemonCondJuge() const;
    void setRespectWazaRiskJudge(bool value);
    bool getRespectWazaRiskJudge() const;
    void setRespectWazaHit(bool value);
    bool getRespectWazaHit() const;
    void setRespectRole(bool value);
    bool getRespectRole() const;
    void setRespectWazaAssmntVal(bool value);
    bool getRespectWazaAssmntVal() const;
    void setSelectRandomWaza(bool value);
    bool getSelectRandomWaza() const;
    void setSelectRandomDefence(bool value);
    bool getSelectRandomDefence() const;
    void setCalcWazaDamage(bool value);
    bool getCalcWazaDamage() const;
    void setAbilityDetermination(bool value);
    bool getAbilityDetermination() const;
    void setAttrDetermination(bool value);
    bool getAttrDetermination() const;
    void setRespectKeyPlayer(bool value);
    bool getRespectKeyPlayer() const;
    void setSelectAceBoss(bool value);
    bool getSelectAceBoss() const;
    void setOrder(bool value);
    bool getOrder() const;
    void setNotSameWeekPoint(bool value);
    bool getNotSameWeekPoint() const;
    void setSelectRandomPokemon(bool value);
    bool getSelectRandomPokemon() const;

private:
    // 0x00: AI decision flags.
    u8 mSelectRandomPokemon : 1;
    u8 mNotSameWeekPoint : 1;
    u8 mOrder : 1;
    u8 mSelectAceBoss : 1;
    u8 mRespectKeyPlayer : 1;
    u8 mAttrDetermination : 1;
    u8 mAbilityDetermination : 1;
    u8 mCalcWazaDamage : 1;
    // 0x01: AI decision flags.
    u8 mSelectRandomDefence : 1;
    u8 mSelectRandomWaza : 1;
    u8 mRespectWazaAssmntVal : 1;
    u8 mRespectRole : 1;
    u8 mRespectWazaHit : 1;
    u8 mRespectWazaRiskJudge : 1;
    u8 mRespectPokemonCondJuge : 1;
    u8 mUnusedFlag : 1; // 0x01 bit 0: unused.
    u8 mUnused[2]; // 0x02
    DeckStatusDetermVal mStatusDetermVal[8]; // 0x04: determination range of each stat the AI judges.
    u8 mStatusDetermValTotalMax; // 0x0C
    u8 mStatusDetermValTotalMin; // 0x0D
    u8 mPokeChgRate; // 0x0E
    u8 mItemUseRate; // 0x0F
    u8 mHitRate; // 0x10
    u8 mAnnihilateDetermRate; // 0x11
    u8 mLowHPDetermRate; // 0x12
    u8 mLastPPDetermRate; // 0x13
    u8 mAttrTypeID[2]; // 0x14
    u8 mAttrTypeParam[2]; // 0x16
    u16 mWazaKindID[2]; // 0x18
    u8 mWazaKindParam[2]; // 0x1C
    u8 mTotalCorrection; // 0x1E
};
#endif

#endif
