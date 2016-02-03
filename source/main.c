/***
 	(c) 2016 PS3ita Team <www.ps3ita.it>

    Funny Mamba Autoloader Installer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Funny Mamba Autoloader Installer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    a long with Funny Mamba Autoloader Installer.  If not, see <http://www.gnu.org/licenses/>.
    
***/

#include <stdio.h>
#include <math.h> 

#include <sys/file.h>
#include <sys/process.h>
#include <io/pad.h>

#include <sysmodule/sysmodule.h>
#include <pngdec/pngdec.h>

#include <libfont.h>

#include "gfx.h"
#include "pad.h"
#include "installer.h"
#include "common.h"

#define NO	0
#define YES	1

#define tiles		0
#define playerL		1
#define playerR		2
#define exit		3
#define install		4
#define uninstall	5
#define buttons		6

extern pngData file_Png[6];
extern u32 file_Png_offset[6];

padActParam actParam;

int mamba;
int destroy_world = NO;
int screen = 0;
int reboot = NO;

void exiting()
{
    sysModuleUnload(SYSMODULE_PNGDEC);
    ioPadEnd();
    if(reboot == YES)
    {
		sysLv2FsUnlink("/dev_hdd0/tmp/turnoff");
		lv2syscall3(379, 0x1200, 0, 0);
	}
}

int bye = NO;
int frame = 0;

int block = 0;
int s = 0;
int exitY = 262;
int installY = 262;
float vel_fall = 0;
float vel_fall2 = 0;

float px = 0, py = 0;
float ptx = 0, pty = 0;
int pblk = 0;

float pw = 47, ph = 60;
float rpw = 186, rph = 236;

#define LEFT	0
#define RIGHT	1
int dir = RIGHT;

float SPD_WALK	= 3;
float SPD_RUN	= 4;
float SPD_WALK_AIR = 3;
float SPD_RUN_AIR = 4;
float JUMP_FORCE = 8.1;//10; same that vel_jump
float speed  = 3; 
float acc_run = 1;
float acc_jump = 26;//30;
float vel_jump = 8.1; //10;

float frm_time = 0.0166667;
float final_v = 0;

#define FRONT	0
#define IDLE	1
#define WALK	2
#define RUN		3
#define JUMP	4
int stat_mov = FRONT;
int stat_mov_tmp  = FRONT;

#define FRAME_FRONT 11
#define FRAME_IDLE	11
#define FRAME_WALK	4
#define FRAME_RUN 	3
#define FRAME_JUMP  1

int vel_mov = FRAME_IDLE;
int hello = 0;

/** TILES **/
//Type
#define T_EMPTY		666
#define T_SOLID		1
#define T_WEIRD		2

//id
#define BG		1
#define BB		2
#define WB		3
#define WG		4

typedef struct {
	int type;
	int id;
	float tex_x;
	float tex_y;
	float tex_h;
	float tex_w;
	int line;
	int col;
} tile;

tile list_tile[5] = 
{
	{T_EMPTY, 0, 0, 0, 32, 32, 0, 0},
	{T_SOLID, BG, 0, 0, 70, 70, 0, 0},
	{T_SOLID, BB, 70, 0, 70, 70, 0, 1},
	{T_WEIRD, WB, 140, 0, 70, 70, 0, 2},
	{T_WEIRD, WG, 210, 0, 70, 70, 0, 3}
};

int T_WIDTH	 = 32;
int T_HEIGHT = 32;

#define LINE	 16
#define COL	 	27

int layer1[LINE][COL] = 
{
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666, WB,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG,
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666, 
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,
	666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666,666
};

void draw_layer1()
{
	int wx, hy;
	
	for(hy = 0; hy < LINE; ++hy)
	{
		for(wx = 0; wx < COL; ++wx)
		{
			if(list_tile[layer1[hy][wx]].type != T_EMPTY) //Tile non vuoti
			{			
				Draw_PNG_adv(wx * T_WIDTH, hy * T_HEIGHT, 0, T_WIDTH, T_HEIGHT, file_Png[tiles].width, file_Png[tiles].height, list_tile[layer1[hy][wx]].tex_x, list_tile[layer1[hy][wx]].tex_y, list_tile[layer1[hy][wx]].tex_w, list_tile[layer1[hy][wx]].tex_h, 0xffffffff, tiles, 1);	
			}
		}
	}
}

