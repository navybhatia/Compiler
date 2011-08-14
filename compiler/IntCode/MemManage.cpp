#include "MemManage.h"
MemManage::MemManage()
{
	currentMemAddress=0;
}
int MemManage::getAddress()
{
	return currentMemAddress;
}
void MemManage::incrementAddress()
{
	currentMemAddress++;
}
void MemManage::setAddress(int address)
{
	currentMemAddress=address;
}
