#include "hv.h"
#include "board.h"

unsigned short dacLevel[HV_MAXLEVEL] = {
    134, 204, 274, 344, 414, 484, 554, 624, 694, 764, 834, 904, 974, 1044, 1114, 1184, 1254, 1324, 1394, 1464, 1534, 1604, 1674, 1744, 1814, 1894, 1964, 2034, 2104, 2234
};

void hvShutdown(void)
{
    halHVEnable(0);
    halSetDac1(0);
}

void hvSetLevel(unsigned char level)
{
    if(level == 0) {
        halHVEnable(0);
        halSetDac1(0);
    } else {
        halHVEnable(1);
        halSetDac1(dacLevel[level - 1]);
    }   
}
