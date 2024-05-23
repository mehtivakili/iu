#ifndef UWB_H
#define UWB_H

#include <DW1000.h>
#include <DW1000Ranging.h>

void newRange();
void newDevice(DW1000Device *device);
void inactiveDevice(DW1000Device *device);

#endif // UWB_H
