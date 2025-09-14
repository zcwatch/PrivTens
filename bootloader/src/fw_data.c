#include "fw_data.h"
#include "board.h"


//读取固件升级标志
unsigned char fw_read_update_flag(void)
{
	uint16_t flag =  *(uint16_t*)(FW_ADDR);
	if(flag == FW_MARK) return 1;
	else return 0;
}

int fw_update(void)
{
	uint32_t len = *(uint32_t*)(FW_ADDR + 2);
	uint32_t addr, wlen, i, crc1, crc = 0;
	uint16_t data;
	
	crc1 = *(uint32_t*)(FW_ADDR + len + 2);
	for(i = 0; i < len - 4; i++) {
		crc += *(uint8_t*)(FW_ADDR + 6 + i);
	}
	
	if(crc1 != crc) {
		fmc_unlock();
		fmc_halfword_program(FW_ADDR, 0);
		fmc_lock();
		return 0;	//check CRC
	}
	
	//erase flash
	fmc_unlock();
	for(i = 0; i < FW_SIZE; i++) {
		feed_dog();
		addr = APP_ADDR + SECTOR_SIZE * i;
		fmc_page_erase(addr);
	}
	//write fw data
	wlen = len - 4;
	wlen = (wlen % 2 == 0) ? (wlen / 2) : (wlen / 2 + 1);
	addr = APP_ADDR;
	for(i = 0; i < wlen; i++) {
		feed_dog();
		data = *(uint16_t*)(FW_ADDR + 6 + i * 2);
		fmc_halfword_program(addr, data);
		addr += 2;
	}
	//clear flag
	fmc_halfword_program(FW_ADDR, 0);
	fmc_lock();
	return 1;
}

	
