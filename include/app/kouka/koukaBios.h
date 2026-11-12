#ifndef APP_KOUKA_KOUKABIOS_H
#define APP_KOUKA_KOUKABIOS_H

enum {
    KOUKA_VALUE_COUNT = 2,
    KOUKA_LINK_COUNT = 8
};

struct KoukaData {
    unsigned char statusKind;
    unsigned char var;
    unsigned short status;
    unsigned short link;
    short value[KOUKA_VALUE_COUNT];
};

struct KoukaLinkData {
    unsigned short kouka[KOUKA_LINK_COUNT];
};

#ifdef __cplusplus
extern "C" {
#endif

extern KoukaData kouka_data[];
extern KoukaLinkData kouka_link_data[];
extern unsigned long kouka_data_number;
extern unsigned long kouka_link_data_number;

unsigned short koukaLinkDataBiosGetKouka(unsigned long linkId, unsigned long index);
unsigned short koukaDataBiosGetLink(unsigned long koukaId);
short koukaDataBiosGetValue(unsigned long koukaId, unsigned long index);
unsigned char koukaDataBiosGetVar(unsigned long koukaId);
unsigned short koukaDataBiosGetStatus(unsigned long koukaId);
unsigned char koukaDataBiosGetStatusKind(unsigned long koukaId);

#ifdef __cplusplus
}
#endif

#endif
