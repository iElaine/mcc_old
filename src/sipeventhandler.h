/**
 * @file sipeventhandler.h
 * @author TommyWang Email<xuchu1986@gmail.com>
 * @brief 
 */
#ifndef __SIPEVENTHANDLER_H__
#define __SIPEVENTHANDLER_H__
#include <pjsua.h>

class SipEventHandler {
	public:
		/**
		 * @brief destructor
		 */
		virtual ~SipEventHandler() {};

		/**
		 * This structure describes application callback to receive various event
		 * notification from PJSUA-API. All of these callbacks are OPTIONAL,
		 * although definitely application would want to implement some of
		 * the important callbacks (such as \a on_incoming_call).
		 */
		/**
		 * Notify application when call state has changed.
		 * Application may then query the call info to get the
		 * detail call states by calling  pjsua_call_get_info() function.
		 *
		 * @param call_id	The call index.
		 * @param e		Event which causes the call state to change.
		 */
		virtual void on_call_state(pjsua_call_id call_id, pjsip_event *e) {}

		/**
		 * Notify application on incoming call.
		 *
		 * @param acc_id	The account which match the incoming call.
		 * @param call_id	The call id that has just been created for
		 *			the call.
		 * @param rdata	The incoming INVITE request.
		 */
		virtual void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
				pjsip_rx_data *rdata) {}

		/**
		 * This is a general notification callback which is called whenever
		 * a transaction within the call has changed state. Application can
		 * implement this callback for example to monitor the state of
		 * outgoing requests, or to answer unhandled incoming requests
		 * (such as INFO) with a final response.
		 *
		 * @param call_id	Call identification.
		 * @param tsx	The transaction which has changed state.
		 * @param e		Transaction event that caused the state change.
		 */
		virtual void on_call_tsx_state(pjsua_call_id call_id,
				pjsip_transaction *tsx,
				pjsip_event *e) {}

		/**
		 * Notify application when media state in the call has changed.
		 * Normal application would need to implement this callback, e.g.
		 * to connect the call's media to sound device. When ICE is used,
		 * this callback will also be called to report ICE negotiation
		 * failure.
		 *
		 * @param call_id	The call index.
		 */
		virtual void on_call_media_state(pjsua_call_id call_id) {}


		/**
		 * Notify application when a call has just created a local SDP (for 
		 * initial or subsequent SDP offer/answer). Application can implement
		 * this callback to modify the SDP, before it is being sent and/or
		 * negotiated with remote SDP, for example to apply per account/call
		 * basis codecs priority or to add custom/proprietary SDP attributes.
		 *
		 * @param call_id	The call index.
		 * @param sdp	The SDP has just been created.
		 * @param pool	The pool instance, application should use this pool
		 *			to modify the SDP.
		 * @param rem_sdp	The remote SDP, will be NULL if local is SDP offerer.
		 */
		virtual void on_call_sdp_created(pjsua_call_id call_id,
				pjmedia_sdp_session *sdp,
				pj_pool_t *pool,
				const pjmedia_sdp_session *rem_sdp) {}


		/**
		 * Notify application when media session is created and before it is
		 * registered to the conference bridge. Application may return different
		 * media port if it has added media processing port to the stream. This
		 * media port then will be added to the conference bridge instead.
		 *
		 * @param call_id	    Call identification.
		 * @param strm	    Media stream.
		 * @param stream_idx    Stream index in the media session.
		 * @param p_port	    On input, it specifies the media port of the
		 *			    stream. Application may modify this pointer to
		 *			    point to different media port to be registered
		 *			    to the conference bridge.
		 */
		virtual void on_stream_created(pjsua_call_id call_id,
				pjmedia_stream *strm,
				unsigned stream_idx,
				pjmedia_port **p_port) {}

		/**
		 * Notify application when media session has been unregistered from the
		 * conference bridge and about to be destroyed.
		 *
		 * @param call_id	    Call identification.
		 * @param strm	    Media stream.
		 * @param stream_idx    Stream index in the media session.
		 */
		virtual void on_stream_destroyed(pjsua_call_id call_id,
				pjmedia_stream *strm,
				unsigned stream_idx) {}

		/**
		 * Notify application upon incoming DTMF digits.
		 *
		 * @param call_id	The call index.
		 * @param digit	DTMF ASCII digit.
		 */
		virtual void on_dtmf_digit(pjsua_call_id call_id, int digit) {}

		/**
		 * Notify application on call being transfered (i.e. REFER is received).
		 * Application can decide to accept/reject transfer request
		 * by setting the code (default is 202). When this callback
		 * is not defined, the default behavior is to accept the
		 * transfer. See also on_call_transfer_request2() callback for
		 * the version with \a pjsua_call_setting in the argument list.
		 *
		 * @param call_id	The call index.
		 * @param dst	The destination where the call will be 
		 *			transfered to.
		 * @param code	Status code to be returned for the call transfer
		 *			request. On input, it contains status code 200.
		 */
		virtual void on_call_transfer_request(pjsua_call_id call_id,
				const pj_str_t *dst,
				pjsip_status_code *code) {}

		/**
		 * Notify application on call being transfered (i.e. REFER is received).
		 * Application can decide to accept/reject transfer request
		 * by setting the code (default is 202). When this callback
		 * is not defined, the default behavior is to accept the
		 * transfer.
		 *
		 * @param call_id	The call index.
		 * @param dst	The destination where the call will be 
		 *			transfered to.
		 * @param code	Status code to be returned for the call transfer
		 *			request. On input, it contains status code 200.
		 * @param opt	The current call setting, application can update
		 *			this setting for the call being transfered.
		 */
		virtual void on_call_transfer_request2(pjsua_call_id call_id,
				const pj_str_t *dst,
				pjsip_status_code *code,
				pjsua_call_setting *opt) {}

		/**
		 * Notify application of the status of previously sent call
		 * transfer request. Application can monitor the status of the
		 * call transfer request, for example to decide whether to 
		 * terminate existing call.
		 *
		 * @param call_id	    Call ID.
		 * @param st_code	    Status progress of the transfer request.
		 * @param st_text	    Status progress text.
		 * @param final	    If non-zero, no further notification will
		 *			    be reported. The st_code specified in
		 *			    this callback is the final status.
		 * @param p_cont	    Initially will be set to non-zero, application
		 *			    can set this to FALSE if it no longer wants
		 *			    to receie further notification (for example,
		 *			    after it hangs up the call).
		 */
		virtual void on_call_transfer_status(pjsua_call_id call_id,
				int st_code,
				const pj_str_t *st_text,
				pj_bool_t final,
				pj_bool_t *p_cont) {}

		/**
		 * Notify application about incoming INVITE with Replaces header.
		 * Application may reject the request by setting non-2xx code.
		 * See also on_call_replace_request2() callback for the version
		 * with \a pjsua_call_setting in the argument list.
		 *
		 * @param call_id	    The call ID to be replaced.
		 * @param rdata	    The incoming INVITE request to replace the call.
		 * @param st_code	    Status code to be set by application. Application
		 *			    should only return a final status (200-699).
		 * @param st_text	    Optional status text to be set by application.
		 */
		virtual void on_call_replace_request(pjsua_call_id call_id,
				pjsip_rx_data *rdata,
				int *st_code,
				pj_str_t *st_text) {}

		/**
		 * Notify application about incoming INVITE with Replaces header.
		 * Application may reject the request by setting non-2xx code.
		 *
		 * @param call_id	    The call ID to be replaced.
		 * @param rdata	    The incoming INVITE request to replace the call.
		 * @param st_code	    Status code to be set by application. Application
		 *			    should only return a final status (200-699).
		 * @param st_text	    Optional status text to be set by application.
		 * @param opt	    The current call setting, application can update
		 *			    this setting for the call being replaced.
		 */
		virtual void on_call_replace_request2(pjsua_call_id call_id,
				pjsip_rx_data *rdata,
				int *st_code,
				pj_str_t *st_text,
				pjsua_call_setting *opt) {}

		/**
		 * Notify application that an existing call has been replaced with
		 * a new call. This happens when PJSUA-API receives incoming INVITE
		 * request with Replaces header.
		 *
		 * After this callback is called, normally PJSUA-API will disconnect
		 * \a old_call_id and establish \a new_call_id.
		 *
		 * @param old_call_id   Existing call which to be replaced with the
		 *			    new call.
		 * @param new_call_id   The new call.
		 * @param rdata	    The incoming INVITE with Replaces request.
		 */
		virtual void on_call_replaced(pjsua_call_id old_call_id,
				pjsua_call_id new_call_id) {}


		/**
		 * Notify application when call has received new offer from remote
		 * (i.e. re-INVITE/UPDATE with SDP is received). Application can
		 * decide to accept/reject the offer by setting the code (default
		 * is 200). If the offer is accepted, application can update the 
		 * call setting to be applied in the answer. When this callback is
		 * not defined, the default behavior is to accept the offer using
		 * current call setting.
		 *
		 * @param call_id	The call index.
		 * @param offer	The new offer received.
		 * @param reserved	Reserved param, currently not used.
		 * @param code	Status code to be returned for answering the
		 *			offer. On input, it contains status code 200.
		 *			Currently, valid values are only 200 and 488.
		 * @param opt	The current call setting, application can update
		 *			this setting for answering the offer.
		 */
		virtual void on_call_rx_offer(pjsua_call_id call_id,
				const pjmedia_sdp_session *offer,
				void *reserved,
				pjsip_status_code *code,
				pjsua_call_setting *opt) {}

		/**
		 * Notify application when registration or unregistration has been
		 * initiated. Note that this only notifies the initial registration
		 * and unregistration. Once registration session is active, subsequent
		 * refresh will not cause this callback to be called.
		 *
		 * @param acc_id	    The account ID.
		 * @param renew	    Non-zero for registration and zero for
		 * 			    unregistration.
		 */
		virtual void on_reg_started(pjsua_acc_id acc_id, pj_bool_t renew) {}

		/**
		 * Notify application when registration status has changed.
		 * Application may then query the account info to get the
		 * registration details.
		 *
		 * @param acc_id	    The account ID.
		 */
		virtual void on_reg_state(pjsua_acc_id acc_id) {}

		/**
		 * Notify application when registration status has changed.
		 * Application may inspect the registration info to get the
		 * registration status details.
		 *
		 * @param acc_id	    The account ID.
		 * @param info	    The registration info.
		 */
		virtual void on_reg_state2(pjsua_acc_id acc_id, pjsua_reg_info *info) {}

		/**
		 * Notification when incoming SUBSCRIBE request is received. Application
		 * may use this callback to authorize the incoming subscribe request
		 * (e.g. ask user permission if the request should be granted).
		 *
		 * If this callback is not implemented, all incoming presence subscription
		 * requests will be accepted.
		 *
		 * If this callback is implemented, application has several choices on
		 * what to do with the incoming request:
		 *	- it may reject the request immediately by specifying non-200 class
		 *    final response in the \a code argument.
		 *	- it may immediately accept the request by specifying 200 as the
		 *	  \a code argument. This is the default value if application doesn't
		 *	  set any value to the \a code argument. In this case, the library
		 *	  will automatically send NOTIFY request upon returning from this
		 *	  callback.
		 *  - it may delay the processing of the request, for example to request
		 *    user permission whether to accept or reject the request. In this 
		 *	  case, the application MUST set the \a code argument to 202, and 
		 *	  later calls #pjsua_pres_notify() to accept or reject the 
		 *	  subscription request.
		 *
		 * Any \a code other than 200 and 202 will be treated as 200.
		 *
		 * Application MUST return from this callback immediately (e.g. it must
		 * not block in this callback while waiting for user confirmation).
		 *
		 * @param srv_pres	    Server presence subscription instance. If
		 *			    application delays the acceptance of the request,
		 *			    it will need to specify this object when calling
		 *			    #pjsua_pres_notify().
		 * @param acc_id	    Account ID most appropriate for this request.
		 * @param buddy_id	    ID of the buddy matching the sender of the
		 *			    request, if any, or PJSUA_INVALID_ID if no
		 *			    matching buddy is found.
		 * @param from	    The From URI of the request.
		 * @param rdata	    The incoming request.
		 * @param code	    The status code to respond to the request. The
		 *			    default value is 200. Application may set this
		 *			    to other final status code to accept or reject
		 *			    the request.
		 * @param reason	    The reason phrase to respond to the request.
		 * @param msg_data	    If the application wants to send additional
		 *			    headers in the response, it can put it in this
		 *			    parameter.
		 */
		virtual void on_incoming_subscribe(pjsua_acc_id acc_id,
				pjsua_srv_pres *srv_pres,
				pjsua_buddy_id buddy_id,
				const pj_str_t *from,
				pjsip_rx_data *rdata,
				pjsip_status_code *code,
				pj_str_t *reason,
				pjsua_msg_data *msg_data) {}

		/**
		 * Notification when server side subscription state has changed.
		 * This callback is optional as application normally does not need
		 * to do anything to maintain server side presence subscription.
		 *
		 * @param acc_id	    The account ID.
		 * @param srv_pres	    Server presence subscription object.
		 * @param remote_uri    Remote URI string.
		 * @param state	    New subscription state.
		 * @param event	    PJSIP event that triggers the state change.
		 */
		virtual void on_srv_subscribe_state(pjsua_acc_id acc_id,
				pjsua_srv_pres *srv_pres,
				const pj_str_t *remote_uri,
				pjsip_evsub_state state,
				pjsip_event *event) {}

		/**
		 * Notify application when the buddy state has changed.
		 * Application may then query the buddy into to get the details.
		 *
		 * @param buddy_id	    The buddy id.
		 */
		virtual void on_buddy_state(pjsua_buddy_id buddy_id) {}


		/**
		 * Notify application when the state of client subscription session
		 * associated with a buddy has changed. Application may use this
		 * callback to retrieve more detailed information about the state
		 * changed event.
		 *
		 * @param buddy_id	    The buddy id.
		 * @param sub	    Event subscription session.
		 * @param event	    The event which triggers state change event.
		 */
		virtual void on_buddy_evsub_state(pjsua_buddy_id buddy_id,
				pjsip_evsub *sub,
				pjsip_event *event) {}

		/**
		 * Notify application on incoming pager (i.e. MESSAGE request).
		 * Argument call_id will be -1 if MESSAGE request is not related to an
		 * existing call.
		 *
		 * See also \a on_pager2() callback for the version with \a pjsip_rx_data
		 * passed as one of the argument.
		 *
		 * @param call_id	    Containts the ID of the call where the IM was
		 *			    sent, or PJSUA_INVALID_ID if the IM was sent
		 *			    outside call context.
		 * @param from	    URI of the sender.
		 * @param to	    URI of the destination message.
		 * @param contact	    The Contact URI of the sender, if present.
		 * @param mime_type	    MIME type of the message.
		 * @param body	    The message content.
		 */
		virtual void on_pager(pjsua_call_id call_id, const pj_str_t *from,
				const pj_str_t *to, const pj_str_t *contact,
				const pj_str_t *mime_type, const pj_str_t *body) {}

		/**
		 * This is the alternative version of the \a on_pager() callback with
		 * \a pjsip_rx_data argument.
		 *
		 * @param call_id	    Containts the ID of the call where the IM was
		 *			    sent, or PJSUA_INVALID_ID if the IM was sent
		 *			    outside call context.
		 * @param from	    URI of the sender.
		 * @param to	    URI of the destination message.
		 * @param contact	    The Contact URI of the sender, if present.
		 * @param mime_type	    MIME type of the message.
		 * @param body	    The message content.
		 * @param rdata	    The incoming MESSAGE request.
		 * @param acc_id	    Account ID most suitable for this message.
		 */
		virtual void on_pager2(pjsua_call_id call_id, const pj_str_t *from,
				const pj_str_t *to, const pj_str_t *contact,
				const pj_str_t *mime_type, const pj_str_t *body,
				pjsip_rx_data *rdata, pjsua_acc_id acc_id) {}

		/**
		 * Notify application about the delivery status of outgoing pager
		 * request. See also on_pager_status2() callback for the version with
		 * \a pjsip_rx_data in the argument list.
		 *
		 * @param call_id	    Containts the ID of the call where the IM was
		 *			    sent, or PJSUA_INVALID_ID if the IM was sent
		 *			    outside call context.
		 * @param to	    Destination URI.
		 * @param body	    Message body.
		 * @param user_data	    Arbitrary data that was specified when sending
		 *			    IM message.
		 * @param status	    Delivery status.
		 * @param reason	    Delivery status reason.
		 */
		virtual void on_pager_status(pjsua_call_id call_id,
				const pj_str_t *to,
				const pj_str_t *body,
				void *user_data,
				pjsip_status_code status,
				const pj_str_t *reason) {}

		/**
		 * Notify application about the delivery status of outgoing pager
		 * request.
		 *
		 * @param call_id	    Containts the ID of the call where the IM was
		 *			    sent, or PJSUA_INVALID_ID if the IM was sent
		 *			    outside call context.
		 * @param to	    Destination URI.
		 * @param body	    Message body.
		 * @param user_data	    Arbitrary data that was specified when sending
		 *			    IM message.
		 * @param status	    Delivery status.
		 * @param reason	    Delivery status reason.
		 * @param tdata	    The original MESSAGE request.
		 * @param rdata	    The incoming MESSAGE response, or NULL if the
		 *			    message transaction fails because of time out 
		 *			    or transport error.
		 * @param acc_id	    Account ID from this the instant message was
		 *			    send.
		 */
		virtual void on_pager_status2(pjsua_call_id call_id,
				const pj_str_t *to,
				const pj_str_t *body,
				void *user_data,
				pjsip_status_code status,
				const pj_str_t *reason,
				pjsip_tx_data *tdata,
				pjsip_rx_data *rdata,
				pjsua_acc_id acc_id) {}

		/**
		 * Notify application about typing indication.
		 *
		 * @param call_id	    Containts the ID of the call where the IM was
		 *			    sent, or PJSUA_INVALID_ID if the IM was sent
		 *			    outside call context.
		 * @param from	    URI of the sender.
		 * @param to	    URI of the destination message.
		 * @param contact	    The Contact URI of the sender, if present.
		 * @param is_typing	    Non-zero if peer is typing, or zero if peer
		 *			    has stopped typing a message.
		 */
		virtual void on_typing(pjsua_call_id call_id, const pj_str_t *from,
				const pj_str_t *to, const pj_str_t *contact,
				pj_bool_t is_typing) {}

		/**
		 * Notify application about typing indication.
		 *
		 * @param call_id	    Containts the ID of the call where the IM was
		 *			    sent, or PJSUA_INVALID_ID if the IM was sent
		 *			    outside call context.
		 * @param from	    URI of the sender.
		 * @param to	    URI of the destination message.
		 * @param contact	    The Contact URI of the sender, if present.
		 * @param is_typing	    Non-zero if peer is typing, or zero if peer
		 *			    has stopped typing a message.
		 * @param rdata	    The received request.
		 * @param acc_id	    Account ID most suitable for this message.
		 */
		virtual void on_typing2(pjsua_call_id call_id, const pj_str_t *from,
				const pj_str_t *to, const pj_str_t *contact,
				pj_bool_t is_typing, pjsip_rx_data *rdata,
				pjsua_acc_id acc_id) {}

		/**
		 * Callback when the library has finished performing NAT type
		 * detection.
		 *
		 * @param res	    NAT detection result.
		 */
		virtual void on_nat_detect(const pj_stun_nat_detect_result *res) {}

		/**
		 * This callback is called when the call is about to resend the 
		 * INVITE request to the specified target, following the previously
		 * received redirection response.
		 *
		 * Application may accept the redirection to the specified target 
		 * (the default behavior if this callback is implemented), reject 
		 * this target only and make the session continue to try the next 
		 * target in the list if such target exists, stop the whole
		 * redirection process altogether and cause the session to be
		 * disconnected, or defer the decision to ask for user confirmation.
		 *
		 * This callback is optional. If this callback is not implemented,
		 * the default behavior is to NOT follow the redirection response.
		 *
		 * @param call_id	The call ID.
		 * @param target	The current target to be tried.
		 * @param e		The event that caused this callback to be called.
		 *			This could be the receipt of 3xx response, or
		 *			4xx/5xx response received for the INVITE sent to
		 *			subsequent targets, or NULL if this callback is
		 *			called from within #pjsua_call_process_redirect()
		 *			context.
		 *
		 * @return		Action to be performed for the target. Set this
		 *			parameter to one of the value below:
		 *			- PJSIP_REDIRECT_ACCEPT: immediately accept the
		 *			  redirection (default value). When set, the
		 *			  call will immediately resend INVITE request
		 *			  to the target.
		 *			- PJSIP_REDIRECT_REJECT: immediately reject this
		 *			  target. The call will continue retrying with
		 *			  next target if present, or disconnect the call
		 *			  if there is no more target to try.
		 *			- PJSIP_REDIRECT_STOP: stop the whole redirection
		 *			  process and immediately disconnect the call. The
		 *			  on_call_state() callback will be called with
		 *			  PJSIP_INV_STATE_DISCONNECTED state immediately
		 *			  after this callback returns.
		 *			- PJSIP_REDIRECT_PENDING: set to this value if
		 *			  no decision can be made immediately (for example
		 *			  to request confirmation from user). Application
		 *			  then MUST call #pjsua_call_process_redirect()
		 *			  to either accept or reject the redirection upon
		 *			  getting user decision.
		 */
		virtual pjsip_redirect_op on_call_redirected(pjsua_call_id call_id, 
				const pjsip_uri *target,
				const pjsip_event *e) {
			pjsip_redirect_op *op = new pjsip_redirect_op();
			return *op;
		}

		/**
		 * This callback is called when message waiting indication subscription
		 * state has changed. Application can then query the subscription state
		 * by calling #pjsip_evsub_get_state().
		 *
		 * @param acc_id	The account ID.
		 * @param evsub	The subscription instance.
		 */
		virtual void on_mwi_state(pjsua_acc_id acc_id, pjsip_evsub *evsub) {}

		/**
		 * This callback is called when a NOTIFY request for message summary / 
		 * message waiting indication is received.
		 *
		 * @param acc_id	The account ID.
		 * @param mwi_info	Structure containing details of the event,
		 *			including the received NOTIFY request in the
		 *			\a rdata field.
		 */
		virtual void on_mwi_info(pjsua_acc_id acc_id, pjsua_mwi_info *mwi_info) {}

		/**
		 * This callback is called when transport state is changed. See also
		 * #pjsip_tp_state_callback.
		 */
		virtual void on_transport_state(pjsip_transport *tp, 
				pjsip_transport_state state,
				const pjsip_transport_state_info *info) {}

		/**
		 * This callback is called when media transport state is changed. See
		 * also #pjsua_med_tp_state_cb.
		 */
		virtual pj_status_t on_call_media_transport_state(pjsua_call_id call_id,
				const pjsua_med_tp_state_info *info) {
			return 0;
		}

		/**
		 * This callback is called to report error in ICE media transport.
		 * Currently it is used to report TURN Refresh error.
		 *
		 * @param index	Transport index.
		 * @param op	Operation which trigger the failure.
		 * @param status	Error status.
		 * @param param	Additional info about the event. Currently this will
		 * 			always be set to NULL.
		 */
		virtual void on_ice_transport_error(int index, pj_ice_strans_op op,
				pj_status_t status, void *param) {}

		/**
		 * Callback when the sound device is about to be opened or closed.
		 * This callback will be called even when null sound device or no
		 * sound device is configured by the application (i.e. the
		 * #pjsua_set_null_snd_dev() and #pjsua_set_no_snd_dev() APIs).
		 * This API is mostly useful when the application wants to manage
		 * the sound device by itself (i.e. with #pjsua_set_no_snd_dev()),
		 * to get notified when it should open or close the sound device.
		 *
		 * @param operation	The value will be set to 0 to signal that sound
		 * 			device is about to be closed, and 1 to be opened.
		 *
		 * @return		The callback must return PJ_SUCCESS at the moment.
		 */
		virtual pj_status_t on_snd_dev_operation(int operation) {
			return 0;
		}

		/**
		 * Notification about media events such as video notifications. This
		 * callback will most likely be called from media threads, thus
		 * application must not perform heavy processing in this callback.
		 * Especially, application must not destroy the call or media in this
		 * callback. If application needs to perform more complex tasks to
		 * handle the event, it should post the task to another thread.
		 *
		 * @param call_id	The call id.
		 * @param med_idx	The media stream index.
		 * @param event 	The media event.
		 */
		virtual void on_call_media_event(pjsua_call_id call_id,
				unsigned med_idx,
				pjmedia_event *event) {}

		/**
		 * This callback can be used by application to implement custom media
		 * transport adapter for the call, or to replace the media transport
		 * with something completely new altogether.
		 *
		 * This callback is called when a new call is created. The library has
		 * created a media transport for the call, and it is provided as the
		 * \a base_tp argument of this callback. Upon returning, the callback
		 * must return an instance of media transport to be used by the call.
		 *
		 * @param call_id       Call ID
		 * @param media_idx     The media index in the SDP for which this media
		 *                      transport will be used.
		 * @param base_tp       The media transport which otherwise will be
		 *                      used by the call has this callback not been
		 *                      implemented.
		 * @param flags         Bitmask from pjsua_create_media_transport_flag.
		 *
		 * @return              The callback must return an instance of media
		 *                      transport to be used by the call.
		 */
		virtual pjmedia_transport* on_create_media_transport(pjsua_call_id call_id,
				unsigned media_idx,
				pjmedia_transport *base_tp,
				unsigned flags) {
			return NULL;
		}
};
#endif /* __SIPEVENTHANDLER_H__ */
