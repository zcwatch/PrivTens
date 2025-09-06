#include "tensdata.h"
#include "tensconfig.h"
#include "board.h"
#include "prescapi.h"
#include "rbuf.h"

PrescData_t prescDatas[PRESC_DATA_SIZE] = { 0
	// {1, 0, 0, 0}, {2, 0, 0, 0}, {3, 0, 0, 0}, {4, 0, 0, 0}, {5, 0, 0, 0},
	// {6, 0, 0, 0}, {7, 0, 0, 0}, {8, 0, 0, 0}, {9, 0, 0, 0}, {10, 0, 0, 0}
};

int tensdataSaveTherapyData(unsigned char id, unsigned char *data, int len)
{
	
}

int tensdataSavePrescData(unsigned char id, unsigned char *data, int len)
{
	unsigned int addr;
	if((id > PRESC_DATA_SIZE) || (id == 0)) return 0;
	addr = PRESC_DATA_ADDR + FLASH_SECTOR * (id - 1);
	halFlashErasePage(addr);
	halFlashWrite(addr, &id, 1);
	halFlashWrite(addr + 2, (unsigned char*)&len, 2);
	halFlashWrite(addr + 4, data, len);
	tensdataLoadPrescData();
	return 1;
}

int tensdataSavePaintData(unsigned char id, unsigned char *data, int len)
{
	unsigned int addr;
	if((id > HPAINT_DATA_SIZE) || (id == 0)) return 0;
	addr = HPAINT_DATA_ADDR + FLASH_SECTOR * (id - 1);
	halFlashErasePage(addr);
	halFlashWrite(addr, data, len);
	return 1;
}

unsigned short tensdataGetPrescDataFromFlash(unsigned char id, unsigned char *data)
{
	if((id > PRESC_DATA_SIZE) || (id == 0)) return 0;
	if(prescDatas[id - 1].len == 0) return 0;

	if(id != prescDatas[id - 1].pos) return 0;
	data = prescDatas[id - 1].data;
	return prescDatas[id - 1].len;
}

void tensdataLoadPrescData(void)
{
	int i;
	for(i = 0; i < PRESC_DATA_SIZE; i++) {
		prescDatas[i].pos = i + 1;
		if(prescDatas[i].pos == *(unsigned char*)(PRESC_DATA_ADDR + i * FLASH_SECTOR)) {
			prescDatas[i].len = *(unsigned short*)(PRESC_DATA_ADDR + i * FLASH_SECTOR + 2);
			prescDatas[i].data = (unsigned char*)(PRESC_DATA_ADDR + i * FLASH_SECTOR + 4);
			prescDatas[i].id = GETSHORT((PRESC_DATA_ADDR + i * FLASH_SECTOR + 6), 0);
		}
	}
}

void tensdataLoadHPaintData(void)
{
	int i;
	for(i = 0; i < HPAINT_DATA_SIZE; i++) {
		if(paintData[i].id == *(unsigned char*)(HPAINT_DATA_ADDR + i * FLASH_SECTOR)) {
			paintData[i].len = *(unsigned short*)(HPAINT_DATA_ADDR + i * FLASH_SECTOR + 2);
			paintData[i].data = (unsigned char*)(HPAINT_DATA_ADDR + i * FLASH_SECTOR + 4);
		}
	}
}

void tensdataInit(void)
{
	tensdataLoadPrescData();
	tensdataLoadHPaintData();
}


void tensdataEraseFWFlash(void)
{
	unsigned char i;
	unsigned int addr;
	fmc_unlock();
	for(i = 0; i < FWFILE_SIZE; i++) {
		addr = FWFILE_ADDR + i * FLASH_SECTOR;
		fmc_page_erase(addr);
	}
	fmc_lock();
}

void tensdataEraseTherapyFlash(void)
{
	unsigned char i;
	unsigned int addr;
	fmc_unlock();
	for(i = 0; i < PRESC_DATA_SIZE; i++) {
		addr = PRESC_DATA_ADDR + i * FLASH_SECTOR;
		fmc_page_erase(addr);
	}
	fmc_lock();	
}

