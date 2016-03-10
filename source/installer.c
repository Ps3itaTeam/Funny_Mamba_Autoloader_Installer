/*
	==============================================================

	Unofficial MAMBA/PRX Autoloader by Ps3ita Team
	Original author NzV

	Load of MAMBA and/or VSH plugins (with MAMBA or PRX Loader) at system boot.

	==============================================================
*/
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <sys/file.h>

#include "common.h"
#include "lv2_utils.h"

#define SC_FS_MOUNT  		(837)

#define VERSION_NAME 	"Unofficial MAMBA/PRX Autoloader v3.1.0 (forked from NzV)\r\n"

//----------------------------------------
//MAMBA/PRX AUTOLOADER
//----------------------------------------

#define PATH_SYS_INI_OSD 		"/dev_blind/sys/internal/sys_init_osd.self"
#define PATH_SYS_INI_OSD_ORIG 	"/dev_blind/sys/internal/sys_init_osd_orig.self"
#define PATH_SYS_INI_OSD_NEW 	"/dev_hdd0/game/MAMBAINST/USRDIR/NewCore/sys_init_osd.self"
#define PATH_DIR_PAYLOAD 		"/dev_hdd0/game/MAMBAINST/USRDIR/payloads/"
#define PATH_DIR_PS2EMU_NEW		"/dev_hdd0/game/MAMBAINST/USRDIR/ps2emu/"
#define	VSH_PLUGINS_PATH_MAMBA	"/dev_hdd0/mamba_plugins.txt"
#define	VSH_PLUGINS_PATH_PRX	"/dev_hdd0/prx_plugins.txt"

int install_autoloader = 0;
int uninstall_autoloader = 0;
char buf[2048];
int n = 0;

int CopyFile(char* path, char* path2)
{
	int ret = 0;
	s32 fd = -1;
	s32 fd2 = -1;
	u64 lenght = 0LL;

	u64 pos = 0ULL;
	u64 readed = 0, writed = 0;

	char *mem = NULL;

	sysFSStat stat;

	ret= sysLv2FsStat(path, &stat);
	if(ret) goto skip;

	if(!memcmp(path, "/dev_hdd0/", 10) && !memcmp(path2, "/dev_hdd0/", 10))
	{
		if(strcmp(path, path2)==0) return ret;

		sysLv2FsUnlink(path2);
		return sysLv2FsLink(path, path2);
	}

    lenght = stat.st_size;

	ret = sysLv2FsOpen(path, 0, &fd, S_IRWXU | S_IRWXG | S_IRWXO, NULL, 0);
	if(ret) goto skip;

	ret = sysLv2FsOpen(path2, SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &fd2, 0777, NULL, 0);
	if(ret) {sysLv2FsClose(fd);goto skip;}

	mem = malloc(0x100000);
	if (mem == NULL) return FAILED;

	while(pos < lenght)
	{
		readed = lenght - pos; if(readed > 0x100000ULL) readed = 0x100000ULL;
		ret=sysLv2FsRead(fd, mem, readed, &writed);
		if(ret<0) goto skip;
		if(readed != writed) {ret = 0x8001000C; goto skip;}

		ret=sysLv2FsWrite(fd2, mem, readed, &writed);
		if(ret<0) goto skip;
		if(readed != writed) {ret = 0x8001000C; goto skip;}

		pos += readed;
	}

skip:

	if(mem) free(mem);
	if(fd >=0) sysLv2FsClose(fd);
	if(fd2>=0) sysLv2FsClose(fd2);
	if(ret>0) ret = SUCCESS;

	return ret;
}

int ps2emu_folder = 0;

