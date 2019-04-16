#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "parsejson.h"
#include "malloc.h"
#include "usart3.h"
#include "lcd.h"
#include "string.h"
#include "text.h"
#include "ff.h"
#include "exfuns.h"
#include "delay.h"
#include "picture.h"
#include "rtc.h"

extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

//数字字符串转换成整形
static u8 str2int(u8 *str)
{
	u8 len,res;
	len = strlen((const char *)str);
	switch(len)
	{
		case 1:
			res = str[0]-0x30;
			break;
		case 2:
			res = (str[0]-0x30)*10+(str[1]-0x30);
			break;
		default:
			break;
	}
	return res;
}

//显示天气图标
static void show_weather_icon(u8 *str,u8 num)
{
	u8 code;
	code = str2int(str);
	switch(code)
	{
		case 0:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_sunny);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_sunny);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_sunny);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_sunny);
					break;
			}			
			break;
		case 1:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_fair);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_fair);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_fair);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_fair);
					break;
			}	
			break;
		case 2:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_sunny);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_sunny);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_sunny);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_sunny);
					break;
			}	
			break;
		case 3:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_fair);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_fair);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_fair);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_fair);
					break;
			}	
			break;
		case 4:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_cloudy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_cloudy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_cloudy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_cloudy);
					break;
			}	
			break;
		case 5:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_partly_cloudy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_partly_cloudy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_partly_cloudy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_partly_cloudy);
					break;
			}	
			break;
		case 6:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_moon_cloudy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_moon_cloudy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_moon_cloudy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_moon_cloudy);
					break;
			}
			break;
		case 7:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_mostly_cloudy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_mostly_cloudy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_mostly_cloudy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_mostly_cloudy);
					break;
			}
			break;
		case 8:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_moon_cloudy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_moon_cloudy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_moon_cloudy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_moon_cloudy);
					break;
			}
			break;
		case 9:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_overcast);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_overcast);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_overcast);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_overcast);
					break;
			}
			break;
		case 10:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_shower_rain);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_shower_rain);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_shower_rain);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_shower_rain);
					break;
			}
			break;
		case 11:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_thunder_shower);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_thunder_shower);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_thunder_shower);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_thunder_shower);
					break;
			}
			break;
		case 12:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_hail);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_hail);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_hail);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_hail);
					break;
			}
			break;
		case 13:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_light_rain);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_light_rain);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_light_rain);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_light_rain);
					break;
			}
			break;
		case 14:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_moderate_rain);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_moderate_rain);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_moderate_rain);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_moderate_rain);
					break;
			}
			break;
		case 15:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_heavy_rain);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_heavy_rain);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_heavy_rain);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_heavy_rain);
					break;
			}
			break;
		case 16:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_storm);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_storm);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_storm);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_storm);
					break;
			}
			break;
		case 17:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_heavy_storm);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_heavy_storm);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_heavy_storm);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_heavy_storm);
					break;
			}
			break;
		case 18:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_severe_storm);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_severe_storm);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_severe_storm);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_severe_storm);
					break;
			}
			break;
		case 19:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_freezing_rain);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_freezing_rain);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_freezing_rain);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_freezing_rain);
					break;
			}
			break;
		case 20:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_sleet);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_sleet);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_sleet);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_sleet);
					break;
			}
			break;
		case 21:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_snow_flurry);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_snow_flurry);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_snow_flurry);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_snow_flurry);
					break;
			}
			break;
		case 22:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_light_snow);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_light_snow);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_light_snow);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_light_snow);
					break;
			}
			break;
		case 23:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_moderate_snow);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_moderate_snow);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_moderate_snow);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_moderate_snow);
					break;
			}
			break;
		case 24:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_heavy_snow);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_heavy_snow);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_heavy_snow);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_heavy_snow);
					break;
			}
			break;
		case 25:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_snow_storm);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_snow_storm);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_snow_storm);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_snow_storm);
					break;
			}
			break;
		case 26:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_dust);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_dust);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_dust);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_dust);
					break;
			}
			break;
		case 27:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_sand);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_sand);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_sand);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_sand);
					break;
			}
			break;
		case 28:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_dust_storm);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_dust_storm);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_dust_storm);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_dust_storm);
					break;
			}
			break;
		case 29:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_sand_storm);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_sand_storm);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_sand_storm);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_sand_storm);
					break;
			}
			break;
		case 30:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_foggy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_foggy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_foggy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_foggy);
					break;
			}
			break;
		case 31:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_haze);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_haze);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_haze);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_haze);
					break;
			}
			break;
		case 32:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_windy);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_windy);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_windy);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_windy);
					break;
			}
			break;
		case 33:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_gale);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_gale);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_gale);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_gale);
					break;
			}
			break;
		case 34:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_hurricane);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_hurricane);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_hurricane);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_hurricane);
					break;
			}
			break;
		case 35:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_hurricane);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_hurricane);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_hurricane);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_hurricane);
					break;
			}
			break;
		case 36:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_hurricane);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_hurricane);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_hurricane);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_hurricane);
					break;
			}
			break;
		case 37:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_cold);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_cold);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_cold);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_cold);
					break;
			}
			break;
		case 38:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_hot);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_hot);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_hot);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_hot);
					break;
			}
			break;
		case 99:
			switch(num)
			{
				case 0:
					LCD_Draw_Picture(30,5,130,105,(u8 *)gImage_unknown);
					break;
				case 1:
					LCD_Draw_Picture(0,140,100,240,(u8 *)gImage_unknown);
					break;
				case 2:
					LCD_Draw_Picture(110,140,210,240,(u8 *)gImage_unknown);
					break;
				case 3:
					LCD_Draw_Picture(220,140,320,240,(u8 *)gImage_unknown);
					break;
			}
			break;
	}
}


