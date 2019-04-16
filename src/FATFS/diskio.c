/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */


/* Definitions of physical drive number for each drive */
#define SD_CARD		0	
//#define EX_FLASH	1	
//#define USB		    2	


//#define FLASH_SECTOR_SIZE 	512			  
//对于EN25Q64
//前6M字节给fatfs用,6M字节后,用于存放字库,字库占用1.5M. 剩余部分给自己用	 			    
//u16	    FLASH_SECTOR_COUNT=2048*6;	//EN25Q64,前6M字节给FATFS占用
//#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	switch (pdrv) {
	case SD_CARD :
		

		// translate the reslut code here

		return stat;

//	case EX_FLASH :
//		

//		// translate the reslut code here

//		return stat;

//	case USB :
//		

		// translate the reslut code here

//		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	int result;

	switch (pdrv) {
	case SD_CARD :
		result=SD_Init();//SD卡初始化 
  		
		// translate the reslut code here

		break;

//	case EX_FLASH :
//		EN25QXX_Init();
//		FLASH_SECTOR_COUNT=2048*6;//EN25Q64,前6M字节分配给FATFS
//	
//		// translate the reslut code here

//		break;

//	case USB :
//		

//		// translate the reslut code here

//		break;
//	
	default:
			result=1; 
	}
	if(result)
		return  STA_NOINIT;
	else 
		return 0; //初始化成功
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	int res;
	
	if (!count)
		return RES_PARERR;//count不能等于0，否则返回参数错误
	
	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here
		res=SD_ReadDisk(buff,sector,count);	 
		while(res)//读出错
		{
			SD_Init();	//重新初始化SD卡
			res=SD_ReadDisk(buff,sector,count);	
			//printf("sd rd error:%d\r\n",res);
		}
		
		// translate the reslut code here

		break;

//	case EX_FLASH :
//		// translate the arguments here
//		for(;count>0;count--)
//		{
//			EN25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
//			sector++;
//			buff+=FLASH_SECTOR_SIZE;
//		}
//		res=0;
//		
//		// translate the reslut code here

//		break;

//	case USB :
//		// translate the arguments here


//		// translate the reslut code here

//		break;
	
	default:
			res=1; 
	}

	if(res==0x00)
		return RES_OK;	 
    else 
		return RES_ERROR;	
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	int res;
	
	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误	
	
	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here
		res=SD_WriteDisk((u8*)buff,sector,count);
		while(res)//写出错
		{
			SD_Init();	//重新初始化SD卡
			res=SD_WriteDisk((u8*)buff,sector,count);	
			//printf("sd wr error:%d\r\n",res);
		}

		// translate the reslut code here

		break;

//	case EX_FLASH :
//		// translate the arguments here
//		for(;count>0;count--)
//		{										    
//			EN25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
//			sector++;
//			buff+=FLASH_SECTOR_SIZE;
//		}
//		res=0;
//		
//		// translate the reslut code here

//		break;

//	case USB :
//		// translate the arguments here

//		
//		// translate the reslut code here

//		break;
	}

	if(res == 0x00)
		return RES_OK;	 
    else 
		return RES_ERROR;	
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch (pdrv) {
	case SD_CARD :

		// Process of the command for the ATA drive
		switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }

//	case EX_FLASH :

//		// Process of the command for the MMC/SD card
//		switch(cmd)
//	    {
//		    case CTRL_SYNC:
//				res = RES_OK; 
//		        break;	 
//		    case GET_SECTOR_SIZE:
//		        *(WORD*)buff = FLASH_SECTOR_SIZE;
//		        res = RES_OK;
//		        break;	 
//		    case GET_BLOCK_SIZE:
//		        *(WORD*)buff = FLASH_BLOCK_SIZE;
//		        res = RES_OK;
//		        break;	 
//		    case GET_SECTOR_COUNT:
//		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
//		        res = RES_OK;
//		        break;
//		    default:
//		        res = RES_PARERR;
//		        break;
//	    }

//	case USB :

//		// Process of the command the USB drive

//		break;
	}

	return res;
}
#endif

//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}


