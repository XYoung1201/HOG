#pragma once
#include "HOGN.h"

// System audio endpoint control (master volume + mute).

CComPtr<IAudioEndpointVolume> getAudioEndpointVolume();
void mute();
void unmute();
void setVolume(float volume);
float getVolume();
void toggleMute();
