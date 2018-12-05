
static void timer_thread(union sigval v)
{
	uloop_end();
}
 
enum {
    FOCAS_QUIT_TIME,
    __FOCAS_QUIT_MAX
};

static const struct blobmsg_policy policy_quit[__FOCAS_QUIT_MAX] = {
    [FOCAS_QUIT_TIME] = { .name = "time", .type = BLOBMSG_TYPE_INT32 },
};

static int focas_quit(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__FOCAS_QUIT_MAX];
	int quit_time = 3;
	int timer_nr;
	timer_t timerid;
	struct sigevent evp;
	struct itimerspec it;
 
	blobmsg_parse(policy_connect, __CONNECT_MAX, tb, blob_data(msg), blob_len(msg));
	if (tb[FOCAS_QUIT_TIME])
		quit_time = blobmsg_get_u32(tb[FOCAS_QUIT_TIME]);
	if (quit_time <= 0) {
		quit_time = 3;
	}
	

	memset(&evp, 0, sizeof(struct sigevent));
	evp.sigev_value.sival_int = timer_nr;
	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = timer_thread;
	evp.sigev_notify_attributes = NULL;
 
	if (timer_create(CLOCK_REALTIME, &evp, &timerid) == -1)
	{
		return -1;
	}
 
	it.it_interval.tv_sec = quit_time;
	it.it_interval.tv_nsec = 0;
	it.it_value.tv_sec = quit_time;
	it.it_value.tv_nsec = 0;
 
	if (timer_settime(timerid, 0, &it, NULL) == -1)
	{
		return -1;
	}


	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "time", quit_time);
	ubus_send_reply(ctx, req, b.head);
	return 0;
}


