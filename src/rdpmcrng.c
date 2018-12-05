
enum {
    RDPMCRNG_HANDLE,
    RDPMCRNG_ADDR_TYPE,
    RDPMCRNG_DATA_TYPE,
    RDPMCRNG_START,
    RDPMCRNG_LENGTH,
    __RDPMCRNG_MAX
};

static const struct blobmsg_policy policy_rdpmcrng[__RDPMCRNG_MAX] = {
    [RDPMCRNG_HANDLE] = { .name = "handle", .type = BLOBMSG_TYPE_INT32 },
	[RDPMCRNG_ADDR_TYPE] = { .name = "addr_type", .type = BLOBMSG_TYPE_INT32 },
	[RDPMCRNG_DATA_TYPE] = { .name = "data_type", .type = BLOBMSG_TYPE_INT32 },
	[RDPMCRNG_START] = { .name = "start", .type = BLOBMSG_TYPE_INT32 },
	[RDPMCRNG_LENGTH] = { .name = "length", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_rdpmcrng(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__RDPMCRNG_MAX];
	unsigned short handle = 0;
	short adr_type = 0;
	short data_type = 0;
	unsigned short start = 0;
	unsigned short length = 0;
	int buf_len = 0;
	short ret = 0;
	int i = 0;
	IODBPMC* data = NULL;
	void* cookie = NULL;

	blobmsg_parse(policy_rdpmcrng, __RDPMCRNG_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[RDPMCRNG_HANDLE] || !tb[RDPMCRNG_ADDR_TYPE] || !tb[RDPMCRNG_DATA_TYPE] || !tb[RDPMCRNG_START] || !tb[RDPMCRNG_LENGTH])
		return UBUS_STATUS_INVALID_ARGUMENT;

	handle = blobmsg_get_u32(tb[RDPMCRNG_HANDLE]);
	adr_type = blobmsg_get_u32(tb[RDPMCRNG_ADDR_TYPE]);
	data_type = blobmsg_get_u32(tb[RDPMCRNG_DATA_TYPE]);
	start = blobmsg_get_u32(tb[RDPMCRNG_START]);
	length = blobmsg_get_u32(tb[RDPMCRNG_LENGTH]);

	//数据类型:字节变量,0-byte 1-word 2-long 4-float 5-double
	//地址类型 3:X
	//PLC起始地址
	//PLC结束地址
	switch (data_type)
	{
		case 0: buf_len = 8 + length; break;
		case 1: buf_len = 8 + length * 2; break;
		case 2: buf_len = 8 + length * 4; break;
		case 4: buf_len = 8 + length * 4; break;
		case 5: buf_len = 8 + length * 8; break;
		default:
			buf_len = 8 + length;
			data_type = 0;
			break;
	}

	if (length < sizeof(IODBPMC) ) {
		data = (IODBPMC*)malloc(sizeof(IODBPMC));
		memset(data, 0, sizeof(IODBPMC));
	} else {
		data = (IODBPMC*)malloc(length);
		memset(data, 0, length);
	}

	if (!data) {
		blob_buf_init(&b, 0);
		blobmsg_add_u32(&b, "rc", -1);
		blobmsg_add_string(&b, "message", "No enough memory!");
		ubus_send_reply(ctx, req, b.head);
	}

	ret = 0;
	data->type_a = adr_type;
	data->type_d = data_type;
	data->datano_s = start;
	data->datano_e = start + buf_len - 8;

	ret = pmc_rdpmcrng(handle, adr_type, data_type, start, data->datano_e, buf_len, data);

	CHECK_FOCAS_RET(ret);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "rc", ret);
	blobmsg_add_u32(&b, "addr_type", data->type_a);
	blobmsg_add_u32(&b, "data_type", data->type_d);
	blobmsg_add_u32(&b, "start", data->datano_s);
	blobmsg_add_u32(&b, "end", data->datano_e);

	cookie = blobmsg_open_array(&b, "data");
	for (i = 0; i < length; ++i) {
		switch(data->type_d) {
			case 0: blobmsg_add_u8(&b, NULL, data->u.cdata[i]); break;
			case 1: blobmsg_add_u16(&b, NULL, data->u.idata[i]); break;
			case 2: blobmsg_add_u32(&b, NULL, data->u.ldata[i]); break;
			case 4: blobmsg_add_double(&b, NULL, data->u.fdata[i]); break;
			case 5: blobmsg_add_double(&b, NULL, data->u.dfdata[i]); break;
			default: blobmsg_add_u8(&b, NULL, data->u.cdata[i]); break;
		}
	}
	blobmsg_close_array(&b, cookie);

	ubus_send_reply(ctx, req, b.head);

	free(data);

	return 0;
}

