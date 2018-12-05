
enum {
    AXIS_HANDLE,
	AXIS_FUNCTION,
    AXIS_INDEX,
    AXIS_INDEX2,
    __AXIS_MAX
};

static const struct blobmsg_policy policy_axis[__AXIS_MAX] = {
    [AXIS_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
    [AXIS_FUNCTION] = { .name = "function", .type = BLOBMSG_TYPE_STRING },
    [AXIS_INDEX] = { .name = "index", .type = BLOBMSG_TYPE_INT32 },
    [AXIS_INDEX2] = { .name = "index2", .type = BLOBMSG_TYPE_INT32 },
};

#define MAP_FUNCTION(NAME) \
	if (strcmp(func, #NAME) == 0) { \
		ret = cnc_##NAME(handle, index, sizeof(ODBAXIS), &data); \
	} \

#define MAP_FUNCTION2(NAME) \
	if (strcmp(func, #NAME) == 0) { \
		ret = cnc_##NAME(handle, index, index2, sizeof(ODBAXIS), &data); \
	} \


static int focas_axis(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__AXIS_MAX];
	unsigned short handle = 0;
	char *func = NULL;
	short index = -1;
	short index2 = -1;
	short ret = 0;
	int i = 0;
	void *cookie = NULL;
	ODBAXIS data;

	blobmsg_parse(policy_axis, __AXIS_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[AXIS_HANDLE] || !tb[AXIS_FUNCTION])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[AXIS_HANDLE]);
	if (tb[AXIS_INDEX])
		index = blobmsg_get_u32(tb[AXIS_INDEX]);
	if (tb[AXIS_INDEX2])
		index2 = blobmsg_get_u32(tb[AXIS_INDEX2]);

	MAP_FUNCTION(absolute)
	//MAP_FUNCTION(absolute_bg)
	//MAP_FUNCTION(absolute_mgi)
	MAP_FUNCTION(absolute2)
	MAP_FUNCTION(machine)
	MAP_FUNCTION2(machine2)
	MAP_FUNCTION2(machine3)
	//MAP_FUNCTION(machine_bg)
	MAP_FUNCTION(relative)
	//MAP_FUNCTION(relative_bg)
	MAP_FUNCTION(distance)
	//MAP_FUNCTION(distancem)
	MAP_FUNCTION(skip)
	MAP_FUNCTION(skip)
	MAP_FUNCTION(srvdelay)
	MAP_FUNCTION(accdecdly)
	MAP_FUNCTION(rd5axovrlap)
	MAP_FUNCTION(allowance)
	MAP_FUNCTION(slide)
	MAP_FUNCTION(rdlnzlmcn)

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "index", data.type);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < MAX_AXIS; ++i) {
		blobmsg_add_u32(&b, NULL, data.data[i]);
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

