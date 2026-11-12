#include "app/kouka/koukaBios.h"

extern "C" {

unsigned short koukaLinkDataBiosGetKouka(unsigned long linkId, unsigned long index) {
    if (linkId > kouka_link_data_number || index > KOUKA_LINK_COUNT) return 0;
    return kouka_link_data[linkId].kouka[index];
}

unsigned short koukaDataBiosGetLink(unsigned long koukaId) {
    if (koukaId > kouka_data_number) return 0;
    return kouka_data[koukaId].link;
}

short koukaDataBiosGetValue(unsigned long koukaId, unsigned long index) {
    if (koukaId > kouka_data_number || index > KOUKA_VALUE_COUNT) return 0;
    return kouka_data[koukaId].value[index];
}

unsigned char koukaDataBiosGetVar(unsigned long koukaId) {
    if (koukaId > kouka_data_number) return 0;
    return kouka_data[koukaId].var;
}

unsigned short koukaDataBiosGetStatus(unsigned long koukaId) {
    if (koukaId > kouka_data_number) return 0;
    return kouka_data[koukaId].status;
}

unsigned char koukaDataBiosGetStatusKind(unsigned long koukaId) {
    if (koukaId > kouka_data_number) return 0;
    return kouka_data[koukaId].statusKind;
}

}
