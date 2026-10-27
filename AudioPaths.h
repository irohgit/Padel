#pragma once

// Source of Truth for names that Padel uses to communicate with FMOD.
// Requirement: Names must match FMOD Studio exactly.
// Check: Game::Initialize checks these


namespace AudioPaths{
    // Banks: strings first as names to GUIDS, then the content
    inline constexpr const char* MASTER_STRINGS_BANK = "Assets/Desktop/Master.strings.bank";
    inline constexpr const char* MASTER_BANK = "Assets/Desktop/Master.bank";

    // Events
    inline constexpr const char* HIT_EVENT = "event:/SFX/Hit";
    inline constexpr const char* WALL_EVENT = "event:/SFX/Wall";

    // Parameters
    inline constexpr const char* WEIGHT_PARAM = "Weight";
    inline constexpr const char* PAN_PARAM = "Pan";

    // Buses
    inline constexpr const char* SFX_BUS = "bus:/SFX";
    inline constexpr const char* VO_BUS = "bus:/VO";

    // New events must be added to this list (not good design but fix later)
    inline constexpr const char* REQUIRED_EVENTS[] = {HIT_EVENT, WALL_EVENT};
}
