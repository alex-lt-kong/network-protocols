# Basic Commands

## Start the Kafka Environment

Enter Kafka directory (make sure JRE is installed and $JAVA_HOME correct set)

```
cd kafka_2.13-3.0.0
```

Start ZooKeeper

```
bin/zookeeper-server-start.sh config/zookeeper.properties
```

Start a broker

```
bin/kafka-server-start.sh config/server.properties
```

## Create a topic for single broker

* If you need to create a topic for multiple brokers, refer to `1_multiple-brokers.md`.

```
bin/kafka-topics.sh --create --partitions 1 --replication-factor 1 --topic quickstart-events --bootstrap-server localhost:9092,localhost:9093,localhost:9094
```

## Write events into the topic

```
bin/kafka-console-producer.sh --topic quickstart-events --bootstrap-server localhost:9092
```

## Read events from the topic

```
bin/kafka-console-consumer.sh --topic quickstart-events --from-beginning --bootstrap-server localhost:9092,localhost:9093,localhost:9094
```

## List topics

```
bin/kafka-topics.sh --list --bootstrap-server localhost:9093
```


## Check details of a topic

```
bin/kafka-topics.sh --describe --topic ak-topic --bootstrap-server localhost:9092,localhost:9093,localhost:9094
```

Typical output:
```
Topic: ak-topic TopicId: tXWqre37TcmKJFIrzW8GVg PartitionCount: 1       ReplicationFactor: 3    Configs: min.insync.replicas=2,segment.bytes=1073741824
        Topic: ak-topic Partition: 0    Leader: 2       Replicas: 0,1,2 Isr: 2,1,0
```

* If you run the command after killing a Leader broker, you may notice that the `Leader`'s id will change;
* If you run the command after killing a broker, `Isr`, meaning  "in-sync" replicas, will exclude the broker being killed.

## Delete a topic

```
./bin/kafka-topics.sh --delete --bootstrap-server debian:9092,debian:9093,debian:9094 --topic ak-new
```