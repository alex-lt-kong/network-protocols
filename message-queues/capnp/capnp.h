#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "../serialization.h"
#include "person.capnp.h"

using namespace std;

person_struct decodeMessageToStructCapnp(kj::Array<capnp::word>& byte_msg);

kj::Array<capnp::word> encodeStructToBytesCapnp(person_struct p);
