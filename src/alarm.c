
enum {
    ALARM_HANDLE,
    __ALARM_MAX
};

static const struct blobmsg_policy policy_alarm[__ALARM_MAX] = {
    [ALARM_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 }
};

static int focas_alarm(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__ALARM_MAX];
	unsigned short handle = 0;
	short ret = 0;
	ODBALM data;
	memset(&data, 0, sizeof(data));

	blobmsg_parse(policy_alarm, __ALARM_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[ALARM_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[ALARM_HANDLE]);

	ret = cnc_alarm(handle, &data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "status", data.data);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

