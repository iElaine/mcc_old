/**
 * @file mccsystem.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __MCCSYSTEM_H__
#define __MCCSYSTEM_H__
class MCCSystem {
	public:
		/**
		 * @brief constructor
		 */
		MCCSystem();
		/**
		 * @brief destructor
		 */
		virtual ~MCCSystem();
		static int Init();
		static int UnInit();
};
#endif /* __MCCSYSTEM_H__ */
