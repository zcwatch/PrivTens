#ifndef __PRESCDATA_H
#define __PRESCDATA_H

#define WAVE_PAINT                  0xe1
#define WAVE_PAINT_SIZE             15

#define WAVE_PULSE					0xf1
#define WAVE_SQUARE                 0xf2
#define WAVE_TRAP                   0xf3
#define WAVE_TRIANGLE               0xf4
#define WAVE_ROUND                  0xf5
#define WAVE_SINE                   0xf6
#define WAVE_EXPO                   0xf7
#define WAVE_LOG                    0xf8
#define WAVE_INTERVAL               0xff

#define AMP_MODE_NO			        0x1
#define AMP_MODE_UP			        0x2
#define AMP_MODE_DOWN		        0x3
#define AMP_MODE_SUP		        0x4
#define AMP_MODE_SDOWN	            0x5

#define PRESC_DATA_LEN              3
#define GROUP_DATA_LEN              6  

#define TIME_RISING 				0					
#define	TIME_HIGH					1					
#define TIME_FALLING				2					
#define TIME_LOW					3


//Paint wave data
typedef struct {
    unsigned char id;               //paint wave data store id
    unsigned char *data;            //paint wave data
    unsigned short len;             //paint wave data length
} PaintData_t;

typedef struct {
    unsigned short time;            //interval time
} WaveInterval_t;

typedef struct {
    unsigned short time[4];         //Rising, high, falling, low
} WaveTrap_t;

typedef struct {
    unsigned char *data;            //paint data
    unsigned short len;             //paint data length
    unsigned short time;            //time
} WavePaint_t;

typedef union  {
    WaveInterval_t interval;        //interval
    WaveTrap_t trap;                //trap wave
    WavePaint_t paint;              //paint wave
} WaveData_t;

typedef struct {
    unsigned char type;             //wave type
    unsigned char freq;             //wave freq
    unsigned char deep:4;           //wave deep
    unsigned char shake:4;          //wave shake
    unsigned char times;            //wave repeat times
    unsigned short interval;        //wave end interval
    WaveData_t data;                //wave data
} Wave_t;

typedef struct {
    unsigned char id;               //group id
    unsigned char mode:4;           //group mode
    unsigned char shake:4;          //group shake
    unsigned char times;            //group repeat times
    unsigned short interval;        //group end interval
    unsigned char count;            //wave count
} WaveGroup_t;

typedef struct {
    unsigned short id;              //prescription id
    unsigned char count;            //wave group count
} Prescription_t;


int decodeWave(Wave_t *wave, unsigned char *data, int len);
int decodeGroup(WaveGroup_t *group, unsigned char *data, int len);
int decodePrescription(Prescription_t *p, unsigned char *data, int len);
int checkPrescription(unsigned char *data, int len);

extern PaintData_t paintData[];

#endif
