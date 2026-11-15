#include <GSmovie.h>

typedef enum THPAdpcmMask {
    THP_ADPCM_NIBBLE_MASK = 0xF,
    THP_ADPCM_PREDICTOR_MASK = 0x70,
    THP_ADPCM_HIGH_NIBBLE_MASK = 0xF0,
    THP_ADPCM_LOW_NIBBLE = 1
} THPAdpcmMask;

typedef struct {
    u32 offsetNextChannel;
    u32 sampleSize;
    s16 lCoef[8][2];
    s16 rCoef[8][2];
    s16 lYn1;
    s16 lYn2;
    s16 rYn1;
    s16 rYn2;
} THPAudioRecordHeader;

typedef struct {
    u8* encodeData;
    u32 offsetNibbles;
    u8 predictor;
    u8 scale;
    s16 yn1;
    s16 yn2;
} THPAudioDecodeInfo;

static s32 __THPAudioGetNewSample(THPAudioDecodeInfo*);
static void __THPAudioInitialize(THPAudioDecodeInfo*, u8*);

u32 THPAudioDecode(s16* audioBuffer, u8* audioFrame, enum THPAudioOutputMode outputMode)
{
    THPAudioRecordHeader* header;
    THPAudioDecodeInfo decInfo;
    u8 *left, *right;
    s16 *decLeftPtr, *decRightPtr;
    s32 yn1, yn2;
    s32 i;
    s32 step;
    s32 sample;
    s64 yn;

    if (audioBuffer == NULL || audioFrame == NULL) {
        return 0;
    }

    header = (THPAudioRecordHeader*)audioFrame;
    left   = audioFrame + sizeof(THPAudioRecordHeader);
    right  = left + header->offsetNextChannel;

    if (outputMode == THP_AUDIO_PLANAR) {
        decRightPtr = audioBuffer;
        decLeftPtr  = audioBuffer + header->sampleSize;
        step        = 1;
    } else {
        decRightPtr = audioBuffer;
        decLeftPtr  = audioBuffer + 1;
        step        = 2;
    }

    if (header->offsetNextChannel == 0) {
        __THPAudioInitialize(&decInfo, left);

        yn1 = header->lYn1;
        yn2 = header->lYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfo);
            yn     = header->lCoef[decInfo.predictor][1] * (s16)yn2;
            yn += header->lCoef[decInfo.predictor][0] * (s16)yn1;
            yn += (sample << decInfo.scale) << 11;
            yn <<= 5;
            yn += 32768;

            if (yn > 2147483647LL) {
                yn = 2147483647LL;
            }

            if (yn < -2147483648LL) {
                yn = -2147483648LL;
            }

            *decLeftPtr = (s16)(yn >> 16);
            decLeftPtr += step;
            *decRightPtr = (s16)(yn >> 16);
            decRightPtr += step;
            yn2 = yn1;
            yn1 = yn >> 16;
        }
    } else {
        __THPAudioInitialize(&decInfo, left);

        yn1 = header->lYn1;
        yn2 = header->lYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfo);
            yn     = header->lCoef[decInfo.predictor][1] * (s16)yn2;
            yn += header->lCoef[decInfo.predictor][0] * (s16)yn1;
            yn += (sample << decInfo.scale) << 11;
            yn <<= 5;
            yn += 32768;

            if (yn > 2147483647LL) {
                yn = 2147483647LL;
            }

            if (yn < -2147483648LL) {
                yn = -2147483648LL;
            }

            *decLeftPtr = (s16)(yn >> 16);
            decLeftPtr += step;
            yn2 = yn1;
            yn1 = yn >> 16;
        }

        __THPAudioInitialize(&decInfo, right);

        yn1 = header->rYn1;
        yn2 = header->rYn2;

        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfo);
            yn     = header->rCoef[decInfo.predictor][1] * (s16)yn2;
            yn += header->rCoef[decInfo.predictor][0] * (s16)yn1;
            yn += (sample << decInfo.scale) << 11;
            yn <<= 5;
            yn += 32768;

            if (yn > 2147483647LL) {
                yn = 2147483647LL;
            }

            if (yn < -2147483648LL) {
                yn = -2147483648LL;
            }

            *decRightPtr = (s16)(yn >> 16);
            decRightPtr += step;
            yn2 = yn1;
            yn1 = yn >> 16;
        }
    }

    return header->sampleSize;
}

static s32 __THPAudioGetNewSample(THPAudioDecodeInfo* info)
{
    s32 sample;

    if (!(info->offsetNibbles & THP_ADPCM_NIBBLE_MASK)) {
        info->predictor = (u8)((*(info->encodeData) & THP_ADPCM_PREDICTOR_MASK) >> 4);
        info->scale     = (u8)((*(info->encodeData) & THP_ADPCM_NIBBLE_MASK));
        info->encodeData++;
        info->offsetNibbles += 2;
    }

    if (info->offsetNibbles & THP_ADPCM_LOW_NIBBLE) {
        sample = (s32)((*(info->encodeData) & THP_ADPCM_NIBBLE_MASK) << 28) >> 28;
        info->encodeData++;
    } else {
        sample = (s32)((*(info->encodeData) & THP_ADPCM_HIGH_NIBBLE_MASK) << 24) >> 28;
    }

    info->offsetNibbles++;
    return sample;
}

static void __THPAudioInitialize(THPAudioDecodeInfo* info, u8* ptr)
{
    info->encodeData    = ptr;
    info->offsetNibbles = 2;
    info->predictor     = (u8)((*(info->encodeData) & THP_ADPCM_PREDICTOR_MASK) >> 4);
    info->scale         = (u8)((*(info->encodeData) & THP_ADPCM_NIBBLE_MASK));
    info->encodeData++;
}
