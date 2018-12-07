
enum {
    RDSPMETER_HANDLE,
    RDSPMETER_TYPE,
    __RDSPMETER_MAX
};

static const struct blobmsg_policy policy_rdspmeter[__RDSPMETER_MAX] = {
    [RDSPMETER_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [RDSPMETER_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
};

#define ADD_LOADELM(NAME, DATA) \
	c2 = blobmsg_open_table(&b, #NAME); \
	blobmsg_add_u32(&b, "data", DATA.data); \
	blobmsg_add_u32(&b, "dec", DATA.dec); \
	blobmsg_add_u32(&b, "unit", DATA.unit); \
	memcpy(buf, &DATA.name, 3); \
	blobmsg_add_string(&b, "name", buf); \
	blobmsg_add_u32(&b, "reserve", DATA.reserve); \
	blobmsg_close_table(&b, c2); \


static int focas_rdspmeter(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDSPMETER_MAX];
	unsigned short handle = 0;
	short type = -1;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ODBSPLOAD data[MAX_AXIS];
	short number = MAX_AXIS;

	blobmsg_parse(policy_rdspmeter, __RDSPMETER_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDSPMETER_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDSPMETER_HANDLE]);
	if (tb[RDSPMETER_TYPE])
		type = blobmsg_get_u32(tb[RDSPMETER_TYPE]);

	ret = cnc_rdspmeter(handle, type, &number, data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < number; ++i) {
		void* c2 = NULL;
		char buf[4];
		memset(buf, 0, 4);
		ADD_LOADELM(spload, data[i].spload);
		ADD_LOADELM(spspeed, data[i].spspeed);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

