
enum {
    DISCONNECT_HANDLE,
    __DISCONNECT_MAX
};

static const struct blobmsg_policy policy_disconnect[__DISCONNECT_MAX] = {
    [DISCONNECT_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 }
};

static int focas_disconnect(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__DISCONNECT_MAX];
	unsigned short handle = 0;
	short ret = 0;

	blobmsg_parse(policy_disconnect, __DISCONNECT_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[DISCONNECT_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[DISCONNECT_HANDLE]);

	ret = cnc_resetconnect(handle);
	if(ret != EW_OK)
	{
		goto _END;
	}
	ret = cnc_freelibhndl(handle);
_END:
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