//解析当前天气
u8 parse_now_weather(void)
{
	cJSON *root;
	cJSON *pSub;
	cJSON *arrayItem;
	cJSON *pItem;
	cJSON *pSubItem;
	cJSON *pChildItem;
	
	char *pr,*utf8str,*gbkstr;
	u8 size = 0;
	int len;
	u8 res;
	u8 temperature;
	
	root = mymalloc(SRAMIN,sizeof(cJSON));
	pSub = mymalloc(SRAMIN,sizeof(cJSON));
	pItem = mymalloc(SRAMIN,sizeof(cJSON));
	pSubItem = mymalloc(SRAMIN,sizeof(cJSON));
	pChildItem = mymalloc(SRAMIN,sizeof(cJSON));
	arrayItem = mymalloc(SRAMIN,sizeof(cJSON));
	
	pr = mymalloc(SRAMIN,1000);
	utf8str = mymalloc(SRAMIN,50);
	gbkstr = mymalloc(SRAMIN,50);
	
	memset(pr,0,1000);
	memset(gbkstr,0,50);
	memset(utf8str,0,50);
	
	file = mymalloc(SRAMIN,sizeof(FIL));
	res=f_open(file,(const TCHAR*)APP_ASCII_5427,FA_READ);//打开文件
	if(res==FR_OK)
	{
		asc2_5427 = mymalloc(SRAMIN,file->fsize);
		if(asc2_5427 != NULL)
		{
			res = f_read(file,asc2_5427,file->fsize,&br);
		}
		f_close(file);
	}
	
	root = cJSON_Parse((const char*)USART3_RX_BUF);
	if(root != NULL)
	{
		pSub = cJSON_GetObjectItem(root,"results");
		if(pSub != NULL)
		{
//			size = cJSON_GetArraySize(pSub);
			arrayItem = cJSON_GetArrayItem(pSub,0);
			pr = cJSON_Print(arrayItem);
			pItem = cJSON_Parse(pr);
			if(pItem != NULL)
			{
				pSubItem = cJSON_GetObjectItem(pItem,"location");
				if(pSubItem != NULL)
				{
					pChildItem = cJSON_GetObjectItem(pSubItem,"name");
					if(pChildItem != NULL)
					{
						utf8str = pChildItem->valuestring;
						SwitchToGbk((const u8*)utf8str,strlen(utf8str),(u8 *)gbkstr,&len);
						Show_Str(0,0,lcddev.width,lcddev.height,(u8 *)gbkstr,16,0);
					}
				}
//				memset(utf8str,0,50);
//				memset(gbkstr,0,50);
				pSubItem = cJSON_GetObjectItem(pItem,"now");
				if(pSubItem != NULL)
				{
					pChildItem = cJSON_GetObjectItem(pSubItem,"text");
					if(pChildItem != NULL)
					{
						utf8str = pChildItem->valuestring;
						SwitchToGbk((const u8*)utf8str,strlen(utf8str),(u8 *)gbkstr,&len);
						Show_Str(220,25,lcddev.width,lcddev.height,(u8 *)gbkstr,16,0);
					}
//					memset(utf8str,0,50);
//					memset(gbkstr,0,50);
					
					pChildItem = cJSON_GetObjectItem(pSubItem,"code");
					if(pChildItem != NULL)
					{
						gbkstr = pChildItem->valuestring;
						show_weather_icon((u8 *)gbkstr,0);
					}
//					memset(gbkstr,0,50);
					
					pChildItem = cJSON_GetObjectItem(pSubItem,"temperature");
					if(pChildItem != NULL)
					{
						gbkstr = pChildItem->valuestring;
						temperature = str2int((u8 *)gbkstr);
						gui_show_num(140,22,2,WHITE,54,temperature,0x80);
						printf("wendu = %d\r\n",temperature);
					}
				}
			}
			cJSON_Delete(pItem);
		}
	}
	cJSON_Delete(root);
	myfree(SRAMIN,root);
	myfree(SRAMIN,pSub);
	myfree(SRAMIN,pItem);
	myfree(SRAMIN,pSubItem);
	myfree(SRAMIN,pChildItem);
	myfree(SRAMIN,arrayItem);
	myfree(SRAMIN,pr);
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	myfree(SRAMIN,file);
	myfree(SRAMIN,asc2_5427);
	return 0;
}

