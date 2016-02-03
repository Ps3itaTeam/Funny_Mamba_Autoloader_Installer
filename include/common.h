#ifndef _COMMON_H_
#define _COMMON_H_

#define SUCCESS 	0
#define FAILED	 	-1


//----------------------------------------
//COBRA/MAMBA
//----------------------------------------

int is_cobra(void);
int is_mamba(void);

//----------------------------------------
//FILE UTILS
//----------------------------------------

int file_exists(const char *path);
int dir_exists(const char *path);
int unlink_secure(void *path);

#endif
