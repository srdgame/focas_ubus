#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <libubox/blobmsg_json.h>
#include <libubox/md5.h>
#include <libubus.h>

#include <fwlib32.h>

#define MAX_CONNECTION 16
#define INVALID_HANDLE 0xFFFF

struct focas_connection {
	uint32_t hash[5]; // MD5SUM for ip and port, and peer if shared is false
	unsigned short handle;
};
struct focas_connection g_connections[MAX_CONNECTION];

static struct ubus_context *ctx;
static struct blob_buf b;
static int g_closing = 0;

static short find_connection(const char* ip, uint16_t port, uint32_t peer, int shared, unsigned short *handle) {
	uint32_t hash[4];
	md5_ctx_t md5;
	char * buf = NULL;
	int i = 0;

	buf = (char*)malloc(sizeof(char) * 128);
	memset(buf, 0, 128);
	if (shared) {
		sprintf(buf, "%s:%u:%u", ip, port, peer);
	} else {
		sprintf(buf, "%s:%u", ip, port);
	}
	md5_begin(&md5);
	md5_hash(buf, 128, &md5);
	md5_end(hash, &md5);

	/// finding exists connections by hash id
	for (i = 0; i < MAX_CONNECTION; ++i) {
		if (g_connections[i].hash[0] == hash[0] &&
			g_connections[i].hash[1] == hash[1] &&
			g_connections[i].hash[2] == hash[2] &&
			g_connections[i].hash[3] == hash[3] )
		{
			*handle = g_connections[i].handle;
			break;
		}
	}

	/// Find a free slot
	if (i == MAX_CONNECTION) {
		for (i = 0; i < MAX_CONNECTION; ++i) {
			if (g_connections[i].handle == INVALID_HANDLE) {
				memcpy(g_connections[i].hash, hash, sizeof(uint32_t) * 4);
			}
		}
	}
	free(buf);

	return i;
}

static void set_connection(short index, unsigned short handle) {
	if (index >= 0 && index < MAX_CONNECTION) {
		g_connections[index].handle = handle;
	}
}

static void close_connection(unsigned short handle) {
	int i = 0;
	for (i = 0; i < MAX_CONNECTION; ++i){
		if (g_connections[i].handle == handle) {
			memset(&g_connections[i], 0, sizeof(struct focas_connection));
			g_connections[i].handle = INVALID_HANDLE;
		}
	}
}

#include "error_info.c"

#define CHECK_FOCAS_RET(RET) \
	if (RET != EW_OK) { \
		blob_buf_init(&b, 0); \
		blobmsg_add_u32(&b, "rc", RET); \
		blobmsg_add_string(&b, "error", get_error_info(RET)); \
		ubus_send_reply(ctx, req, b.head); \
		return 0; \
	} 

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
//#include "rdalmmsg3.c"
#include "getdtailerr.c"
#include "rdexecprog.c"
#include "rdpmcrng.c"
#include "focas_quit.c"

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
	//FOCAS_METHOD(rdalmmsg3),
	FOCAS_METHOD(getdtailerr),
	FOCAS_METHOD(rdexecprog),
	FOCAS_METHOD(rdpmcrng),

	FOCAS_METHOD(quit),
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
