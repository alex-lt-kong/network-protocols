#include <getopt.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <librdkafka/rdkafka.h>

#define MAX_PRINT 64
#define ISO_DATETIME_LEN 21 // This includes the null-terminator.

#define MY_PRINTF(fmt, ...)                                                    \
  printf("[%s] " fmt, get_iso_datetime(iso_dt), ##__VA_ARGS__)
#define MY_FPRINTF_ERR(fmt, ...)                                               \
  fprintf(stderr, "[%s] " fmt, get_iso_datetime(iso_dt), ##__VA_ARGS__)

static volatile int keep_running = 1;

void print_usage(char *binary_name) {
  printf("Usage: %s [OPTION]\n\n", binary_name);
  printf("Description:\n");
  printf("  Forwards Kafka messages from one topic to another\n\n");
  printf("Options:\n");
  printf("  -b, --brokers <broker1:port1,broker2:port2,...,brokerN::portN> A "
         "comma-separate list of brokers\n");
  printf("  -s, --src-topic <topic>                                        "
         "Source Kafka topic\n");
  printf(
      "  -d, --dst-topic <topic>                                        "
      "Destination Kafka topic, it can't be the same as source Kafka topic\n");
  printf("  -h, --help                     Print this help message\n");
}

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @param out_broker_list Caller takes the ownership of the pointer and needs to
 * free() it
 * @param out_src_topic Caller takes the ownership of the pointer and needs to
 * free() it
 * @param out_dst_topic Caller takes the ownership of the pointer and needs to
 * free() it
 */
void parse_options(int argc, char *argv[], char **out_broker_list,
                   char **out_src_topic, char **out_dst_topic) {
  static struct option long_options[] = {
      {"brokers", required_argument, 0, 'b'},
      {"src-topic", required_argument, 0, 's'},
      {"dst-topic", required_argument, 0, 'd'},
      {"help", optional_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int option_index = 0, opt;

  while ((opt = getopt_long(argc, argv, "b:s:d:h", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'b':
      if (optarg != NULL) {
        *out_broker_list = strdup(optarg);
      }
      break;
    case 's':
      if (optarg != NULL) {
        *out_src_topic = strdup(optarg);
      }
      break;
    case 'd':
      if (optarg != NULL) {
        *out_dst_topic = strdup(optarg);
      }
      break;
    default:
      print_usage(argv[0]);
    }
  }
}

char *get_iso_datetime(char *buf) {
  time_t now;
  time(&now);
  strftime(buf, ISO_DATETIME_LEN, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  return buf;
}

static void signal_handler(int signum) {
  char msg[] = "Signal [  ] caught\n";
  msg[8] = '0' + (char)(signum / 10);
  msg[9] = '0' + (char)(signum % 10);
  write(STDIN_FILENO, msg, strlen(msg));
  keep_running = 0;
}

void install_signal_handler() {
  // This design canNOT handle more than 99 signal types
  if (_NSIG > 99) {
    fprintf(stderr, "signal_handler() can't handle more than 99 signals\n");
    abort();
  }
  struct sigaction act;
  // Initialize the signal set to empty, similar to memset(0)
  if (sigemptyset(&act.sa_mask) == -1) {
    perror("sigemptyset()");
    abort();
  }
  act.sa_handler = signal_handler;
  /* SA_RESETHAND means we want our signal_handler() to intercept the signal
  once. If a signal is sent twice, the default signal handler will be used
  again. `man sigaction` describes more possible sa_flags. */
  act.sa_flags = SA_RESETHAND;
  // act.sa_flags = 0;
  if (sigaction(SIGINT, &act, 0) == -1 || sigaction(SIGTERM, &act, 0) == -1) {
    perror("sigaction()");
    abort();
  }
}

/**
 * @brief  when a message has been successfully delivered or permanently failed
 * delivery (after retries).
 */
static void dr_msg_cb(__attribute__((unused)) rd_kafka_t *kafka_handle,
                      const rd_kafka_message_t *rkmessage,
                      __attribute__((unused)) void *opaque) {
  if (rkmessage->err) {
    fprintf(stderr, "Message delivery failed: %s\n",
            rd_kafka_err2str(rkmessage->err));
  }
}

/**
 * @brief
 *
 * @param broker_list
 * @return rd_kafka_conf_t* or NULL on error. If rd_kafka_conf_t* is returned,
 * the caller takes the ownership of the pointer and needs to call either
 * rd_kafka_conf_destroy() to free() the object or call rd_kafka_new() to
 * pass the ownership to a consumer object.
 */
rd_kafka_conf_t *config_consumer(const char *broker_list) {
  char errstr[PATH_MAX];
  char iso_dt[ISO_DATETIME_LEN];
  // The doc doesn't say it could return NULL on error.
  rd_kafka_conf_t *conf = rd_kafka_conf_new();
  // See https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md

  // rd_kafka_conf_destroy() should only be called when an error is detected;
  // otherwise the ownership of conf will be passed to rd_kafka_new() and
  // rd_kafka_new() free()s it internally.
  if (rd_kafka_conf_set(conf, "bootstrap.servers", broker_list, errstr,
                        sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    MY_FPRINTF_ERR("rd_kafka_conf_set() failed: %s", errstr);
    (void)rd_kafka_conf_destroy(conf);
    return NULL;
  }
  if (rd_kafka_conf_set(conf, "group.id", "forwarder", errstr,
                        sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    MY_FPRINTF_ERR("rd_kafka_conf_set() failed: %s", errstr);
    (void)rd_kafka_conf_destroy(conf);
    return NULL;
  }
  if (rd_kafka_conf_set(conf, "auto.offset.reset", "latest", errstr,
                        sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    MY_FPRINTF_ERR("rd_kafka_conf_set() failed: %s", errstr);
    (void)rd_kafka_conf_destroy(conf);
    return NULL;
  }
  return conf;
}

void event_loop(rd_kafka_t *consumer, rd_kafka_t *producer,
                const char *src_topic, const char *dst_topic) {
  size_t count = 0;
  char iso_dt[ISO_DATETIME_LEN];
  rd_kafka_resp_err_t err;
  while (keep_running) {
    rd_kafka_message_t *consumer_message;

    consumer_message = rd_kafka_consumer_poll(consumer, 3 * 1000);
    if (consumer_message == NULL) {
      if (count++ % 20 == 0) {
        MY_PRINTF("Waiting...\n");
      }
      continue;
    }

    if (consumer_message->err) {
      if (consumer_message->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
        // We can ignore this error - it just means we've read everything and
        // are waiting for more data.
      } else {
        fprintf(stderr, "[%s] Consumer error: %s\n", get_iso_datetime(iso_dt),
                rd_kafka_message_errstr(consumer_message));
      }
    } else {
      MY_PRINTF("CONSUMED event from src_topic [%s]\n",
                rd_kafka_topic_name(consumer_message->rkt));
      err =
          rd_kafka_producev(producer, RD_KAFKA_V_TOPIC(dst_topic),
                            RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                            RD_KAFKA_V_KEY((void *)consumer_message->key,
                                           consumer_message->key_len),
                            RD_KAFKA_V_VALUE((void *)consumer_message->payload,
                                             consumer_message->len),
                            RD_KAFKA_V_OPAQUE(NULL), RD_KAFKA_V_END);
      if (err) {
        MY_FPRINTF_ERR("FAILED to produce to dst_topic [%s]: %s\n", src_topic,
                       rd_kafka_err2str(err));
      } else {
        MY_PRINTF("PRODUCED event to dst_topic [%s]: ", dst_topic);
        for (size_t i = 0;
             i < strlen((char *)consumer_message->payload) && i < MAX_PRINT;
             ++i) {
          printf("%c", ((char *)(consumer_message->payload))[i]);
        }
        if (strlen((char *)consumer_message->payload) < MAX_PRINT) {
          printf("\n");
        } else {
          printf("...[truncated at %d]\n", MAX_PRINT);
        }
      }
    }

    // Free the message when we're done.
    rd_kafka_message_destroy(consumer_message);

    rd_kafka_poll(producer, 0);
  }
}

int main(int argc, char **argv) {
  int retval = 0;
  char *broker_list;
  char *src_topic;
  char *dst_topic;
  char iso_dt[ISO_DATETIME_LEN];
  char errstr[PATH_MAX];

  install_signal_handler();
  parse_options(argc, argv, &broker_list, &src_topic, &dst_topic);
  if (broker_list == NULL || src_topic == NULL || dst_topic == NULL) {
    print_usage(argv[1]);
    goto err_parse_options;
  }
  if (strcmp(src_topic, dst_topic) == 0) {
    MY_FPRINTF_ERR("--src-topic and --dst-topic canNOT be the same\n");
    goto err_parse_options;
  }

  rd_kafka_t *consumer;
  rd_kafka_t *producer;
  rd_kafka_conf_t *conf;
  rd_kafka_resp_err_t err;

  conf = config_consumer(broker_list);
  if (conf == NULL) {
    retval = -1;
    goto err_rd_kafka_con_conf_set;
  }
  consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
  if (!consumer) {
    (void)rd_kafka_conf_destroy(conf);
    MY_FPRINTF_ERR("Failed to create new consumer: %s", errstr);
    retval = -1;
    goto err_rd_kafka_new_consumer;
  }
  conf = NULL; // Configuration object is now owned, and freed, by the
               // rd_kafka_t instance.
  err = rd_kafka_poll_set_consumer(consumer);
  if (err) {
    MY_FPRINTF_ERR("err_rd_kafka_poll_set_consumer() failed: %s",
                   rd_kafka_err2str(err));
    retval = -1;
    goto err_rd_kafka_poll_set_consumer;
  }

  // The doc doesn't say it could return NULL on error.
  conf = rd_kafka_conf_new();
  if (rd_kafka_conf_set(conf, "bootstrap.servers", broker_list, errstr,
                        sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    MY_FPRINTF_ERR("%s", errstr);
    (void)rd_kafka_conf_destroy(conf);
    retval = -1;
    goto err_rd_kafka_prod_conf_set;
  }
  (void)rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
  // Create the Producer instance.
  producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
  if (!producer) {
    (void)rd_kafka_conf_destroy(conf);
    MY_FPRINTF_ERR("Failed to create new producer: %s", errstr);
    retval = -1;
    goto err_rd_kafka_new_producer;
  }
  conf = NULL; // Configuration object is now owned, and freed, by the
               // rd_kafka_t instance.

  MY_PRINTF("Start forwarding messages from topic [%s] to [%s]\n", src_topic,
            dst_topic);
  // The doc doesn't say rd_kafka_topic_partition_list_new() could return NULL
  rd_kafka_topic_partition_list_t *subscription =
      rd_kafka_topic_partition_list_new(1);
  (void)rd_kafka_topic_partition_list_add(subscription, src_topic,
                                          RD_KAFKA_PARTITION_UA);
  // Subscribe to the list of topics.
  err = rd_kafka_subscribe(consumer, subscription);
  if (err) {
    MY_FPRINTF_ERR("Failed to subscribe to [%d] topics: %s", subscription->cnt,
                   rd_kafka_err2str(err));
    (void)rd_kafka_topic_partition_list_destroy(subscription);
    retval = -1;
    goto err_rd_kafka_subscribe;
  }
  (void)rd_kafka_topic_partition_list_destroy(subscription);

  event_loop(consumer, producer, src_topic, dst_topic);

  // Block until the messages are all sent.
  printf("[%s] Flushing final messages..\n", get_iso_datetime(iso_dt));
  err = rd_kafka_flush(producer, 10 * 1000);
  if (err) {
    MY_FPRINTF_ERR("rd_kafka_flush() failed: %s. But we will not take any "
                   "action here LOL.",
                   rd_kafka_err2str(err));
  }
  if (rd_kafka_outq_len(producer) > 0) {
    MY_FPRINTF_ERR("%d message(s) NOT delivered\n",
                   rd_kafka_outq_len(producer));
  }

  MY_PRINTF("Closing consumer and producer\n");

  rd_kafka_destroy(producer);
err_rd_kafka_subscribe:
  rd_kafka_consumer_close(consumer);
err_rd_kafka_new_producer:
err_rd_kafka_prod_conf_set:
err_rd_kafka_poll_set_consumer:
  rd_kafka_destroy(consumer);

err_rd_kafka_new_consumer:
err_rd_kafka_con_conf_set:
err_parse_options:
  free(broker_list);
  free(src_topic);
  free(dst_topic);
  return retval;
}
