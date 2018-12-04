
enum {
    RDSPLOAD_HANDLE,
    RDSPLOAD_INDEX,
    __RDSPLOAD_MAX
};

static const struct blobmsg_policy policy_rdspload[__RDSPLOAD_MAX] = {
    [RDSPLOAD_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [RDSPLOAD_INDEX] = { .name = "index", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_rdspload(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDSPLOAD_MAX];
	unsigned short handle = 0;
	short index = -1;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ODBSPN data;

	blobmsg_parse(policy_rdspload, __RDSPLOAD_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDSPLOAD_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDSPLOAD_HANDLE]);
	if (handle <= 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}
	if (tb[RDSPLOAD_INDEX])
		index = blobmsg_get_u32(tb[RDSPLOAD_INDEX]);

	ret = cnc_rdspload(handle, index, &data);

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

