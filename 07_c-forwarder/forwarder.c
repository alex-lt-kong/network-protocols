#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <librdkafka/rdkafka.h>

#define MAX_PRINT 64

static volatile int keep_running = 1;

static size_t iso_dt_len = sizeof("1970-01-01T00:00:00Z");

char* get_iso_datetime(char* buf) {
    time_t now;
    time(&now);
    strftime(buf, iso_dt_len, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
}

void signal_handler(int dummy) {
    keep_running = 0;
}
/**
 * @brief  when a message has been successfully delivered or permanently failed delivery (after retries).
 */
static void dr_msg_cb (rd_kafka_t *kafka_handle, const rd_kafka_message_t *rkmessage, void *opaque) {
    if (rkmessage->err) {
        fprintf(stderr, "Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
    }
}

int main(int argc, char **argv) {
    rd_kafka_t *consumer;
    rd_kafka_t *producer;
    rd_kafka_conf_t *conf;
    rd_kafka_resp_err_t err;
    char iso_dt[iso_dt_len];
    char errstr[512];

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <broker1:port1,broker2:port2,...,brokerN::portN> <src_topic> <dst_topic>\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[2], argv[3]) == 0) {
        fprintf(stderr, "src_topic and dst_topic canNOT be the same--the result will be DISASTROUS!\n");
        return 1;
    }
    const char *src_topic = argv[2];
    const char *dst_topic = argv[3];

    conf = rd_kafka_conf_new();
    // See https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md
    rd_kafka_conf_set(conf, "bootstrap.servers", argv[1], errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "group.id", "forwarder", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "auto.offset.reset", "latest", errstr, sizeof(errstr));

    consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (!consumer) {
        fprintf(stderr, "[%s] Failed to create new consumer: %s", get_iso_datetime(iso_dt), errstr);
        return 1;
    }
    rd_kafka_poll_set_consumer(consumer);
    conf = NULL;// Configuration object is now owned, and freed, by the rd_kafka_t instance.

    conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "bootstrap.servers", argv[1], errstr, sizeof(errstr));
    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
    // Create the Producer instance.
    producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!producer) {
        fprintf(stderr, "[%s] Failed to create new producer: %s", get_iso_datetime(iso_dt), errstr);
        return 1;
    }    
    conf = NULL;// Configuration object is now owned, and freed, by the rd_kafka_t instance.
    
    printf("[%s] Start forwarding messages from topic [%s] to [%s]\n", get_iso_datetime(iso_dt), src_topic, dst_topic);
    rd_kafka_topic_partition_list_t *subscription = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(subscription, src_topic, RD_KAFKA_PARTITION_UA);
    // Subscribe to the list of topics.
    err = rd_kafka_subscribe(consumer, subscription);
    if (err) {
        fprintf(
            stderr, "[%s] Failed to subscribe to [%d] topics: %s",
            get_iso_datetime(iso_dt), subscription->cnt, rd_kafka_err2str(err)
        );
        rd_kafka_topic_partition_list_destroy(subscription);
        rd_kafka_destroy(consumer);
        return 1;
    }

    rd_kafka_topic_partition_list_destroy(subscription);

    int count = 0;
    signal(SIGINT, signal_handler);
    while (keep_running) {
        rd_kafka_message_t *consumer_message;

        consumer_message = rd_kafka_consumer_poll(consumer, 3 * 1000);
        if (consumer_message == NULL) {
            if (count++ % 20 == 0) { 
                printf("[%s] Waiting...\n", get_iso_datetime(iso_dt));
            }
            continue;
        }

        if (consumer_message->err) {
            if (consumer_message->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                // We can ignore this error - it just means we've read everything and are waiting for more data.
            } else {
                fprintf(stderr, "[%s] Consumer error: %s\n", get_iso_datetime(iso_dt), rd_kafka_message_errstr(consumer_message));
                return 1;
            }
        } else {
            printf(
                "[%s] CONSUMED event from src_topic [%s]\n",
                get_iso_datetime(iso_dt), rd_kafka_topic_name(consumer_message->rkt)
            );
            err = rd_kafka_producev(
                producer,
                RD_KAFKA_V_TOPIC(dst_topic),
                RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                RD_KAFKA_V_KEY((void*)consumer_message->key, consumer_message->key_len),
                RD_KAFKA_V_VALUE((void*)consumer_message->payload, consumer_message->len),
                RD_KAFKA_V_OPAQUE(NULL),
                RD_KAFKA_V_END
            );
            if (err) {
                fprintf(
                    stderr, "[%s] FAILED to produce to dst_topic [%s]: %s\n",
                    get_iso_datetime(iso_dt), src_topic, rd_kafka_err2str(err)
                );
            } else {
                printf("[%s] PRODUCED event to dst_topic [%s]: ", get_iso_datetime(iso_dt), src_topic);
                for (int i = 0; i < strlen((char*)consumer_message->payload) && i < MAX_PRINT; ++i) {
                    printf("%c", ((char*)(consumer_message->payload))[i]);
                }
                if (strlen((char*)consumer_message->payload) < MAX_PRINT) { printf("\n"); }
                else { printf("...[truncated at %d]\n", MAX_PRINT); }
            }
        }

        // Free the message when we're done.
        rd_kafka_message_destroy(consumer_message);
        
        rd_kafka_poll(producer, 0);
    }
    // Block until the messages are all sent.
    printf("[%s] Flushing final messages..\n", get_iso_datetime(iso_dt));
    rd_kafka_flush(producer, 10 * 1000);

    if (rd_kafka_outq_len(producer) > 0) {
        fprintf(stderr, "[%s] %d message(s) NOT delivered\n", get_iso_datetime(iso_dt), rd_kafka_outq_len(producer));
    }

    printf("[%s] Closing consumer and producer\n", get_iso_datetime(iso_dt));
    rd_kafka_consumer_close(consumer);
    rd_kafka_destroy(consumer);
    rd_kafka_destroy(producer);

    return 0;
}