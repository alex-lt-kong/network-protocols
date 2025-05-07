#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <librdkafka/rdkafkacpp.h>

using namespace std;
using namespace RdKafka;

static volatile sig_atomic_t run = 1;

static void sigterm(int sig) {
  run = 0;
}


class ExampleDeliveryReportCb : public DeliveryReportCb {
 public:
  void dr_cb(Message &message) {
    /* If message.err() is non-zero the message delivery failed permanently
     * for the message. */
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr()
                << std::endl;
    else
      std::cerr << "% Message delivered to topic " << message.topic_name()
                << " [" << message.partition() << "] at offset "
                << message.offset() << std::endl;
  }
};

int main(int argc, char **argv) {

  string brokers = "localhost:9092";
  string topic   = "test-topic";

  Conf *conf = Conf::create(Conf::CONF_GLOBAL);

  string errstr;

  /* Set bootstrap broker(s) as a comma-separated list of
   * host or host:port (default port 9092).
   * librdkafka will use the bootstrap brokers to acquire the full
   * set of brokers from the cluster. */
  if (conf->set("bootstrap.servers", brokers, errstr) != Conf::CONF_OK) {
    cerr << errstr << std::endl;
    // cout goes to stdout and cerr goes to stderr;
    exit(1);
  }

  signal(SIGINT, sigterm);
  signal(SIGTERM, sigterm);

  /* Set the delivery report callback.
   * This callback will be called once per message to inform
   * the application if delivery succeeded or failed.
   * The callback is only triggered from ::poll() and ::flush().
   *
   * IMPORTANT:
   * Make sure the DeliveryReport instance outlives the Producer object,
   * either by putting it on the heap or as in this case as a stack variable
   * that will NOT go out of scope for the duration of the Producer object.
   */
  ExampleDeliveryReportCb ex_dr_cb;

  if (conf->set("dr_cb", &ex_dr_cb, errstr) != Conf::CONF_OK) {
    cerr << errstr << std::endl;
    exit(1);
  }

  Producer *producer = Producer::create(conf, errstr);
  if (!producer) {
    cerr << "Failed to create producer: " << errstr << endl;
    exit(1);
  }

  delete conf;

  cout << "Type message and hit enter to produce a message." << endl;

  for (string line; run && getline(cin, line);) {
    if (line.empty()) {
      producer->poll(0);
      continue;
    }

    /*
     * Send/Produce message.
     * This is an asynchronous call, on success it will only
     * enqueue the message on the internal producer queue.
     * The actual delivery attempts to the broker are handled
     * by background threads.
     * The previously registered delivery report callback
     * is used to signal back to the application when the message
     * has been delivered (or failed permanently after retries).
     */
  retry: // retry will only be goto'ed if there is an error
    ErrorCode err = producer->produce(
        /* Topic name */
        topic,
        /* Any Partition: the builtin partitioner will be
         * used to assign the message to a topic based
         * on the message key, or random partition if
         * the key is not set. */
        Topic::PARTITION_UA,
        /* Make a copy of the value */
        Producer::RK_MSG_COPY /* Copy payload */,
        /* Value */
        const_cast<char *>(line.c_str()), line.size(),
        /* Key */
        NULL, 0,
        /* Timestamp (defaults to current time) */
        0,
        /* Message headers, if any */
        NULL,
        /* Per-message opaque value passed to
         * delivery report */
        NULL);

    if (err != ERR_NO_ERROR) {
      cerr << "% Failed to produce to topic " << topic << ": "
                << err2str(err) << endl;

      if (err == RdKafka::ERR__QUEUE_FULL) {
        /* If the internal queue is full, wait for
         * messages to be delivered and then retry.
         * The internal queue represents both
         * messages to be sent and messages that have
         * been sent or failed, awaiting their
         * delivery report callback to be called.
         *
         * The internal queue is limited by the
         * configuration property
         * queue.buffering.max.messages */
        producer->poll(1000 /*block for max 1000ms*/);
        goto retry;
      }

    } else {
      cerr << "Enqueued message (" << line.size() << " bytes) "
           << "for topic " << topic << endl;
    }

    /* A producer application should continually serve
     * the delivery report queue by calling poll()
     * at frequent intervals.
     * Either put the poll call in your main loop, or in a
     * dedicated thread, or call it after every produce() call.
     * Just make sure that poll() is still called
     * during periods where you are not producing any messages
     * to make sure previously produced messages have their
     * delivery report callback served (and any other callbacks
     * you register). */
    producer->poll(0);
  }

  /* Wait for final messages to be delivered or fail.
   * flush() is an abstraction over poll() which
   * waits for all messages to be delivered. */
  cerr << "Flushing final messages..." << endl;
  producer->flush(10 * 1000 /* wait for max 10 seconds */);

  if (producer->outq_len() > 0)
    cerr << producer->outq_len()
         << " message(s) were not delivered" << endl;

  delete producer;

  return 0;
}
