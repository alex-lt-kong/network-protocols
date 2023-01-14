#include <iostream>
#include <fstream>
#include <string>
#include "person.pb.h"

using namespace std;


// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int main(int argc, char* argv[]) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    pb_test::Person person;
    person.set_name("Hello");
    person.set_address("my addr/haha");
    person.set_nationality("Chinese");
    std::ofstream ofs("/tmp/person.data", std::ios_base::out | std::ios_base::binary);
    person.SerializeToOstream(&ofs);
    //google::protobuf::ShutdownProtobufLibrary();
    ofs.close();
    pb_test::Person person_;
    fstream input("/tmp/person.data", ios::in | ios::binary);

    if (!person_.ParseFromIstream(&input)) {
      cerr << "Failed to parse person_." << endl;
      return -1;
    }

    cout << "address: " << person_.address() << endl;

    return 0;
}
