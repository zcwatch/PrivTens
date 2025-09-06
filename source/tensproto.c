#include "tensproto.h"

unsigned char decodeBuf[2048], enBuf[2048];

static unsigned char tprotoCrc(unsigned char *data, int len)
{
	unsigned char crc = 0x55;
	int i;
	for(i = 0; i < len; i++) {
		crc ^= data[i];
	}
	return crc;
}

void tprotoInit(TensProto_t *proto, TensCmd_t *cmds, unsigned char cmdcnt, RBuf_t *rbuf, UartSendFunc send)
{
	proto->cmdCnt = cmdcnt;
	proto->cmds = cmds;
	proto->rBuf = rbuf;
	proto->send = send;
	proto->enBuf = enBuf;
	proto->decodeBuf = decodeBuf; 
}

void tprotoRxData(TensProto_t *proto, unsigned char *data, int len)
{
	rbufWriteData(proto->rBuf, data, len);
}

void tprotoEncode(TensProto_t *proto, unsigned char ty, unsigned char seq, unsigned char *payload, unsigned short len)
{
	unsigned short i;
	
	proto->enLen = 0;
	proto->enBuf[proto->enLen++] = TENSPROTO_HEADER;
	proto->enBuf[proto->enLen++] = ((len + 4) << 8) & 0xff;
	proto->enBuf[proto->enLen++] = (len + 4) & 0xff;
	proto->enBuf[proto->enLen++] = TENSPROTO_VERSION;
	proto->enBuf[proto->enLen++] = seq;
	proto->enBuf[proto->enLen++] = ty;
	for(i = 0; i < len; i++) {
		proto->enBuf[proto->enLen++] = payload[i];
	}
	proto->enBuf[proto->enLen++] = tprotoCrc(proto->enBuf + 1, len + 5);
	proto->enBuf[proto->enLen++] = TENSPROTO_END;
	proto->send(proto->enBuf, proto->enLen);
}

void tprotoDecode(TensProto_t *proto)
{
	unsigned char i;
	unsigned char end, crc;
	while(rbufDataLen(proto->rBuf) >= TENSPROTO_MINLEN) {
		if(proto->decodeState == 0) {	//Check Header
			if(rbufData(proto->rBuf, 0) == TENSPROTO_HEADER) {	
				
				proto->decodeLen = rbufData2(proto->rBuf, 1);
				if(proto->decodeLen > TENSPROTO_MAXLEN) {
					rbufReadData(proto->rBuf, proto->decodeBuf, 1);
				} else {
					proto->decodeState = 1;
				}
			} else {
				rbufReadData(proto->rBuf, proto->decodeBuf, 1);
			}
		} else if(proto->decodeState == 1) {	//decode
			if(rbufDataLen(proto->rBuf) < proto->decodeLen + 4) {	//length not enough
				break;
			} else {
				end = rbufData(proto->rBuf, proto->decodeLen + 3);
				if(end == TENSPROTO_END) {
					rbufReadData(proto->rBuf, proto->decodeBuf, proto->decodeLen + 4);
					crc = proto->decodeBuf[proto->decodeLen + 2]; //rbufData(proto->rBuf, proto->decodeLen + 2);
					if(crc == tprotoCrc(proto->decodeBuf + 1, proto->decodeLen + 1)) {
						proto->decodeGCmd = proto->decodeBuf[6];
						proto->decodeCmd = proto->decodeBuf[7];
						proto->decodeSeq = proto->decodeBuf[4];
						proto->decodeType = proto->decodeBuf[5];
						for(i = 0; i < proto->cmdCnt; i++) {
							if(proto->cmds[i].gcmd == proto->decodeGCmd && proto->cmds[i].cmd == proto->decodeCmd) {
								proto->cmds[i].deal(proto->decodeBuf + 8, proto->decodeLen - 6);
								break;
							}
						}
					} else {	// CRC Error
						
					}
				} else {	//END error
					rbufReadData(proto->rBuf, proto->decodeBuf, 1);
				}
				proto->decodeState = 0;
			}
		} else {
			proto->decodeLen = 0;
		}
	}
}

void tprotoAck(TensProto_t *proto, unsigned char *msg, int len)
{
	tprotoEncode(proto, TENSPROTO_TY_ACK, proto->decodeSeq, msg, len);
}
