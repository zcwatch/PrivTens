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
		unsigned char *addr = (unsigned char*)(PRESC_DATA_ADDR + i * FLASH_SECTOR);
		prescDatas[i].pos = i + 1;
		if(prescDatas[i].pos == *addr) {
			prescDatas[i].len = *(addr + 2);
			prescDatas[i].data = (addr + 4);
			prescDatas[i].id = GETSHORT((addr + 6), 0);
		}
	}
}

void tensdataInit(void)
{
	tensdataLoadPrescData();
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