void mamba_install_uninstall()
{
	block = 0;
	destroy_world = NO;
	
	Draw_background(0x000000ff);
	
	if(mamba == SUCCESS)
	{
		if(DrawDialogYesNo("Do you want uninstall Mamba Autoloader?") == YES)
		{
			DrawDialogOKTimer("Please wait..", 1000.0f);

			if(run_uninstall_autoloader() == SUCCESS)
				reboot = YES;
		}
		else
			goto restore;
	}
	else
	{
		if(DrawDialogYesNo("Do you want install Mamba Autoloader?") == YES)
		{
			DrawDialogOKTimer("Please wait..", 1000.0f);

			if(run_install_autoloader() == SUCCESS)
				reboot = YES;
		}
		else
			goto restore;
	}
	frame = 0;
	screen = 2;
	return;
	
restore:
	px = 10;
	py = 0;
	exitY = 262;
	installY = 262;
	vel_fall = 0;
	vel_fall2 = 0;
	dir = RIGHT;
	frame = 0;
			
	int i;
	for(i = 0; i < 27; i++)
		layer1[10][i] = BG;
	
	layer1[5][13] = WB;
	screen = 0;
}

extern char buf[2048];

void draw_log()
{
	Draw_background(0x000000ff);
	SetFontAutoCenter(0);
	SetCurrentFont(1);
	SetFontSize(0, 16);
	SetFontColor(0xffffffdd, 0x0);
	
	DrawFormatString(0, 20, "%s", buf);
	
	SetFontAutoCenter(1);
	SetFontSize(16, 32);
	
	if(frame < 20)
		DrawString(0, 460, "PRESS CIRCLE TO EXIT");
	else if(frame >= 39)
		frame = 0;	
	
	if(old_pad & BUTTON_CIRCLE)
		bye = YES;
}

void animate_character()
{		
	if(stat_mov == IDLE)
	{
		vel_mov = FRAME_IDLE;
		
		if(frame < vel_mov)
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 914;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 0; ptx= 914;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 2)) 
		{
			if(dir == RIGHT)
				{pblk = 1; ptx= 1100;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 1; ptx= 1100;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 3)) 
		{
			if(dir == RIGHT)
				{pblk = 2; ptx= 1286;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 2; ptx= 1286;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 4)) 
		{
			if(dir == RIGHT)
				{pblk = 3; ptx= 1472;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 3; ptx= 1472;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 5)) 
		{
			if(dir == RIGHT)
				{pblk = 4; ptx= 1658;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 4; ptx= 1658;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 6)) 
		{
			if(dir == RIGHT)
				{pblk = 5; ptx= 1844;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 5; ptx= 1844;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 7))
		{
			if(dir == RIGHT)
				{pblk = 6; ptx= 2030;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 6; ptx= 2030;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else if(frame < (vel_mov * 8))
		{
			if(dir == RIGHT)
				{pblk = 7; ptx= 2216;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 7; ptx= 2216;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
		}
		else
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 914;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			else
				{pblk = 0; ptx= 914;  pty= 0;  pw= 47;  ph= 60; rpw= 186; rph= 236;}
			
			frame = 0;
		}
	}
// Cammina
	else if(stat_mov == WALK)
	{	
		vel_mov = FRAME_WALK;
		
		if(frame < vel_mov) 
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
		}
		else if( frame < (vel_mov * 2)) 
		{
			if(dir == RIGHT)
				{pblk = 1; ptx= 186;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 1; ptx= 186;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
		}
		else if( frame < (vel_mov * 3)) 
		{
			if(dir == RIGHT)
				{pblk = 2; ptx= 372;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 2; ptx= 372;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
		}
	    else
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			
			frame = 0;
		}
	}
// Corre
	else if(stat_mov == RUN)
	{	
		vel_mov = FRAME_RUN;
		
		if(frame < vel_mov) 
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
		}
		else if( frame < (vel_mov * 2)) 
		{
			if(dir == RIGHT)
				{pblk = 1; ptx= 186;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 1; ptx= 186;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
		}
		else if( frame < (vel_mov * 3)) 
		{
			if(dir == RIGHT)
				{pblk = 2; ptx= 372;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 2; ptx= 372;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
		}
	    else
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			else
				{pblk = 0; ptx= 0;  pty= 0;  pw= 46;  ph= 60; rpw= 186; rph= 241;}
			
			frame = 0;
		}
	}
// Salta
	else if(stat_mov == JUMP)
	{
		vel_mov = FRAME_JUMP;
		
		if(vel_jump > 0)
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 558;  pty= 0;  pw= 43;  ph= 60; rpw= 178; rph= 248;}
			else
				{pblk = 0; ptx= 558;  pty= 0;  pw= 43;  ph= 60; rpw= 178; rph= 248;}
		}
		else
		{
			if(dir == RIGHT)
				{pblk = 0; ptx= 736;  pty= 0;  pw= 43;  ph= 60; rpw= 178; rph= 248;}
			else
				{pblk = 0; ptx= 736;  pty= 0;  pw= 43;  ph= 60; rpw= 178; rph= 248;}
		}
	}
}

