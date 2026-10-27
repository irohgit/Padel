// ----------------------------------------------------------------
// Modified for .PADEL by Iroh Nguyen (2026): 3D sound removed.
// Migrated from FMOD 1.09 to FMOD Studio 2.03.
//
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See External/Madhav-LICENSE.txt for full details.
// ----------------------------------------------------------------

#pragma once
#include <string>

class AudioSystem;   // forward declaration: only a pointer is stored

// A handle to one playing FMOD event, looked up by ID so it is safe after the sound ends
class SoundEvent
{
public:
	SoundEvent();
	// Returns true if associated FMOD event still exists
	bool IsValid();
	// Restart event from beginning
	void Restart();
    // Start Event
    void Start();
	// Stop this event
	void Stop(bool allowFadeOut = true);
	// Setters
	void SetPaused(bool pause);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetParameter(const std::string& name, float value);
	// Getters
	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name);
	// Removed: Is3D and Set3DAttributes (3D positioning); Padel pans with a parameter
protected:
	// Only AudioSystem can create a SoundEvent with a real ID
	friend class AudioSystem;
	SoundEvent(AudioSystem* system, unsigned int id);
private:
	AudioSystem* mSystem = nullptr;
	unsigned int mID = 0;
};
