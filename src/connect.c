
enum {
    CONNECT_IP,
    CONNECT_PORT,
    CONNECT_TIMEOUT,
    __CONNECT_MAX
};

static const struct blobmsg_policy policy_connect[__CONNECT_MAX] = {
    [CONNECT_IP] = { .name = "ip", .type = BLOBMSG_TYPE_STRING },
    [CONNECT_PORT] = { .name = "port", .type = BLOBMSG_TYPE_INT32 },
    [CONNECT_TIMEOUT] = { .name = "timeout", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_connect(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__CONNECT_MAX];
	char *ip = NULL;
	uint32_t port, timeout = 0;
	unsigned short handle = 0;
	short ret = 0;

	blobmsg_parse(policy_connect, __CONNECT_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[CONNECT_IP] || !tb[CONNECT_PORT])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ip = blobmsg_get_string(tb[CONNECT_IP]);
	port = blobmsg_get_u32(tb[CONNECT_PORT]);
	timeout = blobmsg_get_u32(tb[CONNECT_TIMEOUT]);
	if (!ip || port <= 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}
	if (timeout <= 0) {
		timeout = 5;
	}

	ret = cnc_allclibhndl3(ip, port, timeout, &handle);

	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "handle", handle);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

