#include <types.h>

#define MEMCARD_SAVE_SECTOR_SIZE 20474

u16 _memcard_encrypt_key[4];

static inline void memcardUpdateKey(u16 *encKey)
{
    u32 inc[4] = { 67, 41, 23, 19 };
    u16 key[4];
    int i;

    for (i = 0; i < 4; i++) {
        key[i] = encKey[i];
        key[i] += inc[i];
    }
    _memcard_encrypt_key[0] = (key[0] & 0x000F) | ((key[1] & 0x000F) << 4) | ((key[2] & 0x000F) << 8) | ((key[3] & 0x000F) << 12);
    _memcard_encrypt_key[1] = ((key[0] & 0x00F0) >> 4) | (key[1] & 0x00F0) | ((key[2] & 0x00F0) << 4) | ((key[3] & 0x00F0) << 8);
    _memcard_encrypt_key[2] = ((key[0] & 0x0F00) >> 8) | ((key[1] & 0x0F00) >> 4) | (key[2] & 0x0F00) | ((key[3] & 0x0F00) << 4);
    _memcard_encrypt_key[3] = ((key[0] & 0xF000) >> 12) | ((key[1] & 0xF000) >> 8) | ((key[2] & 0xF000) >> 4) | (key[3] & 0xF000);
}

bool _memcard_Decryption(void *saveImage)
{
    u16 *save = (u16 *)saveImage;
    int size = MEMCARD_SAVE_SECTOR_SIZE * 4;
    u32 sum[4];
    u32 checksum[4];
    int j;
    int i;

    for (i = 0; i < 4; i++) {
        _memcard_encrypt_key[i] = save[i];
    }
    sum[0] = 0;
    sum[1] = 0;
    sum[2] = 0;
    sum[3] = 0;
    for (i = 4; i < size; i += 4) {
        u16 *encKey = _memcard_encrypt_key;
        for (j = 0; j < 4; j++) {
            u16 k = encKey[j];
            k = save[i + j] - k;
            save[i + j] = k;
        }
        memcardUpdateKey(encKey);
    }
    checksum[0] = save[10] | (save[11] << 16);
    checksum[1] = save[8] | (save[9] << 16);
    checksum[2] = save[6] | (save[7] << 16);
    checksum[3] = save[4] | (save[5] << 16);
    for (i = 0; i < 4; i++) {
        save[10 - i * 2] = 0;
        save[10 - i * 2 + 1] = 0;
    }
    for (j = 0; j < 4; j++) {
        int end = (j + 1) * MEMCARD_SAVE_SECTOR_SIZE;
        for (i = j * MEMCARD_SAVE_SECTOR_SIZE; i < end; i++) {
            sum[j] += save[i];
        }
    }
    if (sum[0] == checksum[0] && sum[1] == checksum[1] && sum[2] == checksum[2] && sum[3] == checksum[3]) {
        return true;
    }
    return false;
}
