
enum {
    RDPRGNUM_HANDLE,
    __RDPRGNUM_MAX
};

static const struct blobmsg_policy policy_rdprgnum[__RDPRGNUM_MAX] = {
    [RDPRGNUM_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_rdprgnum(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDPRGNUM_MAX];
	unsigned short handle = 0;
	short ret = 0;
	ODBPRO data;

	blobmsg_parse(policy_rdprgnum, __RDPRGNUM_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDPRGNUM_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDPRGNUM_HANDLE]);

	ret = cnc_rdprgnum(handle, &data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "data", data.data);
	blobmsg_add_u32(&b, "mdata", data.mdata);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

