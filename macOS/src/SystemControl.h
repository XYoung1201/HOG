#pragma once
#include "HOGN.h"

// Shutdown / reboot with a 20-second grace period that CANCELSHUTDOWNORREBOOT
// can abort, matching the Windows InitiateSystemShutdownEx behaviour.

void cancelShutdown();
void shutdown();
void reboot();
