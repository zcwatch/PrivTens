#ifndef __TENSPROTO_H
#define __TENSPROTO_H

#include "rbuf.h"
#include "typedef.h"

#define TENSPROTO_VERSION				0x01
#define PROTO_VERSION						0x10, 0x00, 0x00

#define TENSPROTO_HEADER				0xaa
#define TENSPROTO_END					0x55

#define TENSPROTO_MINLEN				11
#define TENSPROTO_MAXLEN				2048

#define TENSPROTO_TY_PC					0x01
#define TENSPROTO_TY_DEV				0x02
#define TENSPROTO_TY_ACK				0x00

#define TENSPROTO_R   					0x01
#define TENSPROTO_W						0x02

#define TENSPROTO_GCMD_SYS					0x01
#define TENSPROTO_GCMD_CTRL					0x02
#define TENSPROTO_GCMD_DATA					0x03

#define TENSPROTO_SYSCMD_HS					0x01
#define TENSPROTO_SYSCMD_INFO				0x02
#define TENSPROTO_SYSCMD_VER				0x03
#define TENSPROTO_SYSCMD_HEARTBEAT			0x04
#define TENSPROTO_SYSCMD_ERR				0x05
#define TENSPROTO_SYSCMD_FW					0x06

#define TENSPROTO_CTRLCMD_OUTPUTTIME		0x01
#define TENSPROTO_CTRLCMD_CTRL				0x02
#define TENSPROTO_CTRLCMD_STRENGTH			0x03
#define TENSPROTO_CTRLCMD_CHANNEL			0x04

#define TENSPROTO_DATACMD_THERAPY			0x01



typedef struct {
	unsigned char gcmd;
	unsigned char cmd;
	CmdDealFunc deal;
} TensCmd_t;

typedef struct {
	unsigned char decodeState;
	unsigned short decodeLen;
	unsigned char *decodeBuf;
	unsigned char decodeSeq;
	unsigned char decodeType;
	unsigned char decodeCmd;
	unsigned char decodeGCmd;

	unsigned short enLen;
	unsigned char *enBuf;

	
	unsigned char cmdCnt;
	TensCmd_t *cmds;
	RBuf_t *rBuf;
	UartSendFunc send;
} TensProto_t;

void tprotoInit(TensProto_t *proto, TensCmd_t *cmds, unsigned char cmdcnt, RBuf_t *rbuf, UartSendFunc send);
void tprotoRxData(TensProto_t *proto, unsigned char *data, int len);
void tprotoEncode(TensProto_t *proto, unsigned char ty, unsigned char seq, unsigned char *payload, unsigned short len);
void tprotoDecode(TensProto_t *proto);
void tprotoAck(TensProto_t *proto, unsigned char *msg, int len);


#endif
