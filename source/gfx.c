#include <string.h>

#include <ppu-lv2.h>
#include <pngdec/pngdec.h>

#include <tiny3d.h>
#include <libfont.h>

#include "font.h"
#include "font_b.h"
#include "gfx.h"

#include "playerL_png_bin.h"
#include "playerR_png_bin.h"
#include "tiles_png_bin.h"
#include "exit_png_bin.h"
#include "install_png_bin.h"
#include "uninstall_png_bin.h"
#include "buttons_png_bin.h"

pngData file_Png[6];
u32 file_Png_offset[6];  

msgType mdialogyesno = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON | MSG_DIALOG_DEFAULT_CURSOR_NO;
msgType mdialogyesno2 = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON;
msgType mdialogok = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK;
msgType mdialog = MSG_DIALOG_NORMAL | MSG_DIALOG_DISABLE_CANCEL_ON;

volatile int dialog_action = 0;

int usleep(int n_usec)
{
	lv2syscall1(141, (u64)n_usec);
	return_to_user_prog(int);
}

void clear_screen()
{
	tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
        
    tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

    tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
    TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
    TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
}

void wait_dialog() 
{
    while(!dialog_action)
        {
        sysUtilCheckCallback();tiny3d_Flip();
        }

    msgDialogAbort();
    usleep(100000);
}

void my_dialog2(msgButton button, void *userdata)
{
    switch(button) {

        case MSG_DIALOG_BTN_OK:
        case MSG_DIALOG_BTN_ESCAPE:
        case MSG_DIALOG_BTN_NONE:
            dialog_action = 1;
            break;
        default:
		    break;
    }
}

void my_dialog(msgButton button, void *userdata)
{
    switch(button) {

        case MSG_DIALOG_BTN_YES:
            dialog_action = 1;
            break;
        case MSG_DIALOG_BTN_NO:
        case MSG_DIALOG_BTN_ESCAPE:
        case MSG_DIALOG_BTN_NONE:
            dialog_action = 2;
            break;
        default:
		    break;
    }
}

void DrawDialogOKTimer(char * str, float milliseconds)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogok, str, my_dialog2, (void*) 0x0000aaab, NULL );
    msgDialogClose(milliseconds);
            
    wait_dialog();
}

void DrawDialogOK(char * str)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogok, str, my_dialog2, (void*) 0x0000aaab, NULL );
            
    wait_dialog();
}

int DrawDialogYesNo(char * str)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogyesno, str, my_dialog, (void*)  0x0000aaaa, NULL );
            
    wait_dialog();

    return dialog_action;
}

void Draw_background(u32 rgba)
{
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(0  , 0  , 65535);
    tiny3d_VertexColor(rgba);

    tiny3d_VertexPos(847, 0  , 65535);

    tiny3d_VertexPos(847, 511, 65535);

    tiny3d_VertexPos(0  , 511, 65535);
    tiny3d_End();
}

void Draw_PNG(float x, float y, float z, float w, float h, u32 rgba, int i, int smooth)
{
 if(file_Png_offset[i]) {
	tiny3d_SetTextureWrap(0, file_Png_offset[i], file_Png[i].width, 
                             file_Png[i].height, file_Png[i].pitch,      
                             TINY3D_TEX_FORMAT_A8R8G8B8,  TEXTWRAP_CLAMP, TEXTWRAP_CLAMP, smooth);
    
    tiny3d_SetPolygon(TINY3D_QUADS);
    
   
    tiny3d_VertexPos(x    , y    , z);
    tiny3d_VertexColor(rgba);
    tiny3d_VertexTexture(0.0f , 0.0f);

    tiny3d_VertexPos(x + w, y    , z);
    tiny3d_VertexTexture(0.99f, 0.0f);

    tiny3d_VertexPos(x + w, y + h, z);
    tiny3d_VertexTexture(0.99f, 0.99f);

    tiny3d_VertexPos(x    , y + h, z);
    tiny3d_VertexTexture(0.0f , 0.99f);

    tiny3d_End();
    }
}

void Draw_PNG_adv(float x, float y, float z, float w, float h, float w_full, float h_full, float tex_x, float tex_y, float tex_w, float tex_h, u32 rgba, int i, int smooth)
{
 if(file_Png_offset[i]) {
	tiny3d_SetTextureWrap(0, file_Png_offset[i], file_Png[i].width, 
                             file_Png[i].height, file_Png[i].pitch,      
                             TINY3D_TEX_FORMAT_A8R8G8B8,  TEXTWRAP_CLAMP, TEXTWRAP_CLAMP, smooth);
    
    tiny3d_SetPolygon(TINY3D_QUADS);

    double px = (tex_x / w_full);	
    double py = (tex_y / h_full);
    
    double px2 = ((tex_x + tex_w) / w_full);
    double py2 = py;
    
    double px3 = px2;
    double py3 = ((tex_y + tex_h) / h_full);
    
    double px4 = px;
    double py4 = py3;
    
    tiny3d_VertexPos(x    , y   , z);
    tiny3d_VertexColor(rgba);
    tiny3d_VertexTexture(px, py);

    tiny3d_VertexPos((x + w), y   , z);
    tiny3d_VertexTexture(px2, py2);

    tiny3d_VertexPos((x + w), (y + h), z);
    tiny3d_VertexTexture(px3, py3);

    tiny3d_VertexPos(x   , (y + h), z);
    tiny3d_VertexTexture(px4, py4);

    tiny3d_End();
    }
}

void Load_PNG()
{
    pngLoadFromBuffer(tiles_png_bin, tiles_png_bin_size,       &file_Png[0]);
    pngLoadFromBuffer(playerL_png_bin, playerL_png_bin_size,   &file_Png[1]);
    pngLoadFromBuffer(playerR_png_bin, playerR_png_bin_size,   &file_Png[2]);
    pngLoadFromBuffer(exit_png_bin, exit_png_bin_size,   &file_Png[3]);
    pngLoadFromBuffer(install_png_bin, install_png_bin_size,   &file_Png[4]);
    pngLoadFromBuffer(uninstall_png_bin, uninstall_png_bin_size,   &file_Png[5]);
    pngLoadFromBuffer(buttons_png_bin, buttons_png_bin_size,     &file_Png[6]);
}

void LoadTexture()
{
    int i;
    
    u32 * texture_mem = tiny3d_AllocTexture(64*1024*1024); 

    u32 * texture_pointer;

    if(!texture_mem) return; 

    texture_pointer = texture_mem;

    ResetFont();
    
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) font  , (u8 *) texture_pointer, 32, 255, 16, 32, 2, BIT0_FIRST_PIXEL);
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) font_b, (u8 *) texture_pointer, 32, 255, 16, 32, 2, BIT0_FIRST_PIXEL);
    
    Load_PNG();

    for(i = 0; i < 7; i++) {
       
        file_Png_offset[i]   = 0;
       
        if(file_Png[i].bmp_out) {

            memcpy(texture_pointer, file_Png[i].bmp_out, file_Png[i].pitch * file_Png[i].height);
            
            free(file_Png[i].bmp_out); // free the PNG because i don't need this datas

            file_Png_offset[i] = tiny3d_TextureOffset(texture_pointer); 

            texture_pointer += ((file_Png[i].pitch * file_Png[i].height + 15) & ~15) / 4;
         }
    }
}
