#include <stdint.h>
#include <string.h>
//#include "stm32h7xx_hal_flash.h"
//#include "stm32h7xx_hal_flash_ex.h"
#include "main.h"
#include "crc.h"
#include "eeprom.h"

#if 1
#define FLASH_EEPROM_BASE_ADDR 0x0801F800//FLASH模拟 最后一页的起始地址
#define FLASH_EEPROM_END_ADDR  0x0801FFFF

//读取指定地址的字(32 位数据)
//faddr:读地址
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(__IO uint32_t *)faddr;
}
//获取某个地址所在的 flash 扇区,仅用于 BANK1！！
//addr:flash 地址
//返回值:0~11,即 addr 所在的扇区
uint16_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1_BANK1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2_BANK1)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3_BANK1)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4_BANK1)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5_BANK1)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6_BANK1)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7_BANK1)return FLASH_SECTOR_6;
	return FLASH_SECTOR_7;
}
//从指定地址开始写入指定长度的数据
//特别注意:因为 STM32H7 的扇区实在太大,没办法本地保存扇区数据,所以本函数
// 写地址如果非 0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
// 写非 0XFF 的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
// 没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
//该函数对 OTP 区域也有效!可以用来写 OTP 区!
//OTP 区域地址范围:0X1FF0F000~0X1FF0F41F
//WriteAddr:起始地址(此地址必须为 4 的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32 位)数(就是要写入的 32 位数据的个数.)
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)
{
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	uint32_t SectorError=0;
	uint32_t addrx=0;
	uint32_t endaddr=0; 
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;  //非法地址
	HAL_FLASH_Unlock(); //解锁 
	addrx=WriteAddr; //写入的起始地址
	endaddr=WriteAddr+NumToWrite*4; //写入的结束地址
	if(addrx<0X1FF00000)
	{
		while(addrx<endaddr) //扫清一切障碍.(对非 FFFFFFFF 的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)  //要擦除这个扇区
			{
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS; //擦除类型
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);//要擦除的扇区
				FlashEraseInit.Banks=FLASH_BANK_1;  //操作 BANK1
				FlashEraseInit.NbSectors=1; //一次只擦除一个扇区
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3; //电压范围
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK)
				{
					break;  //发生错误了
				}
				SCB_CleanInvalidateDCache(); //清除无效的 D-Cache
			}else addrx+=4;
			FLASH_WaitForLastOperation(FLASH_WAITETIME,FLASH_BANK_1);
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME,FLASH_BANK_1);
	if(FlashStatus==HAL_OK)
	{
		while(WriteAddr<endaddr)//写数据
	{
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,WriteAddr,
		(uint64_t)pBuffer)!=HAL_OK)//写入数据
	{
		break;  //写入异常
	}
		WriteAddr+=32;
		pBuffer+=8;
	}
	}
	HAL_FLASH_Lock(); //上锁
}
//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32 位)数
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)
{
	uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取 4 个字节.
		ReadAddr+=4;//偏移 4 个字节. 
	}
}	

#endif
#if 0
uint8_t device_config_save(void)
{
	uint16_t i = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct = {
	.TypeErase = FLASH_TYPEERASE_PAGES,
	.Page = 63,
	.NbPages = 1
    };
    
	HAL_FLASH_Unlock();
	{       
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
        uint32_t PageError = 0;
         if (HAL_FLASHEx_Erase(&EraseInitStruct,&PageError) != HAL_OK)
        {
            return 1;
        }
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)FLASH_EEPROM_BASE_ADDR, sampleNumber))
        {
            return 1;
        }                 
	}
	HAL_FLASH_Lock();
	
	return 0; 
}

uint8_t device_config_load(void)
{      
	HAL_FLASH_Unlock();
	{
        sampleNumber = *((uint16_t *)((volatile uint8_t *)FLASH_EEPROM_BASE_ADDR));
	}
	HAL_FLASH_Lock();
    
	return 0; 
}
#endif

uint8_t MPU_Set_Protection(uint32_t baseaddr,uint32_t size,uint32_t rnum,uint8_t de,uint8_t ap,uint8_t sen,uint8_t cen,uint8_t ben)
{
MPU_Region_InitTypeDef MPU_Initure;
HAL_MPU_Disable(); //配置 MPU 之前先关闭 MPU,配置完成以后在使能 MPU
MPU_Initure.Enable=MPU_REGION_ENABLE; //使能该保护区域
MPU_Initure.Number=rnum; //设置保护区域
MPU_Initure.BaseAddress=baseaddr; //设置基址
MPU_Initure.Size=size; //设置保护区域大小
MPU_Initure.SubRegionDisable=0X00; //禁止子区域
MPU_Initure.TypeExtField=MPU_TEX_LEVEL0; //设置类型扩展域为 level0
MPU_Initure.AccessPermission=(uint8_t)ap;  //设置访问权限,
MPU_Initure.DisableExec=de;  //允许指令访问(允许读取指令)
MPU_Initure.IsShareable=sen; //是否共用?
MPU_Initure.IsCacheable=cen; //是否 cache?
MPU_Initure.IsBufferable=ben; //是否缓冲?
HAL_MPU_ConfigRegion(&MPU_Initure); //配置 MPU
HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT); //开启 MPU
return 0;
}