void draw_character()
{	
	if(dir == RIGHT)           
		Draw_PNG_adv(px, py, 0, pw, ph, file_Png[playerR].width, file_Png[playerR].height, ptx, pty, rpw, rph, 0xffffffff, playerR, 1);
	else
		Draw_PNG_adv(px, py, 0, pw, ph, file_Png[playerL].width, file_Png[playerL].height, ptx, pty, rpw, rph, 0xffffffff, playerL, 1);
}

void cmd_character()
{		
	if(old_pad & BUTTON_RIGHT) 
	{
		hello = 0;
		speed = SPD_WALK;
		stat_mov = WALK;
		
		if(dir == LEFT)
		{
			dir = RIGHT;
			frame = 0;
			px = (px - pw / 3);
		}
		else
		{	
			dir = RIGHT;
			if(old_pad & BUTTON_SQUARE)
			{
				stat_mov = RUN;
			
				if(speed < SPD_RUN)
					speed = (speed + acc_run);
				
				if(speed > SPD_RUN)
					speed = SPD_RUN;
			}
			px = (px + speed);
			if(px > 848)
				bye = YES;
		}
	}
	else if(old_pad & BUTTON_LEFT) 
	{
		hello = 0;
		speed = SPD_WALK;
		stat_mov = WALK;
		
		if(dir == RIGHT) 
		{
			dir = LEFT;
			frame = 0;
			px = (px + pw / 3);
		}
		else
		{
			dir = LEFT;

			if(old_pad & BUTTON_SQUARE)
			{
				stat_mov = RUN;
		
				if(speed < SPD_RUN)
					speed = (speed + acc_run);
			
				if(speed > SPD_RUN)
					speed = SPD_RUN;
			}
			//Muove il personaggio
			px = (px - speed);
			if(px <= 0)
				px = 1;
		}
	}
	else if(stat_mov != JUMP && stat_mov != IDLE)
	{
		if(hello > 1000) //180 frames
			stat_mov = FRONT;
		else
			stat_mov = IDLE;
			
		frame = 0;
		hello++;
		
		speed = 0;
	}
	if(vel_jump < JUMP_FORCE)
	{
		vel_jump = (vel_jump - (acc_jump * frm_time));
		py = (py - vel_jump);
		stat_mov = JUMP;	
	}
	if(new_pad & BUTTON_CROSS && vel_jump == JUMP_FORCE)
	{
		hello = 0;
		frame = 0;
		stat_mov = JUMP;
		
		vel_jump = (vel_jump - (acc_jump * frm_time));
		py = (py - vel_jump);
	}

	if(new_pad & BUTTON_R3)
		sysProcessExitSpawn2("/dev_hdd0/game/PSL145310/USRDIR/RELOAD.SELF", NULL, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M);
}

int WorldTileIs(float positionX, float positionY) 
{
	float gridSpotX = positionX/T_WIDTH;
    float gridSpotY = positionY/T_WIDTH;
    
    int id = (layer1[(int)(gridSpotY)][(int)(gridSpotX)]);
    
	if(gridSpotX < 0) 
		return 1; 
		
	if(gridSpotX > COL) 
		return 0; 
    
    if(gridSpotY < 0) 
		return 1; 
    
    if(gridSpotY > LINE) 
		return 0;
   
    return id;
}

/** Corregge la posizione nel caso ci sia una collisione **/
float TopOfTile(float thisPositionY, float height) 
{
	int thisY = (int)(thisPositionY);
    thisY = (thisY / T_HEIGHT);
    
    return ((float)(thisY * T_HEIGHT - height));
}

float BottomOfTile(float thisPositionY) 
{
	if(thisPositionY < 0) 
	{
		return 10;
	}
	
	int thisY = (int)(thisPositionY);
    thisY = (thisY / T_HEIGHT);
    
    return ((float)(thisY * T_HEIGHT + T_HEIGHT));
}
 
float LeftOfTile(float thisPositionX, float width) 
{
	int thisX = (int)thisPositionX;
    thisX = (thisX / T_WIDTH);
    
    return ((float)(thisX * T_WIDTH - width));
}

float RightOfTile(float thisPositionX) 
{
	if(thisPositionX < 0) 
    {
      return 0;
    }
    
    int thisX = (int)thisPositionX;
    thisX = (thisX / T_WIDTH);
    
    return ((float)(thisX * T_WIDTH + T_WIDTH));
}

