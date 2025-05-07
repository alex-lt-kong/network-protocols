# Modified from this source file:
# https://github.com/confluentinc/examples/blob/7.0.1-post/clients/cloud/python/consumer.py

from confluent_kafka import Consumer

import socket
import student_pb2


conf = {
        'bootstrap.servers': "uat.lan:5092,uat.lan:5093,uat.lan:5094",
        'client.id': socket.gethostname(),
        'group.id': 'test-group'
        }
topic = 'latest-topic'
consumer = Consumer(conf)
consumer.subscribe([topic])

try:
    while True:
        msg = consumer.poll(2.0)
        if msg is None:
            print("Waiting for message or event/error in poll()")
            continue
        elif msg.error():
            print('error: {}'.format(msg.error()))
        else:
            # Check for Kafka message
            record_key = msg.key()
            record_value = msg.value()
            student = student_pb2.Student()
            student.ParseFromString(record_value)
            print(f"Consumed record with the following human-readable representation:\n{student}")
except KeyboardInterrupt:
    pass
finally:
    # Leave group and commit final offsets
    consumer.close()