#include "therapy.h"

// 初始化
// 参数：
//      tm: therapy manager 结构体指针
//      addr: 数据存储地址
//      mark: 标记
// 返回：无
void tmInit(TherapyManager_t *tm, unsigned int addr, unsigned short mark, FlashEraseFunc erase, FlashWriteFunc write)
{
    tm->erase = erase;
    tm->write = write;

    tm->en = 0;
    tm->dataAddr = addr;
    tm->mark = mark;
    tm->en = tmLoadTherapy(tm);
}

void tmSaveTherapy(TherapyManager_t *tm, unsigned char *data, unsigned short len)
{
    
}

unsigned char tmLoadTherapy(TherapyManager_t *tm)
{
    unsigned short mark = *(unsigned short*)(tm->dataAddr);
    if(mark!= tm->mark) return 0;
    
    tm->dataLen = ((*(unsigned char*)(tm->dataAddr + 6)) << 24) + ((*(unsigned char*)(tm->dataAddr + 7)) << 16) + ((*(unsigned char*)(tm->dataAddr + 8)) << 8) + *(unsigned char*)(tm->dataAddr + 9);
    tm->count = *(unsigned char*)(tm->dataAddr + 10);
    tm->therapy = (unsigned char*)(tm->dataAddr + 11);
    return 1;
}

// 查找对应ID的疗法
// 参数：
//      tm: therapy manager 结构体指针
//      id: 疗法ID
// 返回：疗法数据指针，失败返回0
unsigned char* tmFindTherapy(TherapyManager_t *tm, unsigned int id)
{
    unsigned char *p, i = 0;
    unsigned int idx = 0;
    unsigned short len;
    
    // ID超出数量
    if(id > tm->count || !tm->en) return 0;

    p = tm->therapy;
    while(idx < id) {
        len = ((*(unsigned char*)(p + 4)) << 8) + *(unsigned char*)(p + 5);
        p += len + 4;
        idx++;
    }
    
    return p;
}

// 查找对应ID疗法对应通道的处方数据
// 参数：
//      tm: therapy manager 结构体指针
//      id: 疗法ID
//      chann: 通道
// 返回：处方数据指针，失败返回0
unsigned char* tmFindPrescription(TherapyManager_t *tm, unsigned char id, unsigned char chann)
{
    unsigned char *t, *p, idx = 0, chanCnt;
    unsigned int tid;
    unsigned short tlen, len;
    
    t = tmFindTherapy(tm, id);
    if(t == 0) return 0;
    
    tlen = ((*(unsigned char*)(t + 4)) << 8) + *(unsigned char*)(t + 5);
    chanCnt = *(unsigned char*)(t + 6);
    if(chann >= chanCnt) return 0;

    p = (unsigned char*)(t + 7);
    while(idx < chann) {
        len = ((*(unsigned char*)(p + 0)) << 8) + *(unsigned char*)(p + 1);
        p += len + 2;
        idx++;
    }
    return p;
}
