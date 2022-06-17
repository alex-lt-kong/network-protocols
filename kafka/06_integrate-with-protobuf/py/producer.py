from confluent_kafka import Producer

import student_pb2
import datetime as dt
import socket

student = student_pb2.Student()
student.id = 9527
student.first_name = 'Pak Fu'
student.last_name = 'Tong'
student.dob.FromDatetime(dt.datetime(1470, 3, 6))
# There is a nasty trap: we canNOT assign a google.protobuf.Timestamp object to
# dob, instead, the object has already been there, we can only manipulate its
# value in-place.

courses = [student_pb2.Course(),student_pb2.Course()]
courses[0].name = 'Chinese Literature'
courses[0].score = 100
courses[1].name = 'Calculus'
courses[1].score = 5

student.courses.extend(courses)

print(f'=== Human-readable representation of a student object===\n{student}\n\n')
print(f'===Serialized bytes array of a student object===\n{student.SerializeToString()}')


def acked(err, msg):
    if err is not None:
        print("Failed to deliver message: %s: %s" % (str(msg), str(err)))
    else:
        print(f"Kafka acknowledged the receipt of the produced message: {msg}")

conf = {'bootstrap.servers': "uat.lan:5092,uat.lan:5093,uat.lan:5094",
        'client.id': socket.gethostname()}
topic = 'latest-topic'
producer = Producer(conf)

producer.produce(topic, key="key", value=student.SerializeToString(), callback=acked)
producer.poll(1)
