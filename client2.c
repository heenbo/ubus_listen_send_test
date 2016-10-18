#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <libubox/ustream.h>
#include <libubox/blobmsg_json.h>
#include <pthread.h>
#include "libubus.h"

static struct ubus_context *ctx_listen = NULL;
static struct ubus_context *ctx_send = NULL;
static struct blob_buf blob_buf_send;
static struct ubus_event_handler listener;
void * pthread_ubus_send(void * arg);

#define UBUS_LISTEN_OBJ_STR	"client2"
#define UBUS_SEND_OBJ_STR	"client1"

static void receive_event(struct ubus_context *ctx, struct ubus_event_handler *ev,
			  const char *type, struct blob_attr *msg)
{
	char *str;

	str = blobmsg_format_json(msg, true);
	printf("{ \"%s\": %s }\n", type, str);
	free(str);
}

static void listen_main(void)
{
	const char *event_listen = NULL;
	int ret = 0;

	memset(&listener, 0, sizeof(listener));
	listener.cb = receive_event;
	event_listen = UBUS_LISTEN_OBJ_STR;
	ret = ubus_register_event_handler(ctx_listen, &listener, event_listen);
	if (ret)
	{
		fprintf(stderr, "Error while registering for event '%s': %s\n",
			event_listen, ubus_strerror(ret));
		return;
	}
	
	uloop_init();
	ubus_add_uloop(ctx_listen);
	uloop_run();
	uloop_done();
}

static int my_ubus_send(char *id, char *json_msg)
{
        int i = 0;

        blob_buf_init(&blob_buf_send, 0);
        if (!blobmsg_add_json_from_string(&blob_buf_send, json_msg))
        {
                fprintf(stderr, "Failed to parse message data\n");
                return -1;
        }
        ubus_send_event(ctx_send, id, blob_buf_send.head);
        return 0;
}

void * pthread_ubus_send(void * arg)
{
	int ret = 0;
	char buf[128] = {0};
	char buf_send[256] = {0};

	while(1)
	{
		scanf("%s", buf);
		printf("----------------------- input is %s\n", buf);
		sprintf(buf_send, "{\"heenbo\":\"%s\"}", buf);
		my_ubus_send(UBUS_SEND_OBJ_STR, buf_send);
		fflush(stdin);
	}	

	return NULL;
}

int main(int argc, char **argv)
{
	const char *ubus_socket_listen = NULL;
        const char *ubus_socket_send = NULL;

	printf("sw version: %s, %s\n", __DATE__, __TIME__);

	ctx_listen = ubus_connect(ubus_socket_listen);
	if (!ctx_listen)
	{
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}

	ctx_send = ubus_connect(ubus_socket_send);
	if (!ctx_send)
	{
	        fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}
	ctx_send->sock.registered = true;

	pthread_t pthread_ubus_send_id = -1;
	pthread_create(&pthread_ubus_send_id, NULL, pthread_ubus_send, NULL);
	
	listen_main();
	
	ubus_free(ctx_listen);
	
	return 0;
}
