// HardwareProfile.h

#ifndef _HARDWARE_PROFILE_H_
#define _HARDWARE_PROFILE_H_

#include <p32xxxx.h>
#include <plib.h>

// из libraries/DNETcK/utility/HardwareProfile.x
// (без этого не скомпилируются модули Tcp)
#include "Compiler.h"

// Network hardware
#define DNETcK_LAN_Network
#include <NetworkProfile.x>
#include <CommonHardwareProfile.h>


#endif
