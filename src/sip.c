#include <pjsua.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define THIS_FILE "sip.c"

/* define callbacks */
void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew)
{
	printf("on register started\n");
}
void on_reg_state(pjsua_acc_id acc_id)
{
	printf("on register status\n");
}
void on_reg_state2(pjsua_acc_id acc_id, pjsua_reg_info *info)
{
	printf("on register status 2\n");
}

int main()
{
	/* system variables */
	pj_status_t status;
	pjsua_config			ua_cfg;
	pjsua_acc_config		acc_cfg;
	pjsua_logging_config	log_cfg;
	pjsua_media_config		media_cfg;
	pjsua_transport_config	udp_cfg;
	pjsua_transport_id		transport_id;
	pjsua_acc_id			local_aid;
	pjsua_acc_id			user_aid;
	// used to send message
	pj_str_t		to; 
	pj_str_t		mime_type; 
	pj_str_t		content;
	/**
	 * create pjsua first 
	 */
	status = pjsua_create();
	if(status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
		return status;
	}
	/**
	 * init pjsua framework 
	 */
	/* configuration of ua */
	pjsua_config_default(&(ua_cfg));
	/**
	 * init callback
	 * set callback functions you need
	 * all the prototype of call are declared in pjsua.h
	 */
//	/* call related handle */
//	ua_cfg.cb.on_call_state					  = on_call_state;
//	ua_cfg.cb.on_incoming_call                = on_incoming_call;
//	ua_cfg.cb.on_call_tsx_state               = on_call_tsx_state;
//	ua_cfg.cb.on_call_media_state             = on_call_media_state;
//	ua_cfg.cb.on_call_sdp_created             = on_call_sdp_created;
//	ua_cfg.cb.on_stream_created               = on_stream_created;
//	ua_cfg.cb.on_stream_destroyed             = on_stream_destroyed;
//	ua_cfg.cb.on_dtmf_digit                   = on_dtmf_digit;
//	ua_cfg.cb.on_call_transfer_request        = on_call_transfer_request;
//	ua_cfg.cb.on_call_transfer_request2       = on_call_transfer_request2;
//	ua_cfg.cb.on_call_transfer_status         = on_call_transfer_status;
//	ua_cfg.cb.on_call_replace_request         = on_call_replace_request;
//	ua_cfg.cb.on_call_replace_request2        = on_call_replace_request2;
//	ua_cfg.cb.on_call_replaced                = on_call_replaced;
//	ua_cfg.cb.on_call_rx_offer                = on_call_rx_offer;
//	/* register handle */
	ua_cfg.cb.on_reg_started                  = on_reg_started;
	ua_cfg.cb.on_reg_state                    = on_reg_state;
	ua_cfg.cb.on_reg_state2                   = on_reg_state2;
//	/* subscirbe handle */
//	ua_cfg.cb.on_incoming_subscribe           = on_incoming_subscribe;
//	ua_cfg.cb.on_srv_subscribe_state          = on_srv_subscribe_state;
//	ua_cfg.cb.on_buddy_state                  = on_buddy_state;
//	ua_cfg.cb.on_buddy_evsub_state            = on_buddy_evsub_state;
//	/* message handle */
//	ua_cfg.cb.on_pager                        = on_pager;
//	ua_cfg.cb.on_pager2                       = on_pager2;
//	ua_cfg.cb.on_pager_status                 = on_pager_status;
//	ua_cfg.cb.on_pager_status2                = on_pager_status2;
//	ua_cfg.cb.on_typing                       = on_typing;
//	ua_cfg.cb.on_typing2                      = on_typing2;
//	/* network handle */
//	ua_cfg.cb.on_nat_detect                   = on_nat_detect;
//	ua_cfg.cb.on_call_redirected              = on_call_redirected;
//	ua_cfg.cb.on_mwi_state                    = on_mwi_state;
//	ua_cfg.cb.on_mwi_info                     = on_mwi_info;
//	ua_cfg.cb.on_transport_state              = on_transport_state;
//	ua_cfg.cb.on_call_media_transport_state   = on_call_media_transport_state;
//	ua_cfg.cb.on_ice_transport_error          = on_ice_transport_error;
//	ua_cfg.cb.on_snd_dev_operation            = on_snd_dev_operation;
//	/* media module handle */
//	ua_cfg.cb.on_call_media_event             = on_call_media_event;
//	ua_cfg.cb.on_create_media_transport       = on_create_media_transport;
	/* configure logging */
	pjsua_logging_config_default(&(log_cfg));
	/* configure media */
	pjsua_media_config_default(&(media_cfg));
	status = pjsua_init(&(ua_cfg), 
			&(log_cfg), 
			&(media_cfg));
	if(status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
		return status;
	}

	/**
	 * Setup transport
	 */
	pjsua_transport_config_default(&udp_cfg);
	udp_cfg.port = 5060;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &udp_cfg, &transport_id);
	if(status != PJ_SUCCESS)
		return -1;

	/**
	 * add local account (sip:domain) which handle sip messages that don't matches any
	 * other accounts
	 */
	pjsua_acc_add_local(transport_id, PJ_TRUE, &local_aid);

	/**
	 * add user account and set it to default account 
	 */
	pjsua_acc_config_default(&acc_cfg);
	acc_cfg.id = pj_str("sip:test@192.168.1.197");
	acc_cfg.reg_uri = pj_str("sip:192.168.1.197");
	acc_cfg.reg_timeout = 300;
	acc_cfg.cred_count = 1;
	acc_cfg.cred_info[0].realm = pj_str("192.168.1.197");
	acc_cfg.cred_info[0].scheme = pj_str("digest");
	acc_cfg.cred_info[0].username = pj_str("test");
	acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	acc_cfg.cred_info[0].data = pj_str("test");

	/**
	 * register_on_acc_add is true by default, add:
	 *
	 *		acc_cfg.register_on_acc_add = PJ_FALSE;
	 *
	 * to disable register on add.add later you can call the following 
	 * code to send a REGISTER.
	 *
	 *		pjsua_acc_set_registration(id_of_the_account, PJ_TRUE);
	 *
	 */
	status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &user_aid);
	sleep(2);
	/**
	 * unregister account before exit
	 */
	to = pj_str("sip:manage@192.168.1.197");
	mime_type = pj_str("text/plain");
	content = pj_str("hello");
	pjsua_im_send(user_aid, &to, &mime_type, &content, NULL, NULL);
	sleep(2);
	pjsua_acc_set_registration(user_aid, PJ_FALSE);
	sleep(2);
	return status;

}
