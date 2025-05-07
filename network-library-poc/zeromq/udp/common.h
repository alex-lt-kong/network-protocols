#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for usleep

// #include <czmq.h>
#define ZMQ_BUILD_DRAFT_API
#include <zmq.h>

volatile sig_atomic_t stop;

const char *mcast_url = "udp://239.0.0.1:40000";
const char *unicast_url = "udp://127.0.0.1:40000";

void inthand(int signum) { stop = 1; }

void my_free(void *data, void *hint) { free(data); }
