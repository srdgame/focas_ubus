
enum {
    RDALMMSG2_HANDLE,
    RDALMMSG2_TYPE,
    __RDALMMSG2_MAX
};

static const struct blobmsg_policy policy_rdalmmsg2[__RDALMMSG2_MAX] = {
    [RDALMMSG2_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [RDALMMSG2_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
};

#define ADD_ALMMSG2(DATA) \
	c2 = blobmsg_open_table(&b, NULL); \
	blobmsg_add_u32(&b, "alm_no", DATA.alm_no); \
	blobmsg_add_u32(&b, "type", DATA.type); \
	blobmsg_add_u32(&b, "axis", DATA.axis); \
	blobmsg_add_u32(&b, "dummy", DATA.dummy); \
	blobmsg_add_u32(&b, "msg_len", DATA.msg_len); \
	DATA.alm_msg[DATA.msg_len] = '\0'; \
	blobmsg_add_string(&b, "alm_msg", DATA.alm_msg); \
	blobmsg_close_table(&b, c2); \


static int focas_rdalmmsg2(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDALMMSG2_MAX];
	unsigned short handle = 0;
	short type = 0;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ODBALMMSG2 data[32];
	short number = 32;

	blobmsg_parse(policy_rdalmmsg2, __RDALMMSG2_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDALMMSG2_HANDLE] || !tb[RDALMMSG2_TYPE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDALMMSG2_HANDLE]);
	type = blobmsg_get_u32(tb[RDALMMSG2_TYPE]);
	if (type < 0) {
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	ret = cnc_rdalmmsg2(handle, type, &number, data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < number; ++i) {
		void* c2 = NULL;
		ADD_ALMMSG2(data[i]);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

