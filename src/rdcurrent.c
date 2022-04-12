
enum {
    RDCURRENT_HANDLE,
    __RDCURRENT_MAX
};

static const struct blobmsg_policy policy_rdcurrent[__RDCURRENT_MAX] = {
    [RDCURRENT_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_rdcurrent(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDCURRENT_MAX];
	unsigned short handle = 0;
	short ret = 0;
	short data;
	memset(&data, 0, sizeof(data));

	blobmsg_parse(policy_rdcurrent, __RDCURRENT_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDCURRENT_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDCURRENT_HANDLE]);

	ret = cnc_rdcurrent(handle, &data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "data", data);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

