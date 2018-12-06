
enum {
    RDPOSITION_HANDLE,
    RDPOSITION_TYPE,
    __RDPOSITION_MAX
};

static const struct blobmsg_policy policy_rdposition[__RDPOSITION_MAX] = {
    [RDPOSITION_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [RDPOSITION_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
};


#define ADD_POSELM(NAME, DATA) \
	c2 = blobmsg_open_table(&b, #NAME); \
	blobmsg_add_u32(&b, "data", DATA.data); \
	blobmsg_add_u32(&b, "dec", DATA.dec); \
	blobmsg_add_u32(&b, "unit", DATA.unit); \
	blobmsg_add_u32(&b, "disp", DATA.disp); \
	blobmsg_add_u32(&b, "name", DATA.name); \
	blobmsg_add_u32(&b, "suff", DATA.suff); \
	blobmsg_close_table(&b, c2); \


static int focas_rdposition(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDPOSITION_MAX];
	unsigned short handle = 0;
	short type = -1;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	short number = MAX_AXIS;
	ODBPOS data[MAX_AXIS];
	memset(data, 0, sizeof(ODBPOS) * MAX_AXIS);

	blobmsg_parse(policy_rdposition, __RDPOSITION_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDPOSITION_HANDLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDPOSITION_HANDLE]);
	if (tb[RDPOSITION_TYPE])
		type = blobmsg_get_u32(tb[RDPOSITION_TYPE]);

	ret = cnc_rdposition(handle, type, &number, data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < number; ++i) {
		void* c2 = NULL;
		ADD_POSELM(abs, data[i].abs);
		ADD_POSELM(mach, data[i].mach);
		ADD_POSELM(rel, data[i].rel);
		ADD_POSELM(dist, data[i].dist);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

