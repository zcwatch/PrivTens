#ifndef __TENSCONFIG_H
#define __TENSCONFIG_H

#define FWVERSION						0x10, 0x00, 0x03
#define FWVERSION2					0x10, 0x00, 0x00
#define HWVERSION						0x10, 0x01, 0x00

#define VERSION                 1, 0, 24, 6, 1, 1, 0, 24, 7, 7                  



#define FLASH_SECTOR				2048
#define TENS_DATA_ADDR			((unsigned int)0x08027000)
#define DEV_INFO_ADDR				TENS_DATA_ADDR

#define HPAINT_DATA_SIZE		15
#define HPAINT_DATA_ADDR		TENS_DATA_ADDR + FLASH_SECTOR

#define PRESC_DATA_SIZE			10
#define PRESC_DATA_ADDR			(TENS_DATA_ADDR + FLASH_SECTOR * 16)
#define TPMARK							0xaa55


#define APP_ADDR						((unsigned int)0x08001800)
#define FWFILE_ADDR					((unsigned int)0x08010800)
#define FWFILE_SIZE          30				//60K
#define FWMARK							0xaa55



#endif
