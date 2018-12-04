#include <unistd.h>
#include <signal.h>

#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include <fwlib32.h>

static struct ubus_context *ctx;
static struct blob_buf b;

#include "connect.c"
#include "disconnect.c"
#include "actf.c"
#include "acts.c"
#include "acts2.c"
#include "rdspload.c"
#include "axis.c"
#include "rdposition.c"
#include "rdsvmeter.c"
#include "rdspmeter.c"
#include "rdspdlname.c"
#include "alarm.c"
#include "alarm2.c"
#include "rdalminfo.c"
#include "rdalmmsg.c"
#include "rdalmmsg2.c"
#include "rdalmmsg3.c"
#include "getdtailerr.c"
#include "rdexecprog.c"
#include "rdpmcrng.c"

#define FOCAS_METHOD(NAME) \
	UBUS_METHOD(#NAME, focas_##NAME, policy_##NAME)

static const struct ubus_method focas_methods[] = {
	FOCAS_METHOD(connect),
	FOCAS_METHOD(disconnect),
	FOCAS_METHOD(actf),
	FOCAS_METHOD(acts),
	FOCAS_METHOD(acts2),
	FOCAS_METHOD(rdspload),
	FOCAS_METHOD(axis),
	FOCAS_METHOD(rdposition),
	FOCAS_METHOD(rdsvmeter),
	FOCAS_METHOD(rdspmeter),
	FOCAS_METHOD(rdspdlname),
	FOCAS_METHOD(alarm),
	FOCAS_METHOD(alarm2),
	FOCAS_METHOD(rdalminfo),
	FOCAS_METHOD(rdalmmsg),
	FOCAS_METHOD(rdalmmsg2),
	FOCAS_METHOD(rdalmmsg3),
	FOCAS_METHOD(getdtailerr),
	FOCAS_METHOD(rdexecprog),
	FOCAS_METHOD(rdpmcrng),
};

static struct ubus_object_type focas_object_type =
	UBUS_OBJECT_TYPE("focas", focas_methods);

static struct ubus_object focas_object = {
	.name = "focas",
	.type = &focas_object_type,
	.methods = focas_methods,
	.n_methods = ARRAY_SIZE(focas_methods),
};

static void server_main(void)
{
	int ret;

	ret = ubus_add_object(ctx, &focas_object);
	if (ret)
		fprintf(stderr, "Failed to add object: %s\n", ubus_strerror(ret));

	uloop_run();
}

int main(int argc, char **argv)
{
	const char *ubus_socket = NULL;
	int ch;
	short ret;

	while ((ch = getopt(argc, argv, "cs:")) != -1) {
		switch (ch) {
		case 's':
			ubus_socket = optarg;
			break;
		default:
			break;
		}
	}

	argc -= optind;
	argv += optind;

	ret = cnc_startupprocess(0, "focas2.log");
	if (ret != EW_OK) {
		fprintf(stderr, "Failed initialize focas2 library\n");
		return -1;
	}

	uloop_init();
	signal(SIGPIPE, SIG_IGN);

	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}

	ubus_add_uloop(ctx);

	server_main();

	ubus_free(ctx);
	uloop_done();

	cnc_exitprocess();

	return 0;
}
