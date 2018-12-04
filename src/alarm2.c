
enum {
    ALARM2_HANDLE,
    __ALARM2_MAX
};

static const struct blobmsg_policy policy_alarm2[__ALARM2_MAX] = {
    [ALARM2_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 }
};

static int focas_alarm2(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__ALARM2_MAX];
	unsigned short handle = 0;
	short ret = 0;
	long data;

	blobmsg_parse(policy_alarm2, __ALARM2_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[ALARM2_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[ALARM2_HANDLE]);

	ret = cnc_alarm2(handle, &data);

	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "status", data);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

