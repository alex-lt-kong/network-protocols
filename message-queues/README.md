# Message Queues

* We will test message queues, such as ZeroMQ, on top of a few few popular
serialization libraries including Protobuf and Capnproto.
  * `apt install protobuf-compiler`
  * [Capnproto](https://capnproto.org/install.html)
  * `apt install libzmq3-dev`
  * [zmqpp](https://github.com/zeromq/zmqpp)

* Question: Protobuf uses variable-length encoding while Canproto uses
fixed-length encoding?
  * Protobuf: https://developers.google.com/protocol-buffers/docs/proto


## Serialization libraries comparison

```
sample data are prepared
===== Testing Capn Proto =====
Serializing   5000000 items takes 7335ms (681615 per sec or 1.5us per record)
Deserializing 5000000 items takes 7215ms (692925 per sec or 1.4us per record)

--- Sample item ---
Id: 1755326552
Name: Bob
Email: Bob@outlook.com
Phone: 350171629(home)
Nationality: Britain
Address: Studio 75x Stacey Underpass, Mollyfurt, Grantborough
Birthday: 1999-12-31
Self introduction: “My name is Lucas Martin, and I enjoy meeting new people and finding ways to help them have an uplifting experience. I have had a variety of customer service opportunities, through which I was able to have fewer returned products and increased repeat customers, when compared with co-workers. I am dedicated, outgoing, and a team player. Who could I speak with in your customer service department about your organization’s customer service needs?”
Scores: [18.584482,32.999378,96.289948,52.114178,43.344822,44.240341,85.414139,30.774570,43.910721,52.277302,47.839672,66.868645,82.116493,20.011988,36.576073,30.663294,]



===== Testing ProtoBuf =====
Serializing   5000000 items takes 19874ms (251575 per sec or 4us per record)
Deserializing 5000000 items takes 15368ms (325343 per sec or 3.1us per record)

--- Sample item ---
Id: 1699323676
Name: Frankenstein
Email: Frankenstein@proton.me
Phone: 547690054(home)
Nationality: Finland
Address: Voortmanring 9-p, Beek, Zuid-Holland
Birthday: 0001-01-01
Self introduction: I have done my schooling from Jaipur. For my graduation, I chose Xyz program at Abc University. It was a very enriching experience at the University as not only we were actively involved in practical projects, but also got opportunities to participate in a number of sports and other extra-curricular activities such as plays and skits.
Scores: [26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,26.923361,]

```