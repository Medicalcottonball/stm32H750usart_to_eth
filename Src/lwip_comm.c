#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h" 
#include "lwip/tcpip.h" 
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//lwip通用驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
   
__lwip_dev lwipdev;						//lwip控制结构体 
struct netif lwip_netif;				//定义一个全局的网络接口

uint32_t TCPTimer=0;			//TCP查询计时器
uint32_t ARPTimer=0;			//ARP查询计时器
uint32_t lwip_localtime;		//lwip本地时间计数器,单位:ms

#if LWIP_DHCP
uint32_t DHCPfineTimer=0;	//DHCP精细处理计时器
uint32_t DHCPcoarseTimer=0;	//DHCP粗糙处理计时器
#endif

//LWIP初始化(LWIP启动的时候使用)
//返回值:0,成功
//      1,内存错误
//      2,LAN8720初始化失败
//      3,网卡添加失败.

//当接收到数据后调用 
void lwip_pkt_handle(void)
{
    //从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
    ethernetif_input(&lwip_netif);
}

//LWIP轮询任务
void lwip_periodic_handle(void)
{
    sys_check_timeouts();	
#if LWIP_DHCP //如果使用DHCP的话
  //每500ms调用一次dhcp_fine_tmr()
  if (lwip_localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  lwip_localtime;
    dhcp_fine_tmr();
    if ((lwipdev.dhcpstatus != 2)&&(lwipdev.dhcpstatus != 0XFF))
    { 
      lwip_dhcp_process_handle();  //DHCP处理
    }
  }

  //每60s执行一次DHCP粗糙处理
  if (lwip_localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
  {
    DHCPcoarseTimer =  lwip_localtime;
    dhcp_coarse_tmr();
  }  
#endif
}


//如果使能了DHCP
#if LWIP_DHCP

//DHCP处理任务
void lwip_dhcp_process_handle(void)
{
	uint32_t ip=0,netmask=0,gw=0;
	switch(lwipdev.dhcpstatus)
	{
		case 0: 	//开启DHCP
			dhcp_start(&lwip_netif);
			lwipdev.dhcpstatus = 1;		//等待通过DHCP获取到的地址
			printf("正在查找DHCP服务器,请稍等...........\r\n");  
			break;
		case 1:		//等待获取到IP地址
		{
			ip=lwip_netif.ip_addr.addr;		//读取新IP地址
			netmask=lwip_netif.netmask.addr;//读取子网掩码
			gw=lwip_netif.gw.addr;			//读取默认网关 
			
			if(ip!=0)			//正确获取到IP地址的时候
			{
				lwipdev.dhcpstatus=2;	//DHCP成功
				printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
				//解析出通过DHCP获取到的IP地址
				lwipdev.ip[3]=(uint8_t)(ip>>24); 
				lwipdev.ip[2]=(uint8_t)(ip>>16);
				lwipdev.ip[1]=(uint8_t)(ip>>8);
				lwipdev.ip[0]=(uint8_t)(ip);
				printf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				//解析通过DHCP获取到的子网掩码地址
				lwipdev.netmask[3]=(uint8_t)(netmask>>24);
				lwipdev.netmask[2]=(uint8_t)(netmask>>16);
				lwipdev.netmask[1]=(uint8_t)(netmask>>8);
				lwipdev.netmask[0]=(uint8_t)(netmask);
				printf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
				//解析出通过DHCP获取到的默认网关
				lwipdev.gateway[3]=(uint8_t)(gw>>24);
				lwipdev.gateway[2]=(uint8_t)(gw>>16);
				lwipdev.gateway[1]=(uint8_t)(gw>>8);
				lwipdev.gateway[0]=(uint8_t)(gw);
				printf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			}else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
			{
				lwipdev.dhcpstatus=0XFF;//DHCP超时失败.
				//使用静态IP地址
				IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
				IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
				printf("DHCP服务超时,使用静态IP地址!\r\n");
				printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
				printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
				printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			}
		}
		break;
		default : break;
	}
}
#endif 



























