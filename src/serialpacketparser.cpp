/**
 * @file serialpacketparser.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include "serialpacketparser.h"

/**
 * @brief constructor
 */
SerialPacketParser::SerialPacketParser(const char *raw, int len)
{
	m_raw = new char[len];
	memcpy(m_raw, raw, len);
	m_len = len;
	m_packetlen = 0;
}

/**
 * @brief destructor
 */
SerialPacketParser::~SerialPacketParser()
{
	if(m_raw)
		delete[] m_raw;
}

int SerialPacketParser::Parse() {
	char *raw = m_raw;
	char *cursor = m_raw;
}

int GetPacketLength() {
	return m_packetlen;
}
