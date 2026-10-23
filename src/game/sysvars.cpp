#include <types.h>
#include <GSgfx.h>
#include <GSprim.h>

void _sysvarsProcess(char *arguments);

struct sysvarsFuncEntry {
    char *name;
    void (*handler)(void *, char *);
    s32 type;
    void *value;
};

extern "C" sysvarsFuncEntry _sysvarsFuncList[];

enum sysvarsValueType {
    SYSVARS_BOOLEAN = 0,
    SYSVARS_INTEGER = 1,
    SYSVARS_FLOAT = 2,
    SYSVARS_STRING = 3,
};

extern "C" void sysvarsShowDebug()
{
    GSgfxColAu8 background = {0, 0, 0, 255};
    GSgfxColAu8 textColor = {255, 255, 255, 255};
    GSgfxGPUSetMode(1);
    for (u32 framesRemaining = 900; framesRemaining-- != 0;) {
        GSprimSetup2DRender(0);
        GSprimDraw2DQuad(background, 0.0f, 0.0f, 640.0f, 480.0f);
        GSprimSetTextColor(textColor);
        GSprimSetTextWidth(2.0f);
        GSprimSetTextSize(12.0f, 12.0f);

        f32 y = 40.0f;
        for (sysvarsFuncEntry *entry = _sysvarsFuncList;
             entry->name != 0; ++entry) {
            switch (entry->type) {
            case SYSVARS_BOOLEAN:
            case SYSVARS_INTEGER:
                GSprimWriteStr(100.0f, y, "%s=%d", entry->name,
                               *(s32 *)entry->value);
                break;
            case SYSVARS_FLOAT:
                GSprimWriteStr(100.0f, y, "%s=%f", entry->name,
                               *(f32 *)entry->value);
                break;
            case SYSVARS_STRING:
                GSprimWriteStr(100.0f, y, "%s=%s", entry->name,
                               (char *)entry->value);
                break;
            }
            y += 14.0f;
        }
        GSprimWriteStr(500.0f, 400.0f, "%d", framesRemaining / 60);
        GSgfxRender();
    }
    GSgfxGPUSetMode(2);
}

extern "C" void sysvarsReadSettings(int *argc, char ***argv)
{
    if (*argc >= 2) {
        _sysvarsProcess((*argv)[1]);
        s32 offset;
        s32 index;
        index = 1;
        offset = 4;
        while (index < *argc) {
            char **entry = (char **)((char *)*argv + offset);
            entry[0] = entry[1];
            ++index;
            offset += 4;
        }
        --*argc;
    }
}

extern "C" int stricmp(const char *left, const char *right);
extern "C" int atoi(const char *text);
extern "C" double atof(const char *text);
extern "C" char *strcpy(char *destination, const char *source);
static const char sysvarsOffText[] = "off";
static const char sysvarsFalseText[] = "false";
static const char sysvarsZeroText[] = "0";
static const char sysvarsOnText[] = "on";
static const char sysvarsTrueText[] = "true";
static const char sysvarsOneText[] = "1";
static const char sysvarsSimpleText[] = "simple";
static const char sysvarsDetailText[] = "detail";
static const char sysvarsMemory24Text[] = "24";
static const char sysvarsMemory48Text[] = "48";

sysvarsFuncEntry *_sysvarsFindEntry(char *name);
void _sysvarsProcessData(sysvarsFuncEntry *entry, char *text);

void _sysvarsProcess(char *arguments)
{
    char *cursor = arguments;
    while (*cursor != 0) {
        char *name = cursor;
        while (*cursor != 0 && *cursor != '-') {
            ++cursor;
        }
        if (*cursor == '-') {
            *cursor = 0;
        sysvarsFuncEntry *entry = _sysvarsFindEntry(name);
        ++cursor;

        char *value = cursor;
        while (*cursor != 0 && *cursor != ':') {
            ++cursor;
        }
        if (*cursor == ':') {
            *cursor++ = 0;
        }
        if (entry != 0) {
            _sysvarsProcessData(entry, value);
        }
        }
    }
}

void _sysvarsProcessData(sysvarsFuncEntry *entry, char *text)
{
    if (entry->handler == 0) {
        switch (entry->type) {
        case 0: {
            void *value = entry->value;
            if (stricmp(text, sysvarsOffText) == 0 ||
                stricmp(text, sysvarsFalseText) == 0 ||
                stricmp(text, sysvarsZeroText) == 0) {
                *(s32 *)value = 0;
            } else if (stricmp(text, sysvarsOnText) == 0 ||
                       stricmp(text, sysvarsTrueText) == 0 ||
                       stricmp(text, sysvarsOneText) == 0) {
                *(s32 *)value = 1;
            }
            break;
        }
        case 1: {
            s32 *value = (s32 *)entry->value;
            *value = atoi(text);
            break;
        }
        case 2: {
            f32 *value = (f32 *)entry->value;
            *value = (f32)atof(text);
            break;
        }
        case 3: {
            char *value = (char *)entry->value;
            strcpy(value, text);
            break;
        }
        }
    } else {
        entry->handler(entry->value, text);
    }
}

sysvarsFuncEntry *_sysvarsFindEntry(char *name)
{
    sysvarsFuncEntry *entry = _sysvarsFuncList;
    while (entry->name != 0) {
        if (stricmp(name, entry->name) == 0) {
            return entry;
        }
        ++entry;
    }
    return 0;
}

void _sysvarsPerfbar(void *destination, char *text)
{
    if (stricmp(text, sysvarsOffText) == 0) {
        *(s32 *)destination = 0;
    } else if (stricmp(text, sysvarsSimpleText) == 0) {
        *(s32 *)destination = 1;
    } else if (stricmp(text, sysvarsDetailText) == 0) {
        *(s32 *)destination = 2;
    }
}

void _sysvarsGSmemSize(void *destination, char *text)
{
    if (stricmp(text, sysvarsMemory24Text) == 0) {
        *(s32 *)destination = 0x18;
    } else if (stricmp(text, sysvarsMemory48Text) == 0) {
        *(s32 *)destination = 0x30;
    }
}
