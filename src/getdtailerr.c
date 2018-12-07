
enum {
    GETDTAILERR_HANDLE,
    __GETDTAILERR_MAX
};

static const struct blobmsg_policy policy_getdtailerr[__GETDTAILERR_MAX] = {
    [GETDTAILERR_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_getdtailerr(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__GETDTAILERR_MAX];
	unsigned short handle = 0;
	short ret = 0;
	ODBERR data;
	memset(&data, 0, sizeof(data));

	blobmsg_parse(policy_getdtailerr, __GETDTAILERR_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[GETDTAILERR_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[GETDTAILERR_HANDLE]);

	ret = cnc_getdtailerr(handle, &data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "err_no", data.err_no);
	blobmsg_add_u32(&b, "err_dtno", data.err_dtno);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