int get_firmware()
{
	uint64_t toc = lv2peek(0x8000000000003000ULL);
	switch(toc)
	{
		case 0x8000000000330540ULL:
		//	ps2emu_folder = 0x355;
			return 0x355C;
		break;
		case 0x800000000034AC80ULL:
		//	ps2emu_folder = 0x355;
			return 0x355D;
		break;
		case 0x8000000000346390ULL:
		//	ps2emu_folder = 0x421;
			return 0x421C;
		break;
		case 0x8000000000363E80ULL:
		//	ps2emu_folder = 0x421;
			return 0x421D;
		break;
		case 0x8000000000348200ULL:
		//	ps2emu_folder = 0x430;
			return 0x430C;
		break;
		case 0x8000000000365CA0ULL:
		//	ps2emu_folder = 0x430;
			return 0x430D;
		break;
		case 0x8000000000348210ULL:
		//	ps2emu_folder = 0x431;
			return 0x431C;
		break;
		case 0x80000000003487D0ULL:
		//	ps2emu_folder = 0x440;
			return 0x440C;
		break;
		case 0x80000000003487E0ULL:
		//	ps2emu_folder = 0x441;
			return 0x441C;
		break;
		case 0x80000000003665C0ULL:
		//	ps2emu_folder = 0x441;
			return 0x441D;
		break;
		case 0x8000000000366BD0ULL:
			ps2emu_folder = 0x446;
			return 0x446D;
		break;
		case 0x8000000000348DF0ULL:
			ps2emu_folder = 0x446;
			return 0x446C;
		break;
		case 0x800000000034B160ULL:
			ps2emu_folder = 0x450;
			return 0x450C;
		break;
		case 0x800000000036EC40ULL:
			ps2emu_folder = 0x450;
			return 0x450D;
		break;
		case 0x800000000034B2E0ULL:
		//	ps2emu_folder = 0x453;
			return 0x453C;
		break;
		case 0x8000000000370620ULL:
		//	ps2emu_folder = 0x453;
			return 0x453D;
		break;
		case 0x800000000034E620ULL:
		//	ps2emu_folder = 0x455;
			return 0x455C;
		break;
		case 0x80000000003738E0ULL:
		//	ps2emu_folder = 0x455;
			return 0x455D;
		break;
		case 0x800000000034F950ULL:
		//	ps2emu_folder = 0x460;
			return 0x460C;
		break;
		case 0x8000000000375500ULL:
		//	ps2emu_folder = 0x460;
			return 0x460D;
		break;
		case 0x800000000034F960ULL:
			if(lv2peek(0x80000000002FC938ULL)==0x323031342F31312FULL) {ps2emu_folder = 0x466; return 0x466C;}
			else {ps2emu_folder = 0x465; return 0x465C;}
		break;
		case 0x8000000000375510ULL:
			if(lv2peek(0x800000000031EBA8ULL)==0x323031342F31312FULL) {ps2emu_folder = 0x466; return 0x466D;}
			else {ps2emu_folder = 0x465; return 0x465D;}
		break;
		case 0x800000000034FB10ULL:
			ps2emu_folder = 0x470;
			return 0x470C;
		break;
		case 0x8000000000375850ULL:
			ps2emu_folder = 0x470;
			return 0x470D;
		break;
		case 0x800000000034FBB0ULL:
			if(lv2peek(0x80000000002FCB68ULL)==0x323031352F30382FULL) {ps2emu_folder = 0x476; return 0x476C;}
			else if(lv2peek(0x80000000002FCB68ULL)==0x323031352F31322FULL) {ps2emu_folder = 0x478; return 0x478C;}
			else {ps2emu_folder = 0x475; return 0x475C;}
		break;
		case 0x80000000003758E0ULL:
			if(lv2peek(0x800000000031EF48ULL)==0x323031352F30382FULL) {ps2emu_folder = 0x476; return 0x476D;}
			else if(lv2peek(0x800000000031EF48ULL)== 0x323031352F31322FULL) {ps2emu_folder = 0x478; return 0x478D;}
			else {ps2emu_folder = 0x475; return 0x475D;}
		break;
	}
	return 0;
}

int is_mamba_installed()
{
	if(is_mamba() == SUCCESS && file_exists("/dev_flash/sys/internal/sys_init_osd_orig.self") == SUCCESS)
		return SUCCESS;
	
	return FAILED;
}

