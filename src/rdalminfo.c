
enum {
    RDALMINFO_HANDLE,
    RDALMINFO_TYPE,
    __RDALMINFO_MAX
};

static const struct blobmsg_policy policy_rdalminfo[__RDALMINFO_MAX] = {
    [RDALMINFO_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [RDALMINFO_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
};

#define ADD_ALMINFO(DATA) \
	c2 = blobmsg_open_table(&b, NULL); \
	blobmsg_add_u32(&b, "axis", DATA.axis); \
	blobmsg_add_u32(&b, "alm_no", DATA.alm_no); \
	blobmsg_add_u32(&b, "msg_len", DATA.msg_len); \
	DATA.alm_msg[DATA.msg_len] = '\0'; \
	blobmsg_add_string(&b, "alm_msg", DATA.alm_msg); \
	blobmsg_close_table(&b, c2); \


static int focas_rdalminfo(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDALMINFO_MAX];
	unsigned short handle = 0;
	short type = 0;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ALMINFO data[32];
	short number = 32;

	blobmsg_parse(policy_rdalminfo, __RDALMINFO_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDALMINFO_HANDLE] || !tb[RDALMINFO_TYPE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDALMINFO_HANDLE]);
	type = blobmsg_get_u32(tb[RDALMINFO_TYPE]);
	if (type < 0) {
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	ret = cnc_rdalminfo(handle, 1, type, number, data);

	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < number; ++i) {
		void* c1 = NULL;
		void* c2 = NULL;
		c1 = blobmsg_open_array(&b, NULL);
		ADD_ALMINFO(data[i].u.alm2.alm[0]);
		ADD_ALMINFO(data[i].u.alm2.alm[1]);
		ADD_ALMINFO(data[i].u.alm2.alm[2]);
		ADD_ALMINFO(data[i].u.alm2.alm[3]);
		ADD_ALMINFO(data[i].u.alm2.alm[4]);
		blobmsg_close_array(&b, c1);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

