#ifndef __MCCCLIENT_H__
#define __MCCCLIENT_H__

#if !defined(WIN32)
typedef int SOCKET;
typedef int HANDLE;
#endif /* WIN32 */

namespace mcc {
	struct client_packet {
		int type;
		int matrix_id;
		int bind_kb;
		int cid;
		int mid;
		int ptz_type;
		int ptz_start;
		int lumid;
		int rdmid;
	};

	struct client_event {
		int type;
	};

	struct MCCResult {
		int status_code;
		char message[100];
	};

	class MCCClientListener {
	public:
		virtual void onSwitch() = 0;
		virtual void onPtz() = 0;
	};

	class MCCClient {
	public:
		enum PTZType {
			NONE = 0,
			UP = 0x0001,
			DOWN = 0x0002,
			LEFT = 0x0004,
			RIGHT = 0x0008,
			ZOOMIN = 0x0010,
			ZOOMOUT = 0x0020
		};
		MCCClient();
		virtual ~MCCClient();
		bool Connect(const char*, int port, int binded_keyboard_id);
		void disConnect();
		bool isConnected() { return mConnected;}
		int Switch(int matrix_id, int camera_id, int monitor_id, MCCResult &);
		int Switch(int matrix_id, int camera_id, int left_up_monitor_id, int right_down_monitor_id, MCCResult &);
		int Ptz(int type, int matrix_id, int camera_id, bool bstart, MCCResult &result);
		void setListener(MCCClientListener *listener) { mListener = listener;}
	protected:
#ifdef WIN32
		friend unsigned int __stdcall rx_thread(void *param);
#else
		friend void *rx_thread(void *param);
#endif
		int buildPacket() {return 0;};
		volatile bool mConnected;
		volatile bool mRegistered;
		int mBindedId;
		SOCKET mFd;
		MCCClientListener *mListener;
		HANDLE mThread;
	};
};
#endif /* __MCCCLIENT_H__ */

