#pragma once
#include "HOGN.h"

// Shutdown / reboot / privilege helpers.

bool IsProcessRunAsAdmin();
void cancelShutdown();
void setShutdownPrivileges();
void shutdown();
void reboot();
