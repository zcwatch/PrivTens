#include "tens.h"
#include "board.h"
#include "hv.h"
#include "tensconfig.h"
#include "tensdata.h"
#include "tensgui.h"


#define DEFAULT_RUN_TIME 			1800

#define BUTTON_SIZE					7
#define MOTORTIP_SIZE				1
#define CMD_SIZE 					13

Tens_t privateTens;
unsigned char devCode[10] = {0x01, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char verCode[12] = {HWVERSION, FWVERSION, FWVERSION2, PROTO_VERSION};
unsigned char tmpBuff[1024];

MotorTipInfo_t motorTipInfos[MOTORTIP_SIZE] = {
	{MOTORTIP_MODE1, halMotorWarn, {0}}
};

static void keyTensUpChanged(unsigned char state)
{
	if(!state) return;
	if(privateTens.state != TENS_RUN) return;
	if(privateTens.tensLevel < HV_MAXLEVEL) {
		tensSetLevel(&privateTens, privateTens.tensLevel + 1);
		privateTens.tarLevel = privateTens.tensLevel;
		mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
	}
}

static void keyTensUpPressed(void)
{
	if(privateTens.state != TENS_RUN) return;	
	if(privateTens.tensLevel < HV_MAXLEVEL) {
		
		tensSetLevel(&privateTens, privateTens.tensLevel + 1);
		privateTens.tarLevel = privateTens.tensLevel;
		mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
	}
}

static void keyTensDownChanged(unsigned char state)
{
	if(!state) return;
	if(privateTens.state != TENS_RUN) return;
	if(privateTens.tensLevel > 0) {
		tensSetLevel(&privateTens, privateTens.tensLevel - 1);
		privateTens.tarLevel = privateTens.tensLevel;
		mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
	}
}

static void keyTensDownPressed(void)
{
	if(privateTens.state != TENS_RUN) return;
	if(privateTens.tensLevel > 0) {
		tensSetLevel(&privateTens, privateTens.tensLevel - 1);
		privateTens.tarLevel = privateTens.tensLevel;
		mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
	}
}

static void keyTensConfirmChanged(unsigned char state)
{
	unsigned short tmp;
	if(state == 0) return;
	if(privateTens.state == TENS_FW) return;
	
//	if(state == 0) {
//		if(privateTens.state == TENS_RUN) {
//			tensPauseOutput(&privateTens);
//			guiUpdateTensState(&privateTens);
//			guiDisplayRunTime(&privateTens);
//			guiUpdateHeat(&privateTens);
//			guiUpdateVib(&privateTens);
//			mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
//		} else if(privateTens.state == TENS_IDLE) {
//			privateTens.tarLevel = privateTens.tensLevel;
//		privateTens.tensLevel = 1;
//		tensStartOutput(&privateTens);
//		privateTens.state = TENS_RUN;	
//		} else if(privateTens.state == TENS_PAUSE) {
//			privateTens.tarLevel = privateTens.tensLevel;
//			tmp = privateTens.runTimeCount;
//			privateTens.tensLevel = 1;
//			tensStartOutput(&privateTens);
//			privateTens.runTimeCount = tmp;
//			privateTens.state = TENS_RUN;
//		}
//		return;
//	}
	
	
	
	if(privateTens.state == TENS_IDLE) {
		privateTens.tarLevel = privateTens.tensLevel;
		privateTens.tensLevel = 1;
		tensStartOutput(&privateTens);
		privateTens.state = TENS_RUN;	
	} else if(privateTens.state == TENS_RUN) {
		tensoutPauseOutput(&privateTens.tensOutput);
		tensPauseOutput(&privateTens);
		//return;
	} else if(privateTens.state == TENS_PAUSE) {
		privateTens.tarLevel = privateTens.tensLevel;
		tmp = privateTens.runTimeCount;
		privateTens.tensLevel = 1;
		tensStartOutput(&privateTens);
		privateTens.runTimeCount = tmp;
		privateTens.state = TENS_RUN;
	}
	guiUpdateTensState(&privateTens);
	guiDisplayRunTime(&privateTens);
	guiUpdateHeat(&privateTens);
	guiUpdateVib(&privateTens);
	mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
}

static void keyTensConfirmPressed(void)
{
	if(privateTens.state == TENS_RUN || privateTens.state == TENS_PAUSE) {
		tensoutStopOutput(&privateTens.tensOutput);
		privateTens.state = TENS_IDLE;
		guiUpdateTensState(&privateTens);
		privateTens.runTimeCount = 0;
		//tensSetLevel(&privateTens, 0);
		guiDisplayRunTime(&privateTens);
		mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
	}
}

static void keyPowerChanged(unsigned char state)
{
	if(state) {
		privateTens.pwrCount = 0;
		privateTens.pwrState = 1;
	} else {
		if(privateTens.pwrState == 2) {
			tensPowerDown(&privateTens);
		}
		privateTens.pwrState = 0;
	}
}

static void keyPowerPressed(void)
{
	if(privateTens.pwrState) {
		if(++privateTens.pwrCount == 3) {	//shut down device
			privateTens.pwrState = 2;
			guiPoweroff();
			if(privateTens.state == TENS_RUN) tensoutStopOutput(&privateTens.tensOutput);
			mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
			//tensPowerDown(&privateTens);
		}
	} else {
		
	}
}

static void keyScanChanged(unsigned char state)
{
	unsigned char ret = 0;
	unsigned char flag = 0;
	
	if(state == 6) {		//up
		if(privateTens.state != TENS_RUN) return;	
		switch(privateTens.keyFunc) {
			case KEYFUNC_VIB:
				if(privateTens.vibMode < 4) {
					tensSetVib(&privateTens, privateTens.vibMode + 1);
					mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
				}
				break;
			case KEYFUNC_HEAT:
				if(privateTens.heatLevel < 4) {
					tensSetHeat(&privateTens, privateTens.heatLevel + 1);
					guiUpdateHeat(&privateTens);
					mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
				}
				break;
			case KEYFUNC_TIME:
				if(privateTens.runTime < 3600) {
					privateTens.runTimeCount = 0;
					privateTens.runTime += 300;
					guiDisplayRunTime(&privateTens);
					mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
				}
				break;
			default: 
				break;
		}
	} else if(state == 5) {	//Down
		if(privateTens.state != TENS_RUN) return;	
		switch(privateTens.keyFunc) {
			case KEYFUNC_VIB:
				if(privateTens.vibMode > 0) {
					tensSetVib(&privateTens, privateTens.vibMode - 1);
					mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
				}
				break;
			case KEYFUNC_HEAT:
				if(privateTens.heatLevel > 0) {
					tensSetHeat(&privateTens, privateTens.heatLevel - 1);
					guiUpdateHeat(&privateTens);
					mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
				}
				break;
			case KEYFUNC_TIME:
				if(privateTens.runTime > 300) {
					privateTens.runTimeCount = 0;
					privateTens.runTime -= 300;
					guiDisplayRunTime(&privateTens);
					mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
				}
				break;
			default: 
				break;
		}
	} else if(state == 4) {	//SET
		if(privateTens.state != TENS_RUN) return;	
		//if(privateTens.keyFunc == KEYFUNC_TIME) //save run time data
		if(++privateTens.keyFunc > KEYFUNC_TIME) privateTens.keyFunc = KEYFUNC_VIB;

		guiDisplayRunTime(&privateTens);
		guiUpdateHeat(&privateTens);
		guiUpdateVib(&privateTens);

		mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
	} else if(state == 1) {	//Left
		if(privateTens.prescId > 1) {
			privateTens.prescId--;
			if(privateTens.state == TENS_RUN) {
				flag = 1;
				tensoutStopOutput(&privateTens.tensOutput);
				privateTens.state = TENS_IDLE;
			}
			ret = tensLoadPrescData(&privateTens, 0);
			if(flag && ret) {
				tensStartOutput(&privateTens);
			}
			mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
		}
	} else if(state == 2) {	//Right
		if(privateTens.prescId < PRESC_DATA_SIZE) {
			privateTens.prescId++;
			if(privateTens.state == TENS_RUN) {
				flag = 1;
				tensoutStopOutput(&privateTens.tensOutput);
				privateTens.state = TENS_IDLE;
			}
			ret = tensLoadPrescData(&privateTens, 0);
			if(flag && ret) {
				tensStartOutput(&privateTens);
			}
			mtStart(&privateTens.motorTip, 0, MOTORTIP_MODE1);
		}
	}
}

//todo: Display battery charge indicator
static void batChargeStateChanged(unsigned char state)
{
	privateTens.chargeState = state;
}

//todo: Display bluetooth connected state
static void btStatusChanged(unsigned char state)
{
	privateTens.btState = (state) ? 1 : 0;
	guiUpdateBluetooth(&privateTens);
}

KeyInfo_t keys[BUTTON_SIZE] = {
	{0, 0, halKeyTensDown, keyTensDownChanged, keyTensDownPressed},
	{0, 0, halKeyTensUp, keyTensUpChanged, keyTensUpPressed},
	{0, 0, halKeyTensConfirm, keyTensConfirmChanged, keyTensConfirmPressed},
	{0, 0, halKeyPower, keyPowerChanged, keyPowerPressed},
	{0, 0, halScanKey, keyScanChanged, 0},
	{0, 0, halReadChargeSta, batChargeStateChanged, 0},
	{0, 0, btReadBtSta, btStatusChanged, 0}
};

static void cmdHandshankDeal(unsigned char *data, int len)
{
	unsigned char msg[5] = {TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_HS, 0, ~data[1], ~data[2]};
	tprotoAck(&privateTens.proto, msg, 5);
}

static void cmdGetInfoDeal(unsigned char *data, int len)
{
	unsigned char msg[43] = {
		TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_INFO, 0,							//3
		0x01, 0x02, 0x01, 0x01,												// 4
		'P', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '-', '0', '0', '1', 	//16
		'0', '1', '2', '3',		//4
		'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'	//16
	};
	tprotoAck(&privateTens.proto, msg, 43);
}

static void cmdGetVersionDeal(unsigned char *data, int len)
{
	unsigned char msg[20] = {TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_VER, 0, VERSION};
	tprotoAck(&privateTens.proto, msg, 13);
}

static void cmdHeartbeatDeal(unsigned char *data, int len)
{
	unsigned short time = privateTens.runTime - privateTens.runTimeCount;
	unsigned char msg[20] = {TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_HEARTBEAT, 0, privateTens.battery.per, privateTens.state, time & 0xff, (time >> 8) & 0xff};
	tprotoAck(&privateTens.proto, msg, 7);
}

static void cmdErrorDeal(unsigned char *data, int len)
{

}

#ifndef RELEASE
#define FWDEBUG_ADDR          ((unsigned int)0x08020000)
#define FWDEBUG_SIZE          ((unsigned int)256)
unsigned char debug_fw[FWDEBUG_SIZE];
#endif

unsigned short pkgCount, pkgLen, pkgMax;
unsigned int dataWIdx;
static void cmdFwDeal(unsigned char *data, int len)
{
	unsigned char flag = 0, params[4] = {TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_FW, 0, 0};
	unsigned short dataLen;
	unsigned short dbgIndex;
	if(data[2] == 0) {
		pkgCount = 0;
		dataWIdx = 6;
		tensdataEraseFWFlash();
#ifndef RELEASE
		memset((void*)debug_fw, 0xAA, FWDEBUG_SIZE);
#endif
	}

	pkgLen = GETSHORT(data, 3);
	//if(data[1] > 1 && data[2] == 0) //first package
	if(data[1] != (data[2] + 1) && pkgMax == 0) {
		pkgMax = pkgLen;
	}

#ifndef RELEASE
	dbgIndex = data[2]*4;
	debug_fw[dbgIndex+0] = data[1];
	debug_fw[dbgIndex+1] = data[2];
	debug_fw[dbgIndex+2] = data[3];
	debug_fw[dbgIndex+3] = data[4];
#endif

	if(data[2] == 1) {
		unsigned short temp = pkgCount;
		temp++;
		temp--;
	}

	halFlashWrite(FWFILE_ADDR + 6 + pkgMax*data[2], data + 5, pkgLen);
	dataWIdx += pkgLen;
	pkgCount++;

	//last package
	if(data[1] == data[2] + 1) {
		dataLen = FWMARK;
		halFlashWrite(FWFILE_ADDR, (unsigned char*)&dataLen, 2);
		dataWIdx -= 6;
		halFlashWrite(FWFILE_ADDR + 2, (unsigned char*)&dataWIdx, 4);
		flag = 1;

#ifndef RELEASE
	  debug_fw[pkgCount*4+0] = dataWIdx&0xFF;
	  debug_fw[pkgCount*4+1] = (dataWIdx>>8)&0xFF;
		halFlashWrite(FWDEBUG_ADDR, (unsigned char*)&debug_fw, FWDEBUG_SIZE);
#endif
	}

	params[3] = data[2];
	tprotoAck(&privateTens.proto, params, 4);
	if(flag) guiFwUpdateFsh(&privateTens);
}

static void cmdOutputTimeDeal(unsigned char *data, int len)
{
	unsigned char rw = data[0];
	unsigned char params[10] = {TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_OUTPUTTIME, 0, 0, 0, 0, 0};
	unsigned short rmTime;

	if(rw == 2) {	//Write
		rmTime = data[1] | (data[2] << 8);
		privateTens.runTime = rmTime;
		privateTens.runTimeCount = 0;
	} 
	rmTime = privateTens.runTime - privateTens.runTimeCount;
	params[3] = privateTens.runTime & 0xff;
	params[4] = (privateTens.runTime >> 8) & 0xff;
	params[5] = rmTime & 0xff;
	params[6] = (rmTime >> 8) & 0xff;

	tprotoAck(&privateTens.proto, params, 7);
}

static void cmdControlDeal(unsigned char *data, int len)
{
	unsigned char rw, ctrl, params[10] = {TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_CTRL, 0, 0};
	
	rw = data[0];
	ctrl = data[1];
	if(rw == 2) {
		switch(ctrl) {
			case 0x01:
			tensStartOutput(&privateTens);
			break;
			case 0x02:
			tensPauseOutput(&privateTens);
			break;
			case 0x03:
			tensStopOutput(&privateTens);
			break;
			case 0x04:
			tensPowerDown(&privateTens);
			break;
			case 0x05:
			tensPowerDown(&privateTens);
			break;
			default: break;
		}															
	}
	tprotoAck(&privateTens.proto, params, 4);
}

static void cmdStrengthDeal(unsigned char *data, int len)
{
	unsigned char params[10] = {TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_STRENGTH, 0, 0x02, 0, 0};
	unsigned char rw = data[0];
	if(rw == 0x02) {
		if(data[2] < HV_MAXLEVEL) tensSetLevel(&privateTens, data[2]);
		//for(i = 0; i < 2; i++) params[i + 4] = data[i + 2];
	}
	params[4] = privateTens.tensLevel;
	params[5] = privateTens.tensLevel;
	tprotoAck(&privateTens.proto, params, 6);
}

static void cmdChannelDeal(unsigned char *data, int len)
{
	unsigned char params[4] = {TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_STRENGTH, 0, 0x15};

	tprotoAck(&privateTens.proto, params, 4);
}

static unsigned int tpPkgCount = 0, tpDataWIdx = 0;
static void cmdTherapyDeal(unsigned char *data, int len)
{
	unsigned char flag = 0, params[4] = {TENSPROTO_GCMD_DATA, TENSPROTO_DATACMD_THERAPY, 0, 0};
	unsigned char pkgTotal, pkgIdx, rw = data[0];
	unsigned short pkgLen;
	
	if(rw == TENSPROTO_W) {	// write therapy data
		//pos = data[1];
		pkgTotal = data[1];
		pkgIdx = data[2];
		pkgLen = GETSHORT(data, 3);
		
		if(pkgIdx == 0) {		// first package
			tpPkgCount = 0;
			tpDataWIdx = 6;
			tensdataEraseTherapyFlash();
		}

		// Write data
		halFlashWrite(PRESC_DATA_ADDR + tpDataWIdx, data + 5, pkgLen); 
		tpDataWIdx += pkgLen;
		tpPkgCount++;
		
		// Last package
		if(pkgIdx + 1 == pkgTotal) {
			pkgLen = TPMARK;
			halFlashWrite(PRESC_DATA_ADDR, (unsigned char*)&pkgLen, 2);  //Little end
			tpDataWIdx -= 6;
			halFlashWrite(PRESC_DATA_ADDR + 2, (unsigned char*)&tpDataWIdx, 4);  //Little end
			flag = 1;
		}
		params[3] = pkgIdx;
		tprotoAck(&privateTens.proto, params, 4);		
	} else {	// read therapy data
		
	}	
}

TensCmd_t cmds[CMD_SIZE] = {
	{TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_HS, cmdHandshankDeal},
	{TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_INFO, cmdGetInfoDeal},
	{TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_VER, cmdGetVersionDeal},
	{TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_HEARTBEAT, cmdHeartbeatDeal},
	{TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_ERR, cmdErrorDeal},
	{TENSPROTO_GCMD_SYS, TENSPROTO_SYSCMD_FW, cmdFwDeal},

	{TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_OUTPUTTIME, cmdOutputTimeDeal},
	{TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_CTRL, cmdControlDeal},
	{TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_STRENGTH, cmdStrengthDeal},
	{TENSPROTO_GCMD_CTRL, TENSPROTO_CTRLCMD_CHANNEL, cmdChannelDeal},

	{TENSPROTO_GCMD_DATA, TENSPROTO_DATACMD_THERAPY, cmdTherapyDeal}
};
#define PROTO_BUFF_SIZE 			1100
unsigned char protoRxBuff[PROTO_BUFF_SIZE];
RBuf_t rBuf = {PROTO_BUFF_SIZE, 0, 0, protoRxBuff};

void tensInit(Tens_t *tens)
{
	tensdataInit();
	tens->state = TENS_NONE;
	tens->keyFunc = KEYFUNC_VIB;
	tens->prescId = 1;
	tens->pwrState = 0;
	tens->pwrCount = 0;
	tens->heatLevel = 0;
	tens->runTime = DEFAULT_RUN_TIME;
	tens->defaultTime = DEFAULT_RUN_TIME;
	mtInit(&tens->motorTip, motorTipInfos, MOTORTIP_SIZE);
	buttonInit(&tens->button, keys, BUTTON_SIZE);
	tprotoInit(&tens->proto, cmds, CMD_SIZE, &rBuf, btSenddata);
	
	tens->hc595.sda = halHC595SDA;
	tens->hc595.clk = halHC595CLK;
	tens->hc595.la = halHC595LA;
	hc595Init(&tens->hc595);
	batteryInit(&tens->battery, halGetBatAdc, halReadChargeSta);
	heatInit(&tens->heat, halHeat);
	vibInit(&tens->vib, halMotor);

	tensoutInit(&tens->tensOutput);
#if 1
	tmInit(&tens->therapyManager, PRESC_DATA_ADDR, TPMARK, halFlashErase, halFlashWrite);
#endif
	tensLoadPrescData(tens, 0);
	tensSetOutputChannel(tens, 2, 2);
	
	//guiInit(tens);
	tens->state = TENS_IDLE;
	tens->chargeCnt = 0;
}

void tensProcess(Tens_t *tens)
{
	buttonProcess(&tens->button);
	batteryProcess(&tens->battery);
	if(++tens->chargeCnt == 500) {
		tens->chargeCnt = 0;
		guiUpdateBattery(tens);
	}
	tensChannelProcess(tens);
}

/*
* 				N4		N3		N2		N1		P4		P3		P2		P1
*  			BIT	7		6		5		4		3		2		1		0
*
*				0		1		1		1		0		1		1		1 		0x77			1
*				0		1		1		1		1		0		1		1		0x7B
*				0		1		1		1		1		1		0		1		0x7D
*				1		0		1		1		0		1		1		1		0xB7
*				1		0		1		1		1		0		1		1		0xBB			2
*				1		0		1		1		1		1		1		0		0xBE
*				1		1		0		1		0		1		1		1		0xD7
*				1		1		0		1		1		1		0		1		0xDD			3
*				1		1		0		1		1		1		1		0		0xDE
*				1		1		1		0		1		0		1		1		0xEB
*				1		1		1		0		1		1		0		1		0xED
*				1		1		1		0		1		1		1		0		0xEE			4
*/
//unsigned char chanTb[12] = {0x77, 0x7b, 0x7d, 0xb7, 0xbb, 0xbe, 0xd7, 0xdd, 0xde, 0xeb, 0xed, 0xee};
unsigned char chanMode5[4] = {0xde, 0xed, 0x7b, 0xb7}; //P1N2,P2N1,P3N4,P4N3
unsigned char chanMode4[4] = {0xEB, 0xDD, 0xBE, 0x77}; //N1P3,N2P2,N3P1,N4P4
void tensChannelProcess(Tens_t *tens)
{
	if((tens->vibMode == 4) || (tens->vibMode == 5)) {
		if(++tens->channelCnt == 200) {
			tens->channelCnt = 0;
			if(++tens->tensChannel == 5) tens->tensChannel = 0;
			tensSetOutputChannel(tens, tens->tensChannel, tens->vibMode);
		}
	} 
}	

void tensLevelUpProcess(Tens_t *tens)
{
	if(tens->state != TENS_RUN) return;
	if(tens->tensLevel < tens->tarLevel) {
		tensSetLevel(tens, tens->tensLevel + 1);
	}
}

void tensSetLevel(Tens_t *tens, unsigned char level)
{
	tens->tensLevel = level;
	hvSetLevel(level);
	guiUpdateStrength(tens);
}

void tensPowerDown(Tens_t *tens)
{
	mtStart(&tens->motorTip, 0, MOTORTIP_MODE2);
	halPowerEn(0);
	guiPoweroff();
}

void tensPowerUp(Tens_t *tens)
{
	halPowerEn(1);
	mtStart(&tens->motorTip, 0, MOTORTIP_MODE2);
	tens->pwrState = 0;
	guiInit(tens);
}

void tensCloseOutputChannel(Tens_t *tens)
{
	hc595SetData(&tens->hc595, 0xff);
}

void tensSetOutputChannel(Tens_t *tens, unsigned char channel, unsigned char mode)
{
	unsigned char *p;
	if(mode == 4) p = chanMode4;
	else if(mode == 5) p = chanMode5;
	else {
		hc595SetData(&tens->hc595, 0xde);
		return;
	}
	if(channel < 4) {
		hc595SetData(&tens->hc595, p[channel]);
		//mtStart(&tens->motorTip, 0, MOTORTIP_MODE1);
		tens->channelCnt = 0;
	} else {
		hc595SetData(&tens->hc595, 0xff);
		//mtStart(&tens->motorTip, 0, MOTORTIP_MODE1);
	}
}

unsigned short tensGetPrescData(Tens_t *tens, unsigned char chann, unsigned char *data)
{
	unsigned char *p;
	unsigned short i, len;
	p = tmFindPrescription(&tens->therapyManager, tens->prescId-1, chann);
	if(p == 0) return 0;
	
	len = GETSHORT(p, 0) + 2 + 1; //+Length+CRC
	for(i = 0; i < len; i++) data[i] = *(p + i);
	return len;
}

unsigned short tensLoadPrescData(Tens_t *tens, unsigned char chann)
{
	int len;
	if(tens->prescId <= 0 || tens->prescId > PRESC_DATA_SIZE) return 0;
	len = tensGetPrescData(tens, 0, tmpBuff);
	if(len > 0) {
		tensoutSetPrescription(&tens->tensOutput, tmpBuff, len);
		guiUpdatePresc(tens);
	}
	
	return (len > 0 ? 1 : 0);
}

void tensStartOutput(Tens_t *tens)
{
	spwmEnable();
	tmrSpwmNEn(1);
	tensoutStartOutput(&tens->tensOutput);
	tensSetOutputChannel(tens, 1, tens->vibMode);
	tens->state = TENS_RUN;
	tens->runTimeCount = 0;
	//tensSetLevel(tens, tens->tensLevel);
	//tensSetLevel(tens, 10);
	tensSetHeat(tens, tens->tensLevel);
	tensSetVib(tens, tens->vibMode);
	guiUpdateTensState(tens);
}

void tensStopOutput(Tens_t *tens)
{
	tensoutStopOutput(&tens->tensOutput);
	tensCloseOutputChannel(tens);
	tens->state = TENS_IDLE;
	spwmDisable();
//	tensSetLevel(tens, 0);
//	tensSetHeat(tens, 0);
//	tensSetVib(tens, 0);
	guiUpdateTime(tens);
	guiUpdateTensState(tens);
	
	hvSetLevel(0);
	vibSet(&tens->vib, 0);
	heatSet(&tens->heat, 0);
}

void tensPauseOutput(Tens_t *tens)
{
	tensCloseOutputChannel(tens);
	tensoutPauseOutput(&tens->tensOutput);
	tens->state = TENS_PAUSE;
	guiUpdateTensState(tens);
	
	hvSetLevel(0);
	vibSet(&tens->vib, 0);
	heatSet(&tens->heat, 0);
}

void tensSetOutputTime(Tens_t *tens, unsigned short time, unsigned short defaultTime)
{
	tens->runTime = time;
	tens->defaultTime = defaultTime;
}

void tensRunTimeUpdate(Tens_t *tens)
{
	if(tens->state == TENS_RUN) {
		if(++tens->runTimeCount == tens->runTime) {
			tensStopOutput(tens);
		}
		guiUpdateTime(tens);
	}
}

void tensSetHeat(Tens_t *tens, unsigned char level)
{
	if(level > 4) level = 0;
	tens->heatLevel = level;
	switch(level) {
		case 0:
			level = 0;
			break;
		case 1:
			level = 30;
			break;
		case 2:
			level = 40;
			break;
		case 3:
			level = 50;
			break;
		case 4:
			level = 60;
			break;
		default: break;
	}
	heatSet(&tens->heat, level);
	guiUpdateHeat(tens);
}

void tensSetVib(Tens_t *tens, unsigned char mode)
{
	if(mode > 5) mode = 0;
	tens->vibMode = mode;
	if((mode == 4) || (mode == 5)) {
		vibSet(&tens->vib, 0);
	} else {
		vibSet(&tens->vib, mode * 25);
	}
	tensSetOutputChannel(tens, tens->tensChannel, tens->vibMode);
	guiUpdateVib(tens);
}






