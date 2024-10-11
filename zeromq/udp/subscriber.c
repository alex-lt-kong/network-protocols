#include "common.h"

int main(int argc, char *argv[]) {
  void *ctx = zmq_ctx_new();
  void *dish = zmq_socket(ctx, ZMQ_DISH);

  const char *url = NULL;
  if (argc > 1) {
    if (argv[0] == 'm') {
      url = mcast_url;
    } else {
      url = unicast_url;
    }
  }

  printf("Binding dish\n");
  int rc = zmq_bind(dish, url);
  assert(rc > -1);

  const char *group = "mcast_test";
  printf("Dish joining group\n");
  zmq_join(dish, group);

  signal(SIGINT, inthand);

  printf("Waiting for dish message\n");
  while (!stop) {
    zmq_msg_t recv_msg;
    zmq_msg_init(&recv_msg);
    zmq_recvmsg(dish, &recv_msg, 0);
    printf("%s", (char *)zmq_msg_data(&recv_msg));
    printf("\n");

    zmq_msg_close(&recv_msg);
  }

  printf("Closing");
  zmq_close(dish);
  zmq_ctx_term(ctx);

  return 0;
}