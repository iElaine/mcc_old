#ifndef __CLIENT_H__
#define __CLIENT_H__
struct client_packet {
	int type;
	int matrix_id;
	int bind_kb;
	int cid;
	int mid;
	int ptz_type;
	int ptz_start;
};
#endif
