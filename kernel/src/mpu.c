#include <device.h>

#if __MPU_PRESENT && !defined NOMPU

#include <mpu.h>

#define MPU_REGION_COUNT 8

error_t mpu_init(void)
{
	uint32_t tmp;

	/********** check if MPU is supported **********/
	if ( (MPU->TYPE & MPU_TYPE_IREGION_Msk) != 0 ) //IREGION must be 0
		return ERROR_MPU_NOT_SUPPORTED;
	else if ( ((MPU->TYPE & MPU_TYPE_DREGION_Msk)>>MPU_TYPE_DREGION_Pos) != 0x08 ) //DREGION must be 0x08 (= 8 regions)
		return ERROR_MPU_NOT_SUPPORTED;
	else if ( (MPU->TYPE & MPU_TYPE_SEPARATE_Msk) != 0 ) //SEPERATE must be 0
		return ERROR_MPU_NOT_SUPPORTED;

	/********** set default settings **********/

	//CTRL register
	tmp = MPU->CTRL;
	tmp |= MPU_CTRL_PRIVDEFENA_Msk; //enable use of default memory map as background region in priviliged mode
	tmp &= ~(MPU_CTRL_HFNMIENA_Msk | MPU_CTRL_ENABLE_Msk); //MPU is disabled during hard fault, NMI and FAULTMASK handlers; disable MPU
	MPU->CTRL = tmp;

	//disable all regions
	for (size_t i = 0; i < MPU_REGION_COUNT; i++)
	{
		MPU->RNR = i; //set region number
		MPU->RASR &= ~MPU_RASR_ENABLE_Msk; //reset enable bit
	}

	/********** enable MPU **********/
	MPU->CTRL |= MPU_CTRL_ENABLE_Msk;

	__DSB(); //sync store
	__ISB(); //reset pipeline

	return ERROR_NONE;
}

void mpu_deinit(void)
{
	MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;
}

error_t mpu_enableRegion(uint8_t index, MPU_Region* settings)
{
	/********** check arguments **********/
	if (index > MPU_REGION_COUNT-1)
		return ERROR_INVALID_INDEX;

	if (settings->size < 5)
		return ERROR_INVALID_ARGUMENT;

	uint8_t AP;
	if (settings->accessPrivileged == MPU_ACCESS_NO && settings->accessUnpriviliged == MPU_ACCESS_NO)
		AP = 0x00;
	else if (settings->accessPrivileged == MPU_ACCESS_RW && settings->accessUnpriviliged == MPU_ACCESS_NO)
		AP = 0x01;
	else if (settings->accessPrivileged == MPU_ACCESS_RW && settings->accessUnpriviliged == MPU_ACCESS_RO)
		AP = 0x02;
	else if (settings->accessPrivileged == MPU_ACCESS_RW && settings->accessUnpriviliged == MPU_ACCESS_RW)
		AP = 0x03;
	else if (settings->accessPrivileged == MPU_ACCESS_RO && settings->accessUnpriviliged == MPU_ACCESS_NO)
		AP = 0x05;
	else if (settings->accessPrivileged == MPU_ACCESS_RO && settings->accessUnpriviliged == MPU_ACCESS_RO)
		AP = 0x06;
	else
		return ERROR_MPU_INVALID_ACCESS_COMBINATION;

	if (settings->instructionAccessible != false && settings->instructionAccessible != true)
		return ERROR_INVALID_ARGUMENT;

	//check if baseaddress is valid and if it points to internal memory
	bool validAddress = false, isInternal;
	for (size_t i = 0; i < device_memoryMapEntryCount; i++)
	{
		if (settings->baseAddress >= device_memoryMap[i].start && settings->baseAddress <= device_memoryMap[i].end)
		{
			validAddress = true;
			isInternal = device_memoryMap[i].isInternal;
			break;
		}
	}
	if (!validAddress)
		return ERROR_INVALID_ADDRESS;

	/********** disable region ***********/
	MPU->RNR = index; //set region number
	MPU->RASR &= ~MPU_RASR_ENABLE_Msk; //reset enable bit

	/********** set region settings and enable region **********/
	MPU->RBAR = (uint32_t)settings->baseAddress & MPU_RBAR_ADDR_Msk; //set base address

	//set XN Bit if instructionsAccessible is false, set attributtes (AP, normal memory type, shareable, write back if internal memory else write back/write allocate)
	//enable all subregions , set size (size-1), enable region
	MPU->RASR = (settings->instructionAccessible ? 0 : MPU_RASR_XN_Msk) | (AP<<MPU_RASR_AP_Pos) |
				MPU_RASR_C_Msk | (isInternal ? 0 : MPU_RASR_B_Msk) | MPU_RASR_S_Msk |
				((settings->size-1)<<MPU_RASR_SIZE_Pos) | MPU_RASR_ENABLE_Msk;

	__DSB(); //sync store
	__ISB(); //reset pipeline

	return ERROR_NONE;
}

error_t mpu_disableRegion(uint8_t index)
{
	if (index > MPU_REGION_COUNT-1)
		return ERROR_INVALID_INDEX;

	//disable region
	MPU->RNR = index; //set region number
	MPU->RASR &= ~MPU_RASR_ENABLE_Msk; //reset enable bit

	__DSB(); //sync store
	__ISB(); //reset pipeline

	return ERROR_NONE;
}

#endif
