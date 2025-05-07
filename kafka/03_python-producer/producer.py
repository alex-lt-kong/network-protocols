from confluent_kafka import Producer

import datetime as dt
import json
import socket


conf = {'bootstrap.servers': "debian:9092,debian:9093,debian:9094",
        'client.id': socket.gethostname()}

topic = 'test-topic'

producer = Producer(conf)

def acked(err, msg):
    if err is not None:
        print("Failed to deliver message: %s: %s" % (str(msg), str(err)))
    else:
        print("Message produced: %s" % (str(msg)))

alert_dict = {
  "Message": "Test message from a python client, timestamp from Python " + dt.datetime.now().strftime('%Y%m%d-%H%M%S'),
  "Level": "Error",
  "AlertID": "Jan17Alert",
  "Subcategory": "SubCatInfo",
  "Status": "Passed",
  "published_at": "2022-01-21T02:15:30.276Z",
  "created_by": "string",
  "updated_by": "string"
}

producer.produce(topic, key="key", value=json.dumps(alert_dict), callback=acked)

# Wait up to 1 second for events. Callbacks will be invoked during
# this method call if the message is acknowledged.
producer.poll(1)
