#include "SoundControl.h"

CComPtr<IAudioEndpointVolume> getAudioEndpointVolume() {
	HRESULT hr;
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	CComPtr<IMMDevice> defaultDevice;
	CComPtr<IAudioEndpointVolume> endpointVolume;

	hr = CoInitialize(NULL);
	if (FAILED(hr))
		return nullptr;

	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&deviceEnumerator
	);
	if (FAILED(hr))
		return nullptr;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	if (FAILED(hr))
		return nullptr;

	hr = defaultDevice->Activate(
		__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&endpointVolume
	);
	if (FAILED(hr))
		return nullptr;
	return endpointVolume;
}

void mute() {
	auto endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) return;
	endpointVolume->SetMute(TRUE, NULL);
}

void unmute() {
	auto endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) return;
	endpointVolume->SetMute(FALSE, NULL);
}

void setVolume(float volume) {
	auto endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) return;
	if (volume < 0.0f) volume = 0.0f;
	else if (volume > 1.0f) volume = 1.0f;
	endpointVolume->SetMasterVolumeLevelScalar(volume, NULL);
}

float getVolume() {
	auto endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) return -1.0f;
	float currentVolume = 0.0f;
	HRESULT hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	if (FAILED(hr)) return -1.0f;
	return currentVolume;
}

void toggleMute() {
	auto endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) return;
	BOOL isMuted;
	if (FAILED(endpointVolume->GetMute(&isMuted)))
		return;
	if (isMuted) {
		unmute();
		ShowToastMessage("Sound on");
	} else {
		mute();
		ShowToastMessage("Sound off");
	}
}
