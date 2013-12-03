/**
 * @file abstractdb.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __ABSTRACTDB_H__
#define __ABSTRACTDB_H__
class DBQueryResult {
	int col, row;
	getValueStr(int row, int col);
	getValueInt(int row, int col);
	getValueFlt(int row, int col);
};

class AbstractDB {
	public:
		/**
		 * @brief constructor
		 */
		AbstractDB(const char *host, int port, const char *database, const char *user, const char *password, const char *encode);
		/**
		 * @brief destructor
		 */
		virtual ~AbstractDB();
		virtual int Connect();
		virtual int Execute(const char *sql);
		virtual DBQueryResult &Query(const char *sql);
	protected:
		void *sql_handle;
};
#endif /* __ABSTRACTDB_H__ */