//解析3天天气
u8 parse_3days_weather(void)
{
	cJSON *root;
	cJSON *pSub;
	cJSON *arrayItem;
	cJSON *pItem;
	cJSON *pSubItem;
	cJSON *pChildItem;
	cJSON *pLastItem;
	
	char *pr,*utf8str,*gbkstr;
	u8 size = 0;
	int len;
	u8 i;
	
	root = mymalloc(SRAMIN,sizeof(cJSON));
	pSub = mymalloc(SRAMIN,sizeof(cJSON));
	pItem = mymalloc(SRAMIN,sizeof(cJSON));
	pSubItem = mymalloc(SRAMIN,sizeof(cJSON));
	pChildItem = mymalloc(SRAMIN,sizeof(cJSON));
	arrayItem = mymalloc(SRAMIN,sizeof(cJSON));
	pLastItem = mymalloc(SRAMIN,sizeof(cJSON));
	
	pr = mymalloc(SRAMIN,1500);
	utf8str = mymalloc(SRAMIN,50);
	gbkstr = mymalloc(SRAMIN,50);
	
	memset(pr,0,1500);
	memset(gbkstr,0,50);
	memset(utf8str,0,50);
	
	

	root = cJSON_Parse((const char*)USART3_RX_BUF);
	if(root != NULL)
	{
		pSub = cJSON_GetObjectItem(root,"results");
		if(pSub != NULL)
		{
//			size = cJSON_GetArraySize(pSub);
			arrayItem = cJSON_GetArrayItem(pSub,0);
			pr = cJSON_Print(arrayItem);
			pItem = cJSON_Parse(pr);
			if(pItem != NULL)
			{
				pSubItem = cJSON_GetObjectItem(pItem,"daily");
				if(pSubItem != NULL)
				{
					size = cJSON_GetArraySize(pSubItem);
					for(i=0;i<size;i++)
					{
						if(i==3)break;
						arrayItem = cJSON_GetArrayItem(pSubItem,i);
						pr = cJSON_Print(arrayItem);
						pLastItem = cJSON_Parse(pr);
						if(pLastItem != NULL)
						{
							if(i==0)
							{
								pChildItem =  cJSON_GetObjectItem(pLastItem,"wind_direction");
								utf8str = pChildItem->valuestring;
								SwitchToGbk((const u8*)utf8str,strlen(utf8str),(u8 *)gbkstr,&len);
								LCD_Fill(220,45,320,61,BLACK);
								Show_Str(220,45,lcddev.width,lcddev.height,(u8 *)gbkstr,16,0);
							
								pChildItem =  cJSON_GetObjectItem(pLastItem,"wind_scale");
								gbkstr = pChildItem->valuestring;
								LCD_ShowString(220,65,20,20,16,(u8 *)gbkstr);
								
								pChildItem =  cJSON_GetObjectItem(pLastItem,"wind_speed");
								gbkstr = pChildItem->valuestring;
								LCD_ShowxNum(260,85,str2int((u8 *)gbkstr),2,16,0x80);
//								LCD_ShowString(260,85,20,20,16,(u8 *)gbkstr);
							}
							
							pChildItem =  cJSON_GetObjectItem(pLastItem,"high");
							gbkstr = pChildItem->valuestring;
							LCD_ShowxNum(30+110*i,130,str2int((u8 *)gbkstr),2,12,0x80);
//							LCD_ShowString(30+110*i,130,20,20,12,(u8 *)gbkstr);
							
							pChildItem =  cJSON_GetObjectItem(pLastItem,"low");
							gbkstr = pChildItem->valuestring;
							LCD_ShowxNum(48+110*i,130,str2int((u8 *)gbkstr),2,12,0x80);
//							LCD_ShowString(48+110*i,130,20,20,12,(u8 *)gbkstr);
							
							
							pChildItem =  cJSON_GetObjectItem(pLastItem,"code_day");												
							gbkstr = pChildItem->valuestring;
							show_weather_icon((u8 *)gbkstr,i+1);
														
						}
						cJSON_Delete(pLastItem);
					}
				}
			}
			cJSON_Delete(pItem);
		}
	}
	cJSON_Delete(root);
	myfree(SRAMIN,root);
	myfree(SRAMIN,pSub);
	myfree(SRAMIN,pItem);
	myfree(SRAMIN,pSubItem);
	myfree(SRAMIN,pChildItem);
	myfree(SRAMIN,arrayItem);
	myfree(SRAMIN,pLastItem);
	myfree(SRAMIN,pr);
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);


	return 0;
}

