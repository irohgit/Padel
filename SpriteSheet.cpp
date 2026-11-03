#include "SpriteSheet.h"
#include <stb/stb_image.h>

bool SpriteSheet::LoadTextureData(SDL_Renderer* Renderer, const char* Path,const SDL_Rect* InFrames,int InFrameCount)
{
    //Guards
    auto ParameterFail = [](const char* Why){ SDL_Log("SpriteSheet::LoadTextureData failed: %s", Why); return false;};
    if(!Renderer) return ParameterFail("Renderer is null");
    if(!Path) return ParameterFail("Path is null");
    if(!InFrames) return ParameterFail("frame rect null");
    if(InFrameCount <= 0) return ParameterFail("frame count is 0 or negative. it should be at least 1");
    if(Texture) return ParameterFail("Texture is already loaded.");
    
    //STBIDEF stbi_uc *stbi_load            (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    // unsigned char *result; this is the returned result
    // Local Variables
    int Width = 0;
    int Height = 0;
    int ChannelsInFile = 0;
    constexpr int DesiredChannels = 4; //RGBA = 4 channels thus int 4
    
    //Decode the texture using stb
    //Use stbi_load, which will overwrite local variables in stbi load (itll do it itself)
    unsigned char *Pixels = stbi_load(Path,&Width,&Height,&ChannelsInFile,DesiredChannels);
    
    if(!Pixels){
        SDL_Log("SpriteSheet::LoadTextureData failed to decode spritesheet '%s': %s", Path, stbi_failure_reason());
        return false; //exit
    }
    
    // Create an Empty GPU texture, of which its format matches stb's RGBA bytes (RGBA32).
    Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC, Width,Height); 
    if(!Texture){
        SDL_Log("SpriteSheet::LoadTextureData failed. Failure: '%s'", SDL_GetError());
        //Free Image STBIDEF void stbi_image_free(void *retval_from_stbi_load)
        stbi_image_free(Pixels);
        return false;
    }

    // Upload? 
    int Pitch = Width * DesiredChannels;
    int UploadResult = SDL_UpdateTexture(Texture,nullptr, Pixels,Pitch); 
    if (UploadResult != 0) {
        SDL_Log("SpriteSheet::LoadTextureData failed. Failure: '%s' ", SDL_GetError());
        //Free CPU Memory 
        stbi_image_free(Pixels);
        //Free GPU Memory & ensure address is nullptr
        Release();
        // Return early with false
        return false;
    }
    // Ensure Alpha is correctly set via BlendMode = Blend
    int BlendResult = SDL_SetTextureBlendMode(Texture,SDL_BLENDMODE_BLEND);
    if (BlendResult != 0){
        SDL_Log("SpriteSheet::LoadTextureData has failed. Failure '%s'", SDL_GetError());
        // Free CPU Memory
        stbi_image_free(Pixels);
        // Free GPU Memory
        Release();
        // Return early with false
        return false;
    }
    
    // Store Frame Data
    Frames = InFrames;
    FrameCount = InFrameCount;
    //Free Image STBIDEF void stbi_image_free(void *retval_from_stbi_load)
    stbi_image_free(Pixels);
    return true;
} 

void SpriteSheet::Release(){
    // Return SpriteSheet to empty state
    if(Texture){ 
        SDL_DestroyTexture(Texture);
        Texture = nullptr;
    }
    Frames = nullptr;
    FrameCount = 0;
}
// Getters
SDL_Texture* SpriteSheet::GetTexture() const{
    return Texture;
}

int SpriteSheet::GetFrameCount() const{
    return FrameCount;
}

const SDL_Rect* SpriteSheet::GetFrame(int Index) const{
    // Guard Out of bound range
    if(Index < 0 || Index >= FrameCount){ return nullptr;}
    return &Frames[Index];
}
