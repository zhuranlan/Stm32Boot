#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"  
#include "boot.h"

void TIM2_Base_Init(u16 Count); 

int main(void)
{
//	/*TEST*/
//  Set_APP1_Flag();
//	Set_APP2_Flag();
//	/*TEST*/
//	Wipe_APP1_Flag();
//	Wipe_APP2_Flag();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	
	delay_init();
	
	uart_init(115200);
	
//	Start_load();//开机进入BOOT引导
	TIM2_Base_Init(10000);//1s 一次中断 验证 中断向量表是否正确
	printf("This is APP1 !! \r\n");
	
//	{
//	u8 bindata[10]={1,2,3,4,5,6,7,8,9,0};
//	u8 bindata1[10]={0,9,8,7,6,5,4,3,2,1};	
//	/*测试读写*/
//	write_bin(10,bindata1,10,1);//测试
//	write_bin(0,bindata1,10,1);//测试
//	
//	/*测试读写*/
//	write_bin(10,bindata1,10,2);//测试
//	write_bin(0,bindata1,10,2);//测试
//  }
	
	while(1)
	{
		
	}
}


/*-------------------------------------------------------------------------------
程序名称：TIM2_Base_Init
程序描述：定时器TIM2通用定时功能
输入参数：定时器重装值
返回参数：无
备    注：无
---------------------------------------------------------------------------------*/
void TIM2_Base_Init(u16 Count)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = Count-1;  //重装值，
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //分频系数，72M/7200=10KHz,其他依此类推
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     //把上述数值写入对应寄存器
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);            //使能或者失能指定的TIM中断
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                 //把上述数值写入对应寄存

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
							 
}


