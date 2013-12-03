/**
 * @file sipua.cpp
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#include <pjsua.h>
#include "sipua.h"
#include "sipeventhandler.h"
#define THIS_FILE "sipua.cpp"

class _internal_sip_data {
	public:
		pjsua_config			ua_cfg;
		pjsua_acc_config		acc_cfg;
		pjsua_logging_config	log_cfg;
		pjsua_media_config		media_cfg;
		pjsua_transport_id		transport_id;
		pjsua_transport_config	udp_cfg;
		pjsua_acc_id			local_aid;
		pjsua_acc_id			user_aid;
};

static SipEventHandler default_sip_event_handler;

SipUA *SipUA::m_instance = NULL;
//-------------------------------
//  internal callbacks
//


void on_call_state(pjsua_call_id call_id, pjsip_event *e) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_state(call_id, e);
}

void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
		pjsip_rx_data *rdata) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_incoming_call(acc_id, call_id, rdata);
}

void on_call_tsx_state(pjsua_call_id call_id,
		pjsip_transaction *tsx,
		pjsip_event *e) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_tsx_state(call_id, tsx, e);
}

void on_call_media_state(pjsua_call_id call_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_media_state(call_id);
}

void on_call_sdp_created(pjsua_call_id call_id,
		pjmedia_sdp_session *sdp,
		pj_pool_t *pool,
		const pjmedia_sdp_session *rem_sdp) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_sdp_created(call_id, sdp, pool, rem_sdp);
}



void on_stream_created(pjsua_call_id call_id,
		pjmedia_stream *strm,
		unsigned stream_idx,
		pjmedia_port **p_port) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_stream_created(call_id, strm, stream_idx, p_port);
}


void on_stream_destroyed(pjsua_call_id call_id,
		pjmedia_stream *strm,
		unsigned stream_idx) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_stream_destroyed(call_id, strm, stream_idx);
}

void on_dtmf_digit(pjsua_call_id call_id, int digit) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_dtmf_digit(call_id, digit);
}

void on_call_transfer_request(pjsua_call_id call_id,
		const pj_str_t *dst,
		pjsip_status_code *code) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_transfer_request(call_id, dst, code);
}

void on_call_transfer_request2(pjsua_call_id call_id,
		const pj_str_t *dst,
		pjsip_status_code *code,
		pjsua_call_setting *opt) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_transfer_request2(call_id, dst, code, opt);
}

void on_call_transfer_status(pjsua_call_id call_id,
		int st_code,
		const pj_str_t *st_text,
		pj_bool_t final,
		pj_bool_t *p_cont) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_transfer_status(call_id, st_code, st_text,  final, p_cont);
}

void on_call_replace_request(pjsua_call_id call_id,
		pjsip_rx_data *rdata,
		int *st_code,
		pj_str_t *st_text) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_replace_request(call_id, rdata, st_code, st_text);
}


void on_call_replace_request2(pjsua_call_id call_id,
		pjsip_rx_data *rdata,
		int *st_code,
		pj_str_t *st_text,
		pjsua_call_setting *opt) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_replace_request2(call_id, rdata, st_code, st_text, opt);
}


void on_call_replaced(pjsua_call_id old_call_id,
		pjsua_call_id new_call_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_replaced(old_call_id, new_call_id);
}


void on_call_rx_offer(pjsua_call_id call_id,
		const pjmedia_sdp_session *offer,
		void *reserved,
		pjsip_status_code *code,
		pjsua_call_setting *opt) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_rx_offer(call_id, offer, reserved, code, opt);
}

void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_reg_started(acc_id, renew);;
}


void on_reg_state(pjsua_acc_id acc_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_reg_state(acc_id);
}


void on_reg_state2(pjsua_acc_id acc_id, pjsua_reg_info *info) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_reg_state2(acc_id, info);
}


void on_incoming_subscribe(pjsua_acc_id acc_id,
		pjsua_srv_pres *srv_pres,
		pjsua_buddy_id buddy_id,
		const pj_str_t *from,
		pjsip_rx_data *rdata,
		pjsip_status_code *code,
		pj_str_t *reason,
		pjsua_msg_data *msg_data) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_incoming_subscribe(acc_id, srv_pres, buddy_id, from, rdata, code, reason, msg_data);
}


void on_srv_subscribe_state(pjsua_acc_id acc_id,
		pjsua_srv_pres *srv_pres,
		const pj_str_t *remote_uri,
		pjsip_evsub_state state,
		pjsip_event *event) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_srv_subscribe_state(acc_id, srv_pres, remote_uri, state, event);
}


void on_buddy_state(pjsua_buddy_id buddy_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_buddy_state(buddy_id);
}


void on_buddy_evsub_state(pjsua_buddy_id buddy_id,
		pjsip_evsub *sub,
		pjsip_event *event) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_buddy_evsub_state(buddy_id, sub, event);
}

void on_pager(pjsua_call_id call_id, const pj_str_t *from,
		const pj_str_t *to, const pj_str_t *contact,
		const pj_str_t *mime_type, const pj_str_t *body) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_pager(call_id, from, to, contact, mime_type, body);
}

void on_pager2(pjsua_call_id call_id, const pj_str_t *from,
		const pj_str_t *to, const pj_str_t *contact,
		const pj_str_t *mime_type, const pj_str_t *body,
		pjsip_rx_data *rdata, pjsua_acc_id acc_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_pager2(call_id, from, to, contact, mime_type, body, rdata, acc_id);
}

void on_pager_status(pjsua_call_id call_id,
		const pj_str_t *to,
		const pj_str_t *body,
		void *user_data,
		pjsip_status_code status,
		const pj_str_t *reason) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_pager_status(call_id, to, body, user_data, status, reason);
}

void on_pager_status2(pjsua_call_id call_id,
		const pj_str_t *to,
		const pj_str_t *body,
		void *user_data,
		pjsip_status_code status,
		const pj_str_t *reason,
		pjsip_tx_data *tdata,
		pjsip_rx_data *rdata,
		pjsua_acc_id acc_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_pager_status2(call_id, to, body, user_data, status, reason, tdata, rdata, acc_id);
}

void on_typing(pjsua_call_id call_id, const pj_str_t *from,
		const pj_str_t *to, const pj_str_t *contact,
		pj_bool_t is_typing) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_typing(call_id, from, to, contact, is_typing);
}

void on_typing2(pjsua_call_id call_id, const pj_str_t *from,
		const pj_str_t *to, const pj_str_t *contact,
		pj_bool_t is_typing, pjsip_rx_data *rdata,
		pjsua_acc_id acc_id) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_typing2(call_id, from, to, contact, is_typing, rdata, acc_id);
}

void on_nat_detect(const pj_stun_nat_detect_result *res) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_nat_detect(res);
}

pjsip_redirect_op on_call_redirected(pjsua_call_id call_id, 
		const pjsip_uri *target,
		const pjsip_event *e) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	return handle->on_call_redirected(call_id, target, e);
}

void on_mwi_state(pjsua_acc_id acc_id, pjsip_evsub *evsub) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_mwi_state(acc_id, evsub);
}

void on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_mwi_info(acc_id, mwi_info);
}

void on_transport_state( pjsip_transport *tp,
		pjsip_transport_state state,
		const pjsip_transport_state_info *info) {
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_transport_state(tp, state, info);
}

pj_status_t on_call_media_transport_state(pjsua_call_id call_id,
		const pjsua_med_tp_state_info *info) {
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	return handle->on_call_media_transport_state(call_id , info);
}

void on_ice_transport_error(int index, pj_ice_strans_op op,
		pj_status_t status, void *param) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_ice_transport_error(index, op, status, param);
}

pj_status_t on_snd_dev_operation(int operation) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	return handle->on_snd_dev_operation(operation);
}

void on_call_media_event(pjsua_call_id call_id,
		unsigned med_idx,
		pjmedia_event *event) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	handle->on_call_media_event(call_id, med_idx, event);
}

pjmedia_transport* on_create_media_transport(pjsua_call_id call_id,
		unsigned media_idx,
		pjmedia_transport *base_tp,
		unsigned flags) { 
	SipEventHandler *handle = SipUA::GetInstance()->GetHandler();
	return handle->on_create_media_transport(call_id, media_idx, base_tp, flags);
}

//--------------------------------------
//		implement
//

/**
 * @brief constructor
 */
