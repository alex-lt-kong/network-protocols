// Originally:
// https://github.com/confluentinc/librdkafka/blob/master/examples/producer.c

#include <librdkafka/rdkafka.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static volatile sig_atomic_t run = 1;

/**
 * @brief Signal termination of program
 */
static void stop(int sig) {
  run = 0;
  fclose(stdin); /* abort fgets() */
}

/**
 * @brief Message delivery report callback.
 *
 * This callback is called exactly once per message, indicating if
 * the message was succesfully delivered
 * (rkmessage->err == RD_KAFKA_RESP_ERR_NO_ERROR) or permanently
 * failed delivery (rkmessage->err != RD_KAFKA_RESP_ERR_NO_ERROR).
 *
 * The callback is triggered from rd_kafka_poll() and executes on
 * the application's thread.
 */
static void dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage,
                      void *opaque) {
  if (rkmessage->err)
    fprintf(stderr, "%% Message delivery failed: %s\n",
            rd_kafka_err2str(rkmessage->err));
  else
    fprintf(stderr,
            "%% Message delivered (%zd bytes, "
            "partition %" PRId32 ")\n",
            rkmessage->len, rkmessage->partition);

  /* The rkmessage is destroyed automatically by librdkafka */
}

int main(int argc, char **argv) {
  rd_kafka_t *rk;        /* Producer instance handle */
  rd_kafka_conf_t *conf; /* Temporary configuration object */
  char errstr[512];      /* librdkafka API error reporting buffer */
  char buf[512];         /* Message value temporary buffer */
  const char *brokers;   /* Argument: broker list */
  const char *topic;     /* Argument: topic to produce to */

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <broker> <topic>\n", argv[0]);
    return 1;
  }

  brokers = argv[1];
  topic = argv[2];

  /*
   * Create Kafka client configuration place-holder
   */
  conf = rd_kafka_conf_new();

  /* Set bootstrap broker(s) as a comma-separated list of
   * host or host:port (default port 9092).
   * librdkafka will use the bootstrap brokers to acquire the full
   * set of brokers from the cluster. */
  if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr,
                        sizeof(errstr)) != RD_KAFKA_CONF_OK) {
    fprintf(stderr, "%s\n", errstr);
    (void)rd_kafka_conf_destroy(conf);
    return 1;
  }

  /* Set the delivery report callback.
   * This callback will be called once per message to inform
   * the application if delivery succeeded or failed.
   * See dr_msg_cb() above.
   * The callback is only triggered from rd_kafka_poll() and
   * rd_kafka_flush(). */
  (void)rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

  /*
   * Create producer instance.
   *
   * NOTE: rd_kafka_new() takes ownership of the conf object
   *       and the application must not reference it again after
   *       this call.
   */
  rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
  if (!rk) {
    fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
    return 1;
  }

  /* Signal handler for clean shutdown */
  signal(SIGINT, stop);

  fprintf(stderr, "Type some text and hit enter to produce message\n"
                  "Or just hit enter to only serve delivery reports\n"
                  "Press Ctrl-C or Ctrl-D to exit\n");

  while (run && fgets(buf, sizeof(buf), stdin)) {
    size_t len = strlen(buf);
    rd_kafka_resp_err_t err;

    if (buf[len - 1] == '\n') /* Remove newline */
      buf[--len] = '\0';

    if (len == 0) {
      /* Empty line: only serve delivery reports */
      rd_kafka_poll(rk, 0 /*non-blocking */);
      continue;
    }

  /*
   * Send/Produce message.
   * This is an asynchronous call, on success it will only
   * enqueue the message on the internal producer queue.
   * The actual delivery attempts to the broker are handled
   * by background threads.
   * The previously registered delivery report callback
   * (dr_msg_cb) is used to signal back to the application
   * when the message has been delivered (or failed).
   */
  retry:
    err = rd_kafka_producev(
        /* Producer handle */
        rk,
        /* Topic name */
        RD_KAFKA_V_TOPIC(topic),
        /* Make a copy of the payload. */
        RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
        /* Message value and length */
        RD_KAFKA_V_VALUE(buf, len),
        /* Per-Message opaque, provided in
         * delivery report callback as
         * msg_opaque. */
        RD_KAFKA_V_OPAQUE(NULL),
        /* End sentinel */
        RD_KAFKA_V_END);

    if (err) {
      /*
       * Failed to *enqueue* message for producing.
       */
      fprintf(stderr, "%% Failed to produce to topic %s: %s\n", topic,
              rd_kafka_err2str(err));

      if (err == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
        /* If the internal queue is full, wait for
         * messages to be delivered and then retry.
         * The internal queue represents both
         * messages to be sent and messages that have
         * been sent or failed, awaiting their
         * delivery report callback to be called.
         *
         * The internal queue is limited by the
         * configuration property
         * queue.buffering.max.messages and
         * queue.buffering.max.kbytes */
        rd_kafka_poll(rk, 1000 /*block for max 1000ms*/);
        goto retry;
      }
    } else {
      fprintf(stderr,
              "%% Enqueued message (%zd bytes) "
              "for topic %s\n",
              len, topic);
    }

    /* A producer application should continually serve
     * the delivery report queue by calling rd_kafka_poll()
     * at frequent intervals.
     * Either put the poll call in your main loop, or in a
     * dedicated thread, or call it after every
     * rd_kafka_produce() call.
     * Just make sure that rd_kafka_poll() is still called
     * during periods where you are not producing any messages
     * to make sure previously produced messages have their
     * delivery report callback served (and any other callbacks
     * you register). */
    rd_kafka_poll(rk, 0 /*non-blocking*/);
  }

  /* Wait for final messages to be delivered or fail.
   * rd_kafka_flush() is an abstraction over rd_kafka_poll() which
   * waits for all messages to be delivered. */
  int timeout_sec = 10;
  fprintf(stderr, "Flushing final messages (timeout: %d sec)..\n", timeout_sec);
  rd_kafka_flush(rk, timeout_sec * 1000);

  /* If the output queue is still not empty there is an issue
   * with producing messages to the clusters. */
  if (rd_kafka_outq_len(rk) > 0) {
    fprintf(stderr,
            "rd_kafka_flush() timed out but %d message(s) are still not "
            "delivered, they will be discarded\n",
            rd_kafka_outq_len(rk));
  }

  /* Destroy the producer instance */
  rd_kafka_destroy(rk);

  return 0;
}
