#pragma once
#include "HOGN.h"

// ============================================================================
// Config file watcher: polls the config file's last-write timestamp and
// triggers a reload once edits settle. Used after CONFIG opens the editor.
// ============================================================================

void StartConfigWatcher();
