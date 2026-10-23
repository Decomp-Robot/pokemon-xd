#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H
#include <types.h>

extern "C" {
void scriptModelStopMotion(unsigned long resourceID);
void scriptModelClose(unsigned long resourceID);
unsigned long scriptModelOpen(unsigned long resourceID);
void scriptModelSetMotion(unsigned long resourceID, long motion, long frame, long loop);
}
#endif
