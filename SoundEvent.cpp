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

#include "SoundEvent.h"
#include "AudioSystem.h"
#include <fmod_studio.hpp>

SoundEvent::SoundEvent(AudioSystem* system, unsigned int id) // Constructor & Params
	:mSystem(system),mID(id) // Initializer List
{} // Empty Body

SoundEvent::SoundEvent() = default;   // members already default to nullptr and 0 in the header

bool SoundEvent::IsValid()
{
	return (mSystem && mSystem->GetEventInstance(mID) != nullptr);
}

void SoundEvent::Restart()
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event) { event->start(); }
}
void SoundEvent::Start()
{
    auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
    if (event){event->start();}
}

void SoundEvent::Stop(bool allowFadeOut /* true */)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_STUDIO_STOP_MODE mode = allowFadeOut ?
			FMOD_STUDIO_STOP_ALLOWFADEOUT :
			FMOD_STUDIO_STOP_IMMEDIATE;
		event->stop(mode);
	}
}

void SoundEvent::SetPaused(bool pause)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPaused(pause);
	}
}

void SoundEvent::SetVolume(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setVolume(value);
	}
}

void SoundEvent::SetPitch(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPitch(value);
	}
}

void SoundEvent::SetParameter(const std::string& name, float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setParameterByName(name.c_str(), value);
	}
}

bool SoundEvent::GetPaused() const
{
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPaused(&retVal);
	}
	return retVal;
}

float SoundEvent::GetVolume() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getVolume(&retVal);
	}
	return retVal;
}

float SoundEvent::GetPitch() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPitch(&retVal);
	}
	return retVal;
}

float SoundEvent::GetParameter(const std::string& name)
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getParameterByName(name.c_str(), &retVal);
	}
	return retVal;
}

// Removed for .PADEL: Is3D, VecToFMOD and Set3DAttributes (3D positioning).
// Padel is 2D: panning comes from the Pan parameter set in FMOD Studio.
