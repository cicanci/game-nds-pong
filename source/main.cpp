
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "background.h"
#include "blue.h"
#include "red.h"

const int SPEED = 4;

typedef struct
{
    u16* gfx;
    SpriteSize size;
    SpriteColorFormat format;
    int rotationIndex;
    int paletteAlpha;
    int x;
    int y;
} SpriteInfo;

void initVideo()
{
    /*
     *  Map VRAM to display a background on the main and sub screens.
     *
     *  The vramSetPrimaryBanks function takes four arguments, one for each of
     *  the major VRAM banks. We can use it as shorthand for assigning values to
     *  each of the VRAM bank's control registers.
     *
     *  We map banks A and B to main screen  background memory. This gives us
     *  256KB, which is a healthy amount for 16-bit graphics.
     *
     *  We map bank C to sub screen background memory.
     *
     *  We map bank D to LCD. This setting is generally used for when we aren't
     *  using a particular bank.
     *
     *  We map bank E to main screen sprite memory (aka object memory).
     */
    vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
                        VRAM_B_MAIN_BG_0x06020000,
                        VRAM_C_SUB_BG_0x06200000,
                        VRAM_D_LCD);
    
    vramSetBankE(VRAM_E_MAIN_SPRITE);
    
    /*  Set the video mode on the main screen. */
    videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
                 //DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                 DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                 DISPLAY_SPR_ACTIVE | // Enable sprites for display
                 DISPLAY_SPR_1D     // Enable 1D tiled sprites
                 );
    
    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
                    DISPLAY_BG3_ACTIVE); // Enable BG3 for display
}

void initBackground()
{
    bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    decompress(backgroundBitmap, BG_GFX, LZ77Vram);
    
    bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    decompress(backgroundBitmap, BG_GFX_SUB, LZ77Vram);
}

int main(void)
{
    /*  Turn on the 2D graphics core. */
    powerOn(POWER_ALL_2D);
    
    initVideo();
    initBackground();
    
    // Players
    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
    oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
    
    SpriteInfo redPlayer = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 0, 0};
    redPlayer.gfx = oamAllocateGfx(&oamMain, redPlayer.size, redPlayer.format);
    dmaCopy(redBitmap, redPlayer.gfx, 32*32);
    
    SpriteInfo bluePlayer = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 0, 150};
    bluePlayer.gfx = oamAllocateGfx(&oamSub, bluePlayer.size, bluePlayer.format);
    dmaCopy(blueBitmap, bluePlayer.gfx, 32*32);
    
    //    u16* gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
    //    u16* gfxSub = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    //
    //    int i = 0;
    //    for(i = 0; i < 32 * 32 / 2; i++)
    //    {
    //        gfx[i] = 1 | (1 << 8);
    //        gfxSub[i] = 1 | (1 << 8);
    //    }
    //
    //    SPRITE_PALETTE[1] = RGB15(31, 31, 31);
    //    SPRITE_PALETTE_SUB[1] = RGB15(31, 31, 31);
    
    SPRITE_PALETTE[1] = RGB15(0, 31, 0);
    SPRITE_PALETTE_SUB[1] = RGB15(0, 31, 0);
    
    int x = 0;
    int y = 150;
    
    while(1)
    {
        scanKeys();
        
        int held = keysHeld();
        
        if(held & KEY_LEFT)
        {
            x -= SPEED;
        }
        
        if(held & KEY_RIGHT)
        {
            x += SPEED;
        }
        
        oamSet(&oamMain, //main graphics engine context
               0,           //oam index (0 to 127)
               0, 0, //touch.px, touch.py,   //x and y pixle location of the sprite
               0,                    //priority, lower renders last (on top)
               0,                      //this is the palette index if multiple palettes or the alpha value if bmp sprite
               redPlayer.size, //SpriteSize_32x32,
               redPlayer.format, //SpriteColorFormat_256Color,
               redPlayer.gfx, //gfx,                  //pointer to the loaded graphics
               -1,                  //sprite rotation data
               false,               //double the size when rotating?
               false,            //hide the sprite?
               false, false, //vflip, hflip
               false    //apply mosaic
               );
        
        
        oamSet(&oamSub,
               0,
               x,
               y,
               0,
               0,
               bluePlayer.size,
               bluePlayer.format,
               bluePlayer.gfx,
               -1,
               false,
               false,
               false, false,
               false
               );
        
        swiWaitForVBlank();
        
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);
    }
    
    return 0;
}
