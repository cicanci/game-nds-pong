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
}MySprite;

int main(void)
{
//    int i = 0;
    int x = 0;
    int y = 150;
    
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);
    
    vramSetBankA(VRAM_A_MAIN_BG);
    //vramSetBankA(VRAM_A_MAIN_SPRITE);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    
    // Backgrounds
    
    bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    decompress(backgroundBitmap, BG_GFX, LZ77Vram);
    
    bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    decompress(backgroundBitmap, BG_GFX_SUB, LZ77Vram);

    // Players
    
    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
    oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
    
    MySprite red = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 0, 0};
    red.gfx = oamAllocateGfx(&oamMain, red.size, red.format);
    dmaCopy(redBitmap, red.gfx, 32*32);
    
    MySprite blue = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 0, 150};
    blue.gfx = oamAllocateGfx(&oamSub, blue.size, blue.format);
    dmaCopy(blueBitmap, blue.gfx, 32*32);

//    u16* gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
//    u16* gfxSub = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
//    
//    for(i = 0; i < 32 * 32 / 2; i++)
//    {
//        gfx[i] = 1 | (1 << 8);
//        gfxSub[i] = 1 | (1 << 8);
//    }
//    
//    SPRITE_PALETTE[1] = RGB15(31, 31, 31);
//    SPRITE_PALETTE_SUB[1] = RGB15(31, 31, 31);
    
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
               0,					  //this is the palette index if multiple palettes or the alpha value if bmp sprite
               red.size, //SpriteSize_32x32,
               red.format, //SpriteColorFormat_256Color,
               red.gfx, //gfx,                  //pointer to the loaded graphics
               -1,                  //sprite rotation data
               false,               //double the size when rotating?
               false,			//hide the sprite?
               false, false, //vflip, hflip
               false	//apply mosaic
               );
        
        
        oamSet(&oamSub, 
               0,
               x,
               y,
               0, 
               0,
               blue.size,
               blue.format,
               blue.gfx,
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
