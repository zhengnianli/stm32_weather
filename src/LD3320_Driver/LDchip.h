#ifndef LD_CHIP_H
#define LD_CHIP_H

#include "config.h"

//	以下三个状态定义用来记录程序是在运行ASR识别还是在运行MP3播放
#define LD_MODE_IDLE		0x00
#define LD_MODE_ASR_RUN		0x08
#define LD_MODE_MP3		 	0x40


//	以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE				0x00	//	表示没有在作ASR识别
#define LD_ASR_RUNING			0x01	//	表示LD3320正在作ASR识别中
#define LD_ASR_FOUNDOK			0x10	//	表示一次识别流程结束后，有一个识别结果
#define LD_ASR_FOUNDZERO 		0x11	//	表示一次识别流程结束后，没有识别结果
#define LD_ASR_ERROR	 		0x31	//	表示一次识别流程中LD3320芯片内部出现不正确的状态


#define CLK_IN   	22.1184	/* user need modify this value according to clock in */
#define LD_PLL_11			(uint8)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

// LD chip fixed values.
#define        RESUM_OF_MUSIC               0x01
#define        CAUSE_MP3_SONG_END           0x20

#define        MASK_INT_SYNC				0x10
#define        MASK_INT_FIFO				0x04
#define    	   MASK_AFIFO_INT				0x01
#define        MASK_FIFO_STATUS_AFULL		0x08



void LD_reset(void);

void LD_Init_Common(void);

void LD_Init_ASR(void);

void LD_ReloadMp3Data(void);
void LD_ReloadMp3Data_2(void);

uint8 LD_ProcessAsr(uint32 RecogAddr);
void LD_AsrStart(void);
uint8 LD_AsrRun(void);
uint8 LD_AsrAddFixed(void);
uint8 LD_GetResult(void);

void LD_ReadMemoryBlock(uint8 dev, uint8 * ptr, uint32 addr, uint8 count);
void LD_WriteMemoryBlock(uint8 dev, uint8 * ptr, uint32 addr, uint8 count);

extern uint8  nLD_Mode;

//关键词
//-------------------------搜索天气-----------------------------------
#define	STR00  "xiao tian"							// 小明
#define	STR01  "sou suo fu zhou tian qi"			// 搜索福州天气
#define	STR02  "sou suo shang hai tian qi"			// 搜索上海天气
#define	STR03  "sou suo shen zhen tian qi"			// 搜索深圳天气
#define	STR04  "sou suo bei jing tian qi"			// 搜索北京天气
#define	STR05  "sou suo guang zhou tian qi"			// 搜索广州天气
#define	STR06  "sou suo nan ning tian qi"			// 搜索南宁天气
#define	STR07  "sou suo xia men tian qi"			// 搜索厦门天气
#define	STR08  "sou suo quan zhou tian qi"			// 搜索泉州天气
#define	STR09  "sou suo pu tian tian qi"			// 搜索莆田天气
#define	STR10  "sou suo nan ping tian qi"			// 搜索南平天气
//-------------------------询问屏幕上的信息----------------------------
#define	STR11  "yu yin bo bao tian qi"				// 语音播报天气
#define	STR12  "jin tian de qi wen shi duo duo shao"// 今天的气温是多少
#define	STR13  
#define	STR14  "xian zai ji dian"					// 现在几点
#define	STR15  ""`				
#define	STR16  ""
#define	STR17  ""
#define	STR18  "" 
#define	STR19  ""
#define	STR20  ""
#define	STR21  ""
#define	STR22  ""
#define	STR23  ""
#define	STR24  ""
#define	STR25  ""
#define	STR26  ""
#define	STR27  ""
#define	STR28  ""
#define	STR29  ""
#define	STR30  ""
#define	STR31  ""
#define	STR32  ""
#define	STR33  ""
#define	STR34  ""
#define	STR35  ""
#define	STR36  ""
#define	STR37  ""
#define	STR38  ""
#define	STR39  ""
#define	STR40  ""
#define	STR41  ""
#define	STR42  ""
#define	STR43  ""
#define	STR44  ""
#define	STR45  ""
#define	STR46  ""
#define	STR47  ""
#define	STR48  ""
#define	STR49  ""



//识别码（客户修改处）
#define CODE00	0	 
#define CODE01	1	
#define CODE02	2
#define CODE03	3 
#define CODE04	4
#define CODE05	5
#define CODE06	6
#define CODE07	7
#define CODE08	8
#define CODE09	9
#define CODE10	10
#define CODE11	11	 
#define CODE12	12	
#define CODE13	13
#define CODE14	14
#define CODE15	15
#define CODE16	16
#define CODE17	17
#define CODE18	18
#define CODE19	19
#define CODE20	20
#define CODE21	21
#define CODE22	22	 
#define CODE23	23	
#define CODE24	24
#define CODE25	25
#define CODE26	26
#define CODE27	27
#define CODE28	28
#define CODE29	29
#define CODE30	30
#define CODE31	31
#define CODE32	32
#define CODE33	33	 
#define CODE34	34	
#define CODE35	35
#define CODE36	36
#define CODE37	37
#define CODE38	38
#define CODE39	39
#define CODE40	40
#define CODE41	41
#define CODE42	42
#define CODE43	43
#define CODE44	44	 
#define CODE45	45	
#define CODE46	46
#define CODE47	47
#define CODE48	48
#define CODE49	49


#define MIC_VOL 0x43

void  LD3320_delay(unsigned long uldata);
void ProcessInt0(void);
char PlayDemoSound_mp3(char *filename,u8 Volume);



#endif

