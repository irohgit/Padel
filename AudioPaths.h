#pragma once


// Source of Truth for names that Padel uses to communicate with FMOD.
// Requirement: Names must match FMOD Studio exactly.
// Check: Game::Initialize checks these


namespace AudioPaths{
    // Banks: strings first as names to GUIDS, then the content
    inline constexpr const char* MASTER_STRINGS_BANK = "Assets/Desktop/Master.strings.bank";
    inline constexpr const char* MASTER_BANK = "Assets/Desktop/Master.bank";

    // Events SFX
    inline constexpr const char* HIT_EVENT = "event:/SFX/Hit";
    inline constexpr const char* WALL_EVENT = "event:/SFX/Wall";
    // Events MUSIC
    inline constexpr const char* MENU_MUSIC_EVENT = "event:/Music/Menu";
    inline constexpr const char* MATCH_MUSIC_EVENT = "event:/Music/Theme";
    // Events Stingers
    inline constexpr const char* TITLE_STING_EVENT           = "event:/Stinger/TitleLogo";
    inline constexpr const char* WIN_STING_EVENT             = "event:/Stinger/Win";
    // Events: umpire VO
    inline constexpr const char* UMPIRE_SCORE_EVENT          = "event:/VO/Umpire/Score";
    inline constexpr const char* UMPIRE_ADVANTAGE_EVENT      = "event:/VO/Umpire/Advantage";
    inline constexpr const char* UMPIRE_GAME_EVENT           = "event:/VO/Umpire/Game";
    inline constexpr const char* UMPIRE_DEUCE_EVENT          = "event:/VO/Umpire/Deuce";
    inline constexpr const char* UMPIRE_GAME_SET_MATCH_EVENT = "event:/VO/Umpire/GameSetMatch";

    // Parameters: SFX
    inline constexpr const char* WEIGHT_PARAM = "Weight"; // Light 0, Medium 1, Heavy 2
    inline constexpr const char* PAN_PARAM = "Pan"; // -1 left to +1 right
    // Parameters: umpire
    inline constexpr const char* FIRST_PARAM  = "First";    // love 0, fifteen 1, thirty 2, forty 3
    inline constexpr const char* SECOND_PARAM = "Second";   // love 0, fifteen 1, thirty 2, forty 3, all 4
    inline constexpr const char* SIDE_PARAM   = "Side";     // left 0, right 1
    // Buses
    inline constexpr const char* SFX_BUS        = "bus:/SFX";
    inline constexpr const char* CROWD_BUS      = "bus:/Crowd";
    inline constexpr const char* MUSIC_BUS      = "bus:/Music";
    inline constexpr const char* UI_BUS         = "bus:/UI";
    inline constexpr const char* VO_BUS         = "bus:/VO";
    inline constexpr const char* UMPIRE_BUS     = "bus:/VO/Umpire";
    inline constexpr const char* COMMENTARY_BUS = "bus:/VO/Commentary";   
     
    // New events must be added to this list (not good design but fix later)
    inline constexpr const char* REQUIRED_EVENTS[] = {
    HIT_EVENT, 
    UMPIRE_ADVANTAGE_EVENT,
    UMPIRE_DEUCE_EVENT,
    UMPIRE_GAME_EVENT,
    UMPIRE_GAME_SET_MATCH_EVENT,
    UMPIRE_SCORE_EVENT,
    WALL_EVENT,
    TITLE_STING_EVENT,
    MENU_MUSIC_EVENT,
    MATCH_MUSIC_EVENT,
    WIN_STING_EVENT
    //TODO:
    };
}