void collision_character()
{	
	float tmp = 0;
	
	int pos;
	
	float TopLeftX = px + (pw / 3);
	float TopLeftY = py;
	
	float TopRightX = px + ((pw / 3) * 2);
	float TopRightY = py;
	
	float HeadX = px + (pw/2);
	float HeadY = py;
	
	float RightBodyHighX = (px + pw);
	float RightBodyHighY = (py + (ph / 3));
	
	float LeftBodyHighX = px;
	float LeftBodyHighY = (py + (ph / 3));
	
	float RightBodyLowX = (px + pw);
	float RightBodyLowY = (py + ((ph / 3) * 2));
	
	float LeftBodyLowX = px;
	float LeftBodyLowY = (py + ((ph / 3) * 2));
	
	float BottomLeftX = (px + (pw / 3));
	float BottomLeftY = (py + ph);
	
	float BottomRightX = (px + ((pw / 3) * 2));
	float BottomRightY = (py + ph);
	
	pos = WorldTileIs(HeadX, HeadY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			py = BottomOfTile(HeadY);
			vel_jump = 0;
			break;
		case T_WEIRD:
			py = BottomOfTile(HeadY);
			layer1[(int)(HeadY/T_WIDTH)][(int)(HeadX/T_WIDTH)] = BB;
			destroy_world = YES;
			vel_jump = 0;
			break;
		default:
			break;
	}
//collisione alto sinistra
	pos = WorldTileIs(TopLeftX, TopLeftY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			py = BottomOfTile(TopLeftY);
			vel_jump = 0;
			break;
		case T_WEIRD:
			py = BottomOfTile(TopLeftY);
			layer1[(int)(TopLeftY/T_WIDTH)][(int)(TopLeftX/T_WIDTH)] = BB;
			destroy_world = YES;
			vel_jump = 0;
			break;
		default:
			break;
	}	
//collisione alto destra
	pos = WorldTileIs(TopRightX, TopRightY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			py = BottomOfTile(TopRightY);
			vel_jump = 0;
			break;
		case T_WEIRD:
			py = BottomOfTile(TopRightY);
			layer1[(int)(TopRightY/T_WIDTH)][(int)(TopRightX/T_WIDTH)] = BB;
			destroy_world = YES;	
			vel_jump = 0;
			break;
		default:
			break;
	}

//collisone sinistra corpo basso
	pos = WorldTileIs(LeftBodyLowX, LeftBodyLowY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			tmp = RightOfTile(LeftBodyLowX);
			px = tmp;
			break;
		default:
			break;
	}
//collisone sinistra corpo alto
	pos = WorldTileIs(LeftBodyHighX, LeftBodyHighY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			tmp = RightOfTile(LeftBodyHighX);
			px = tmp;
			break;
		default:
			break;
	}
//collisione destra corpo basso
	pos = WorldTileIs(RightBodyLowX, RightBodyLowY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			tmp = LeftOfTile(RightBodyLowX, pw);
			px = tmp;
			break;
		default:
			break;
	}
//collisione destra corpo alto
	pos = WorldTileIs(RightBodyHighX, RightBodyHighY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			tmp = LeftOfTile(RightBodyHighX, pw);
			px = tmp;
			break;
		default:
			break;
	}
//collisione basso sinistra
	pos = WorldTileIs(BottomLeftX, BottomLeftY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			if(stat_mov != JUMP || vel_jump <= 0)
			{
				py = TopOfTile(BottomLeftY, ph);
			
				if(stat_mov == JUMP)
				{
					stat_mov = WALK;
					vel_jump = JUMP_FORCE;
				}
			}
			break;
		case T_WEIRD:
			if(stat_mov != JUMP || vel_jump <= 0)
			{
				py = TopOfTile(BottomLeftY, ph);
			
				if(stat_mov == JUMP)
				{
					stat_mov = WALK;
					vel_jump = JUMP_FORCE;
				}
			//	layer1[(int)(BottomLeftY/T_WIDTH)][(int)(BottomLeftX/T_WIDTH)] = BG;
			}
			break;
		default:
			break;
	}
//collisione basso destra
	pos = WorldTileIs(BottomRightX, BottomRightY);
	switch(list_tile[pos].type)
	{
		case T_SOLID:
			if(stat_mov != JUMP || vel_jump <= 0)
			{
				py = TopOfTile(BottomRightY, ph);
			
				if(stat_mov == JUMP)
				{
					stat_mov = WALK;
					vel_jump = JUMP_FORCE;
				}
			}
			break;
		case T_WEIRD:
			if(stat_mov != JUMP || vel_jump <= 0)
			{
				py = TopOfTile(BottomRightY, ph);
			
				if(stat_mov == JUMP)
				{
					stat_mov = WALK;
					vel_jump = JUMP_FORCE;
				}
			//	layer1[(int)(BottomRightY/T_WIDTH)][(int)(BottomRightX/T_WIDTH)] = BG;
			//	layer1[(int)(BottomRightY/T_WIDTH)][(int)(BottomRightX/T_WIDTH + 1)] = BG;
			//	layer1[(int)(BottomRightY/T_WIDTH)][(int)(BottomRightX/T_WIDTH + 2)] = BG;
			}
			break;
		default:
			break;
	}
	
	if(WorldTileIs(BottomLeftX, BottomLeftY) == T_EMPTY && WorldTileIs(BottomRightX, BottomRightY) == T_EMPTY) 
	{
		if(stat_mov != JUMP)
		{
			vel_jump = 0;
		}
	}
// Install/Uninstall MAMBA
	if(py > 512 )
		screen = 1;
}

