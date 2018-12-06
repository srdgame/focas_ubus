
enum {
    RDSVMETER_HANDLE,
    __RDSVMETER_MAX
};

static const struct blobmsg_policy policy_rdsvmeter[__RDSVMETER_MAX] = {
    [RDSVMETER_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
};


#define ADD_LOADELM(NAME, DATA) \
	c2 = blobmsg_open_table(&b, #NAME); \
	blobmsg_add_u32(&b, "data", DATA.data); \
	blobmsg_add_u32(&b, "dec", DATA.dec); \
	blobmsg_add_u32(&b, "unit", DATA.unit); \
	blobmsg_add_u32(&b, "name", DATA.name); \
	blobmsg_add_u32(&b, "suff1", DATA.suff1); \
	blobmsg_add_u32(&b, "suff2", DATA.suff2); \
	blobmsg_add_u32(&b, "reserve", DATA.reserve); \
	blobmsg_close_table(&b, c2); \


static int focas_rdsvmeter(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDSVMETER_MAX];
	unsigned short handle = 0;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ODBSVLOAD data[MAX_AXIS];
	short number = MAX_AXIS;
	memset(data, 0, sizeof(ODBSVLOAD) * MAX_AXIS);

	blobmsg_parse(policy_rdsvmeter, __RDSVMETER_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDSVMETER_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDSVMETER_HANDLE]);

	ret = cnc_rdsvmeter(handle, &number, data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < number; ++i) {
		void* c2 = NULL;
		ADD_LOADELM(svload, data[i].svload);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

