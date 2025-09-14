#ifndef __TENSDATA_H
#define __TENSDATA_H


typedef struct {
	unsigned char pos;			//position in flash
	unsigned short id;			//prescription id
	unsigned char *data;		//pointer of data
	unsigned short len;			//length of data
} PrescData_t;

typedef struct {
	unsigned char id;
	unsigned char *data;
	unsigned short len;
} HPaintData_t;

extern PrescData_t prescDatas[];

void tensdataInit(void);

void tensdataEraseTherapyFlash(void);

int tensdataSaveTherapyData(unsigned char id, unsigned char *data, int len);
unsigned short tensdataGetPrescDataFromFlash(unsigned char id, unsigned char *data);
void tensdataLoadPrescData(void);
void tensdataEraseFWFlash(void);

#endif
