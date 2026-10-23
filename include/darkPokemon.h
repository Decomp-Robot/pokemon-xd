#ifndef DARK_POKEMON_H
#define DARK_POKEMON_H

#include <types.h>

#ifdef __cplusplus
enum darkPokemonStatus {
    DARK_POKEMON_STATUS_NONE = 0,
    DARK_POKEMON_STATUS_FIGHT = 1,
    DARK_POKEMON_SNATCHED = 3,
    DARK_POKEMON_STATUS_RELIVED = 4
};
enum darkPokemonWhere { DARK_POKEMON_EXCHANGED = 5 };
class DarkPokemon {
public:
    u16 getPokemonID() const;
    void setSnatchIndex(u8 index);
    u8 isWazaNew(u8 index) const;
    void setWazaNew(u8 index, bool value);
    darkPokemonStatus getStatus() const;
    void setStatus(darkPokemonStatus status);
    void setPlace(darkPokemonWhere place, unsigned char slot);
    void setUpdate(bool update);
private:
    u8 reserved[0x48];
};
extern u32 darkPokemonList_number;
#else
typedef struct DarkPokemon DarkPokemon;
#endif

#ifdef __cplusplus
extern "C" {
#endif

DarkPokemon *darkPokemonGetDarkPokemon(u32 darkID);
u32 darkPokemonGetSnatchSuccessNum(void);
s32 darkPokemonGetReliveNum(void);

#ifdef __cplusplus
}
#endif

#endif
