#include "button.h"

void buttonInit(Button_t *pb, KeyInfo_t *keys, unsigned char count)
{
	pb->count = count;
	pb->keys = keys;
	buttonReset(pb);
}

void buttonReset(Button_t *pb)
{
	unsigned char i;
	for(i = 0; i < pb->count; i++) {
		pb->keys[i].count = 0;
		pb->keys[i].state = pb->keys[i].readKey();
	}
}

void buttonProcess(Button_t *pb)
{
	unsigned char i;
	pb->changed = 0;
	for(i = 0; i < pb->count; i++) {
		if(pb->keys[i].count < KEY_ANTI) {
			if(pb->keys[i].state != pb->keys[i].readKey()) pb->keys[i].count++;
			else pb->keys[i].count = 0;
		} else if(pb->keys[i].count == KEY_ANTI) {
			if(pb->keys[i].state != pb->keys[i].readKey()) {
				pb->keys[i].state = pb->keys[i].readKey();
				if(pb->keys[i].keyChanged != 0) {
					pb->keys[i].keyChanged(pb->keys[i].state);
					pb->changed = 1;
				}
				if(pb->keys[i].keyPressed == 0) pb->keys[i].count = 0;		//无连按
				else if(pb->keys[i].state == 0) pb->keys[i].count = 0;		//弹起状态
				else pb->keys[i].count++;
			} else pb->keys[i].state = 0;
		} else if(pb->keys[i].count > KEY_ANTI) {
			if(pb->keys[i].readKey() != pb->keys[i].state) {
				pb->keys[i].count = 0;
				continue;
			}
			if(((++pb->keys[i].count - KEY_ANTI) % KEY_REPEAT) == 0) {	//repeat
				pb->keys[i].keyPressed();
			}
		}
	}
}
