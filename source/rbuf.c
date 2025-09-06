#include "rbuf.h"

void rbufInit(RBuf_t *rbuf, unsigned char *buf, unsigned int size)
{
    rbuf->size = size;
    rbuf->buf = buf;
    rbuf->rIdx = 0;
    rbuf->wIdx = 0;
}

unsigned int rbufWriteData(RBuf_t *rbuf, unsigned char *data, unsigned int len)
{
    unsigned int i, wlen = rbufFreeBuf(rbuf);
    if(wlen > len) wlen = len;
    for(i = 0; i < wlen; i++) {
        rbuf->buf[rbuf->wIdx++] = data[i];
        if(rbuf->wIdx == rbuf->size) rbuf->wIdx = 0;
    }

    return wlen;
}

unsigned int rbufFreeBuf(RBuf_t *rbuf)
{
    unsigned int len;
    len = (rbuf->wIdx >= rbuf->rIdx) ? (rbuf->wIdx - rbuf->rIdx) : (rbuf->wIdx + rbuf->size - rbuf->rIdx);
    return (rbuf->size - len);
}

unsigned int rbufDataLen(RBuf_t *rbuf)
{
    unsigned int len;
    len = (rbuf->wIdx >= rbuf->rIdx) ? (rbuf->wIdx - rbuf->rIdx) : (rbuf->wIdx + rbuf->size - rbuf->rIdx);
    return len;
}

unsigned int rbufReadData(RBuf_t *rbuf, unsigned char *buf, unsigned int len)
{
    unsigned int i, rlen = rbuf->size - rbufFreeBuf(rbuf);
    if(rlen > len) rlen = len;
    for(i = 0; i < rlen; i++) {
        buf[i] = rbuf->buf[rbuf->rIdx++];
        if(rbuf->rIdx == rbuf->size) rbuf->rIdx = 0;
    }
    return rlen;
}

unsigned char rbufHeadData(RBuf_t *rbuf)
{
    return rbuf->buf[rbuf->rIdx];
}

unsigned char rbufData(RBuf_t *rbuf, unsigned int pos)
{
    return rbuf->buf[(rbuf->rIdx + pos) % rbuf->size];
}

unsigned short rbufData2(RBuf_t *rbuf, unsigned int pos)
{
    return ((unsigned short)rbufData(rbuf, pos) << 8) + rbufData(rbuf, pos+1);
}
