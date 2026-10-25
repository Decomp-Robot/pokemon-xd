#ifndef SOUND_TEST_DATA_H
#define SOUND_TEST_DATA_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Sound test resource tables.  Each category pairs the number of sound ids
 * in its table with the table itself, both filled in when the data tables
 * are loaded. */
extern u32 *snd_test_se_number;
extern u16 *snd_test_se;
extern u32 *snd_test_nakigoe_number;
extern u16 *snd_test_nakigoe;
extern u32 *snd_test_env_number;
extern u16 *snd_test_env;
extern u32 *snd_test_bgm_number;
extern u16 *snd_test_bgm;

#ifdef __cplusplus
}
#endif

#endif
