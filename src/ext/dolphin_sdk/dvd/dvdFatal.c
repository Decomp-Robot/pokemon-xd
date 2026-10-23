#include <dolphin.h>
#include <dolphin/dvd.h>

#include "__dvd.h"

static void (*FatalFunc)();

const char* Japanese =
    "\x0a\x0a\x0a\x83\x47\x83\x89\x81\x5b\x82\xaa\x94\xad\x90\xb6\x82\xb5\x82\xdc\x82\xb5\x82\xbd\x81\x42\x0a\x0a\x96\x7b\x91\xcc\x82\xcc\x83\x70\x83\x8f\x81\x5b\x83\x7b\x83\x5e\x83\x93\x82\xf0\x89\x9f\x82\xb5\x82\xc4\x93\x64\x8c\xb9\x82\xf0\x4f\x46\x46\x82\xc9\x82\xb5\x81\x41\x0a\x96\x7b\x91\xcc\x82\xcc\x8e\xe6\x88\xb5\x90\xe0\x96\xbe\x8f\x91\x82\xcc\x8e\x77\x8e\xa6\x82\xc9\x8f\x5d\x82\xc1\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42\x00\x00";

const char* English = 
    "\n\n\nAn error has occurred.\n"
    "Turn the power off and refer to the\n"
    "Nintendo GameCube Instruction Booklet\n"
    "for further instructions.";

// TODO: need solution to compile special characters in a cleaner way
const char* const Europe[6] = {
    {
        "\n\n\nAn error has occurred.\n"
        "Turn the power off and refer to the\n"
        "Nintendo GameCube Instruction Booklet\n"
        "for further instructions."
    },
    {
        "\n\n\nEin Fehler ist aufgetreten.\n"
        "Bitte schalten Sie den Nintendo GameCube\n"
        "aus und lesen Sie die Bedienungsanleitung,\n"
        "um weitere Informationen zu erhalten."
    },
    {
        "\n\n\nUne erreur est survenue.\n"
        "Eteignez la console et r\xE9" "f" "\xE9rez-vous au\n"
        "manuel d'instructions Nintendo GameCube\n"
        "pour de plus amples informations."
    },
    {
        "\n\n\nSe ha producido un error.\n"
        "Apaga la consola y consulta el manual\n"
        "de instrucciones de Nintendo GameCube\n"
        "para obtener m\xE1" "s informaci\xF3" "n."
    },
    {
        "\n\n\nSi \xE8" " verificato un errore.\n"
        "Spegni (OFF) e controlla il manuale\n"
        "d'istruzioni del Nintendo GameCube\n"
        "per ulteriori indicazioni."
    },
    {
        "\n\n\nEr is een fout opgetreden.\n"
        "Zet de Nintendo GameCube uit en\n"
        "raadpleeg de handleiding van de\n"
        "Nintendo GameCube voor nadere\n"
        "instructies."
    },
};

static void ShowMessage(void) {
    const char* message;
    GXColor bg = {0x00, 0x00, 0x00, 0x00};
    GXColor fg = {0xFF, 0xFF, 0xFF, 0x00};

    if (VIGetTvFormat() == VI_NTSC) {
        if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS) {
            message = Japanese;
        } else {
            message = English;
        }
    } else {
        message = Europe[OSGetLanguage()];
    }

    OSFatal(fg, bg, message);
}

int DVDSetAutoFatalMessaging(BOOL enable) {
    BOOL enabled;
    int prev;

    enabled = OSDisableInterrupts();

    prev = FatalFunc ? 1 : 0;
    FatalFunc = enable ? ShowMessage : NULL;

    OSRestoreInterrupts(enabled);
    return prev;
}

void __DVDPrintFatalMessage(void) {
    if (FatalFunc) {
        FatalFunc();
    }
}
