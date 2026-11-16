#include <waza.h>
#include <fightTrainerAi.h>
#include <fightSeq.h>

extern "C" {

// Move effect dispatch table, looked up by the function data id of a move
// (WazaData::mWazaFuncDataId). The index is the move id: 0 is unused, 1 to
// 354 are the normal moves, 355 marks the end of them, 356 to 373 are the
// shadow moves and 374 is the shadow special.
WazaFuncData waza_func_data[] = {
    // 0 unused
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHitNull,
        fightTrainerAiWazaDamageNull,
        fightTrainerAiWazaDamageFightPokemonNull,
        fightWazaWzxTypeFuncNull,
        fightWazaWzxVariationFuncNull
    },
    // 1 Pound
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 2 Karate Chop
    {
        fightTrainerAiWazaValuePhyKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 3 Double Slap
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 4 Comet Punch
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 5 Mega Punch
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 6 Pay Day
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit034,
        fightTrainerAiWazaDamage034,
        fightTrainerAiWazaDamageFightPokemon034,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 7 Fire Punch
    {
        fightTrainerAiWazaValueIryokuHonoo,
        fightTrainerAiWazaHit004,
        fightTrainerAiWazaDamage004,
        fightTrainerAiWazaDamageFightPokemon004,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 8 Ice Punch
    {
        fightTrainerAiWazaValueTuikaKoori,
        fightTrainerAiWazaHit005,
        fightTrainerAiWazaDamage005,
        fightTrainerAiWazaDamageFightPokemon005,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 9 Thunder Punch
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 10 Scratch
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 11 Vice Grip
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 12 Guillotine
    {
        fightTrainerAiWazaValueItigekihissatu,
        fightTrainerAiWazaHit038,
        fightTrainerAiWazaDamage038,
        fightTrainerAiWazaDamageFightPokemon038,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 13 Razor Wind
    {
        fightTrainerAiWazaValueKamaitati,
        fightTrainerAiWazaHit039,
        fightTrainerAiWazaDamage039,
        fightTrainerAiWazaDamageFightPokemon039,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 14 Swords Dance
    {
        fightTrainerAiWazaValueKougekiAppu,
        fightTrainerAiWazaHit050,
        fightTrainerAiWazaDamage050,
        fightTrainerAiWazaDamageFightPokemon050,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 15 Cut
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 16 Gust
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit149,
        fightTrainerAiWazaDamage149,
        fightTrainerAiWazaDamageFightPokemon149,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 17 Wing Attack
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 18 Whirlwind
    {
        fightTrainerAiWazaValueKyouseikoutai,
        fightTrainerAiWazaHit028,
        fightTrainerAiWazaDamage028,
        fightTrainerAiWazaDamageFightPokemon028,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 19 Fly
    {
        fightTrainerAiWazaValueSorawotobu,
        fightTrainerAiWazaHit155,
        fightTrainerAiWazaDamage155,
        fightTrainerAiWazaDamageFightPokemon155,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 20 Bind
    {
        fightTrainerAiWazaValueSokubaku,
        fightTrainerAiWazaHit042,
        fightTrainerAiWazaDamage042,
        fightTrainerAiWazaDamageFightPokemon042,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 21 Slam
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 22 Vine Whip
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 23 Stomp
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit150,
        fightTrainerAiWazaDamage150,
        fightTrainerAiWazaDamageFightPokemon150,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 24 Double Kick
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit044,
        fightTrainerAiWazaDamage044,
        fightTrainerAiWazaDamageFightPokemon044,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 25 Mega Kick
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 26 Jump Kick
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit045,
        fightTrainerAiWazaDamage045,
        fightTrainerAiWazaDamageFightPokemon045,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 27 Rolling Kick
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit031,
        fightTrainerAiWazaDamage031,
        fightTrainerAiWazaDamageFightPokemon031,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 28 Sand Attack
    {
        fightTrainerAiWazaValueMeityuuDaun,
        fightTrainerAiWazaHit023,
        fightTrainerAiWazaDamage023,
        fightTrainerAiWazaDamageFightPokemon023,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 29 Headbutt
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit031,
        fightTrainerAiWazaDamage031,
        fightTrainerAiWazaDamageFightPokemon031,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 30 Horn Attack
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 31 Fury Attack
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 32 Horn Drill
    {
        fightTrainerAiWazaValueItigekihissatu,
        fightTrainerAiWazaHit038,
        fightTrainerAiWazaDamage038,
        fightTrainerAiWazaDamageFightPokemon038,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 33 Tackle
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 34 Body Slam
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 35 Wrap
    {
        fightTrainerAiWazaValueSokubaku,
        fightTrainerAiWazaHit042,
        fightTrainerAiWazaDamage042,
        fightTrainerAiWazaDamageFightPokemon042,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 36 Take Down
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit048,
        fightTrainerAiWazaDamage048,
        fightTrainerAiWazaDamageFightPokemon048,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 37 Thrash
    {
        fightTrainerAiWazaValueAbareru,
        fightTrainerAiWazaHit027,
        fightTrainerAiWazaDamage027,
        fightTrainerAiWazaDamageFightPokemon027,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 38 Double-Edge
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit198,
        fightTrainerAiWazaDamage198,
        fightTrainerAiWazaDamageFightPokemon198,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 39 Tail Whip
    {
        fightTrainerAiWazaValueBougyoDaun,
        fightTrainerAiWazaHit019,
        fightTrainerAiWazaDamage019,
        fightTrainerAiWazaDamageFightPokemon019,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 40 Poison Sting
    {
        fightTrainerAiWazaValueTuikaDoku,
        fightTrainerAiWazaHit002,
        fightTrainerAiWazaDamage002,
        fightTrainerAiWazaDamageFightPokemon002,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 41 Twineedle
    {
        fightTrainerAiWazaValueTuikaDoku,
        fightTrainerAiWazaHit077,
        fightTrainerAiWazaDamage077,
        fightTrainerAiWazaDamageFightPokemon077,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 42 Pin Missile
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 43 Leer
    {
        fightTrainerAiWazaValueBougyoDaun,
        fightTrainerAiWazaHit019,
        fightTrainerAiWazaDamage019,
        fightTrainerAiWazaDamageFightPokemon019,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 44 Bite
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit031,
        fightTrainerAiWazaDamage031,
        fightTrainerAiWazaDamageFightPokemon031,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 45 Growl
    {
        fightTrainerAiWazaValueKougekiDaun,
        fightTrainerAiWazaHit018,
        fightTrainerAiWazaDamage018,
        fightTrainerAiWazaDamageFightPokemon018,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 46 Roar
    {
        fightTrainerAiWazaValueKyouseikoutai,
        fightTrainerAiWazaHit028,
        fightTrainerAiWazaDamage028,
        fightTrainerAiWazaDamageFightPokemon028,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 47 Sing
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit001,
        fightTrainerAiWazaDamage001,
        fightTrainerAiWazaDamageFightPokemon001,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 48 Supersonic
    {
        fightTrainerAiWazaValueKonran,
        fightTrainerAiWazaHit049,
        fightTrainerAiWazaDamage049,
        fightTrainerAiWazaDamageFightPokemon049,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 49 Sonic Boom
    {
        fightTrainerAiWazaValueTeiryouDameeji,
        fightTrainerAiWazaHit130,
        fightTrainerAiWazaDamage130,
        fightTrainerAiWazaDamageFightPokemon130,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 50 Disable
    {
        fightTrainerAiWazaValueKanasibari,
        fightTrainerAiWazaHit086,
        fightTrainerAiWazaDamage086,
        fightTrainerAiWazaDamageFightPokemon086,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 51 Acid
    {
        fightTrainerAiWazaValueTuikouBougyoDaun,
        fightTrainerAiWazaHit069,
        fightTrainerAiWazaDamage069,
        fightTrainerAiWazaDamageFightPokemon069,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 52 Ember
    {
        fightTrainerAiWazaValueIryokuHonoo,
        fightTrainerAiWazaHit004,
        fightTrainerAiWazaDamage004,
        fightTrainerAiWazaDamageFightPokemon004,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 53 Flamethrower
    {
        fightTrainerAiWazaValueIryokuHonoo,
        fightTrainerAiWazaHit004,
        fightTrainerAiWazaDamage004,
        fightTrainerAiWazaDamageFightPokemon004,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 54 Mist
    {
        fightTrainerAiWazaValueSiroikiri,
        fightTrainerAiWazaHit046,
        fightTrainerAiWazaDamage046,
        fightTrainerAiWazaDamageFightPokemon046,
        fightWazaWzxTypeFuncAllAlly,
        fightWazaWzxVariationFuncNormal
    },
    // 55 Water Gun
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 56 Hydro Pump
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 57 Surf
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 58 Ice Beam
    {
        fightTrainerAiWazaValueTuikaKoori,
        fightTrainerAiWazaHit005,
        fightTrainerAiWazaDamage005,
        fightTrainerAiWazaDamageFightPokemon005,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 59 Blizzard
    {
        fightTrainerAiWazaValueTuikaKoori,
        fightTrainerAiWazaHit005,
        fightTrainerAiWazaDamage005,
        fightTrainerAiWazaDamageFightPokemon005,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 60 Psybeam
    {
        fightTrainerAiWazaValueTuikouKonran,
        fightTrainerAiWazaHit076,
        fightTrainerAiWazaDamage076,
        fightTrainerAiWazaDamageFightPokemon076,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 61 Bubble Beam
    {
        fightTrainerAiWazaValueTuikouSubayasaDaun,
        fightTrainerAiWazaHit070,
        fightTrainerAiWazaDamage070,
        fightTrainerAiWazaDamageFightPokemon070,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 62 Aurora Beam
    {
        fightTrainerAiWazaValueOororabiimu,
        fightTrainerAiWazaHit068,
        fightTrainerAiWazaDamage068,
        fightTrainerAiWazaDamageFightPokemon068,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 63 Hyper Beam
    {
        fightTrainerAiWazaValueHandou,
        fightTrainerAiWazaHit080,
        fightTrainerAiWazaDamage080,
        fightTrainerAiWazaDamageFightPokemon080,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 64 Peck
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 65 Drill Peck
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 66 Submission
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit048,
        fightTrainerAiWazaDamage048,
        fightTrainerAiWazaDamageFightPokemon048,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 67 Low Kick
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit196,
        fightTrainerAiWazaDamage196,
        fightTrainerAiWazaDamageFightPokemon196,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncKetaguri
    },
    // 68 Counter
    {
        fightTrainerAiWazaValueKauntaa,
        fightTrainerAiWazaHit089,
        fightTrainerAiWazaDamage089,
        fightTrainerAiWazaDamageFightPokemon089,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 69 Seismic Toss
    {
        fightTrainerAiWazaValueTeiryouDameeji,
        fightTrainerAiWazaHit087,
        fightTrainerAiWazaDamage087,
        fightTrainerAiWazaDamageFightPokemon087,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncTikyuunage
    },
    // 70 Strength
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 71 Absorb
    {
        fightTrainerAiWazaValueKyuusyuu,
        fightTrainerAiWazaHit003,
        fightTrainerAiWazaDamage003,
        fightTrainerAiWazaDamageFightPokemon003,
        fightWazaWzxTypeFuncVampire,
        fightWazaWzxVariationFuncNormal
    },
    // 72 Mega Drain
    {
        fightTrainerAiWazaValueKyuusyuu,
        fightTrainerAiWazaHit003,
        fightTrainerAiWazaDamage003,
        fightTrainerAiWazaDamageFightPokemon003,
        fightWazaWzxTypeFuncVampire,
        fightWazaWzxVariationFuncNormal
    },
    // 73 Leech Seed
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit084,
        fightTrainerAiWazaDamage084,
        fightTrainerAiWazaDamageFightPokemon084,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 74 Growth
    {
        fightTrainerAiWazaValueTokukouAppu,
        fightTrainerAiWazaHit013,
        fightTrainerAiWazaDamage013,
        fightTrainerAiWazaDamageFightPokemon013,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 75 Razor Leaf
    {
        fightTrainerAiWazaValueSpeKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 76 Solar Beam
    {
        fightTrainerAiWazaValueSooraabiimu,
        fightTrainerAiWazaHit151,
        fightTrainerAiWazaDamage151,
        fightTrainerAiWazaDamageFightPokemon151,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 77 Poison Powder
    {
        fightTrainerAiWazaValueDoku,
        fightTrainerAiWazaHit066,
        fightTrainerAiWazaDamage066,
        fightTrainerAiWazaDamageFightPokemon066,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 78 Stun Spore
    {
        fightTrainerAiWazaValueMahi,
        fightTrainerAiWazaHit067,
        fightTrainerAiWazaDamage067,
        fightTrainerAiWazaDamageFightPokemon067,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 79 Sleep Powder
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit001,
        fightTrainerAiWazaDamage001,
        fightTrainerAiWazaDamageFightPokemon001,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 80 Petal Dance
    {
        fightTrainerAiWazaValueAbareru,
        fightTrainerAiWazaHit027,
        fightTrainerAiWazaDamage027,
        fightTrainerAiWazaDamageFightPokemon027,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 81 String Shot
    {
        fightTrainerAiWazaValueSubayasaDaun,
        fightTrainerAiWazaHit020,
        fightTrainerAiWazaDamage020,
        fightTrainerAiWazaDamageFightPokemon020,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 82 Dragon Rage
    {
        fightTrainerAiWazaValueTeiryouDameeji,
        fightTrainerAiWazaHit041,
        fightTrainerAiWazaDamage041,
        fightTrainerAiWazaDamageFightPokemon041,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 83 Fire Spin
    {
        fightTrainerAiWazaValueHonoonouzu,
        fightTrainerAiWazaHit042,
        fightTrainerAiWazaDamage042,
        fightTrainerAiWazaDamageFightPokemon042,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 84 Thunder Shock
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 85 Thunderbolt
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 86 Thunder Wave
    {
        fightTrainerAiWazaValueMahi,
        fightTrainerAiWazaHit067,
        fightTrainerAiWazaDamage067,
        fightTrainerAiWazaDamageFightPokemon067,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 87 Thunder
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit152,
        fightTrainerAiWazaDamage152,
        fightTrainerAiWazaDamageFightPokemon152,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 88 Rock Throw
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 89 Earthquake
    {
        fightTrainerAiWazaValueJisin,
        fightTrainerAiWazaHit147,
        fightTrainerAiWazaDamage147,
        fightTrainerAiWazaDamageFightPokemon147,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 90 Fissure
    {
        fightTrainerAiWazaValueItigekihissatu,
        fightTrainerAiWazaHit038,
        fightTrainerAiWazaDamage038,
        fightTrainerAiWazaDamageFightPokemon038,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 91 Dig
    {
        fightTrainerAiWazaValueSorawotobu,
        fightTrainerAiWazaHit155,
        fightTrainerAiWazaDamage155,
        fightTrainerAiWazaDamageFightPokemon155,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 92 Toxic
    {
        fightTrainerAiWazaValueDoku,
        fightTrainerAiWazaHit033,
        fightTrainerAiWazaDamage033,
        fightTrainerAiWazaDamageFightPokemon033,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 93 Confusion
    {
        fightTrainerAiWazaValueTuikouKonran,
        fightTrainerAiWazaHit076,
        fightTrainerAiWazaDamage076,
        fightTrainerAiWazaDamageFightPokemon076,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 94 Psychic
    {
        fightTrainerAiWazaValueTuikouTokubouDaun,
        fightTrainerAiWazaHit072,
        fightTrainerAiWazaDamage072,
        fightTrainerAiWazaDamageFightPokemon072,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 95 Hypnosis
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit001,
        fightTrainerAiWazaDamage001,
        fightTrainerAiWazaDamageFightPokemon001,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 96 Meditate
    {
        fightTrainerAiWazaValueKougekiAppu,
        fightTrainerAiWazaHit010,
        fightTrainerAiWazaDamage010,
        fightTrainerAiWazaDamageFightPokemon010,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 97 Agility
    {
        fightTrainerAiWazaValueKousokuidou,
        fightTrainerAiWazaHit052,
        fightTrainerAiWazaDamage052,
        fightTrainerAiWazaDamageFightPokemon052,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 98 Quick Attack
    {
        fightTrainerAiWazaValueKouPuraioritii,
        fightTrainerAiWazaHit103,
        fightTrainerAiWazaDamage103,
        fightTrainerAiWazaDamageFightPokemon103,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 99 Rage
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit081,
        fightTrainerAiWazaDamage081,
        fightTrainerAiWazaDamageFightPokemon081,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 100 Teleport
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHit153,
        fightTrainerAiWazaDamage153,
        fightTrainerAiWazaDamageFightPokemon153,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 101 Night Shade
    {
        fightTrainerAiWazaValueTeiryouDameeji,
        fightTrainerAiWazaHit087,
        fightTrainerAiWazaDamage087,
        fightTrainerAiWazaDamageFightPokemon087,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 102 Mimic
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit082,
        fightTrainerAiWazaDamage082,
        fightTrainerAiWazaDamageFightPokemon082,
        fightWazaWzxTypeFuncAttDefSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 103 Screech
    {
        fightTrainerAiWazaValueBougyoDaun,
        fightTrainerAiWazaHit059,
        fightTrainerAiWazaDamage059,
        fightTrainerAiWazaDamageFightPokemon059,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 104 Double Team
    {
        fightTrainerAiWazaValueKaihirituAppu,
        fightTrainerAiWazaHit016,
        fightTrainerAiWazaDamage016,
        fightTrainerAiWazaDamageFightPokemon016,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 105 Recover
    {
        fightTrainerAiWazaValueKaihuku2,
        fightTrainerAiWazaHit032,
        fightTrainerAiWazaDamage032,
        fightTrainerAiWazaDamageFightPokemon032,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 106 Harden
    {
        fightTrainerAiWazaValueBougyoAppu,
        fightTrainerAiWazaHit011,
        fightTrainerAiWazaDamage011,
        fightTrainerAiWazaDamageFightPokemon011,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 107 Minimize
    {
        fightTrainerAiWazaValueKaihirituAppu,
        fightTrainerAiWazaHit108,
        fightTrainerAiWazaDamage108,
        fightTrainerAiWazaDamageFightPokemon108,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 108 Smoke Screen
    {
        fightTrainerAiWazaValueMeityuuDaun,
        fightTrainerAiWazaHit023,
        fightTrainerAiWazaDamage023,
        fightTrainerAiWazaDamageFightPokemon023,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 109 Confuse Ray
    {
        fightTrainerAiWazaValueKonran,
        fightTrainerAiWazaHit049,
        fightTrainerAiWazaDamage049,
        fightTrainerAiWazaDamageFightPokemon049,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 110 Withdraw
    {
        fightTrainerAiWazaValueBougyoAppu,
        fightTrainerAiWazaHit011,
        fightTrainerAiWazaDamage011,
        fightTrainerAiWazaDamageFightPokemon011,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 111 Defense Curl
    {
        fightTrainerAiWazaValueMarukunaru,
        fightTrainerAiWazaHit156,
        fightTrainerAiWazaDamage156,
        fightTrainerAiWazaDamageFightPokemon156,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 112 Barrier
    {
        fightTrainerAiWazaValueBougyoAppu,
        fightTrainerAiWazaHit051,
        fightTrainerAiWazaDamage051,
        fightTrainerAiWazaDamageFightPokemon051,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 113 Light Screen
    {
        fightTrainerAiWazaValueHikarinokabe,
        fightTrainerAiWazaHit035,
        fightTrainerAiWazaDamage035,
        fightTrainerAiWazaDamageFightPokemon035,
        fightWazaWzxTypeFuncAllAlly,
        fightWazaWzxVariationFuncNormal
    },
    // 114 Haze
    {
        fightTrainerAiWazaValueKuroikiri,
        fightTrainerAiWazaHit025,
        fightTrainerAiWazaDamage025,
        fightTrainerAiWazaDamageFightPokemon025,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 115 Reflect
    {
        fightTrainerAiWazaValueRihurekutaa,
        fightTrainerAiWazaHit065,
        fightTrainerAiWazaDamage065,
        fightTrainerAiWazaDamageFightPokemon065,
        fightWazaWzxTypeFuncAllAlly,
        fightWazaWzxVariationFuncNormal
    },
    // 116 Focus Energy
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit047,
        fightTrainerAiWazaDamage047,
        fightTrainerAiWazaDamageFightPokemon047,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 117 Bide
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit026,
        fightTrainerAiWazaDamage026,
        fightTrainerAiWazaDamageFightPokemon026,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 118 Metronome
    {
        fightTrainerAiWazaValueTokubetuYuusen,
        fightTrainerAiWazaHit083,
        fightTrainerAiWazaDamage083,
        fightTrainerAiWazaDamageFightPokemon083,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 119 Mirror Move
    {
        fightTrainerAiWazaValueOomugaesi,
        fightTrainerAiWazaHit009,
        fightTrainerAiWazaDamage009,
        fightTrainerAiWazaDamageFightPokemon009,
        fightWazaWzxTypeFuncNull,
        fightWazaWzxVariationFuncNull
    },
    // 120 Self-Destruct
    {
        fightTrainerAiWazaValueJibaku,
        fightTrainerAiWazaHit007,
        fightTrainerAiWazaDamage007,
        fightTrainerAiWazaDamageFightPokemon007,
        fightWazaWzxTypeFuncAttDef,
        fightWazaWzxVariationFuncNormal
    },
    // 121 Egg Bomb
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 122 Lick
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 123 Smog
    {
        fightTrainerAiWazaValueTuikaDoku,
        fightTrainerAiWazaHit002,
        fightTrainerAiWazaDamage002,
        fightTrainerAiWazaDamageFightPokemon002,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 124 Sludge
    {
        fightTrainerAiWazaValueTuikaDoku,
        fightTrainerAiWazaHit002,
        fightTrainerAiWazaDamage002,
        fightTrainerAiWazaDamageFightPokemon002,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 125 Bone Club
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit031,
        fightTrainerAiWazaDamage031,
        fightTrainerAiWazaDamageFightPokemon031,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 126 Fire Blast
    {
        fightTrainerAiWazaValueIryokuHonoo,
        fightTrainerAiWazaHit004,
        fightTrainerAiWazaDamage004,
        fightTrainerAiWazaDamageFightPokemon004,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 127 Waterfall
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 128 Clamp
    {
        fightTrainerAiWazaValueSokubaku,
        fightTrainerAiWazaHit042,
        fightTrainerAiWazaDamage042,
        fightTrainerAiWazaDamageFightPokemon042,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 129 Swift
    {
        fightTrainerAiWazaValueHittyuu,
        fightTrainerAiWazaHit017,
        fightTrainerAiWazaDamage017,
        fightTrainerAiWazaDamageFightPokemon017,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 130 Skull Bash
    {
        fightTrainerAiWazaValueRokettoZutuki,
        fightTrainerAiWazaHit145,
        fightTrainerAiWazaDamage145,
        fightTrainerAiWazaDamageFightPokemon145,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 131 Spike Cannon
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 132 Constrict
    {
        fightTrainerAiWazaValueTuikouSubayasaDaun,
        fightTrainerAiWazaHit070,
        fightTrainerAiWazaDamage070,
        fightTrainerAiWazaDamageFightPokemon070,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 133 Amnesia
    {
        fightTrainerAiWazaValueDowasure,
        fightTrainerAiWazaHit054,
        fightTrainerAiWazaDamage054,
        fightTrainerAiWazaDamageFightPokemon054,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 134 Kinesis
    {
        fightTrainerAiWazaValueMeityuuDaun,
        fightTrainerAiWazaHit023,
        fightTrainerAiWazaDamage023,
        fightTrainerAiWazaDamageFightPokemon023,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 135 Soft-Boiled
    {
        fightTrainerAiWazaValueKaihuku2,
        fightTrainerAiWazaHit157,
        fightTrainerAiWazaDamage157,
        fightTrainerAiWazaDamageFightPokemon157,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 136 High Jump Kick
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit045,
        fightTrainerAiWazaDamage045,
        fightTrainerAiWazaDamageFightPokemon045,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 137 Glare
    {
        fightTrainerAiWazaValueMahi,
        fightTrainerAiWazaHit067,
        fightTrainerAiWazaDamage067,
        fightTrainerAiWazaDamageFightPokemon067,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 138 Dream Eater
    {
        fightTrainerAiWazaValueYumekui,
        fightTrainerAiWazaHit008,
        fightTrainerAiWazaDamage008,
        fightTrainerAiWazaDamageFightPokemon008,
        fightWazaWzxTypeFuncAttDefSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 139 Poison Gas
    {
        fightTrainerAiWazaValueDoku,
        fightTrainerAiWazaHit066,
        fightTrainerAiWazaDamage066,
        fightTrainerAiWazaDamageFightPokemon066,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 140 Barrage
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 141 Leech Life
    {
        fightTrainerAiWazaValueKyuusyuu,
        fightTrainerAiWazaHit003,
        fightTrainerAiWazaDamage003,
        fightTrainerAiWazaDamageFightPokemon003,
        fightWazaWzxTypeFuncVampire,
        fightWazaWzxVariationFuncNormal
    },
    // 142 Lovely Kiss
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit001,
        fightTrainerAiWazaDamage001,
        fightTrainerAiWazaDamageFightPokemon001,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 143 Sky Attack
    {
        fightTrainerAiWazaValueGoddobaado,
        fightTrainerAiWazaHit075,
        fightTrainerAiWazaDamage075,
        fightTrainerAiWazaDamageFightPokemon075,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 144 Transform
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit057,
        fightTrainerAiWazaDamage057,
        fightTrainerAiWazaDamageFightPokemon057,
        fightWazaWzxTypeFuncHensin,
        fightWazaWzxVariationFuncNormal
    },
    // 145 Bubble
    {
        fightTrainerAiWazaValueTuikouSubayasaDaun,
        fightTrainerAiWazaHit070,
        fightTrainerAiWazaDamage070,
        fightTrainerAiWazaDamageFightPokemon070,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 146 Dizzy Punch
    {
        fightTrainerAiWazaValueTuikouKonran,
        fightTrainerAiWazaHit076,
        fightTrainerAiWazaDamage076,
        fightTrainerAiWazaDamageFightPokemon076,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 147 Spore
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit001,
        fightTrainerAiWazaDamage001,
        fightTrainerAiWazaDamageFightPokemon001,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 148 Flash
    {
        fightTrainerAiWazaValueMeityuuDaun,
        fightTrainerAiWazaHit023,
        fightTrainerAiWazaDamage023,
        fightTrainerAiWazaDamageFightPokemon023,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 149 Psywave
    {
        fightTrainerAiWazaValueTeiryouDameeji,
        fightTrainerAiWazaHit088,
        fightTrainerAiWazaDamage088,
        fightTrainerAiWazaDamageFightPokemon088,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 150 Splash
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHit085,
        fightTrainerAiWazaDamage085,
        fightTrainerAiWazaDamageFightPokemon085,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 151 Acid Armor
    {
        fightTrainerAiWazaValueBougyoAppu,
        fightTrainerAiWazaHit051,
        fightTrainerAiWazaDamage051,
        fightTrainerAiWazaDamageFightPokemon051,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 152 Crabhammer
    {
        fightTrainerAiWazaValueSpeKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 153 Explosion
    {
        fightTrainerAiWazaValueJibaku,
        fightTrainerAiWazaHit007,
        fightTrainerAiWazaDamage007,
        fightTrainerAiWazaDamageFightPokemon007,
        fightWazaWzxTypeFuncAttDef,
        fightWazaWzxVariationFuncNormal
    },
    // 154 Fury Swipes
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 155 Bonemerang
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit044,
        fightTrainerAiWazaDamage044,
        fightTrainerAiWazaDamageFightPokemon044,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 156 Rest
    {
        fightTrainerAiWazaValueNemuru,
        fightTrainerAiWazaHit037,
        fightTrainerAiWazaDamage037,
        fightTrainerAiWazaDamageFightPokemon037,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 157 Rock Slide
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit031,
        fightTrainerAiWazaDamage031,
        fightTrainerAiWazaDamageFightPokemon031,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 158 Hyper Fang
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit031,
        fightTrainerAiWazaDamage031,
        fightTrainerAiWazaDamageFightPokemon031,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 159 Sharpen
    {
        fightTrainerAiWazaValueKougekiAppu,
        fightTrainerAiWazaHit010,
        fightTrainerAiWazaDamage010,
        fightTrainerAiWazaDamageFightPokemon010,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 160 Conversion
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit030,
        fightTrainerAiWazaDamage030,
        fightTrainerAiWazaDamageFightPokemon030,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 161 Tri Attack
    {
        fightTrainerAiWazaValueToraiatakku,
        fightTrainerAiWazaHit036,
        fightTrainerAiWazaDamage036,
        fightTrainerAiWazaDamageFightPokemon036,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 162 Super Fang
    {
        fightTrainerAiWazaValueIkarinomaeba,
        fightTrainerAiWazaHit040,
        fightTrainerAiWazaDamage040,
        fightTrainerAiWazaDamageFightPokemon040,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 163 Slash
    {
        fightTrainerAiWazaValuePhyKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 164 Substitute
    {
        fightTrainerAiWazaValueMigawari,
        fightTrainerAiWazaHit079,
        fightTrainerAiWazaDamage079,
        fightTrainerAiWazaDamageFightPokemon079,
        fightWazaWzxTypeFuncMigawari,
        fightWazaWzxVariationFuncNormal
    },
    // 165 Struggle
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHit048,
        fightTrainerAiWazaDamage048,
        fightTrainerAiWazaDamageFightPokemon048,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 166 Sketch
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHit095,
        fightTrainerAiWazaDamage095,
        fightTrainerAiWazaDamageFightPokemon095,
        fightWazaWzxTypeFuncAttDefSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 167 Triple Kick
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit104,
        fightTrainerAiWazaDamage104,
        fightTrainerAiWazaDamageFightPokemon104,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncTripleKick
    },
    // 168 Thief
    {
        fightTrainerAiWazaValueDouguUbau,
        fightTrainerAiWazaHit105,
        fightTrainerAiWazaDamage105,
        fightTrainerAiWazaDamageFightPokemon105,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 169 Spider Web
    {
        fightTrainerAiWazaValueKoukanKinsi,
        fightTrainerAiWazaHit106,
        fightTrainerAiWazaDamage106,
        fightTrainerAiWazaDamageFightPokemon106,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 170 Mind Reader
    {
        fightTrainerAiWazaValueRokkuon,
        fightTrainerAiWazaHit094,
        fightTrainerAiWazaDamage094,
        fightTrainerAiWazaDamageFightPokemon094,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 171 Nightmare
    {
        fightTrainerAiWazaValueAkumu,
        fightTrainerAiWazaHit107,
        fightTrainerAiWazaDamage107,
        fightTrainerAiWazaDamageFightPokemon107,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 172 Flame Wheel
    {
        fightTrainerAiWazaValueJikokaitou,
        fightTrainerAiWazaHit125,
        fightTrainerAiWazaDamage125,
        fightTrainerAiWazaDamageFightPokemon125,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 173 Snore
    {
        fightTrainerAiWazaValueNemuriKoudou,
        fightTrainerAiWazaHit092,
        fightTrainerAiWazaDamage092,
        fightTrainerAiWazaDamageFightPokemon092,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 174 Curse
    {
        fightTrainerAiWazaValueNoroi,
        fightTrainerAiWazaHit109,
        fightTrainerAiWazaDamage109,
        fightTrainerAiWazaDamageFightPokemon109,
        fightWazaWzxTypeFuncAttDef,
        fightWazaWzxVariationFuncNormal
    },
    // 175 Flail
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit099,
        fightTrainerAiWazaDamage099,
        fightTrainerAiWazaDamageFightPokemon099,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 176 Conversion 2
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit093,
        fightTrainerAiWazaDamage093,
        fightTrainerAiWazaDamageFightPokemon093,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 177 Aeroblast
    {
        fightTrainerAiWazaValuePhyKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 178 Cotton Spore
    {
        fightTrainerAiWazaValueSubayasaDaun,
        fightTrainerAiWazaHit060,
        fightTrainerAiWazaDamage060,
        fightTrainerAiWazaDamageFightPokemon060,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 179 Reversal
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit099,
        fightTrainerAiWazaDamage099,
        fightTrainerAiWazaDamageFightPokemon099,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 180 Spite
    {
        fightTrainerAiWazaValueUrami,
        fightTrainerAiWazaHit100,
        fightTrainerAiWazaDamage100,
        fightTrainerAiWazaDamageFightPokemon100,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 181 Powder Snow
    {
        fightTrainerAiWazaValueTuikaKoori,
        fightTrainerAiWazaHit005,
        fightTrainerAiWazaDamage005,
        fightTrainerAiWazaDamageFightPokemon005,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 182 Protect
    {
        fightTrainerAiWazaValueMamoru,
        fightTrainerAiWazaHit111,
        fightTrainerAiWazaDamage111,
        fightTrainerAiWazaDamageFightPokemon111,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 183 Mach Punch
    {
        fightTrainerAiWazaValueKouPuraioritii,
        fightTrainerAiWazaHit103,
        fightTrainerAiWazaDamage103,
        fightTrainerAiWazaDamageFightPokemon103,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 184 Scary Face
    {
        fightTrainerAiWazaValueSubayasaDaun,
        fightTrainerAiWazaHit060,
        fightTrainerAiWazaDamage060,
        fightTrainerAiWazaDamageFightPokemon060,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 185 Faint Attack
    {
        fightTrainerAiWazaValueHittyuu,
        fightTrainerAiWazaHit017,
        fightTrainerAiWazaDamage017,
        fightTrainerAiWazaDamageFightPokemon017,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 186 Sweet Kiss
    {
        fightTrainerAiWazaValueKonran,
        fightTrainerAiWazaHit049,
        fightTrainerAiWazaDamage049,
        fightTrainerAiWazaDamageFightPokemon049,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 187 Belly Drum
    {
        fightTrainerAiWazaValueHaradaiko,
        fightTrainerAiWazaHit142,
        fightTrainerAiWazaDamage142,
        fightTrainerAiWazaDamageFightPokemon142,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 188 Sludge Bomb
    {
        fightTrainerAiWazaValueTuikaDoku,
        fightTrainerAiWazaHit002,
        fightTrainerAiWazaDamage002,
        fightTrainerAiWazaDamageFightPokemon002,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 189 Mud-Slap
    {
        fightTrainerAiWazaValueDorokake,
        fightTrainerAiWazaHit073,
        fightTrainerAiWazaDamage073,
        fightTrainerAiWazaDamageFightPokemon073,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 190 Octazooka
    {
        fightTrainerAiWazaValueTuikouMeityuuDaun,
        fightTrainerAiWazaHit073,
        fightTrainerAiWazaDamage073,
        fightTrainerAiWazaDamageFightPokemon073,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 191 Spikes
    {
        fightTrainerAiWazaValueMakibisi,
        fightTrainerAiWazaHit112,
        fightTrainerAiWazaDamage112,
        fightTrainerAiWazaDamageFightPokemon112,
        fightWazaWzxTypeFuncAllEnemy,
        fightWazaWzxVariationFuncNormal
    },
    // 192 Zap Cannon
    {
        fightTrainerAiWazaValueDenjihou,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 193 Foresight
    {
        fightTrainerAiWazaValueMiyaburu,
        fightTrainerAiWazaHit113,
        fightTrainerAiWazaDamage113,
        fightTrainerAiWazaDamageFightPokemon113,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 194 Destiny Bond
    {
        fightTrainerAiWazaValueMitizure,
        fightTrainerAiWazaHit098,
        fightTrainerAiWazaDamage098,
        fightTrainerAiWazaDamageFightPokemon098,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 195 Perish Song
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit114,
        fightTrainerAiWazaDamage114,
        fightTrainerAiWazaDamageFightPokemon114,
        fightWazaWzxTypeFuncAttDef,
        fightWazaWzxVariationFuncNormal
    },
    // 196 Icy Wind
    {
        fightTrainerAiWazaValueKogoerukaze,
        fightTrainerAiWazaHit070,
        fightTrainerAiWazaDamage070,
        fightTrainerAiWazaDamageFightPokemon070,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 197 Detect
    {
        fightTrainerAiWazaValueMamoru,
        fightTrainerAiWazaHit111,
        fightTrainerAiWazaDamage111,
        fightTrainerAiWazaDamageFightPokemon111,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 198 Bone Rush
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 199 Lock-On
    {
        fightTrainerAiWazaValueRokkuon,
        fightTrainerAiWazaHit094,
        fightTrainerAiWazaDamage094,
        fightTrainerAiWazaDamageFightPokemon094,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 200 Outrage
    {
        fightTrainerAiWazaValueAbareru,
        fightTrainerAiWazaHit027,
        fightTrainerAiWazaDamage027,
        fightTrainerAiWazaDamageFightPokemon027,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 201 Sandstorm
    {
        fightTrainerAiWazaValueSunaarasi,
        fightTrainerAiWazaHit115,
        fightTrainerAiWazaDamage115,
        fightTrainerAiWazaDamageFightPokemon115,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 202 Giga Drain
    {
        fightTrainerAiWazaValueKyuusyuu,
        fightTrainerAiWazaHit003,
        fightTrainerAiWazaDamage003,
        fightTrainerAiWazaDamageFightPokemon003,
        fightWazaWzxTypeFuncVampire,
        fightWazaWzxVariationFuncNormal
    },
    // 203 Endure
    {
        fightTrainerAiWazaValueKoraeru,
        fightTrainerAiWazaHit116,
        fightTrainerAiWazaDamage116,
        fightTrainerAiWazaDamageFightPokemon116,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 204 Charm
    {
        fightTrainerAiWazaValueKougekiDaun,
        fightTrainerAiWazaHit058,
        fightTrainerAiWazaDamage058,
        fightTrainerAiWazaDamageFightPokemon058,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 205 Rollout
    {
        fightTrainerAiWazaValueKorogaru,
        fightTrainerAiWazaHit117,
        fightTrainerAiWazaDamage117,
        fightTrainerAiWazaDamageFightPokemon117,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncKorogaru
    },
    // 206 False Swipe
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit101,
        fightTrainerAiWazaDamage101,
        fightTrainerAiWazaDamageFightPokemon101,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 207 Swagger
    {
        fightTrainerAiWazaValueIbaru,
        fightTrainerAiWazaHit118,
        fightTrainerAiWazaDamage118,
        fightTrainerAiWazaDamageFightPokemon118,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 208 Milk Drink
    {
        fightTrainerAiWazaValueKaihuku2,
        fightTrainerAiWazaHit157,
        fightTrainerAiWazaDamage157,
        fightTrainerAiWazaDamageFightPokemon157,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 209 Spark
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 210 Fury Cutter
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit119,
        fightTrainerAiWazaDamage119,
        fightTrainerAiWazaDamageFightPokemon119,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncRenzokugiri
    },
    // 211 Steel Wing
    {
        fightTrainerAiWazaValueHaganenotubasa,
        fightTrainerAiWazaHit138,
        fightTrainerAiWazaDamage138,
        fightTrainerAiWazaDamageFightPokemon138,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 212 Mean Look
    {
        fightTrainerAiWazaValueKoukanKinsi,
        fightTrainerAiWazaHit106,
        fightTrainerAiWazaDamage106,
        fightTrainerAiWazaDamageFightPokemon106,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 213 Attract
    {
        fightTrainerAiWazaValueMeromero,
        fightTrainerAiWazaHit120,
        fightTrainerAiWazaDamage120,
        fightTrainerAiWazaDamageFightPokemon120,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 214 Sleep Talk
    {
        fightTrainerAiWazaValueNemuriKoudou,
        fightTrainerAiWazaHit097,
        fightTrainerAiWazaDamage097,
        fightTrainerAiWazaDamageFightPokemon097,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 215 Heal Bell
    {
        fightTrainerAiWazaValueAroma,
        fightTrainerAiWazaHit102,
        fightTrainerAiWazaDamage102,
        fightTrainerAiWazaDamageFightPokemon102,
        fightWazaWzxTypeFuncAllAlly,
        fightWazaWzxVariationFuncNormal
    },
    // 216 Return
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit121,
        fightTrainerAiWazaDamage121,
        fightTrainerAiWazaDamageFightPokemon121,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncOngaesi
    },
    // 217 Present
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit122,
        fightTrainerAiWazaDamage122,
        fightTrainerAiWazaDamageFightPokemon122,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncPresent
    },
    // 218 Frustration
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit123,
        fightTrainerAiWazaDamage123,
        fightTrainerAiWazaDamageFightPokemon123,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncYatuatari
    },
    // 219 Safeguard
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit124,
        fightTrainerAiWazaDamage124,
        fightTrainerAiWazaDamageFightPokemon124,
        fightWazaWzxTypeFuncAllAlly,
        fightWazaWzxVariationFuncNormal
    },
    // 220 Pain Split
    {
        fightTrainerAiWazaValueItamiwake,
        fightTrainerAiWazaHit091,
        fightTrainerAiWazaDamage091,
        fightTrainerAiWazaDamageFightPokemon091,
        fightWazaWzxTypeFuncItamiwake,
        fightWazaWzxVariationFuncNormal
    },
    // 221 Sacred Fire
    {
        fightTrainerAiWazaValueJikokaitou,
        fightTrainerAiWazaHit125,
        fightTrainerAiWazaDamage125,
        fightTrainerAiWazaDamageFightPokemon125,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 222 Magnitude
    {
        fightTrainerAiWazaValueJisin,
        fightTrainerAiWazaHit126,
        fightTrainerAiWazaDamage126,
        fightTrainerAiWazaDamageFightPokemon126,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncMagnitude
    },
    // 223 Dynamic Punch
    {
        fightTrainerAiWazaValueBakuretuPanti,
        fightTrainerAiWazaHit076,
        fightTrainerAiWazaDamage076,
        fightTrainerAiWazaDamageFightPokemon076,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 224 Megahorn
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 225 Dragon Breath
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 226 Baton Pass
    {
        fightTrainerAiWazaValueBatontatti,
        fightTrainerAiWazaHit127,
        fightTrainerAiWazaDamage127,
        fightTrainerAiWazaDamageFightPokemon127,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 227 Encore
    {
        fightTrainerAiWazaValueAnkooru,
        fightTrainerAiWazaHit090,
        fightTrainerAiWazaDamage090,
        fightTrainerAiWazaDamageFightPokemon090,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 228 Pursuit
    {
        fightTrainerAiWazaValueOiuti,
        fightTrainerAiWazaHit128,
        fightTrainerAiWazaDamage128,
        fightTrainerAiWazaDamageFightPokemon128,
        fightWazaWzxTypeFuncNormal2,
        fightWazaWzxVariationFuncOiuchi
    },
    // 229 Rapid Spin
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit129,
        fightTrainerAiWazaDamage129,
        fightTrainerAiWazaDamageFightPokemon129,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 230 Sweet Scent
    {
        fightTrainerAiWazaValueKaihiDaun,
        fightTrainerAiWazaHit024,
        fightTrainerAiWazaDamage024,
        fightTrainerAiWazaDamageFightPokemon024,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 231 Iron Tail
    {
        fightTrainerAiWazaValueTuikouBougyoDaun,
        fightTrainerAiWazaHit069,
        fightTrainerAiWazaDamage069,
        fightTrainerAiWazaDamageFightPokemon069,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 232 Metal Claw
    {
        fightTrainerAiWazaValueTuikaKougekiAppu,
        fightTrainerAiWazaHit139,
        fightTrainerAiWazaDamage139,
        fightTrainerAiWazaDamageFightPokemon139,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 233 Vital Throw
    {
        fightTrainerAiWazaValueAteminage,
        fightTrainerAiWazaHit078,
        fightTrainerAiWazaDamage078,
        fightTrainerAiWazaDamageFightPokemon078,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 234 Morning Sun
    {
        fightTrainerAiWazaValueKaihuku1,
        fightTrainerAiWazaHit132,
        fightTrainerAiWazaDamage132,
        fightTrainerAiWazaDamageFightPokemon132,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncWeatherHP
    },
    // 235 Synthesis
    {
        fightTrainerAiWazaValueKaihuku1,
        fightTrainerAiWazaHit133,
        fightTrainerAiWazaDamage133,
        fightTrainerAiWazaDamageFightPokemon133,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncWeatherHP
    },
    // 236 Moonlight
    {
        fightTrainerAiWazaValueKaihuku1,
        fightTrainerAiWazaHit134,
        fightTrainerAiWazaDamage134,
        fightTrainerAiWazaDamageFightPokemon134,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncWeatherHP
    },
    // 237 Hidden Power
    {
        fightTrainerAiWazaValueTokubetuYuusen,
        fightTrainerAiWazaHit135,
        fightTrainerAiWazaDamage135,
        fightTrainerAiWazaDamageFightPokemon135,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 238 Cross Chop
    {
        fightTrainerAiWazaValuePhyKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 239 Twister
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit146,
        fightTrainerAiWazaDamage146,
        fightTrainerAiWazaDamageFightPokemon146,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 240 Rain Dance
    {
        fightTrainerAiWazaValueAmagoi,
        fightTrainerAiWazaHit136,
        fightTrainerAiWazaDamage136,
        fightTrainerAiWazaDamageFightPokemon136,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 241 Sunny Day
    {
        fightTrainerAiWazaValueNihonbare,
        fightTrainerAiWazaHit137,
        fightTrainerAiWazaDamage137,
        fightTrainerAiWazaDamageFightPokemon137,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 242 Crunch
    {
        fightTrainerAiWazaValueTuikouTokubouDaun,
        fightTrainerAiWazaHit072,
        fightTrainerAiWazaDamage072,
        fightTrainerAiWazaDamageFightPokemon072,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 243 Mirror Coat
    {
        fightTrainerAiWazaValueMiraakooto,
        fightTrainerAiWazaHit144,
        fightTrainerAiWazaDamage144,
        fightTrainerAiWazaDamageFightPokemon144,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 244 Psych Up
    {
        fightTrainerAiWazaValueJikoanji,
        fightTrainerAiWazaHit143,
        fightTrainerAiWazaDamage143,
        fightTrainerAiWazaDamageFightPokemon143,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 245 Extreme Speed
    {
        fightTrainerAiWazaValueKouPuraioritii,
        fightTrainerAiWazaHit103,
        fightTrainerAiWazaDamage103,
        fightTrainerAiWazaDamageFightPokemon103,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 246 Ancient Power
    {
        fightTrainerAiWazaValueZennouryokuappu,
        fightTrainerAiWazaHit140,
        fightTrainerAiWazaDamage140,
        fightTrainerAiWazaDamageFightPokemon140,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 247 Shadow Ball
    {
        fightTrainerAiWazaValueTuikouTokubouDaun,
        fightTrainerAiWazaHit072,
        fightTrainerAiWazaDamage072,
        fightTrainerAiWazaDamageFightPokemon072,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 248 Future Sight
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit148,
        fightTrainerAiWazaDamage148,
        fightTrainerAiWazaDamageFightPokemon148,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 249 Rock Smash
    {
        fightTrainerAiWazaValueTuikouBougyoDaun,
        fightTrainerAiWazaHit069,
        fightTrainerAiWazaDamage069,
        fightTrainerAiWazaDamageFightPokemon069,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 250 Whirlpool
    {
        fightTrainerAiWazaValueSokubaku,
        fightTrainerAiWazaHit042,
        fightTrainerAiWazaDamage042,
        fightTrainerAiWazaDamageFightPokemon042,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 251 Beat Up
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit154,
        fightTrainerAiWazaDamage154,
        fightTrainerAiWazaDamageFightPokemon154,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 252 Fake Out
    {
        fightTrainerAiWazaValueNekodamasi,
        fightTrainerAiWazaHit158,
        fightTrainerAiWazaDamage158,
        fightTrainerAiWazaDamageFightPokemon158,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 253 Uproar
    {
        fightTrainerAiWazaValueSawagu,
        fightTrainerAiWazaHit159,
        fightTrainerAiWazaDamage159,
        fightTrainerAiWazaDamageFightPokemon159,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 254 Stockpile
    {
        fightTrainerAiWazaValueTakuwaeru,
        fightTrainerAiWazaHit160,
        fightTrainerAiWazaDamage160,
        fightTrainerAiWazaDamageFightPokemon160,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 255 Spit Up
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit161,
        fightTrainerAiWazaDamage161,
        fightTrainerAiWazaDamageFightPokemon161,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncHakidasu
    },
    // 256 Swallow
    {
        fightTrainerAiWazaValueKaihuku2,
        fightTrainerAiWazaHit162,
        fightTrainerAiWazaDamage162,
        fightTrainerAiWazaDamageFightPokemon162,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNomikomu
    },
    // 257 Heat Wave
    {
        fightTrainerAiWazaValueIryokuHonoo,
        fightTrainerAiWazaHit004,
        fightTrainerAiWazaDamage004,
        fightTrainerAiWazaDamageFightPokemon004,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 258 Hail
    {
        fightTrainerAiWazaValueArare,
        fightTrainerAiWazaHit164,
        fightTrainerAiWazaDamage164,
        fightTrainerAiWazaDamageFightPokemon164,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 259 Torment
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit165,
        fightTrainerAiWazaDamage165,
        fightTrainerAiWazaDamageFightPokemon165,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 260 Flatter
    {
        fightTrainerAiWazaValueOdareru,
        fightTrainerAiWazaHit166,
        fightTrainerAiWazaDamage166,
        fightTrainerAiWazaDamageFightPokemon166,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 261 Will-O-Wisp
    {
        fightTrainerAiWazaValueOnibi,
        fightTrainerAiWazaHit167,
        fightTrainerAiWazaDamage167,
        fightTrainerAiWazaDamageFightPokemon167,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 262 Memento
    {
        fightTrainerAiWazaValueMitizure,
        fightTrainerAiWazaHit168,
        fightTrainerAiWazaDamage168,
        fightTrainerAiWazaDamageFightPokemon168,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 263 Facade
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit169,
        fightTrainerAiWazaDamage169,
        fightTrainerAiWazaDamageFightPokemon169,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 264 Focus Punch
    {
        fightTrainerAiWazaValueKiaipanti,
        fightTrainerAiWazaHit170,
        fightTrainerAiWazaDamage170,
        fightTrainerAiWazaDamageFightPokemon170,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 265 Smelling Salt
    {
        fightTrainerAiWazaValueKituke,
        fightTrainerAiWazaHit171,
        fightTrainerAiWazaDamage171,
        fightTrainerAiWazaDamageFightPokemon171,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 266 Follow Me
    {
        fightTrainerAiWazaValueKonoyubitomare,
        fightTrainerAiWazaHit172,
        fightTrainerAiWazaDamage172,
        fightTrainerAiWazaDamageFightPokemon172,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 267 Nature Power
    {
        fightTrainerAiWazaValueSizennotikara,
        fightTrainerAiWazaHit173,
        fightTrainerAiWazaDamage173,
        fightTrainerAiWazaDamageFightPokemon173,
        fightWazaWzxTypeFuncNull,
        fightWazaWzxVariationFuncNull
    },
    // 268 Charge
    {
        fightTrainerAiWazaValueJyuuden,
        fightTrainerAiWazaHit174,
        fightTrainerAiWazaDamage174,
        fightTrainerAiWazaDamageFightPokemon174,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 269 Taunt
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit175,
        fightTrainerAiWazaDamage175,
        fightTrainerAiWazaDamageFightPokemon175,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 270 Helping Hand
    {
        fightTrainerAiWazaValueTedasuke,
        fightTrainerAiWazaHit176,
        fightTrainerAiWazaDamage176,
        fightTrainerAiWazaDamageFightPokemon176,
        fightWazaWzxTypeFuncTedasuke,
        fightWazaWzxVariationFuncNormal
    },
    // 271 Trick
    {
        fightTrainerAiWazaValuetorikku,
        fightTrainerAiWazaHit177,
        fightTrainerAiWazaDamage177,
        fightTrainerAiWazaDamageFightPokemon177,
        fightWazaWzxTypeFuncAttDefSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 272 Role Play
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit178,
        fightTrainerAiWazaDamage178,
        fightTrainerAiWazaDamageFightPokemon178,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 273 Wish
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit179,
        fightTrainerAiWazaDamage179,
        fightTrainerAiWazaDamageFightPokemon179,
        fightWazaWzxTypeFuncNegaigoto,
        fightWazaWzxVariationFuncNormal
    },
    // 274 Assist
    {
        fightTrainerAiWazaValueTokubetuYuusen,
        fightTrainerAiWazaHit180,
        fightTrainerAiWazaDamage180,
        fightTrainerAiWazaDamageFightPokemon180,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 275 Ingrain
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit181,
        fightTrainerAiWazaDamage181,
        fightTrainerAiWazaDamageFightPokemon181,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 276 Superpower
    {
        fightTrainerAiWazaValueBakadikara,
        fightTrainerAiWazaHit182,
        fightTrainerAiWazaDamage182,
        fightTrainerAiWazaDamageFightPokemon182,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 277 Magic Coat
    {
        fightTrainerAiWazaValueMajikkukooto,
        fightTrainerAiWazaHit183,
        fightTrainerAiWazaDamage183,
        fightTrainerAiWazaDamageFightPokemon183,
        fightWazaWzxTypeFuncMagiccort,
        fightWazaWzxVariationFuncNormal
    },
    // 278 Recycle
    {
        fightTrainerAiWazaValueRisaikuru,
        fightTrainerAiWazaHit184,
        fightTrainerAiWazaDamage184,
        fightTrainerAiWazaDamageFightPokemon184,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 279 Revenge
    {
        fightTrainerAiWazaValueRibenji,
        fightTrainerAiWazaHit185,
        fightTrainerAiWazaDamage185,
        fightTrainerAiWazaDamageFightPokemon185,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 280 Brick Break
    {
        fightTrainerAiWazaValueKawarawari,
        fightTrainerAiWazaHit186,
        fightTrainerAiWazaDamage186,
        fightTrainerAiWazaDamageFightPokemon186,
        fightWazaWzxTypeFuncKawarawari,
        fightWazaWzxVariationFuncKawarawari
    },
    // 281 Yawn
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit187,
        fightTrainerAiWazaDamage187,
        fightTrainerAiWazaDamageFightPokemon187,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 282 Knock Off
    {
        fightTrainerAiWazaValueHatakiotosu,
        fightTrainerAiWazaHit188,
        fightTrainerAiWazaDamage188,
        fightTrainerAiWazaDamageFightPokemon188,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 283 Endeavor
    {
        fightTrainerAiWazaValueGamusyara,
        fightTrainerAiWazaHit189,
        fightTrainerAiWazaDamage189,
        fightTrainerAiWazaDamageFightPokemon189,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 284 Eruption
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit190,
        fightTrainerAiWazaDamage190,
        fightTrainerAiWazaDamageFightPokemon190,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 285 Skill Swap
    {
        fightTrainerAiWazaValueSukirusuwappu,
        fightTrainerAiWazaHit191,
        fightTrainerAiWazaDamage191,
        fightTrainerAiWazaDamageFightPokemon191,
        fightWazaWzxTypeFuncAttDefSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 286 Imprison
    {
        fightTrainerAiWazaValueHuuin,
        fightTrainerAiWazaHit192,
        fightTrainerAiWazaDamage192,
        fightTrainerAiWazaDamageFightPokemon192,
        fightWazaWzxTypeFuncHuuin,
        fightWazaWzxVariationFuncNormal
    },
    // 287 Refresh
    {
        fightTrainerAiWazaValueRihuressyu,
        fightTrainerAiWazaHit193,
        fightTrainerAiWazaDamage193,
        fightTrainerAiWazaDamageFightPokemon193,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 288 Grudge
    {
        fightTrainerAiWazaValueMitizure,
        fightTrainerAiWazaHit194,
        fightTrainerAiWazaDamage194,
        fightTrainerAiWazaDamageFightPokemon194,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 289 Snatch
    {
        fightTrainerAiWazaValueYokodori,
        fightTrainerAiWazaHit195,
        fightTrainerAiWazaDamage195,
        fightTrainerAiWazaDamageFightPokemon195,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 290 Secret Power
    {
        fightTrainerAiWazaValueHimitunotikara,
        fightTrainerAiWazaHit197,
        fightTrainerAiWazaDamage197,
        fightTrainerAiWazaDamageFightPokemon197,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 291 Dive
    {
        fightTrainerAiWazaValueSorawotobu,
        fightTrainerAiWazaHit155,
        fightTrainerAiWazaDamage155,
        fightTrainerAiWazaDamageFightPokemon155,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 292 Arm Thrust
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 293 Camouflage
    {
        fightTrainerAiWazaValueRandamuSentaku,
        fightTrainerAiWazaHit213,
        fightTrainerAiWazaDamage213,
        fightTrainerAiWazaDamageFightPokemon213,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 294 Tail Glow
    {
        fightTrainerAiWazaValueTokukouAppu,
        fightTrainerAiWazaHit053,
        fightTrainerAiWazaDamage053,
        fightTrainerAiWazaDamageFightPokemon053,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 295 Luster Purge
    {
        fightTrainerAiWazaValueTuikouTokubouDaun,
        fightTrainerAiWazaHit072,
        fightTrainerAiWazaDamage072,
        fightTrainerAiWazaDamageFightPokemon072,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 296 Mist Ball
    {
        fightTrainerAiWazaValueTuikouTokukouDaun,
        fightTrainerAiWazaHit071,
        fightTrainerAiWazaDamage071,
        fightTrainerAiWazaDamageFightPokemon071,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 297 Feather Dance
    {
        fightTrainerAiWazaValueKougekiDaun,
        fightTrainerAiWazaHit058,
        fightTrainerAiWazaDamage058,
        fightTrainerAiWazaDamageFightPokemon058,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 298 Teeter Dance
    {
        fightTrainerAiWazaValueHurahuradansu,
        fightTrainerAiWazaHit199,
        fightTrainerAiWazaDamage199,
        fightTrainerAiWazaDamageFightPokemon199,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 299 Blaze Kick
    {
        fightTrainerAiWazaValueBureizukikku,
        fightTrainerAiWazaHit200,
        fightTrainerAiWazaDamage200,
        fightTrainerAiWazaDamageFightPokemon200,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 300 Mud Sport
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit201,
        fightTrainerAiWazaDamage201,
        fightTrainerAiWazaDamageFightPokemon201,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 301 Ice Ball
    {
        fightTrainerAiWazaValueKorogaru,
        fightTrainerAiWazaHit117,
        fightTrainerAiWazaDamage117,
        fightTrainerAiWazaDamageFightPokemon117,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncIceball
    },
    // 302 Needle Arm
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit150,
        fightTrainerAiWazaDamage150,
        fightTrainerAiWazaDamageFightPokemon150,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 303 Slack Off
    {
        fightTrainerAiWazaValueKaihuku2,
        fightTrainerAiWazaHit032,
        fightTrainerAiWazaDamage032,
        fightTrainerAiWazaDamageFightPokemon032,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 304 Hyper Voice
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 305 Poison Fang
    {
        fightTrainerAiWazaValueDokudokunokiba,
        fightTrainerAiWazaHit202,
        fightTrainerAiWazaDamage202,
        fightTrainerAiWazaDamageFightPokemon202,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 306 Crush Claw
    {
        fightTrainerAiWazaValueTuikouBougyoDaun,
        fightTrainerAiWazaHit069,
        fightTrainerAiWazaDamage069,
        fightTrainerAiWazaDamageFightPokemon069,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 307 Blast Burn
    {
        fightTrainerAiWazaValueHandou,
        fightTrainerAiWazaHit080,
        fightTrainerAiWazaDamage080,
        fightTrainerAiWazaDamageFightPokemon080,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 308 Hydro Cannon
    {
        fightTrainerAiWazaValueHandou,
        fightTrainerAiWazaHit080,
        fightTrainerAiWazaDamage080,
        fightTrainerAiWazaDamageFightPokemon080,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 309 Meteor Mash
    {
        fightTrainerAiWazaValueTuikaKougekiAppu,
        fightTrainerAiWazaHit139,
        fightTrainerAiWazaDamage139,
        fightTrainerAiWazaDamageFightPokemon139,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 310 Astonish
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit150,
        fightTrainerAiWazaDamage150,
        fightTrainerAiWazaDamageFightPokemon150,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 311 Weather Ball
    {
        fightTrainerAiWazaValueUezaabooru,
        fightTrainerAiWazaHit203,
        fightTrainerAiWazaDamage203,
        fightTrainerAiWazaDamageFightPokemon203,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncWeatherBall
    },
    // 312 Aromatherapy
    {
        fightTrainerAiWazaValueAroma,
        fightTrainerAiWazaHit102,
        fightTrainerAiWazaDamage102,
        fightTrainerAiWazaDamageFightPokemon102,
        fightWazaWzxTypeFuncAllAlly,
        fightWazaWzxVariationFuncNormal
    },
    // 313 Fake Tears
    {
        fightTrainerAiWazaValueTokubouDaun,
        fightTrainerAiWazaHit062,
        fightTrainerAiWazaDamage062,
        fightTrainerAiWazaDamageFightPokemon062,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 314 Air Cutter
    {
        fightTrainerAiWazaValuePhyKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 315 Overheat
    {
        fightTrainerAiWazaValueOobaahiito,
        fightTrainerAiWazaHit204,
        fightTrainerAiWazaDamage204,
        fightTrainerAiWazaDamageFightPokemon204,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 316 Odor Sleuth
    {
        fightTrainerAiWazaValueMiyaburu,
        fightTrainerAiWazaHit113,
        fightTrainerAiWazaDamage113,
        fightTrainerAiWazaDamageFightPokemon113,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 317 Rock Tomb
    {
        fightTrainerAiWazaValueKanarazuSubayasaDaun,
        fightTrainerAiWazaHit070,
        fightTrainerAiWazaDamage070,
        fightTrainerAiWazaDamageFightPokemon070,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 318 Silver Wind
    {
        fightTrainerAiWazaValueZennouryokuappu,
        fightTrainerAiWazaHit140,
        fightTrainerAiWazaDamage140,
        fightTrainerAiWazaDamageFightPokemon140,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 319 Metal Sound
    {
        fightTrainerAiWazaValueTokubouDaun,
        fightTrainerAiWazaHit062,
        fightTrainerAiWazaDamage062,
        fightTrainerAiWazaDamageFightPokemon062,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 320 Grass Whistle
    {
        fightTrainerAiWazaValueNemurare,
        fightTrainerAiWazaHit001,
        fightTrainerAiWazaDamage001,
        fightTrainerAiWazaDamageFightPokemon001,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 321 Tickle
    {
        fightTrainerAiWazaValueKusuguruDaun,
        fightTrainerAiWazaHit205,
        fightTrainerAiWazaDamage205,
        fightTrainerAiWazaDamageFightPokemon205,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 322 Cosmic Power
    {
        fightTrainerAiWazaValueKosumopawaa,
        fightTrainerAiWazaHit206,
        fightTrainerAiWazaDamage206,
        fightTrainerAiWazaDamageFightPokemon206,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 323 Water Spout
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit190,
        fightTrainerAiWazaDamage190,
        fightTrainerAiWazaDamageFightPokemon190,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 324 Signal Beam
    {
        fightTrainerAiWazaValueTuikouKonran,
        fightTrainerAiWazaHit076,
        fightTrainerAiWazaDamage076,
        fightTrainerAiWazaDamageFightPokemon076,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 325 Shadow Punch
    {
        fightTrainerAiWazaValueHittyuu,
        fightTrainerAiWazaHit017,
        fightTrainerAiWazaDamage017,
        fightTrainerAiWazaDamageFightPokemon017,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 326 Extrasensory
    {
        fightTrainerAiWazaValueTuikaHirumi,
        fightTrainerAiWazaHit150,
        fightTrainerAiWazaDamage150,
        fightTrainerAiWazaDamageFightPokemon150,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 327 Sky Uppercut
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit207,
        fightTrainerAiWazaDamage207,
        fightTrainerAiWazaDamageFightPokemon207,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 328 Sand Tomb
    {
        fightTrainerAiWazaValueSokubaku,
        fightTrainerAiWazaHit042,
        fightTrainerAiWazaDamage042,
        fightTrainerAiWazaDamageFightPokemon042,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 329 Sheer Cold
    {
        fightTrainerAiWazaValueItigekihissatu,
        fightTrainerAiWazaHit038,
        fightTrainerAiWazaDamage038,
        fightTrainerAiWazaDamageFightPokemon038,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 330 Muddy Water
    {
        fightTrainerAiWazaValueTuikouMeityuuDaun,
        fightTrainerAiWazaHit073,
        fightTrainerAiWazaDamage073,
        fightTrainerAiWazaDamageFightPokemon073,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 331 Bullet Seed
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 332 Aerial Ace
    {
        fightTrainerAiWazaValueHittyuu,
        fightTrainerAiWazaHit017,
        fightTrainerAiWazaDamage017,
        fightTrainerAiWazaDamageFightPokemon017,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 333 Icicle Spear
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 334 Iron Defense
    {
        fightTrainerAiWazaValueBougyoAppu,
        fightTrainerAiWazaHit051,
        fightTrainerAiWazaDamage051,
        fightTrainerAiWazaDamageFightPokemon051,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 335 Block
    {
        fightTrainerAiWazaValueKoukanKinsi,
        fightTrainerAiWazaHit106,
        fightTrainerAiWazaDamage106,
        fightTrainerAiWazaDamageFightPokemon106,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 336 Howl
    {
        fightTrainerAiWazaValueKougekiAppu,
        fightTrainerAiWazaHit010,
        fightTrainerAiWazaDamage010,
        fightTrainerAiWazaDamageFightPokemon010,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 337 Dragon Claw
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 338 Frenzy Plant
    {
        fightTrainerAiWazaValueHandou,
        fightTrainerAiWazaHit080,
        fightTrainerAiWazaDamage080,
        fightTrainerAiWazaDamageFightPokemon080,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 339 Bulk Up
    {
        fightTrainerAiWazaValueBirudoAppu,
        fightTrainerAiWazaHit208,
        fightTrainerAiWazaDamage208,
        fightTrainerAiWazaDamageFightPokemon208,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 340 Bounce
    {
        fightTrainerAiWazaValueTobihaneru,
        fightTrainerAiWazaHit155,
        fightTrainerAiWazaDamage155,
        fightTrainerAiWazaDamageFightPokemon155,
        fightWazaWzxTypeFuncTame,
        fightWazaWzxVariationFuncNormal
    },
    // 341 Mud Shot
    {
        fightTrainerAiWazaValueKanarazuSubayasaDaun,
        fightTrainerAiWazaHit070,
        fightTrainerAiWazaDamage070,
        fightTrainerAiWazaDamageFightPokemon070,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 342 Poison Tail
    {
        fightTrainerAiWazaValuePoizunteeru,
        fightTrainerAiWazaHit209,
        fightTrainerAiWazaDamage209,
        fightTrainerAiWazaDamageFightPokemon209,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 343 Covet
    {
        fightTrainerAiWazaValueDouguUbau,
        fightTrainerAiWazaHit105,
        fightTrainerAiWazaDamage105,
        fightTrainerAiWazaDamageFightPokemon105,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 344 Volt Tackle
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit198,
        fightTrainerAiWazaDamage198,
        fightTrainerAiWazaDamageFightPokemon198,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 345 Magical Leaf
    {
        fightTrainerAiWazaValueHittyuu,
        fightTrainerAiWazaHit017,
        fightTrainerAiWazaDamage017,
        fightTrainerAiWazaDamageFightPokemon017,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 346 Water Sport
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit210,
        fightTrainerAiWazaDamage210,
        fightTrainerAiWazaDamageFightPokemon210,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 347 Calm Mind
    {
        fightTrainerAiWazaValueMeisou,
        fightTrainerAiWazaHit211,
        fightTrainerAiWazaDamage211,
        fightTrainerAiWazaDamageFightPokemon211,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 348 Leaf Blade
    {
        fightTrainerAiWazaValueSpeKuritikaru,
        fightTrainerAiWazaHit043,
        fightTrainerAiWazaDamage043,
        fightTrainerAiWazaDamageFightPokemon043,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 349 Dragon Dance
    {
        fightTrainerAiWazaValueRyuunomai,
        fightTrainerAiWazaHit212,
        fightTrainerAiWazaDamage212,
        fightTrainerAiWazaDamageFightPokemon212,
        fightWazaWzxTypeFuncSpc,
        fightWazaWzxVariationFuncNormal
    },
    // 350 Rock Blast
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit029,
        fightTrainerAiWazaDamage029,
        fightTrainerAiWazaDamageFightPokemon029,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 351 Shock Wave
    {
        fightTrainerAiWazaValueHittyuu,
        fightTrainerAiWazaHit017,
        fightTrainerAiWazaDamage017,
        fightTrainerAiWazaDamageFightPokemon017,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 352 Water Pulse
    {
        fightTrainerAiWazaValueTuikouKonran,
        fightTrainerAiWazaHit076,
        fightTrainerAiWazaDamage076,
        fightTrainerAiWazaDamageFightPokemon076,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 353 Doom Desire
    {
        fightTrainerAiWazaValueToriaezutukae,
        fightTrainerAiWazaHit148,
        fightTrainerAiWazaDamage148,
        fightTrainerAiWazaDamageFightPokemon148,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 354 Psycho Boost
    {
        fightTrainerAiWazaValueSaikobuusuto,
        fightTrainerAiWazaHit204,
        fightTrainerAiWazaDamage204,
        fightTrainerAiWazaDamageFightPokemon204,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 355 end of the normal moves
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHitNull,
        fightTrainerAiWazaDamageNull,
        fightTrainerAiWazaDamageFightPokemonNull,
        fightWazaWzxTypeFuncNull,
        fightWazaWzxVariationFuncNull
    },
    // 356 Dark Blitz
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 357 Dark Rush
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 358 Dark Break
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 359 Dark End
    {
        fightTrainerAiWazaValueDarkEnd,
        fightTrainerAiWazaHit048,
        fightTrainerAiWazaDamage048,
        fightTrainerAiWazaDamageFightPokemon048,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 360 Dark Wave
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 361 Dark Rave
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 362 Dark Storm
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 363 Dark Fire
    {
        fightTrainerAiWazaValueIryokuHonoo,
        fightTrainerAiWazaHit004,
        fightTrainerAiWazaDamage004,
        fightTrainerAiWazaDamageFightPokemon004,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 364 Dark Bolt
    {
        fightTrainerAiWazaValueTuikaMahi,
        fightTrainerAiWazaHit006,
        fightTrainerAiWazaDamage006,
        fightTrainerAiWazaDamageFightPokemon006,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 365 Dark Chill
    {
        fightTrainerAiWazaValueTuikaKoori,
        fightTrainerAiWazaHit005,
        fightTrainerAiWazaDamage005,
        fightTrainerAiWazaDamageFightPokemon005,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 366 Dark Blast
    {
        fightTrainerAiWazaValueTuujouIryoku,
        fightTrainerAiWazaHit000,
        fightTrainerAiWazaDamage000,
        fightTrainerAiWazaDamageFightPokemon000,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 367 Dark Sky
    {
        fightTrainerAiWazaValueDarkWeather,
        fightTrainerAiWazaHit215,
        fightTrainerAiWazaDamage215,
        fightTrainerAiWazaDamageFightPokemon215,
        fightWazaWzxTypeFuncAtt,
        fightWazaWzxVariationFuncNormal
    },
    // 368 Dark Hold
    {
        fightTrainerAiWazaValueDarkHold,
        fightTrainerAiWazaHit106,
        fightTrainerAiWazaDamage106,
        fightTrainerAiWazaDamageFightPokemon106,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 369 Dark Mist
    {
        fightTrainerAiWazaValueDarkMist,
        fightTrainerAiWazaHit214,
        fightTrainerAiWazaDamage214,
        fightTrainerAiWazaDamageFightPokemon214,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 370 Dark Panic
    {
        fightTrainerAiWazaValueDarkPanic,
        fightTrainerAiWazaHit049,
        fightTrainerAiWazaDamage049,
        fightTrainerAiWazaDamageFightPokemon049,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 371 Dark Down
    {
        fightTrainerAiWazaValueBougyoDaun,
        fightTrainerAiWazaHit059,
        fightTrainerAiWazaDamage059,
        fightTrainerAiWazaDamageFightPokemon059,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 372 Dark Shed
    {
        fightTrainerAiWazaValueDarkRemove,
        fightTrainerAiWazaHit216,
        fightTrainerAiWazaDamage216,
        fightTrainerAiWazaDamageFightPokemon216,
        fightWazaWzxTypeFuncAllEnemy,
        fightWazaWzxVariationFuncNormal
    },
    // 373 Dark Half
    {
        fightTrainerAiWazaValueDarkHalf,
        fightTrainerAiWazaHit217,
        fightTrainerAiWazaDamage217,
        fightTrainerAiWazaDamageFightPokemon217,
        fightWazaWzxTypeFuncNormal,
        fightWazaWzxVariationFuncNormal
    },
    // 374 shadow special
    {
        fightTrainerAiWazaValueNull,
        fightTrainerAiWazaHitNull,
        fightTrainerAiWazaDamageNull,
        fightTrainerAiWazaDamageFightPokemonNull,
        fightWazaWzxTypeFuncNull,
        fightWazaWzxVariationFuncNull
    }
};

u32 waza_func_data_number = sizeof(waza_func_data) / sizeof(waza_func_data[0]);

}

