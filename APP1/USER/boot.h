#ifndef __BOOT_H__
#define __BOOT_H__
#include "sys.h"
/*stm32f103vet6 flash 空间规划  256K*/
/*
名称                         起始地址
BOOT：         50K           0x8000000
APP1：         100K          0x800C800
APP2：         100K          0x8025800
共享数据区域： 6K            0x803E800
*/
typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
#define BOOT_ADDR 0x8000000
#define BOOT_APP1 0x800C800
#define BOOT_APP2 0x8025800
#define USR_DATA  0x803E800

extern u8 App1_Flag[10];  //0x803E800
extern u8 App2_Flag[10];  //0x803E800+10

void write_bin(u32 offset_addr,void *bin_data , u32 number ,u8 app);
void iap_load_app(u32 appxaddr);
u8 Boot_App1(void);
u8 Boot_App2(void);
u8 Boot_Boot(void);
char Start_load(void);
void Set_APP1_Flag(void);
void Set_APP2_Flag(void);
void Wipe_APP1_Flag(void);
void Wipe_APP2_Flag(void);
void Wipe_App1(void);
void Wipe_App2(void);
#endif
