
enum {
    RDEXECPROG_HANDLE,
    RDEXECPROG_LENGTH,
    __RDEXECPROG_MAX
};

static const struct blobmsg_policy policy_rdexecprog[__RDEXECPROG_MAX] = {
    [RDEXECPROG_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
	[RDEXECPROG_LENGTH] = { .name = "length", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_rdexecprog(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDEXECPROG_MAX];
	unsigned short handle = 0;
	unsigned short length = 1024 * 10;
	short blknum = 0;
	short ret = 0;
	char* data = NULL;

	blobmsg_parse(policy_rdexecprog, __RDEXECPROG_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDEXECPROG_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDEXECPROG_HANDLE]);
	if (tb[RDEXECPROG_LENGTH]) {
		length = blobmsg_get_u32(tb[RDEXECPROG_HANDLE]);
	}

	data = (char*)malloc(sizeof(char) * length);
	if (!data) {
		blob_buf_init(&b, 0);
		blobmsg_add_u32(&b, "rc", -1);
		blobmsg_add_string(&b, "message", "No enough memory!");
		ubus_send_reply(ctx, req, b.head);
	}
	ret = cnc_rdexecprog(handle, &length, &blknum, data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "blknum", blknum);
	blobmsg_add_u32(&b, "length", length);
	blobmsg_add_field(&b, BLOBMSG_TYPE_STRING, "data", data, length + 1);
	ubus_send_reply(ctx, req, b.head);

	free(data);

	return 0;
}

