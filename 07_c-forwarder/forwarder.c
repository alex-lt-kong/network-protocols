#include <glib.h>
#include <time.h>
#include <stdlib.h>
#include <librdkafka/rdkafka.h>

#include "common.c"

static volatile sig_atomic_t run = 1;

/**
 * @brief Signal termination of program
 */
static void stop(int sig) {
    run = 0;
}

/**
 * @brief  when a message has been successfully delivered or permanently failed delivery (after retries).
 */
static void dr_msg_cb (rd_kafka_t *kafka_handle, const rd_kafka_message_t *rkmessage, void *opaque) {
    if (rkmessage->err) {
        g_error("Message delivery failed: %s", rd_kafka_err2str(rkmessage->err));
    }
}

int main (int argc, char **argv) {
    rd_kafka_t *consumer;
    rd_kafka_t *producer;
    rd_kafka_conf_t *conf;
    rd_kafka_resp_err_t err;
    char errstr[512];

    if (argc != 3) {
        g_error("Usage: %s src_topic dst_topic", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], argv[2]) == 0) {
        g_error("src_topic and dst_topic canNOT be the same!");
        return 1;
    }
    
    // Parse the configuration.
    // See https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md
    const char *config_file = "config.ini";

    g_autoptr(GError) error = NULL;
    g_autoptr(GKeyFile) key_file = g_key_file_new();
    if (!g_key_file_load_from_file (key_file, config_file, G_KEY_FILE_NONE, &error)) {
        g_error ("Error loading config file: %s", error->message);
        return 1;
    }

    // Load the relevant configuration sections.
    conf = rd_kafka_conf_new();
    load_config_group(conf, key_file, "default");
    load_config_group(conf, key_file, "consumer");
    // Create the Consumer instance.
    consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (!consumer) {
        g_error("Failed to create new consumer: %s", errstr);
        return 1;
    }
    
    rd_kafka_poll_set_consumer(consumer);

    // Load the relevant configuration sections.
    conf = rd_kafka_conf_new();
    load_config_group(conf, key_file, "default");    
    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);
    // Create the Producer instance.
    producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!producer) {
        g_error("Failed to create new producer: %s", errstr);
        return 1;
    }
    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    conf = NULL;

    // Convert the list of topics to a format suitable for librdkafka.
    const char *src_topic = argv[1];//"als-prod-app-alert-topic";
    const char *dst_topic = argv[2];//"als-uat-app-alert-topic";
    g_message("Forwarding messages from topic [%s] to [%s]", src_topic, dst_topic);
    rd_kafka_topic_partition_list_t *subscription = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(subscription, src_topic, RD_KAFKA_PARTITION_UA);

    // Subscribe to the list of topics.
    err = rd_kafka_subscribe(consumer, subscription);
    if (err) {
        g_error("Failed to subscribe to %d topics: %s", subscription->cnt, rd_kafka_err2str(err));
        rd_kafka_topic_partition_list_destroy(subscription);
        rd_kafka_destroy(consumer);
        return 1;
    }

    rd_kafka_topic_partition_list_destroy(subscription);


    // Install a signal handler for clean shutdown.
    signal(SIGINT, stop);
    srand(time(NULL));
    // Start polling for messages.
    while (run) {
        rd_kafka_message_t *consumer_message;

        consumer_message = rd_kafka_consumer_poll(consumer, 5 * 1000);
        if (consumer_message == NULL) {
            if (rand() % 10 == 0) { g_message("Waiting..."); }
            continue;
        }

        if (consumer_message->err) {
            if (consumer_message->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                // We can ignore this error - it just means we've read everything and are waiting for more data.
            } else {
                g_message("Consumer error: %s", rd_kafka_message_errstr(consumer_message));
                return 1;
            }
        } else {
            g_message(
                "Consumed event from src_topic [%s]: %s",
                rd_kafka_topic_name(consumer_message->rkt), (char*)consumer_message->payload
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
                g_error("Failed to produce to dst_topic [%s]: %s", src_topic, rd_kafka_err2str(err));
            } else {
                g_message("Produced event to dst_topic [%s]: value = %s", src_topic, consumer_message->payload);
            }
        }

        // Free the message when we're done.
        rd_kafka_message_destroy(consumer_message);
        
        rd_kafka_poll(producer, 0);
    }
    // Block until the messages are all sent.
    g_message("Flushing final messages..");
    rd_kafka_flush(producer, 10 * 1000);

    if (rd_kafka_outq_len(producer) > 0) {
        g_error("%d message(s) were not delivered", rd_kafka_outq_len(producer));
    }

    g_message( "Closing consumer");    
    rd_kafka_consumer_close(consumer);
    rd_kafka_destroy(consumer);

    g_message( "Closing producer");
    rd_kafka_destroy(producer);

    return 0;
}