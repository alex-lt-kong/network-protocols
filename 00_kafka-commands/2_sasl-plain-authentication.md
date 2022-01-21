# SASL Plain Authentication

Refernece is made to this VMWare document: 
https://docs.vmware.com/en/VMware-Smart-Assurance/10.1.0/sa-ui-installation-config-guide-10.1.0/GUID-3E473EC3-732A-4963-81BD-13BCCD3AC700.html

## Modify the `server-[i].properties` file

Make sure the following rows exist

```
listeners=SASL_PLAINTEXT://:9092
security.inter.broker.protocol=SASL_PLAINTEXT
sasl.mechanism.inter.broker.protocol=PLAIN
sasl.enabled.mechanisms=PLAIN
authorizer.class.name=kafka.security.authorizer.AclAuthorizer
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

You can save these files to wherever you like

### zookeeper_jaas.conf

```
Server {
org.apache.zookeeper.server.auth.DigestLoginModule required
  user_super="zookeeper"
  user_admin="admin-secret";
};
```

### kafka_server_jaas.conf

```
KafkaServer {
  org.apache.kafka.common.security.plain.PlainLoginModule required
  username="admin"
  password="admin-secret"
  user_admin="admin-secret";
};
Client {
  org.apache.zookeeper.server.auth.DigestLoginModule required
  username="admin"
  password="admin-secret";
};
```

### kafka_client_jaas.conf

This conf file is only needed if you want to run
`./bin/kafka-console-consumer.sh` as a test consumer
```
KafkaClient {
  org.apache.kafka.common.security.plain.PlainLoginModule required
  username="admin"
  password="admin-secret";
};
Client {
  org.apache.zookeeper.server.auth.DigestLoginModule required
  username="admin"
  password="admin-secret";
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

### Consumer

```
KAFKA_OPTS="-Djava.security.auth.login.config=./jaas/kafka_client_jaas.conf" ./bin/kafka-console-consumer.sh --topic test-topic --from-beginning config/consumer.properties --bootstrap-server=localhost:9092,localhost:9093,localhost:9094
```
