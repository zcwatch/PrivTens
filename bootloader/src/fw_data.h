#ifndef __FW_DATA_H
#define __FW_DATA_H

#include "gd32f30x.h"

#define SECTOR_SIZE			2048

#define APP_ADDR				((unsigned int)0x08001800)

#define FW_ADDR					((unsigned int)0x08010800)
#define FW_MARK					0xaa55
#define FW_SIZE					30


typedef  void (*pFunction)(void);


unsigned char fw_read_update_flag(void);
int fw_update(void);

#endif
