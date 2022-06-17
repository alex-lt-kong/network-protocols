# SASL Plain Authentication

Reference is made to this VMWare document: 
https://docs.vmware.com/en/VMware-Smart-Assurance/10.1.0/sa-ui-installation-config-guide-10.1.0/GUID-3E473EC3-732A-4963-81BD-13BCCD3AC700.html

## Modify the `server-[i].properties` file

Make sure the following rows exist

```
listeners=SASL_PLAINTEXT://:9092
security.inter.broker.protocol=SASL_PLAINTEXT
sasl.mechanism.inter.broker.protocol=PLAIN
sasl.enabled.mechanisms=PLAIN
authorizer.class.name=kafka.security.authorizer.AclAuthorizer
# `kafka.security.authorizer.AclAuthorizer` was introduced in Apache Kafka® 2.4/Confluent Platform 5.4.0. Prior to that, the authorizer was kafka.security.auth.SimpleAclAuthorizer.
allow.everyone.if.no.acl.found=true
# Use of the allow.everyone.if.no.acl.found=true configuration option in production environments is strongly discouraged.
```

## Modify the `zookeeper.properties` file

Make sure the following rows exist

```
authProvider.1=org.apache.zookeeper.server.auth.SASLAuthenticationProvider
requireClientAuthScheme=sasl
jaasLoginRenew=3600000
```

## Modify the `consumer.properties` file

Make sure the follow rows exist

```
security.protocol=SASL_PLAINTEXT
sasl.mechanism=PLAIN
```

## Create JAAS configuration files

Save these files to wherever you like--their paths are added as a parameter.

### zookeeper_jaas.conf

```
Server {
org.apache.zookeeper.server.auth.DigestLoginModule required
  user_super="zookeeper"
  user_admin="admin-secret"
  user_zkadmin="zkpwd";
};
# username is zkadmin and password is zkpwd
```

### kafka_server_jaas.conf

```
# KafkaServer section defines the credentials used by clients 
# (i.e., producers and consumers) to connect to brokers
# Client section defines the credentials used by brokers to connect
# zookeepers
KafkaServer {
  org.apache.kafka.common.security.plain.PlainLoginModule required
  username="admin"
  password="admin-secret"
  user_admin="admin-secret"
  user_admin1="password666"
  user_helloyou="lol";
};
Client {
  org.apache.zookeeper.server.auth.DigestLoginModule required
  username="zkadmin"
  password="zkpwd";
};
```

### kafka_client_jaas.conf

This conf file is only needed if you want to run
`./bin/kafka-console-consumer.sh` as a test consumer
```
# KafkaClient section defines the credentials used by clients to connect
# to brokers.
KafkaClient {
  org.apache.kafka.common.security.plain.PlainLoginModule required
  username="helloyou"
  password="lol";
};
```

## Start services with `KAFKA_OPTS`

### Zookeeper

```
KAFKA_OPTS="-Djava.security.auth.login.config=./jaas/zookeeper_jaas.conf" bin/zookeeper-server-start.sh config/zookeeper.properties
```

### Brokers

```
KAFKA_OPTS="-Djava.security.auth.login.config=./jaas/kafka_server_jaas.conf" bin/kafka-server-start.sh config/server-[i].properties
```

### Producers

Projects `04_csharp-sasl-plain-producer` and `05_cpp-sasl-plain-producer` are
producers with SASL Plain Authentication support developed in C# and C++.

### Consumer

```
KAFKA_OPTS="-Djava.security.auth.login.config=./jaas/kafka_client_jaas.conf" ./bin/kafka-console-consumer.sh --topic ak-topic --from-beginning --consumer.config=config/consumer.properties --bootstrap-server=172.16.0.2:9092,172.16.0.2:9093,172.16.0.2:9094
```

## Common operations (with SASL Plain Authentication enabled)

Enabling authentication breaks almost everything that documented in `0_basic.md` and `1_multiple-brokers.md`...

### Common config file `config/config.properties`

```
sasl.jaas.config=org.apache.kafka.common.security.plain.PlainLoginModule required username="admin" password="admin-secret";
security.protocol=SASL_PLAINTEXT
sasl.mechanism=PLAIN
```

### List topics

According to my test, no `KAFKA_OPTS` is needed

```
./bin/kafka-topics.sh --list --bootstrap-server 172.16.0.2:9092,172.16.0.2:9093,172.16.0.2:9094 --command-config config/config.properties
```

### Check details of a topic

According to my test, no `KAFKA_OPTS` is needed

```
./bin/kafka-topics.sh --describe --topic ak-topic --bootstrap-server 172.16.0.2:9094 --command-config config/config.properties
```

### Create a topic for multiple brokers

According to my test, no `KAFKA_OPTS` is needed

```
./bin/kafka-topics.sh --create --partitions 1 --replication-factor 3 --topic ak-topic --bootstrap-server 172.16.0.2:9092,172.16.0.2:9093,172.16.0.2:9094 --command-config config/config.properties
```