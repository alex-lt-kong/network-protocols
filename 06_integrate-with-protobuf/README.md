## Install Python dependency

```
pip install protobuf
```

## Generate Python class of a ProtoBuf contract

```
protoc --proto_path=.\src --python_out=.\py .\src\my.proto
```

## References

* https://developers.google.com/protocol-buffers/docs/pythontutorial