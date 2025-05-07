#include "common.h"

int main(int argc, char *argv[]) {
  zmq_has("draft");
  void *ctx = zmq_ctx_new();
  void *radio = zmq_socket(ctx, ZMQ_RADIO);

  char *url = NULL;
  if (argc > 1) {
    if (argv[0] == 'm') {
      url = mcast_url;
    } else {
      url = unicast_url;
    }
  }

  printf("Connecting radio\n");
  int rc = zmq_connect(radio, url);
  assert(rc > -1);

  const char *group = "mcast_test";

  signal(SIGINT, inthand);
  int message_num = 0;
  while (!stop) {
    void *data = malloc(255);
    memset(data, 0, sizeof(data));
    sprintf(data, "%d", message_num);

    zmq_msg_t msg;
    zmq_msg_init_data(&msg, data, 255, my_free, NULL);
    zmq_msg_set_group(&msg, group);

    printf("Sending message %d\n", message_num);
    zmq_sendmsg(radio, &msg, 0);

    message_num++;
    usleep(100);
  }

  printf("Closing");
  zmq_close(radio);
  zmq_ctx_term(ctx);
  return 0;
}
