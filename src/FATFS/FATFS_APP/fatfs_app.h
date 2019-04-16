#ifndef _fatfs_app_H
#define _fatfs_app_H


#include "config.h"
#include "ff.h" 


typedef struct
{
	uint8_t type[6];		   //后缀6个字节
	uint8_t name[100];		   //路径和文件名字100个字节（支持25个汉字大小名字）
} FileNameTypeDef;


extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern u8 *fatbuf;//SD卡数据缓存区


//f_typetell返回的类型定义
//根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
#define T_BIN		0X00	//bin文件
#define T_LRC		0X10	//lrc文件
#define T_NES		0X20	//nes文件

#define T_TEXT		0X30	//.txt文件
#define T_C			0X31	//.c文件
#define T_H			0X32    //.h文件

#define T_WAV		0X40	//WAV文件
#define T_MP3		0X41	//MP3文件 
#define T_APE		0X42	//APE文件
#define T_FLAC		0X43	//FLAC文件

#define T_BMP		0X50	//bmp文件
#define T_JPG		0X51	//jpg文件
#define T_JPEG		0X52	//jpeg文件		 
#define T_GIF		0X53	//gif文件   

#define T_AVI		0X60	//avi文件  

 
u8 FATFS_Init(void);							//申请内存
u8 FATFS_ScanFilesType(u8 *fname);						//识别文件类型
u8 FATFS_ScanFiles(uint8_t *dir, FileNameTypeDef *file);
u8 FATFS_GetFree(u8 *drv,u32 *total,u32 *free);	//得到磁盘总容量和剩余容量



#endif
