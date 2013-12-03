/**
 * @file serialpacketparser.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SERIALPACKETPARSER_H__
#define __SERIALPACKETPARSER_H__
class SerialPacketParser {
	public:
		/**
		 * @brief constructor
		 */
		SerialPacketParser(const char *raw, int len);
		/**
		 * @brief destructor
		 */
		virtual ~SerialPacketParser();
		int Parse();
		int GetPacketLength();
	protected:
		int m_len;
		int m_packetlen;
		char *m_raw;
};
#endif /* __SERIALPACKETPARSER_H__ */
