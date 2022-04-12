
enum {
    RDLIFE_HANDLE,
	RDLIFE_GROUP,
    __RDLIFE_MAX
};

static const struct blobmsg_policy policy_rdlife[__RDLIFE_MAX] = {
    [RDLIFE_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [RDLIFE_GROUP] = { .name = "group", .type = BLOBMSG_TYPE_INT16 },
};

static int focas_rdlife(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDLIFE_MAX];
	unsigned short handle = 0;
	unsigned short group = 0;
	short ret = 0;
	ODBTLIFE3 data;
	memset(&data, 0, sizeof(data));

	blobmsg_parse(policy_rdlife, __RDLIFE_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDLIFE_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDLIFE_HANDLE]);
	group = blobmsg_get_u16(tb[RDLIFE_GROUP]);

	ret = cnc_rdlife(handle, (short)group, &data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "datano", data.datano);
	blobmsg_add_u32(&b, "dummy", data.dummy);
	blobmsg_add_u32(&b, "data", data.data);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