int frequency_of_primes (int n) 
{
	int i,j;
	int freq=n-1;
	for (i=2; i<=n; ++i) for (j=sqrt(i);j>1;--j) if (i%j==0) {--freq; break;}
	return freq;
}

s32 main(s32 argc, const char* argv[])
{
	atexit(exiting);
	sysModuleLoad(SYSMODULE_PNGDEC);
	
	tiny3d_Init(1024*1024);
	ioPadInit(7);
	LoadTexture();
	
	double sx = (double) Video_Resolution.width;
    double sy = (double) Video_Resolution.height;
    double psy = (double) ((1000 - 0.01)/1000.0); // Crazy..but work..no black lines between tiles and full (nearly) screen
    
    tiny3d_UserViewport(1, 
        (float) ((sx - sx) / 2.0), // 2D position
        (float) ((sy - sy * psy) / 2.0), 
        (float) (sx / 848.0),    // 2D scale
        (float) ((sy * psy) / 512.0),
        (float) (sx / 1920.0),  // 3D scale
        (float) ((sy / 1080.0) * psy));
	
	px = 10;
	py = 0;
	dir = RIGHT;
	
	SetFontAutoCenter(0);
	SetCurrentFont(1);
	SetFontSize(10, 20);
	SetFontColor(0x000000ff, 0); 
	
	clock_t t;
	actParam.small_motor = 0;
	actParam.large_motor = 0;
	
	mamba = is_mamba_installed();

	while(bye == NO) 
	{	
		t = clock();
		frequency_of_primes(1000);
		t = clock() - t; 

		clear_screen();
		ps3pad_read();
		
		if((py > 352 && layer1[10][13] == T_EMPTY) || py > 512)
		{
			actParam.small_motor = 0;
			actParam.large_motor = 0;
		}
		ioPadSetActDirect(0, &actParam);
		
		switch(screen)
		{
			case 0:
				if(s == 19)
				{
					s = 0;
			
					if(block == 13)
					{
						s -= 70;
						block = 14;
						actParam.small_motor = 0;
						actParam.large_motor = 0;
					}
					else if(block == 14)
					{
						layer1[10][13] = T_EMPTY;
						s = 0;
						block = 0;
						destroy_world = NO;
						actParam.small_motor = 1;
						actParam.large_motor = 140;
					}
					else
					{
						actParam.small_motor = 1;
						actParam.large_motor = 140;
						layer1[10][0 + block] = T_EMPTY;
						layer1[10][26 - block] = T_EMPTY;
						block++;
					}
				}
				cmd_character();
				animate_character();
				collision_character();
				
				Draw_background(0xffffffff);
				
				if(layer1[10][23] == T_EMPTY)
				{
					vel_fall = (vel_fall - (acc_jump * frm_time));
					exitY = (exitY - vel_fall);
				}
				Draw_PNG(722, exitY, 0, 58, 58, 0xffffffff, exit, 1);
		
				if(layer1[10][5] == T_EMPTY)
				{
					vel_fall2 = (vel_fall2 - (acc_jump * frm_time));
					installY = (installY - vel_fall2);
				}
				if(mamba != SUCCESS)
					Draw_PNG(140, installY, 0, 56, 62, 0xffffffff, install, 1);
				else
					Draw_PNG(140, installY, 0, 56, 62, 0xffffffff, uninstall, 1);
				
				draw_layer1();
				draw_character();

				if(destroy_world == YES)
					s++;
				break;
			case 1:
				mamba_install_uninstall();
				break;
			case 2:
				draw_log();
			break;
			default:
				break;
		}
		frame++;
		tiny3d_Flip();	
	}
	return 0;
}

