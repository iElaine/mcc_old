/**
 * @file mccglobalid.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "mccglobalid.h"

using namespace std;
const MCCGlobalID MCCGlobalID::NullID = MCCGlobalID(-1, -1);
/**
 * @brief ructor
 */
MCCGlobalID::MCCGlobalID(int matrix_id, int local_id) :
	mid(matrix_id),
	lid(local_id)
{
}

MCCGlobalID::MCCGlobalID() :
	mid(-1),
	lid(-1)
{
}
/**
 * @brief destructor
 */
MCCGlobalID::~MCCGlobalID()
{

}

bool MCCGlobalID::operator==(const MCCGlobalID & id) const
{
	return mid == id.mid && lid == id.lid;
}

bool MCCGlobalID::operator!=(const MCCGlobalID & id) const
{
	return !(*this == id);
}

bool MCCGlobalID::operator>(const MCCGlobalID & id) const
{
	if(mid > id.mid || (mid == id.mid && lid > id.lid))
		return true;
	else 
		return false;
}

bool MCCGlobalID::operator<(const MCCGlobalID & id) const
{
	if(mid < id.mid || (mid == id.mid && lid < id.lid))
		return true;
	else 
		return false;
}
bool MCCGlobalID::operator>=(const MCCGlobalID & id) const
{
	return !(*this < id);
}
bool MCCGlobalID::operator<=(const MCCGlobalID & id) const
{
	return !(*this > id);
}

ostream &operator<<(ostream &os, const MCCGlobalID &id)
{
	os<<"("<<id.GetMatrixID()<<","<<id.GetLocalID()<<")";
	return os;
}
