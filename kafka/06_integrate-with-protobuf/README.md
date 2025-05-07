## Install Python dependency

```
pip install protobuf
```

## Generate Python class of a ProtoBuf contract

```
protoc --proto_path=.\src --python_out=.\py .\src\my.proto
```

## Sample outputs

```
> python .\producer.py
=== Human-readable representation of a student object===
id: 9527
first_name: "Pak Fu"
last_name: "Tong"
dob {
  seconds: -15772924800
}
courses {
  name: "Chinese Literature"
  score: 100
}
courses {
  name: "Calculus"
  score: 5
}

===Serialized bytes array of a student object===
b'\x08\xb7J\x12\x06Pak Fu\x1a\x04Tong"\x0b\x08\x80\x89\xf1\x9e\xc5\xff\xff\xff\xff\x01*\x16\n\x12Chinese Literature\x10d*\x0c\n\x08Calculus\x10\x05'
Kafka acknowledged the receipt of the produced message: <cimpl.Message object at 0x00000209D0D30A48>
```

```
> python .\consumer.py
Waiting for message or event/error in poll()
Waiting for message or event/error in poll()
Consumed record with the following human-readable representation:
first_name: "Pak Fu"
last_name: "Tong"
  seconds: -15772924800
}
courses {
  name: "Chinese Literature"
  score: 100
}
courses {
  name: "Calculus"
  score: 5
}

Waiting for message or event/error in poll()
Waiting for message or event/error in poll()
Waiting for message or event/error in poll()
```

## References

* https://developers.google.com/protocol-buffers/docs/pythontutorial