int run_uninstall_autoloader()
{
	char filename[128];
	int fw_list[33] = { 0x355C,0x421C,0x430C,0x431C,0x440C,0x441C,0x446C,0x450C,0x453C,0x455C,0x460C,0x465C,0x466C,0x470C,0x475C,0x476C,0x478C,
						0x355D,0x421D,0x430D,               0x441D,0x446D,0x450D,0x453D,0x455D,0x460D,0x465D,0x466D,0x470D,0x475D,0x476D,0x478D };
	int i;	
	
	n += sprintf(&buf[n], "[UNINSTALLER]\r\n");
	
	//Check if installed
	if(file_exists("/dev_flash/sys/internal/sys_init_osd_orig.self") != SUCCESS)
	{
		n += sprintf(&buf[n], "sys_init_osd_orig.self not found\r\n");
		goto residual;
	}
	//Remove flags
	if(file_exists("/dev_hdd0/tmp/core_flags/nousb") == SUCCESS)
	{
		unlink_secure("/dev_hdd0/tmp/core_flags/nousb");
		n += sprintf(&buf[n], "Flag nousb removed\r\n");
	}
	else n += sprintf(&buf[n],"Flags nousb always removed\r\n");
	
	if(dir_exists("/dev_hdd0/tmp/core_flags") == SUCCESS)
	{
		sysLv2FsRmdir("/dev_hdd0/tmp/core_flags");
		n += sprintf(&buf[n],"Directory core_flags removed\r\n");
	}
	else n += sprintf(&buf[n],"Directory core_flags always removed\r\n");
	
	//Enable dev_blind
	if(dir_exists("/dev_blind") != SUCCESS)
		{lv2syscall8(SC_FS_MOUNT, (u64)(char*)"CELL_FS_IOS:BUILTIN_FLSH1", (u64)(char*)"CELL_FS_FAT", (u64)(char*)"/dev_blind", 0, 0, 0, 0, 0); }
		
	//Restore original ps2emu
	int firmware = get_firmware();
	char filename_ps2[256];
		
	sprintf (filename_ps2, "/dev_hdd0/tmp/ps2_emu_%X.self", firmware);
	if(file_exists(filename_ps2) == SUCCESS)
	{
		unlink_secure("/dev_blind/ps2emu/ps2_emu.self");
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2_emu.self");
		n += sprintf(&buf[n],"Original ps2_emu.self restored\r\n");
	}
	else n += sprintf(&buf[n],"Original ps2_emu.self not found!\r\n");
	sprintf (filename_ps2, "/dev_hdd0/tmp/ps2_gxemu_%X.self", firmware);
	if(file_exists(filename_ps2) == SUCCESS)
	{
		unlink_secure("/dev_blind/ps2emu/ps2_gxemu.self");
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2_gxemu.self");
		n += sprintf(&buf[n],"Original ps2_gxemu.self restored\r\n");
	}
	else n += sprintf(&buf[n],"Original ps2_gxemu.self not found!\r\n");
	sprintf (filename_ps2, "/dev_hdd0/tmp/ps2_netemu_%X.self", firmware);
	if(file_exists(filename_ps2) == SUCCESS)
	{
		unlink_secure("/dev_blind/ps2emu/ps2_netemu.self");
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2_netemu.self");
		n += sprintf(&buf[n],"Original ps2_netemu.self restored\r\n");
	}
	else n += sprintf(&buf[n],"Original ps2_netemu.self not found!\r\n");
	   if(file_exists("/dev_blind/ps2emu/ps2gxemu_stage2.bin") == SUCCESS) 
		unlink_secure("/dev_blind/ps2emu/ps2gxemu_stage2.bin");
	if(file_exists("/dev_blind/ps2emu/ps2hwemu_stage2.bin") == SUCCESS)
		unlink_secure("/dev_blind/ps2emu/ps2hwemu_stage2.bin");
	//Restore original sys_init_osd.self
	if(file_exists(PATH_SYS_INI_OSD_ORIG) == SUCCESS)
	{
		if(file_exists(PATH_SYS_INI_OSD) == SUCCESS)
		{
			unlink_secure(PATH_SYS_INI_OSD);
			sysLv2FsRename(PATH_SYS_INI_OSD_ORIG, PATH_SYS_INI_OSD);
		}
		else
		{
			n += sprintf(&buf[n],"Error: sys_init_osd.self not found\r\n");
			return FAILED;
		}
	}
	else
	{
		n += sprintf(&buf[n],"Error: /dev_blind not mounted\r\n");
		return FAILED;
	}
	//Remove payload and residual ps2emu
residual:

	for (i = 0; i < 33; i++)
	{
		if (fw_list[i] == 0) break;
		sprintf (filename, "/dev_blind/sys/internal/mpl_payload_%X.bin", fw_list[i]);
		if (file_exists(filename) == SUCCESS) unlink_secure(filename);
		sprintf (filename, "/dev_blind/sys/internal/mamba_%X.bin", fw_list[i]);
		if (file_exists(filename) == SUCCESS) unlink_secure(filename);
		//ps2emu
		sprintf (filename, "/dev_hdd0/tmp/ps2_emu_%X.self", fw_list[i]);
		if (file_exists(filename) == SUCCESS) unlink_secure(filename);
		sprintf (filename, "/dev_hdd0/tmp/ps2_gxemu_%X.self", fw_list[i]);
		if (file_exists(filename) == SUCCESS) unlink_secure(filename);
		sprintf (filename, "/dev_hdd0/tmp/ps2_netemu_%X.self", fw_list[i]);
		if (file_exists(filename) == SUCCESS) unlink_secure(filename);
	}
	n += sprintf(&buf[n],"Success: MAMBA/PRX Autoloader uninstalled\r\n");
	return SUCCESS;
}

