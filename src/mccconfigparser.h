/**
 * @file mccconfigparser.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCCONFIGPARSER_H__
#define __MCCCONFIGPARSER_H__
class MCCConfigParser {
	public:
		/**
		 * @brief constructor
		 */
		MCCConfigParser(int argc, char *argv[], const char *filename) {};
		/**
		 * @brief destructor
		 */
		int Parse() {};
		virtual ~MCCConfigParser() {};
		int getConfigInt(const char *name, int def_val) {};
		double getConfigDouble(const char *name, double def_val) {};
		char * getConfigStr(const char *name, char *def_val) {};
	private:
		class ConfigItem {
			const char *name;
			const char *value;
		};
		inline char *getline();
		int find(const char *key, int len, const char *buf, int buf_len, int &offset = 0) 
		{
			int i = 0, j = 0;
			char *p = buf + offset;
			while(*(p+len) != 0)
			{
				if(*p = key[0])
				{
					for(i=0;i<len;i++)
					{
						if(key[i] != p[i])
							break;
					}
					if(key[i] == p[i])
					{
						offset = p - buf;
						return 1;
					}
				}
				p++;
			}
			return 0;
		}
		char *buf_arg
		char *buf_cfgfile;
};
#endif /* __MCCCONFIGPARSER_H__ */
