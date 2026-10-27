// ----------------------------------------------------------------
// Modified for .PADEL by Iroh Nguyen (2026): 3D sound and Game dependency removed.
// Migrated from FMOD 1.09 to FMOD Studio 2.03.
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See External/Madhav-LICENSE.txt for full details. 
// ----------------------------------------------------------------

/* FMOD is namespace: Core API
 * - System class: Core System (audio engine: mixer, channels, output)
 * - Studio: namespace of Studio API 
 * - Studio -> System : class Studio Runtime (banks, events, parameters)
 */
#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"
// Removed: only needed for 3D (Matrix4 and Vector3) #include "Math.h" 

// Forward Declarations to avoid including FMOD header
namespace FMOD
{
    class System;
    namespace Studio{
        class Bank; //
        class Bus;
        class EventDescription;
        class EventInstance;
        class System;
    };
};

class AudioSystem
{
public: 
    // AudioSystem(class Game* game); Removed as there is no Game Dependency 
    AudioSystem();
    ~AudioSystem();

    bool Initialize();
    void ShutDown();
    
    // Load/Unload banks
    void LoadBank(const std::string& name);
    void UnloadBank(const std::string& name);
    void UnloadAllBanks();

    SoundEvent PlayEvent(const std::string& name);
    SoundEvent CreateEvent(const std::string& name);

    void Update(float deltaTime);
    
    // void SetListener(const Matrix4& viewMatrix); Positional Audio disabled
    // Control buses
    float GetBusVolume(const std::string& name) const;
    bool GetBusPaused(const std::string& name) const;
    void SetBusVolume(const std::string& name, float volume);
    void SetBusPaused(const std:: string& name, bool pause);

protected:
    friend class SoundEvent;
    FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);
private:
    // Tracks the next ID 
    static unsigned int sNextID;

    //class Game* mGame; Removed no game dependency
    std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks; // Loaded Banks
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents; // Event name to Event Description
    std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances; // Event ID to Event Instance
    std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses; 

    FMOD::Studio::System* mSystem = nullptr;
    FMOD::System* mCoreSystem = nullptr;
};
