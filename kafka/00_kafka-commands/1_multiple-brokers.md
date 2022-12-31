# Running multiple brokers

## Configure a 2nd broker

Make a copy of `config/server.properties` and change the following items
```
broker.id=0
# or 1, 2...
listeners=PLAINTEXT://localhost:9093
# or 9094, 9095...
log.dirs=/tmp/kafka-logs-0
# or kafka-logs-1, kafka-logs-2...

offsets.topic.replication.factor=3
transaction.state.log.replication.factor=3
# the above two items are said to make sure that even if a broker is down, Kafka can still be operational

auto.create.topics.enable=false (optional)
```

## Create a topic for multiple brokers

* Topics are **partitioned**, meaning a topic is spread over a number of "buckets" located on different Kafka brokers. 
When a new event is published to a topic, it is actually appended to one of the topic's partitions. 

* To make data fault-tolerant and highly-available, every topic can be replicated so that there are always multiple brokers
that have a copy of the data just in case things go wrong, you want to do maintenance on the brokers, and so on. A common production
setting is a replication factor of 3, i.e., there will always be three copies of your data. This replication is performed at the level of topic-partitions. 

```
bin/kafka-topics.sh --create --partitions 1 --replication-factor 3 --topic latest-topic --bootstrap-server localhost:9092,localhost:9093

# --bootstrap-server as long as one of these brokers are up, Kafka should work
# --config min.insync.replicas=2: the minimum number of brokers that must be up, if it is unset, seems the default value is 1
```


## Check broker status

* List active brokers (`localhost`: zookeeper address, `2181`: zookeeper port)
```
./bin/zookeeper-shell.sh localhost:2181 ls /brokers/ids
```

* Show detailed status of a broker by `id` (`localhost`: zookeeper address, `2181`: zookeeper port, `0`: broker ID)

```
./bin/zookeeper-shell.sh localhost:2181  get /brokers/ids/0
```

