#include "sys.h"
#include "stmflash.h"  
#include "boot.h"
#include "usart.h"
u8 App1_Flag[10]={0X00,0X11,0X22,0X33,0X44,0X55,0X66,0X77,0X88,0X99};
u8 App2_Flag[10]={0X99,0X88,0X77,0X66,0X55,0X44,0X33,0X22,0X11,0X00};
/*stm32f103vet6 flash 空间规划  256K*/
/*
名称                         起始地址
BOOT：         50K           0x8000000
APP1：         100K          0x800C800
APP2：         100K          0x8025800
共享数据区域： 6K            0x803E800
*/


/*
void write_bin(u32 offset_addr,u8 *bin_data , u32 number ,u8 app);
升级APP操作
参数说明:
offset_addr： APP bin 偏移地址
bin_data   ： bin数据指针
number     ： 需要写入的bin数据个数
app        ： 1 ：APP1  2：APP2
*/
void write_bin(u32 offset_addr,void *bin_data , u32 number ,u8 app)
{
	u32 addr=0;
	
	if(app==1)
		addr=BOOT_APP1;
	else if(app==2)
		addr=BOOT_APP2;
	
	STMFLASH_Write(addr+offset_addr,bin_data,number/2);
}


#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif	
void Wipe_App1(void)
{
	u32 secpos;	   //扇区地址
	FLASH_Unlock();		
	for(secpos=50/2;secpos<150/2;secpos++)
	FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);
	FLASH_Lock();//上锁
}


void Wipe_App2(void)
{
	u32 secpos;	   //扇区地址
	FLASH_Unlock();		
	for(secpos=150/2;secpos<250/2;secpos++)
	FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);
	FLASH_Lock();//上锁
}



/*引导*/
u8 Boot_App1(void)
{
		if(((*(vu32*)(BOOT_APP1+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{	 
			iap_load_app(BOOT_APP1);//执行FLASH APP代码
			return  1;
		}
    else
		{			
      printf("APP1  引导失败！！ \r\n");	//出现这个错误说明引导失败	
		  return 0;
		}
}



u8 Boot_App2(void)
{
		if(((*(vu32*)(BOOT_APP2+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{	 
			iap_load_app(BOOT_APP2);//执行FLASH APP代码
			return  1;
		}
    else
		{			
      printf("APP2  引导失败！！ \r\n");	//出现这个错误说明引导失败	
		  return 0;
		}
}


u8 Boot_Boot(void)
{
		if(((*(vu32*)(BOOT_APP2+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{	 
			iap_load_app(BOOT_ADDR);//执行FLASH APP代码
			return  1;
		}
    else
		{			
      printf("BOOT  引导失败！！ \r\n");	//出现这个错误说明引导失败	
		  return 0;
		}
}



/*
写入APP1标志
注意:在BOOT里慎用  建议不要在BOOT里使用
程序完成升级后，进入APP1使用  写入标志
*/
void Set_APP1_Flag(void)
{
	write_bin(0x803E800,App1_Flag,10,0);
}



/*
写入APP2标志
注意:在BOOT里慎用  建议不要在BOOT里使用
程序完成升级后，进入APP2使用  写入标志
*/
void Set_APP2_Flag(void)
{
	write_bin(0x803E800+10,App2_Flag,10,0);
}



/*
擦除APP1升级完成标志
*/
void Wipe_APP1_Flag(void)
{
	u8 Temp[10]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
	write_bin(0x803E800,Temp,10,0);
}



/*
擦除APP2升级完成标志
*/
void Wipe_APP2_Flag(void)
{
	u8 Temp[10]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
	write_bin(0x803E800+10,Temp,10,0);
}



/*
BOOT开机验证引导程序
首先验证APP1是否存在其次APP2
*/
char Start_load(void)
{
	u8 app_flag_f,i;
	u16 pBuffer[5];
	STMFLASH_Read(0x803E800,pBuffer,5);
	app_flag_f=0;
	for(i=0;i<10;i++)
	{
	if(((pBuffer[i/2]>>((i%2)*8))&0xFF)!=App1_Flag[i])
		app_flag_f=1;
  }
	if(app_flag_f==0)
	{
		printf("Bootloader App1 !!\r\n");
		Boot_App1();
	}
		else if(app_flag_f==1)
	printf("None App1 !!\r\n");
	
		
	STMFLASH_Read(0x803E800+10,pBuffer,5);
	app_flag_f=0;
	for(i=0;i<10;i++)
	{
	if(((pBuffer[i/2]>>((i%2)*8))&0xFF)!=App2_Flag[i])
		app_flag_f=1;
  }
	if(app_flag_f==0)
	{
		printf("Bootloader App2 !!\r\n");
		Boot_App2();
	}
	else if(app_flag_f==1)
	printf("None App2 !!\r\n");
	
	return 0;
}



//跳转到应用程序段
//appxaddr:用户代码起始地址.
iapfun jump2app; 
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}	
