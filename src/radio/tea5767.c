#include "tea5767.h"

static void tea5767_Write(uint8_t *_ucaBuf, uint8_t _count);


/*
*********************************************************************************************************
*	函 数 名: tea5767_Write
*	功能说明: 设置TEA5767, 连续写入_count个字节;
*	形    参：	_ucaBuf ： 待写入的数据;
*				_count  : 数据个数
*	返 回 值: 无
*********************************************************************************************************
*/
static void tea5767_Write(uint8_t *_ucaBuf, uint8_t _count)
{
	uint8_t i;
	uint8_t ucAck;

	i2c_Start();

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(TEA5767_SLAVE_ADDRESS | I2C_WR);

	/* 检测ACK */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		goto err_quit;
	}

	for (i = 0; i < _count; i++)
	{
		/* 发送数据 */
		i2c_SendByte(_ucaBuf[i]);

		/* 检测ACK */
		ucAck = i2c_WaitAck();
		if (ucAck == 1)
		{
			goto err_quit;
		}
	}

err_quit:
	i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: tea5767_Read
*	功能说明: 读取TEA5767的寄存器, 连续读出5个字节，存放在_ucaBuf
*	形    参：_ucaBuf ： 结果缓冲区指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void tea5767_Read(uint8_t *_ucaBuf)
{
	uint8_t i;
	uint8_t ucAck;

	i2c_Start();

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(TEA5767_SLAVE_ADDRESS | I2C_RD);

	/* 检测ACK */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		goto err_quit;
	}

	for (i = 0; i < 5; i++)
	{
		_ucaBuf[i] = i2c_ReadByte();
		if (i == 4)
		{
			i2c_NAck();
		}
		else
		{
			i2c_Ack();
		}
	}

err_quit:
	i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: CaclPLL
*	功能说明: 根据实际的频率值计算出PLL寄存器的值
*	形    参：_freq ： 频率
*		计算公式 （见TEA5767HN  datasheet的20页)
*			N = 4 X (fRF - fIF) / fref;
*			N = decimal value of PLL word;
*			fRF = the wanted tuning frequency [Hz]; 最大 108000000Hz
*			fIF = the intermediate frequency [Hz] = 225 kHz;
*			fref = the reference frequency [Hz] = 32.768 kHz
*	返 回 值: 无
*********************************************************************************************************
*/
static uint16_t CaclPLL(uint32_t _freq)
{
	return (uint16_t)(((_freq - 225000) * 4) / 32768);
}

/*
*********************************************************************************************************
*	函 数 名: CaclFreq
*	功能说明: 根据PLL计算频率
*	形    参：_pll ： pll值
*		计算公式 （见TEA5767HN  datasheet的20页)
*			N = 4 X (fRF - fIF) / fref;
*			N = decimal value of PLL word;
*			fRF = the wanted tuning frequency [Hz]; 最大 108000000Hz
*			fIF = the intermediate frequency [Hz] = 225 kHz;
*			fref = the reference frequency [Hz] = 32.768 kHz
*	返 回 值: 无
*********************************************************************************************************
*/
static uint32_t CaclFreq(uint16_t _pll)
{
	return ((uint32_t)_pll * 32768) / 4 + 225000;
}

/*
*********************************************************************************************************
*	函 数 名: tea5767_Set
*	功能说明: 初始化TEA767
* 	形    参: _Freq 	: 频率
*		  	_ucMuteEn	: 静音选项
*			_ucSerchEn ：搜索选项
*			_ucSearchUp ：向上搜索使能
*			_ucAdcLevel ： 自动搜台停止电平，1最灵敏，3最不灵敏
*	返 回 值: 无
*********************************************************************************************************
*/
void tea5767_Set(uint32_t _Freq, uint8_t _ucMuteEn, uint8_t _ucSerchEn, uint8_t _ucSearchUp, uint8_t _ucAdcLevel)
{
	uint8_t buf[5];
	uint16_t pll;

	/* 美国/欧洲(87.5-108M), 日本(76-91MHz) */
	pll = CaclPLL(_Freq);

	/*
		第1个寄存器：
		MUTE (bit7) = 0, L R输出有效
		SM(bit6) = 0 , 不搜索
	*/
	buf[0] = ((pll >> 8) & 0x3F);
	if (_ucMuteEn == MUTE_ON)
	{
		buf[0] |= (1 << 7);
	}
	if (_ucSerchEn == 1)
	{
		buf[0] |= (1 << 6);
	}
		
	/*
		第2个寄存器：PLL的低位
	*/
	buf[1] = pll;

	/*
		第3个寄存器
		SUD(BIT7) = 1, 向上搜索
		SSL(bit6 bit5) = 10  搜索停止级别。
			00 连续搜索
			01 低级，ADC 输出 = 5
			10 低级，ADC 输出 = 7
			11 低级，ADC 输出 = 10
			
		HLSI(BIT4) = 0, 低边LO注入
		MS (BIT3) = 0, 选择立体声
		MR (BIT2) = 0, R通道不静音
		ML (BIT1) = 0, L通道不静音
		SWP1（bit0) = 0 : 软件可编程端口
	*/
	if ((_ucAdcLevel < 1) || (_ucAdcLevel > 2))
	{
		_ucAdcLevel = 2;
	} 
	buf[2] = (2 << 5);
	if (_ucSearchUp == SEARCH_UP)
	{
		buf[2] |= (1 << 7);
	}

	/*
		第4个寄存器
		SWP2（bit7) = 0 : 软件可编程端口
		STBY（bit6) = 0 : 选择非待机模式
		BL (bit5) = 0, 选择美国/欧洲频带
		XTAL(bit4) - 1， 选择32768Hz晶振
		SMUTE(bit3) = 0, 软件静音关闭
		HCC(bit2) = 0, 高切控制关(不明白)
		SNC(bit1) = 1, 立体声消噪功能打开
		SI(bit0) = 1, swport引脚作为就绪标志（未用)
	*/
	buf[3] = (1 << 4) | (1 << 1) | (1 << 0);

	/*
		第5个寄存器
		PLLREF(BIT7) = 0, PLL的6.5MHz参考频率关闭
		DTC(bit6) = 1, 去加重时间常数75uS
	*/
	buf[4] = (1 << 6);

	tea5767_Write(buf, 5);
}

/*
*********************************************************************************************************
*	函 数 名: tea5767_ReadStatus
*	功能说明: 读取TEA芯片的状态
*	形    参：_tStatus ： 存放结果的结构体变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
void tea5767_ReadStatus(TEA5767_T *_tStatus)
{
	uint8_t buf[5];

	tea5767_Read(buf);

	_tStatus->ucReady = (buf[0] >> 7) & 0x01;
	_tStatus->ucBandLimit = (buf[0] >> 6) & 0x01;
	_tStatus->usPll = ((buf[0] & 0x3f) << 8) + buf[1];
	_tStatus->ucStereo =  (buf[2] >> 7) & 0x01;
	_tStatus->ucIFCount = buf[2] & 0x7F;
	_tStatus->ucAdcLevel = (buf[3] >> 4) & 0x0F;

	_tStatus->ulFreq = CaclFreq(_tStatus->usPll);
}

