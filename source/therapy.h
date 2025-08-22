#ifndef __THERAPY_H
#define __THERAPY_H

/*
    疗法数据存储结构 
        [MARK 2字节] [疗法数据包总长度] [疗法数据包]

    疗法数据包
        [疗法数据包长度 4字节] [疗法数量 1字节] [疗法数据]...
        
    疗法数据
        [疗法编号 4字节] [疗法数据长度 2字节] [通道数 1字节] [通道1类型] [通道1处方数据]...

*/

typedef int(*FlashEraseFunc)(unsigned int addr, unsigned int len);
typedef int(*FlashWriteFunc)(unsigned int addr, unsigned char *data, unsigned int len);

// typedef struct {
//     unsigned short id;
//     unsigned short len;
//     unsigned char *data;
// } Prescription_t;

typedef struct {
    unsigned int id;
    unsigned short len;
    unsigned char chann;
} Therapy_t;

typedef struct {
    unsigned char en;           //  疗法数据是否存在
    unsigned char count;        //  疗法数量
    unsigned short mark;        //  MARK
    unsigned int dataLen;       //  数量长度
    unsigned int dataAddr;      //  数量地址
    unsigned char *therapy;     //  疗法数据地址

    FlashEraseFunc erase;
    FlashWriteFunc write;
} TherapyManager_t;


void tmInit(TherapyManager_t *tm, unsigned int addr, unsigned short mark, FlashEraseFunc erase, FlashWriteFunc write);
void tmSaveTherapy(TherapyManager_t *tm, unsigned char *data, unsigned short len);
unsigned char tmLoadTherapy(TherapyManager_t *tm);
unsigned char* tmFindTherapy(TherapyManager_t *tm, unsigned int id);
unsigned char* tmFindPrescription(TherapyManager_t *tm, unsigned char id, unsigned char chann);




#endif
