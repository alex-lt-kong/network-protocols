## Implementation based on Boost.asio

### Statistics

#### Unicast

- Test with a consumer-grade router and computers

```
./sender -a 192.168.0.22 -p 30001 -m unicast
2025-04-07T02:41:09.634271|884887|    info| Starting sender with method=unicast, interface=0.0.0.0, address=192.168.0.22, port=30001
2025-04-07T02:41:09.634382|884887|    info| 0 msg/sec sent. Latest msg: sent_size=9, seq_num=0, timestamp_us=1743964869634336
2025-04-07T02:41:14.634353|884887|    info| 203553 msg/sec sent. Latest msg: sent_size=13, seq_num=1017766, timestamp_us=1743964874634339
2025-04-07T02:41:19.634358|884887|    info| 202603 msg/sec sent. Latest msg: sent_size=15, seq_num=2030781, timestamp_us=1743964879634343
2025-04-07T02:41:24.634358|884887|    info| 200058 msg/sec sent. Latest msg: sent_size=16, seq_num=3031071, timestamp_us=1743964884634344
2025-04-07T02:41:29.634359|884887|    info| 199851 msg/sec sent. Latest msg: sent_size=14, seq_num=4030330, timestamp_us=1743964889634346
2025-04-07T02:41:34.634361|884887|    info| 201001 msg/sec sent. Latest msg: sent_size=14, seq_num=5035336, timestamp_us=1743964894634347
```

```
./receiver -p 30001 -a 192.168.0.22 -m unicast
2025-04-07T02:40:37.746681|2115332|    info| Starting receiver with method=unicast, interface=0.0.0.0, address=192.168.0.22, port=30001
2025-04-07T02:41:09.638057|2115332|    info| 0 msg/sec received. Latest msg: length=14, seq_num=313, timestamp_us=1743964869635868, latency(ms)=2
2025-04-07T02:41:14.638185|2115332|    info| 143197 msg/sec received. Latest msg: length=15, seq_num=1013529, timestamp_us=1743964874613434, latency(ms)=24
2025-04-07T02:41:19.638244|2115332|    info| 143332 msg/sec received. Latest msg: length=15, seq_num=2027449, timestamp_us=1743964879617862, latency(ms)=20
2025-04-07T02:41:24.638310|2115332|    info| 145916 msg/sec received. Latest msg: length=14, seq_num=3027124, timestamp_us=1743964884614524, latency(ms)=23
2025-04-07T02:41:29.638406|2115332|    info| 147169 msg/sec received. Latest msg: length=14, seq_num=4027280, timestamp_us=1743964889618947, latency(ms)=19
```

- Result comparable to the results in [How to receive a million packets per second](https://blog.cloudflare.com/how-to-receive-a-million-packets/)
