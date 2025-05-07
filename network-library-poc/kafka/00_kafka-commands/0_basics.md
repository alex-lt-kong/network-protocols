# Basic Commands

* KRaft is the consensus protocol developed to allow metadata management
directly in Apache Kafka (instead of relaying on Apache ZooKeeper).

* Kafka has KRaft support built-in since version 3. As a progressive repo,
here we will focus on making Kafka work with KRaft instead of Apache ZooKeeper.

## Quickstart

* RTFM [here](https://kafka.apache.org/quickstart)

## List topics

```
./bin/kafka-topics.sh --list --bootstrap-server localhost:9093
```

## Check details of a topic

```
./bin/kafka-topics.sh --describe --topic ak-topic --bootstrap-server localhost:9092,localhost:9093,localhost:9094
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