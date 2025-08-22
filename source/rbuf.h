#ifndef __RBUF_H
#define __RBUF_H

typedef struct {
    unsigned int size;
    unsigned int rIdx;
    unsigned int wIdx;
    unsigned char *buf;
} RBuf_t;

void rbufInit(RBuf_t *rbuf, unsigned char *buf, unsigned int size);
unsigned int rbufWriteData(RBuf_t *rbuf, unsigned char *data, unsigned int len);
unsigned int rbufReadData(RBuf_t *rbuf, unsigned char *buf, unsigned int len);
unsigned int rbufFreeBuf(RBuf_t *rbuf);
unsigned int rbufDataLen(RBuf_t *rbuf);
unsigned char rbufHeadData(RBuf_t *rbuf);
unsigned char rbufData(RBuf_t *rbuf, unsigned int pos);

#endif
