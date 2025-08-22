#ifndef __BUTTON_H
#define __BUTTON_H

#define KEY_ANTI			2
#define KEY_REPEAT			40

typedef unsigned char(*readKeyFunc)(void);
typedef void(*keyChangedCallbackFunc)(unsigned char state);
typedef void(*keyPressedCallbackFunc)(void);

typedef struct {
	unsigned char state;
	unsigned int count;
	readKeyFunc readKey;
	keyChangedCallbackFunc keyChanged;
	keyPressedCallbackFunc keyPressed;
} KeyInfo_t;

typedef struct {
	unsigned char count;
	unsigned char changed;
	KeyInfo_t *keys;
} Button_t;

void buttonInit(Button_t *pb, KeyInfo_t *keys, unsigned char count);
void buttonReset(Button_t *pb);
void buttonProcess(Button_t *pb);

/* 
* ʹ��˵����
* 1. ������������{״̬, ����, ������ȡ����, ����ֵ�ı�ص�����?, ���������ص�����}
* 2. ��ʼ��
* 3. ÿ10ms����һ��buttonProcess
*/

#endif
