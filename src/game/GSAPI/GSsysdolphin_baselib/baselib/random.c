#include <hsdRandom.h>
#include <hsd/memory.h>

static u32 __hsd_rand_seed_default = 1;
u32 *__hsd_rand_seed_hold = &__hsd_rand_seed_default;

void _HSD_RandForgetMemory(void)
{
    if (_HSD_MemCheckOwn(__hsd_rand_seed_hold)) {
        __hsd_rand_seed_hold = &__hsd_rand_seed_default;
    }
}

s32 HSD_Randi(s32 limit)
{
    *__hsd_rand_seed_hold = *__hsd_rand_seed_hold * 214013 + 2531011;
    return (limit * (s32)(*__hsd_rand_seed_hold >> 16)) / 65536;
}

f32 HSD_Randf(void)
{
    *__hsd_rand_seed_hold = *__hsd_rand_seed_hold * 214013 + 2531011;
    return (f32)(*__hsd_rand_seed_hold >> 16) / 65536.0f;
}

unsigned long HSD_Rand(void)
{
    *__hsd_rand_seed_hold = *__hsd_rand_seed_hold * 214013 + 2531011;
    return *__hsd_rand_seed_hold >> 16;
}
