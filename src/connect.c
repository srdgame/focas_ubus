
enum {
    CONNECT_IP,
    CONNECT_PORT,
    CONNECT_TIMEOUT,
	CONNECT_SHARED,
    __CONNECT_MAX
};

static const struct blobmsg_policy policy_connect[__CONNECT_MAX] = {
    [CONNECT_IP] = { .name = "ip", .type = BLOBMSG_TYPE_STRING },
    [CONNECT_PORT] = { .name = "port", .type = BLOBMSG_TYPE_INT32 },
    [CONNECT_TIMEOUT] = { .name = "timeout", .type = BLOBMSG_TYPE_INT32 },
    [CONNECT_SHARED] = { .name = "shared", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_connect(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__CONNECT_MAX];
	char *ip = NULL;
	uint16_t port = 0;
	uint32_t timeout = 0;
	uint32_t shared = 1;
	unsigned short handle = INVALID_HANDLE;
	short ret = EW_OK;

	blobmsg_parse(policy_connect, __CONNECT_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[CONNECT_IP] || !tb[CONNECT_PORT])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ip = blobmsg_get_string(tb[CONNECT_IP]);
	port = blobmsg_get_u32(tb[CONNECT_PORT]);
	if (!ip || port <= 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}
	if (tb[CONNECT_TIMEOUT]) {
		timeout = blobmsg_get_u32(tb[CONNECT_TIMEOUT]);
		if (timeout <= 0) {
			timeout = 5;
		}
	}
	if (tb[CONNECT_SHARED]) {
		shared = blobmsg_get_u32(tb[CONNECT_SHARED]);
	}
	printf("Connect to device. IP:%s\tPort:%u\n", ip, port);
	ret = find_connection(ip, port, req->peer, shared, &handle);	
	printf("Find connection returns %d handle: %u\n", ret, handle);
	if (ret < MAX_CONNECTION && handle == INVALID_HANDLE ) {
		ret = cnc_allclibhndl3(ip, port, timeout, &handle);
		if (ret == EW_OK) {
			printf("Connected handle:%u\n", handle);
			set_connection(ret, handle);
		} else {
			printf("Connected failed:%d\n", ret);
		}
	} else {
		if (ret == MAX_CONNECTION && handle == INVALID_HANDLE) {
			ret = CONNECTION_FULL;
		}
	}

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "handle", handle);
	ubus_send_reply(ctx, req, b.head);

	return 0;
}

