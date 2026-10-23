#ifndef APP_SCRIPT_H
#define APP_SCRIPT_H

#include <types.h>
#include <GSscript.h>

class AppScript : public GSscript::Tiga {
public:
    AppScript();
    void unload();
    virtual int execExtendMethod(int, int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    virtual int execExtendGlobalFunction(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    virtual int getExtendVariable(GSscript::tigaVariant*, int, int);
    virtual int setExtendVariable(GSscript::tigaVariant*, int, int);
    virtual void mapping();
    virtual void unmapping();
    virtual ~AppScript();
    int commonCall(GSscript::tigaVariant*, GSscript::tigaVariant*);
    int orFlagTest(GSscript::tigaVariant*);
    int andFlagTest(GSscript::tigaVariant*);
    int fncPrintf(GSscript::tigaVariant*);
    int cmpPeople(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpCamera(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpPokemon(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpFloor(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpMenu(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpHero(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpEsaba(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpDarkPokemon(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpWaza(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpSodateya(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpTv(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpDirection(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpMail(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpPad(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpSound(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpCol(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpModel(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpLight(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpFight(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpFade(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    int cmpTask(int, GSscript::tigaVariant*, GSscript::tigaVariant*);
    GSscript::tigaWaza* allocWazaPtr();
    void freeWazaPtr(GSscript::tigaWaza*);

    GSscript::tigaWaza mWaza[7];
};

GSscript::Tiga* scriptCreateAppScript();

#endif
