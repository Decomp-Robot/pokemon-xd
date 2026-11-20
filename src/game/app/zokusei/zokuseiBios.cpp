#include <zokusei.h>

// Matchup of the attacking type against one defending type.
u16 zokuseiBiosGetWazaJoutai(u32 zokuseiDataId, u32 defenseZokuseiDataId)
{
    if (zokuseiDataId >= *zokusei_data_number) return 0;
    if (defenseZokuseiDataId >= ZOKUSEI_DATA_ID_COUNT) return 0;
    return zokusei_data[zokuseiDataId].joutai[defenseZokuseiDataId];
}

// Message ID of the type's name.
u32 zokuseiBiosGetName(u16 zokuseiDataId)
{
    if (zokuseiDataId >= *zokusei_data_number) return 0;
    return zokusei_data[zokuseiDataId].nameMessageId;
}

// Battle kind of moves belonging to the type.
u8 zokuseiBiosGetWazaKind(u16 zokuseiDataId)
{
    if (zokuseiDataId >= *zokusei_data_number) return 0;
    return zokusei_data[zokuseiDataId].wazaKind;
}

// Sprite ID of the icon of the type.
u16 zokuseiBiosGetSpriteID(u16 zokuseiDataId)
{
    if (zokuseiDataId >= *zokusei_data_number) return 0;
    return zokusei_data[zokuseiDataId].spriteID;
}

// Sprite ID of the small icon of the type.
u32 zokuseiBiosGetSmallSpriteID(u16 zokuseiDataId)
{
    if (zokuseiDataId >= *zokusei_data_number) return 0;
    return zokusei_data[zokuseiDataId].smallSpriteID;
}
