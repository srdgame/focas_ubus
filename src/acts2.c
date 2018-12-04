
enum {
    ACTS2_HANDLE,
    ACTS2_INDEX,
    __ACTS2_MAX
};

static const struct blobmsg_policy policy_acts2[__ACTS2_MAX] = {
    [ACTS2_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [ACTS2_INDEX] = { .name = "index", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_acts2(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__ACTS2_MAX];
	unsigned short handle = 0;
	short index = -1;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ODBACT2 data;

	blobmsg_parse(policy_acts2, __ACTS2_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[ACTS2_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[ACTS2_HANDLE]);

	if (tb[ACTS2_INDEX])
		index = blobmsg_get_u32(tb[ACTS2_INDEX]);

	ret = cnc_acts2(handle, index, &data);

	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "index", data.datano);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < MAX_SPINDLE; ++i) {
		blobmsg_add_u32(&b, NULL, data.data[i]);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

