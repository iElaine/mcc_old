/**
 * @file mccglobalid.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCGLOBALID_H__
#define __MCCGLOBALID_H__
#include <ostream>
class MCCGlobalID {
	public:
		/**
		 * @brief ructor
		 */
		MCCGlobalID(int matrix_id, int local_id);
		MCCGlobalID();
		/**
		 * @brief destructor
		 */
		virtual ~MCCGlobalID();
		void set(int matrix_id, int local_id) { mid = matrix_id; lid = local_id;}
		bool operator==(const MCCGlobalID & id) const;
		bool operator!=(const MCCGlobalID & id) const;
		bool operator>=(const MCCGlobalID & id) const;
		bool operator<=(const MCCGlobalID & id) const;
		bool operator>(const MCCGlobalID & id) const;
		bool operator<(const MCCGlobalID & id) const;
		inline int GetMatrixID() const { return mid;}
		inline int GetLocalID() const { return lid;}
		inline int matrixId() const { return mid;}
		inline int localId() const { return lid;}
		static const MCCGlobalID NullID;
	protected:
		int mid;
		int lid;
};
std::ostream &operator<<(std::ostream &os, const MCCGlobalID &id);
#endif /* __MCCGLOBALID_H__ */
