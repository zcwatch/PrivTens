#include "board.h"
#include "fw_data.h"

void iap_load_app(void)
{
	uint32_t JumpAddress = *(__IO uint32_t*)(APP_ADDR + 4);
	pFunction JumpToApp;
	
	if(((*(__IO uint32_t*)APP_ADDR) & 0x2FFE0000) == 0x20000000) {
		rcu_deinit();
		__set_MSP(*(__IO uint32_t*) APP_ADDR);
		SCB->VTOR = APP_ADDR;
		//__disable_irq();
		
		JumpAddress = *(__IO uint32_t*)(APP_ADDR + 4);
		JumpToApp = (pFunction)JumpAddress;
		JumpToApp();
	}
}

int main(void)
{
	hardware_init();
	
	if(fw_read_update_flag()) {
		fw_update();
		//iap_load_app();
		NVIC_SystemReset();
	} else {
		iap_load_app();
		//fw_update();
	}
	return 0;
}

