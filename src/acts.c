
enum {
    ACTS_HANDLE,
    __ACTS_MAX
};

static const struct blobmsg_policy policy_acts[__ACTS_MAX] = {
    [ACTS_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_acts(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__ACTS_MAX];
	unsigned short handle = 0;
	short ret = 0;
	ODBACT data;

	blobmsg_parse(policy_acts, __ACTS_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[ACTS_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[ACTS_HANDLE]);

	ret = cnc_acts(handle, &data);

	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "data", data.data);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

