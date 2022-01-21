# SASL Plain Authentication

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
authProvider.1=org.apache.zookeeper.server.auth.SASLAuthenticationProvideri
requireClientAuthScheme=sasl
jaasLoginRenew=3600000
```