int run_install_autoloader()
{
	//Init FW
	int firmware = get_firmware();
	int firmware2 = firmware;
	int is_rebug = -1, fd = -1;
		
	//Is Rebug Rex
	if(dir_exists("/dev_flash/rebug") == SUCCESS && file_exists("/dev_flash/sys/external/libda-vsh.sprx") == SUCCESS)
	{
		if(firmware % 2)
			firmware2 -= 1; // C
		else firmware2 += 1; //D
		is_rebug = 0;
	}
	
	n += sprintf(&buf[n],"%s", VERSION_NAME);	
	n += sprintf(&buf[n],"[INSTALLER]\r\n");
	n += sprintf(&buf[n], "%s\r\n", PATH_SYS_INI_OSD_NEW);
	
	if(file_exists(PATH_SYS_INI_OSD_NEW) != SUCCESS)
	{
		n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
		return FAILED;
	}
	//mamba loader	
	char filename_mpl_payload[256];
	char filename_mpl_payload2[256];
	sprintf (filename_mpl_payload, "%smpl_payload_%X.bin",PATH_DIR_PAYLOAD , firmware);
	
	n += sprintf(&buf[n],"%s\r\n", filename_mpl_payload);
	
	if(file_exists(filename_mpl_payload) != SUCCESS)
	{
		n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
		return FAILED;
	}
	if(is_rebug == 0) 
	{
		sprintf (filename_mpl_payload2, "%smpl_payload_%X.bin",PATH_DIR_PAYLOAD , firmware2);
		n += sprintf(&buf[n],"%s\r\n", filename_mpl_payload2);
		
		if(file_exists(filename_mpl_payload2) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
	}
	//mamba payload
	char filename_mamba_payload[256];
	char filename_mamba_payload2[256];
	sprintf (filename_mamba_payload, "%smamba_%X.bin",PATH_DIR_PAYLOAD , firmware);
	
	n += sprintf(&buf[n],"%s\r\n", filename_mamba_payload);
	
	if(file_exists(filename_mamba_payload) != SUCCESS)
	{
		n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
		return FAILED;
	}
	if(is_rebug == 0) 
	{
		sprintf (filename_mamba_payload2, "%smamba_%X.bin",PATH_DIR_PAYLOAD , firmware2);
		n += sprintf(&buf[n],"%s\r\n", filename_mamba_payload2);
		if(file_exists(filename_mamba_payload2) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
	}
	//ps2emu
	if(ps2emu_folder)
	{
		char filename_ps2emu[256];
		sprintf (filename_ps2emu, "%s%X/ps2_emu.self",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		n += sprintf(&buf[n], "%s\r\n", filename_ps2emu);

		if(file_exists(filename_ps2emu) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
		sprintf (filename_ps2emu, "%s%X/ps2_gxemu.self",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		n += sprintf(&buf[n], "%s\r\n", filename_ps2emu);
		
		if(file_exists(filename_ps2emu) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
		sprintf (filename_ps2emu, "%s%X/ps2gxemu_stage2.bin",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		n += sprintf(&buf[n], "%s\r\n", filename_ps2emu);
		
		if(file_exists(filename_ps2emu) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
		sprintf (filename_ps2emu, "%s%X/ps2hwemu_stage2.bin",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		n += sprintf(&buf[n], "%s\r\n", filename_ps2emu);
		
		if(file_exists(filename_ps2emu) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
		sprintf (filename_ps2emu, "%s%X/ps2_netemu.self",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		n += sprintf(&buf[n], "%s\r\n", filename_ps2emu);
		
		if(file_exists(filename_ps2emu) != SUCCESS)
		{
			n += sprintf(&buf[n],"Error: Unable to find file\r\nYOU LOSE!");
			return FAILED;
		}
	}
	//Uninstall New_Core
	run_uninstall_autoloader();
	
	n += sprintf(&buf[n],"[INSTALLER]\r\n");
	
	//Create directory core_flags and file nousb
	if(dir_exists("/dev_hdd0/tmp/core_flags") != SUCCESS)
	{
		sysLv2FsMkdir("/dev_hdd0/tmp/core_flags", 0777);
		n += sprintf(&buf[n],"Create directory core_flags done\r\n");
	}
	else n += sprintf(&buf[n],"Directory core_flags always there\r\n");
	
	if(file_exists("/dev_hdd0/tmp/core_flags/nousb") != SUCCESS)
	{
		sysLv2FsOpen("/dev_hdd0/tmp/core_flags/nousb", SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &fd, 0777, NULL, 0);
		sysLv2FsClose(fd);
		n += sprintf(&buf[n],"Create flags nousb done\r\n");
	}
	else n += sprintf(&buf[n],"Flags nousb always there\r\n");
	
	//Enable dev_blind
	if(file_exists(PATH_SYS_INI_OSD) != SUCCESS)
		{{lv2syscall8(SC_FS_MOUNT, (u64)(char*)"CELL_FS_IOS:BUILTIN_FLSH1", (u64)(char*)"CELL_FS_FAT", (u64)(char*)"/dev_blind", 0, 0, 0, 0, 0); }}
	//Install New_Core
	if(file_exists(PATH_SYS_INI_OSD_ORIG) != SUCCESS)
	{
		if((file_exists(PATH_SYS_INI_OSD) == SUCCESS))
		{
			sysLv2FsChmod(PATH_SYS_INI_OSD, 0777);
			sysLv2FsRename(PATH_SYS_INI_OSD, PATH_SYS_INI_OSD_ORIG);
			CopyFile(PATH_SYS_INI_OSD_NEW, PATH_SYS_INI_OSD);
			char filename[128];
			char filename2[128];
			//rebug
			if(is_rebug == 0) 
			{
				sprintf (filename2, "/dev_blind/sys/internal/mpl_payload_%X.bin", firmware2);
				if (file_exists(filename2) == SUCCESS) unlink_secure(filename2);
					CopyFile(filename_mpl_payload2, filename2);
				sprintf (filename2, "/dev_blind/sys/internal/mamba_%X.bin", firmware2);
				if (file_exists(filename2) == SUCCESS) unlink_secure(filename2);
					CopyFile(filename_mamba_payload2, filename2);
			}
			sprintf (filename, "/dev_blind/sys/internal/mpl_payload_%X.bin", firmware);
			if (file_exists(filename) == SUCCESS) unlink_secure(filename);
				CopyFile(filename_mpl_payload, filename);
			sprintf (filename, "/dev_blind/sys/internal/mamba_%X.bin", firmware);
			if (file_exists(filename) == SUCCESS) unlink_secure(filename);
				CopyFile(filename_mamba_payload, filename);
			if ((file_exists("/dev_hdd0/game/MAMBAINST/USRDIR/mamba_plugins.txt") == SUCCESS) && (file_exists(VSH_PLUGINS_PATH_MAMBA) != SUCCESS))
				CopyFile("/dev_hdd0/game/MAMBAINST/USRDIR/mamba_plugins.txt", VSH_PLUGINS_PATH_MAMBA);
			if ((file_exists("/dev_hdd0/game/MAMBAINST/USRDIR/prx_plugins.txt") == SUCCESS)  && (file_exists(VSH_PLUGINS_PATH_PRX) != SUCCESS))
				CopyFile("/dev_hdd0/game/MAMBAINST/USRDIR/prx_plugins.txt", VSH_PLUGINS_PATH_PRX);
		}
		else
		{
			n += sprintf(&buf[n], "Error: sys_init_osd.self not found (/dev_blind not mounted?)\r\nYOU LOSE!");
			return FAILED;
		}
	}
	else
	{
		n += sprintf(&buf[n], "Error: sys_init_osd_orig.self already exist\r\nYOU LOSE!");
		return FAILED;
	}
	//Install ps2emu
	if(ps2emu_folder)
	{
		char filename_ps2[256];
			
		//backup
		sprintf (filename_ps2, "/dev_hdd0/tmp/ps2_emu_%X.self", firmware);
		CopyFile("/dev_blind/ps2emu/ps2_emu.self", filename_ps2);
		sprintf (filename_ps2, "/dev_hdd0/tmp/ps2_gxemu_%X.self", firmware);
		CopyFile("/dev_blind/ps2emu/ps2_gxemu.self", filename_ps2);
		sprintf (filename_ps2, "/dev_hdd0/tmp/ps2_netemu_%X.self", firmware);
		CopyFile("/dev_blind/ps2emu/ps2_netemu.self", filename_ps2);
		//remove
		unlink_secure("/dev_blind/ps2emu/ps2_emu.self");
		unlink_secure("/dev_blind/ps2emu/ps2_gxemu.self");
		unlink_secure("/dev_blind/ps2emu/ps2_netemu.self");
		//copy new
		sprintf (filename_ps2, "%s%X/ps2_emu.self",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2_emu.self");
		sprintf (filename_ps2, "%s%X/ps2_gxemu.self",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2_gxemu.self");
		sprintf (filename_ps2, "%s%X/ps2gxemu_stage2.bin",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2gxemu_stage2.bin");
		sprintf (filename_ps2, "%s%X/ps2hwemu_stage2.bin",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2hwemu_stage2.bin");
		sprintf (filename_ps2, "%s%X/ps2_netemu.self",PATH_DIR_PS2EMU_NEW, ps2emu_folder);
		CopyFile(filename_ps2, "/dev_blind/ps2emu/ps2_netemu.self");
	}
	else
		n += sprintf(&buf[n],"There is no PS2 ISO support for your CFW\r\n");
	
	n += sprintf(&buf[n],"Success: MAMBA/PRX Autoloader installed\r\nYOU WIN!");
	return SUCCESS;
}

//----------------------------------------
//MAIN
//----------------------------------------

/*
#define	LOG_PATH	 	"/dev_hdd0/tmp/MAMBA_PRX_Loader.log"

int mamba_off = 0;
int noplugins = 0;

int main()
{
	
	verbose = 1;
	Open_Log(LOG_PATH);
	
	//Detect startup mode
    unsigned button = 0;
    padInfo padinfo;
    padData paddata;
    ioPadInit(7);
    int n, r;
    for(r=0; r<10; r++)
    {
        ioPadGetInfo(&padinfo);
        for(n = 0; n < 7; n++)
        {
            if(padinfo.status[n])
            {
                ioPadGetData(n, &paddata);
                button = (paddata.button[2] << 8) | (paddata.button[3] & 0xff);
                break;
            }
        }
        if(button) break;
		else usleep(20000);
    }
    ioPadEnd();
	switch(button)
	{
		case BUTTON_CROSS:
			install_autoloader=1;
		break;

		case BUTTON_SQUARE:
			uninstall_autoloader=1;
		break;

		case BUTTON_R1:
			noplugins=1;
		break;

		case BUTTON_L1:
			mamba_off=1;
		break;
	}
	//Install MAMBA/PRX Autoloader if cross was hold
	if(install_autoloader)
	{
		if (run_install_autoloader() ==  SUCCESS)
		{
			
			CloseLog();
			
			{ BEEP1 } //1 Beep
			 // Reboot
			return SUCCESS;
		}
		else
		{
			goto err_back_to_xmb;
		}
	}
	//Uninstall MAMBA/PRX Autoloader if square was hold
	else if(uninstall_autoloader)
	{
		if (run_uninstall_autoloader() ==  SUCCESS)
		{
			
			CloseLog();
			
			{ BEEP1 } //1 Beep
			sysLv2FsUnlink("/dev_hdd0/tmp/turnoff");
			{lv2syscall3(SC_SYS_POWER, SYS_HARD_REBOOT, 0, 0);} // Reboot
			return SUCCESS;
		}
		else
		{
			goto err_back_to_xmb;
		}
	}
	//Run MAMBA/PRX Loader
	else if ( mamba_prx_loader(mamba_off, noplugins) ==  SUCCESS)
	{	
		
		CloseLog();
		
		{ BEEP1 } //1 Beep
		return SUCCESS; //Get back to xmb
	}
	//Error get back to xmb
err_back_to_xmb:
    
	CloseLog();
	
	{ BEEP3 } //3 Beep
	return FAILED; //Get back to xmb
}
*/
