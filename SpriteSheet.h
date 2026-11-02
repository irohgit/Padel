#pragma once

#include <SDL/SDL.h>

/* [x] SpriteSheet Class
 * [1] Owns the player sprite texture, spritesheet.
 * [2] It receives an index (frame number) and outputs the frames (x,y,w,h).
 * [3] It does not need to know who uses this class, nor the titles of the Animation Segments "idle" .
 * [4] This class creates the following liabilities that must be released manually
 *         Liability: GPU Texture, created by SDL_CreateTexture in LoadTextureData. Release();
 * [5] Construction and Initialization uses a Two Phase process to make initialization errors explicit.
 */

class SpriteSheet{
public:
    // Constructor
    SpriteSheet() = default; // "Let compiler write both bodies of constructor and destructor"
    // Destructor
    ~SpriteSheet() = default; // "as above" 
    // Rule of Three 
        // Delete Copy Constructor
    SpriteSheet(const SpriteSheet&) = delete;
        // Delete Copy Assignment 
    SpriteSheet& operator=(const SpriteSheet&) = delete;

    // After successful construction of empty SpriteSheet() object, acquire the sheet's texture and assign frame data. 
    // Returns true when the sprite sheet is loaded and ready to draw.
    // Returns false if anything fails. The sheet will stay in its empty state.
    bool LoadTextureData(SDL_Renderer* Renderer, // Game's renderer of which this texture object belongs to.
                         const char* Path, // File path to desired texture
                         const SDL_Rect* InFrames, // address of the first rect in the pre-processed frame table
                         int InFrameCount // How many frame rects are in the spritesheet. 
                         );

    //TODO: Change the release function to an enforced contract
    // Release(): Frees everything that this object acquired. It returns it to an empty state. It is safe to call more than once.
    void Release(); // Free the texture 

    // Read Only Data - Getters
    SDL_Texture* GetTexture() const;
    const SDL_Rect* GetFrame(int Index) const; // Returns a nullptr if Index < 0 || Index >= FrameCount  
    int GetFrameCount() const;

private:
    SDL_Texture* Texture = nullptr;
    const SDL_Rect* Frames = nullptr;
    int FrameCount = 0;
};


// [class.pre] 
// class-specifier: class-head {member-specificationopt}
// class-head: class-key , class-head-name  then optionalal extras
// class-key: class 
//
// class-specifer: class, SpriteSheet 
// Lifetime




