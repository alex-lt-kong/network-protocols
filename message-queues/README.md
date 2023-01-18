# Message Queues

* We will test message queues, such as ZeroMQ, on top of a few popular
serialization libraries including ProtoBuf and Capnproto.
  * `apt install protobuf-compiler`
  * [Capnproto](https://capnproto.org/install.html)
  * `apt install libzmq3-dev`
  * [zmqpp](https://github.com/zeromq/zmqpp)

* Question: Protobuf uses variable-length encoding while Canproto uses
fixed-length encoding?
  * Protobuf: https://developers.google.com/protocol-buffers/docs/proto
  * https://stackoverflow.com/questions/61347404/protobuf-vs-flatbuffers-vs-capn-proto-which-is-faster

## Serialization libraries comparison

```
Generating random data...
Random data are prepared

===== Testing CapnProto =====
Serializing   2000000 items takes 2437ms (820346 per sec or 1.2us per record). Average size: 737 bytes
Deserializing 2000000 items takes 2012ms (993896 per sec or 1us per record)

--- Sample item ---
Id: 2131682895
Name: Charlie
Email: Charlie@yahoo.com
Phone: 1816214383(home)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [93.622932, 11.812199, 25.208839, 19.163185, 26.207649, 88.873962, 80.516075, 75.689217, 62.897102, 84.709427, 36.256771, 76.789230, 76.569679, 79.155983, 23.007631, 52.965248, ]



===== Testing ProtoBuf =====
Serializing   2000000 items takes 6454ms (309880 per sec or 3.2us per record). Average size: 616 bytes
Deserializing 2000000 items takes 5791ms (345348 per sec or 2.9us per record)

--- Sample item ---
Id: 1881309369
Name: Frankenstein
Email: Frankenstein@proton.me
Phone: 2110298390(work)
Nationality: Finland
Address: Voortmanring 9-p, Beek, Zuid-Holland
Birthday: 0001-01-01
Self introduction: I have done my schooling from Jaipur. For my graduation, I chose Xyz program at Abc University. It was a very enriching experience at the University as not only we were actively involved in practical projects, but also got opportunities to participate in a number of sports and other extra-curricular activities such as plays and skits.
Scores: [87.786484, 36.084415, 17.574436, 94.353554, 57.835590, 34.006779, 78.929840, 20.176176, 72.734360, 42.735447, 61.995369, 57.316875, 98.661377, 51.499470, 47.462971, 76.098129, ]



Generating random data...
Random data are prepared

===== Testing CapnProto (batch_size: 1, batch_count: 2000000)=====
Serializing   2000000 items takes 2175ms (919445 per sec or 1.1us per record)
Deserializing 2000000 items takes 4797ms (416889 per sec or 2.4us per record)

--- Sample item ---
Id: 1395273613
Name: Charlie
Email: Charlie@yahoo.com
Phone: 64993291(home)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [4.323489, 61.395763, 69.521782, 39.093693, 59.727036, 26.482618, 28.224867, 73.223228, 33.775368, 77.035622, 0.071238, 78.657768, 55.266029, 89.797623, 75.272522, 67.522392, ]



===== Testing ProtoBuf (batch_size: 1, batch_count: 2000000)=====
Serializing   2000000 items takes 7063ms (283144 per sec or 3.5us per record)
Deserializing 2000000 items takes 6332ms (315850 per sec or 3.2us per record)

--- Sample item ---
Id: 1445520195
Name: Bob
Email: Bob@outlook.com
Phone: 48418525(home)
Nationality: Britain
Address: Studio 75x Stacey Underpass, Mollyfurt, Grantborough
Birthday: 1999-12-31
Self introduction: “My name is Lucas Martin, and I enjoy meeting new people and finding ways to help them have an uplifting experience. I have had a variety of customer service opportunities, through which I was able to have fewer returned products and increased repeat customers, when compared with co-workers. I am dedicated, outgoing, and a team player. Who could I speak with in your customer service department about your organization’s customer service needs?”
Scores: [55.234138, 96.145309, 47.741020, 81.062233, 67.371155, 2.004593, 27.931244, 6.882428, 51.198284, 2.079007, 10.920374, 16.057808, 83.941566, 47.875656, 37.769924, 14.001356, ]



Generating random data...
Random data are prepared

===== Testing CapnProto (batch_size: 2, batch_count: 1000000)=====
Serializing   2000000 items takes 1971ms (1014683 per sec or 0.99us per record)
Deserializing 2000000 items takes 4627ms (432233 per sec or 2.3us per record)

--- Sample item ---
Id: 159214891
Name: Frankenstein
Email: Frankenstein@proton.me
Phone: 1589206329(mobile)
Nationality: Finland
Address: Voortmanring 9-p, Beek, Zuid-Holland
Birthday: 0001-01-01
Self introduction: I have done my schooling from Jaipur. For my graduation, I chose Xyz program at Abc University. It was a very enriching experience at the University as not only we were actively involved in practical projects, but also got opportunities to participate in a number of sports and other extra-curricular activities such as plays and skits.
Scores: [74.985069, 40.226219, 66.682861, 57.654526, 58.174927, 63.022198, 72.321915, 72.081139, 99.266266, 52.652390, 5.757673, 72.343048, 77.983139, 30.605925, 48.720501, 1.349972, ]



===== Testing ProtoBuf (batch_size: 2, batch_count: 1000000)=====
Serializing   2000000 items takes 6587ms (303607 per sec or 3.3us per record)
Deserializing 2000000 items takes 6225ms (321237 per sec or 3.1us per record)

--- Sample item ---
Id: 87426709
Name: Bob
Email: Bob@outlook.com
Phone: 1751392260(mobile)
Nationality: Britain
Address: Studio 75x Stacey Underpass, Mollyfurt, Grantborough
Birthday: 1999-12-31
Self introduction: “My name is Lucas Martin, and I enjoy meeting new people and finding ways to help them have an uplifting experience. I have had a variety of customer service opportunities, through which I was able to have fewer returned products and increased repeat customers, when compared with co-workers. I am dedicated, outgoing, and a team player. Who could I speak with in your customer service department about your organization’s customer service needs?”
Scores: [2.128808, 33.429230, 12.336354, 83.839813, 74.841049, 75.926811, 25.940340, 78.365036, 63.653564, 62.917095, 29.815397, 86.632545, 93.818703, 74.466057, 53.509487, 69.195457, ]



Generating random data...
Random data are prepared

===== Testing CapnProto (batch_size: 4, batch_count: 500000)=====
Serializing   2000000 items takes 1782ms (1122174 per sec or 0.89us per record)
Deserializing 2000000 items takes 4568ms (437735 per sec or 2.3us per record)

--- Sample item ---
Id: 1229951285
Name: Charlie
Email: Charlie@yahoo.com
Phone: 2104878775(home)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [70.234772, 43.210785, 83.106461, 32.743999, 88.344780, 19.645006, 40.009834, 90.550423, 28.563166, 5.352758, 11.808709, 28.194025, 78.122879, 48.365860, 74.652695, 62.304478, ]



===== Testing ProtoBuf (batch_size: 4, batch_count: 500000)=====
Serializing   2000000 items takes 6392ms (312863 per sec or 3.2us per record)
Deserializing 2000000 items takes 6157ms (324797 per sec or 3.1us per record)

--- Sample item ---
Id: 1729671406
Name: Charlie
Email: Charlie@yahoo.com
Phone: 1611940006(home)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [14.720157, 62.611343, 55.000706, 25.038464, 93.565544, 18.724028, 64.357605, 70.986923, 21.630535, 92.513069, 7.452739, 89.954781, 14.516685, 11.854578, 24.830687, 16.720741, ]



Generating random data...
Random data are prepared

===== Testing CapnProto (batch_size: 8, batch_count: 250000)=====
Serializing   2000000 items takes 1672ms (1195715 per sec or 0.84us per record)
Deserializing 2000000 items takes 4545ms (440000 per sec or 2.3us per record)

--- Sample item ---
Id: 1289262840
Name: Dave
Email: Dave@qq.com
Phone: 2025594046(home)
Nationality: Dominica
Address: Rácz üdülőpart 813., Budapest, Nógrád
Birthday: 1111-11-11
Self introduction: “I am a dedicated person with a family of four. I enjoy reading, and the knowledge and perspective that my reading gives me has strengthened my teaching skills and presentation abilities. I have been successful at raising a family, and I attribute this success to my ability to plan, schedule, and handle many different tasks at once. This flexibility will help me in the classroom, where there are many different personalities and learning styles.”
Scores: [78.357040, 12.453821, 41.047951, 18.940929, 23.333363, 28.775602, 52.527744, 54.991604, 35.909214, 26.061073, 94.084152, 80.063568, 8.181577, 76.066971, 95.840004, 40.739491, ]



===== Testing ProtoBuf (batch_size: 8, batch_count: 250000)=====
Serializing   2000000 items takes 6392ms (312851 per sec or 3.2us per record)
Deserializing 2000000 items takes 6035ms (331396 per sec or 3us per record)

--- Sample item ---
Id: 132571615
Name: Charlie
Email: Charlie@yahoo.com
Phone: 109653942(mobile)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [20.940092, 27.044720, 23.718994, 84.205139, 82.245865, 89.672348, 7.127998, 32.048588, 62.930264, 49.621235, 75.857956, 75.513847, 99.635300, 77.403801, 98.378853, 33.800022, ]



Generating random data...
Random data are prepared

===== Testing CapnProto (batch_size: 16, batch_count: 125000)=====
Serializing   2000000 items takes 1719ms (1163412 per sec or 0.86us per record)
Deserializing 2000000 items takes 5153ms (388102 per sec or 2.6us per record)

--- Sample item ---
Id: 1725960754
Name: Charlie
Email: Charlie@yahoo.com
Phone: 1522611689(work)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [77.357254, 7.868674, 63.090317, 23.136822, 89.402878, 32.203213, 75.198845, 69.017258, 88.747345, 62.799919, 88.719879, 33.016567, 3.635854, 80.734024, 86.866989, 66.731903, ]



===== Testing ProtoBuf (batch_size: 16, batch_count: 125000)=====
Serializing   2000000 items takes 6315ms (316657 per sec or 3.2us per record)
Deserializing 2000000 items takes 6189ms (323128 per sec or 3.1us per record)

--- Sample item ---
Id: 301339702
Name: Charlie
Email: Charlie@yahoo.com
Phone: 1965917158(home)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [36.571079, 37.673615, 11.397442, 31.413273, 33.446281, 0.411436, 73.003235, 87.567848, 16.588623, 49.642361, 78.695892, 2.646809, 52.903549, 93.813965, 2.798166, 33.302570, ]



Generating random data...
Random data are prepared

===== Testing CapnProto (batch_size: 200, batch_count: 10000)=====
Serializing   2000000 items takes 1745ms (1145552 per sec or 0.87us per record)
Deserializing 2000000 items takes 6790ms (294550 per sec or 3.4us per record)

--- Sample item ---
Id: 646184660
Name: Dave
Email: Dave@qq.com
Phone: 1569469416(mobile)
Nationality: Dominica
Address: Rácz üdülőpart 813., Budapest, Nógrád
Birthday: 1111-11-11
Self introduction: “I am a dedicated person with a family of four. I enjoy reading, and the knowledge and perspective that my reading gives me has strengthened my teaching skills and presentation abilities. I have been successful at raising a family, and I attribute this success to my ability to plan, schedule, and handle many different tasks at once. This flexibility will help me in the classroom, where there are many different personalities and learning styles.”
Scores: [43.258873, 83.775291, 93.566208, 68.680504, 18.711777, 72.441414, 69.743210, 54.226143, 72.038132, 99.891815, 10.845010, 80.842697, 94.164566, 85.171280, 46.712883, 42.302704, ]



===== Testing ProtoBuf (batch_size: 200, batch_count: 10000)=====
Serializing   2000000 items takes 6681ms (299324 per sec or 3.3us per record)
Deserializing 2000000 items takes 6642ms (301089 per sec or 3.3us per record)

--- Sample item ---
Id: 1064853501
Name: Charlie
Email: Charlie@yahoo.com
Phone: 1957526755(home)
Nationality: China
Address: 63 Nok Teu Ya East Road, New Territories, Kowloon
Birthday: 2000-01-01
Self introduction: “People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”
Scores: [10.861562, 73.538254, 68.866600, 28.669861, 58.154942, 73.720703, 4.995245, 1.619596, 90.260208, 67.670654, 16.967096, 75.845520, 18.168161, 30.043734, 62.899578, 58.275814, ]


```

* Let's include C# versions of them, just for fun.

```
===== Testing CapnProto =====
sample data are prepared
Serializing   2000000 items takes 18047 ms (110821.74 records per sec or 9.02 us per record)
Deserializing 2000000 items takes 10653 ms (187740.54 records per sec or 5.33 us per record)
--- Sample item ---
Id: 913228427
Name: Bob
E-mail: Bob@outlook.com
Self-intro: "My name is Lucas Martin, and I enjoy meeting new people and finding ways to help them have an uplifting experience. I have had a variety of customer service opportunities, through which I was able to have fewer returned products and increased repeat customers, when compared with co-workers. I am dedicated, outgoing, and a team player. Who could I speak with in your customer service department about your organization's customer service needs?"
work: 1887492300
Scores: [51, 70.6, 22.2, 17.9, 7, 48.3, 22.1, 6.9, 58.2, 69.7, 32.4, 39, 66.8, 53.4, 66.5, 46.8, ] 


===== Testing ProtoBuf =====
sample data are prepared
Serializing  2000000 items takes 7709 ms (259437.02 records per sec or 3.85 us per record)
Deserializing 2000000 items takes 20340 ms (98328.42 records per sec or 10.17 us per record)
--- Sample item ---
Id: 55666510
Name: Ella
E-mail: Ella@icloud.com
Self-intro: I have moved to this city three months ago. I love the street food here. On weekends, I explore new eating joints. This way, I get to learn the routes of this city and prepare myself professionally. Being a sales professional, my profile requires a lot of traveling.
Home: 5551212
Scores: [33.1, 72.5, 47, 47.7, 89.3, 20.5, 57.4, 6.6, 23.8, 21.1, 53.7, 71.2, 92.8, 10.9, 57.8, 2.8, ]   
```

## ZeroMQ benchmarking

* `zmq-lient.out`:
  ```
  Opening connection to tcp://localhost:4242...
  Filling 2000000 items with random data...
  Serializing 2000000 items for ZeroMQ...
  Sending 2000000 items over ZeroMQ...
  Sending 2000000 items takes 8045ms (248593 per sec or 4us per record)
  ```
* `zmq-server.out`:
  ```
  Binding to tcp://*:4242...
  Id: 1316074830
  Name: Dave
  Email: Dave@qq.com
  Phone: 606587996(work)
  Nationality: Dominica
  Address: Rácz üdülőpart 813., Budapest, Nógrád
  Birthday: 1111-11-11
  Self introduction: “I am a dedicated person with a family of four. I enjoy reading, and the knowledge and perspective that my reading gives me has strengthened my teaching skills and presentation abilities. I have been successful at raising a family, and I attribute this success to my ability to plan, schedule, and handle many different tasks at once. This flexibility will help me in the classroom, where there are many different personalities and learning styles.”
  Scores: [33.575752, 65.372322, 52.944416, 56.873955, 99.711571, 49.082947, 22.428776, 75.558029, 26.563078, 45.914017, 80.841522, 71.072426, 39.001526, 41.962193, 16.374081, 5.806120, ]
  ```