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
    // STBIDEF void stbi_image_free(void *retval_from_stbi_load)
    stbi_image_free(Pixels);

    //DECODE FINISHED
    
    //CREATE EMPTY GPU TEXTURE

    return false; // return true when finished

}

