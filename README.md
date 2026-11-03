# .PADEL

A local two-player 2D pong-like game with roguelike elements.

**Goal:** Demonstrate audio middleware (FMOD Studio) integration into a C++/SDL2 engine built following Sanjay Madhav's *Game Programming in C++*, through a complete game.

## Technologies

1. **Language:** C++17, built with MSVC (Visual Studio 2022), x64
2. **Platform layer and rendering:** SDL2 (window, input, game controllers, 2D renderer)
3. **Audio:** FMOD Studio API (banks and events authored in FMOD Studio)
4. **Image decoding:** stb_image v2.30 (single-header, one implementation unit in `StbImage.cpp`)
5. **Art:** Aseprite (sprite sheets exported as PNG plus JSON, preprocessed into frame tables at build time)
6. **Editor:** Neovim with clangd

## Code Style

[C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) for code behaviour, plus the [Epic C++ Coding Standard](https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine) for naming and layout.

## Credits

<div align="center">

<sub>ENGINE FOUNDATIONS</sub><br>
<b>Sanjay Madhav</b><br>
<i>Game Programming in C++</i><br>
<sub>BSD 3-clause, see External/Madhav-LICENSE</sub>

<br>

<sub>AUDIO ENGINE</sub><br>
<b>Made with FMOD Studio by Firelight Technologies Pty Ltd.</b>

<br>

<sub>ART</sub><br>
<b><a href="https://www.artstation.com/marketplace/p/r5XP2/tiny-tennis-players">Tiny Tennis Players</a></b><br>
by TokeGameArt<br>
<sub>ArtStation Marketplace, purchased licence</sub>

<br>

<sub>MATCH MUSIC</sub><br>
<b>Vitalezzz - Virtual Rush</b><br>
<sub>via <a href="https://opengameart.org/content/virtual-rush">OpenGameArt</a>, CC0</sub>

<br>

<sub>MENU MUSIC</sub><br>
<b>"Trance Menu"</b><br>
by <a href="http://soundcloud.com/rezoner/">rezoner</a><br>
<sub>via <a href="https://opengameart.org/content/trance-menu">OpenGameArt</a>, <a href="https://creativecommons.org/licenses/by/3.0/">CC-BY 3.0</a></sub>

<br>

<sub>SOUND EFFECTS</sub><br>
<b>Kenney</b><br>
<sub><a href="https://kenney.nl">kenney.nl</a>, CC0</sub>

<br>

<sub>VOICE</sub><br>
<b>Iroh Nguyen</b><br>
<sub>umpire, players</sub>

</div>
