// StbImage.cpp
// The one translation unit that compiles stb_image's definitions.
// Every other file includes <stb/stb_image.h> WITHOUT the define below,
// so the program has exactly one definition of each stb function ([basic.def.odr]).
//
// Why a dedicated file instead of SpriteSheet.cpp?
// Following John Lakos's physical-design principles, keep physical dependencies
// explicit and matching logical ones. If the define lived in SpriteSheet.cpp,
// any other user of stb (e.g. TitleSequence or MenuScreen) would depend on
// SpriteSheet.obj for code that has nothing to do with sprites.
// It also keeps ~7,200 lines of decoder from recompiling on every sprite edit.
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Relevant stb_image.h use documentation
/* stb_image - v2.30 - public domain image loader - http://nothings.org/stb
                                  no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the trivial interface
          
      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
      PNG 1/2/4/8/16-bit-per-channel
      BMP non-1bpp, non-RLE
      GIF (*comp always reports as 4-channel)

      Animated GIF still needs a proper API, but here's one way to do it:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)
*/

