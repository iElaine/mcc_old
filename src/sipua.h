/**
 * @file sipua.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SIPUA_H__
#define __SIPUA_H__
#include "sipeventhandler.h"
class _internal_sip_data;
class SipUA {
	public:
		/**
		 * @brief constructor
		 */
		/**
		 * @brief destructor
		 */
		virtual ~SipUA();
		int Init(SipEventHandler *handler);
		int Start();
		int Stop();
		int SetAccount(const char *username, const char *password);
		int SendRequest();
		int SendResponse();
		SipEventHandler *GetHandler() const { return m_handle;}
		static SipUA *GetInstance() { 
			if(m_instance == NULL)
				m_instance = new SipUA();
			return m_instance;
		}
	protected:
		SipUA();
		SipUA(const SipUA &) {};
		const SipUA & operator=(const SipUA &a) { *this = a; return *this;}
		virtual int Run(void *param) { return 0;};
		SipEventHandler *m_handle;
		_internal_sip_data *sys_var;
		static SipUA *m_instance;
};
#endif /* __SIPUA_H__ */
