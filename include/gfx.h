#ifndef GFX_H
#define GFX_H

#include <tiny3d.h>
#include <sysutil/sysutil.h>
#include <sysutil/msg.h>

void clear_screen();

void wait_dialog() ;

void my_dialog2(msgButton button, void *userdata);

void my_dialog(msgButton button, void *userdata);

void DrawDialogOKTimer(char * str, float milliseconds);

void DrawDialogOK(char * str);

int DrawDialogYesNo(char * str);

void Draw_background(u32 rgba);

void Draw_PNG(float x, float y, float z, float w, float h, u32 rgba, int i, int smooth);

void Draw_PNG_adv(float x, float y, float z, float w, float h, float w_full, float h_full, float tex_x, float tex_y, float tex_w, float tex_h, u32 rgba, int i, int smooth);

void Load_PNG();

void LoadTexture();
#endif