SipUA::SipUA()
{
	sys_var = new _internal_sip_data();
}

/**
 * @brief destructor
 */
SipUA::~SipUA()
{
	pjsua_destroy();
}

int SipUA::Init(SipEventHandler *handle)
{
	pj_status_t status;
	if(handle == NULL)
		m_handle = &default_sip_event_handler;
	status = pjsua_create();
	if(status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
		return status;
	}
	pjsua_config_default(&(sys_var->ua_cfg));
	// init callback
	// ...
	/* call related handle */
	sys_var->ua_cfg.cb.on_call_state					= on_call_state;
	sys_var->ua_cfg.cb.on_incoming_call                = on_incoming_call;
	sys_var->ua_cfg.cb.on_call_tsx_state               = on_call_tsx_state;
	sys_var->ua_cfg.cb.on_call_media_state             = on_call_media_state;
	sys_var->ua_cfg.cb.on_call_sdp_created             = on_call_sdp_created;
	sys_var->ua_cfg.cb.on_stream_created               = on_stream_created;
	sys_var->ua_cfg.cb.on_stream_destroyed             = on_stream_destroyed;
	sys_var->ua_cfg.cb.on_dtmf_digit                   = on_dtmf_digit;
	sys_var->ua_cfg.cb.on_call_transfer_request        = on_call_transfer_request;
	sys_var->ua_cfg.cb.on_call_transfer_request2       = on_call_transfer_request2;
	sys_var->ua_cfg.cb.on_call_transfer_status         = on_call_transfer_status;
	sys_var->ua_cfg.cb.on_call_replace_request         = on_call_replace_request;
	sys_var->ua_cfg.cb.on_call_replace_request2        = on_call_replace_request2;
	sys_var->ua_cfg.cb.on_call_replaced                = on_call_replaced;
	sys_var->ua_cfg.cb.on_call_rx_offer                = on_call_rx_offer;
	/* register handle */
	sys_var->ua_cfg.cb.on_reg_started                  = on_reg_started;
	sys_var->ua_cfg.cb.on_reg_state                    = on_reg_state;
	sys_var->ua_cfg.cb.on_reg_state2                   = on_reg_state2;
	/* subscirbe handle */
	sys_var->ua_cfg.cb.on_incoming_subscribe           = on_incoming_subscribe;
	sys_var->ua_cfg.cb.on_srv_subscribe_state          = on_srv_subscribe_state;
	sys_var->ua_cfg.cb.on_buddy_state                  = on_buddy_state;
	sys_var->ua_cfg.cb.on_buddy_evsub_state            = on_buddy_evsub_state;
	/* message handle */
	sys_var->ua_cfg.cb.on_pager                        = on_pager;
	sys_var->ua_cfg.cb.on_pager2                       = on_pager2;
	sys_var->ua_cfg.cb.on_pager_status                 = on_pager_status;
	sys_var->ua_cfg.cb.on_pager_status2                = on_pager_status2;
	sys_var->ua_cfg.cb.on_typing                       = on_typing;
	sys_var->ua_cfg.cb.on_typing2                      = on_typing2;
	/* network handle */
	sys_var->ua_cfg.cb.on_nat_detect                   = on_nat_detect;
	sys_var->ua_cfg.cb.on_call_redirected              = on_call_redirected;
	sys_var->ua_cfg.cb.on_mwi_state                    = on_mwi_state;
	sys_var->ua_cfg.cb.on_mwi_info                     = on_mwi_info;
	sys_var->ua_cfg.cb.on_transport_state              = on_transport_state;
	sys_var->ua_cfg.cb.on_call_media_transport_state   = on_call_media_transport_state;
	sys_var->ua_cfg.cb.on_ice_transport_error          = on_ice_transport_error;
	sys_var->ua_cfg.cb.on_snd_dev_operation            = on_snd_dev_operation;
	/* media module handle */
	sys_var->ua_cfg.cb.on_call_media_event             = on_call_media_event;
	sys_var->ua_cfg.cb.on_create_media_transport       = on_create_media_transport;
	pjsua_logging_config_default(&(sys_var->log_cfg));
	pjsua_media_config_default(&(sys_var->media_cfg));
	status = pjsua_init(&(sys_var->ua_cfg), 
			&(sys_var->log_cfg), 
			&(sys_var->media_cfg));
	if(status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
		return status;
	}
	// Setup transport
	pjsua_transport_config *transport_cfg = &sys_var->udp_cfg;
	pjsua_transport_config_default(transport_cfg);
	transport_cfg->port = 5060;

	// Setup local account
	pjsip_transport_type_e type = PJSIP_TRANSPORT_UDP;
	status = pjsua_transport_create(type, &sys_var->udp_cfg, &sys_var->transport_id);
	if(status != PJ_SUCCESS)
		return -1;
	pjsua_acc_add_local(sys_var->transport_id, PJ_TRUE, &sys_var->local_aid);
	pjsua_acc_config_default(&sys_var->acc_cfg);
//	pjsua_acc_config *p_acc_cfg = &sys_var->acc_cfg;
	pjsua_acc_set_online_status(sys_var->local_aid, PJ_TRUE);

	return 0;
}

int SipUA::SetAccount(const char *username, const char *password)
{
	return 0;
}

int SipUA::Start()
{
	pj_status_t status;
	status = pjsua_start();
	if(status != PJ_SUCCESS) {
		pjsua_destroy();
		pjsua_perror(THIS_FILE, "Error starting pjsua", status);
		return status;
	}
	return Run(NULL);
}

int SipUA::Stop()
{
	pjsua_destroy();
	return 0;
}

int SipUA::SendRequest()
{
	return 0;
}

int SipUA::SendResponse()
{
	return 0;
}


