#include "person.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>

using namespace std;


void printAddressBook(int fd) {
  ::capnp::PackedFdMessageReader message(fd);

  Person::Reader person = message.getRoot<Person>();
    std::cout << person.getName().cStr() << ": "
                << person.getEmail().cStr() << std::endl;
    for (Person::PhoneNumber::Reader phone: person.getPhones()) {
        const char* typeName = "UNKNOWN";
        switch (phone.getType()) {
        case Person::PhoneNumber::Type::MOBILE: typeName = "mobile"; break;
        case Person::PhoneNumber::Type::HOME: typeName = "home"; break;
        case Person::PhoneNumber::Type::WORK: typeName = "work"; break;
        }
        std::cout << "  " << typeName << " phone: "
                << phone.getNumber() << std::endl;
    }
    Person::Employment::Reader employment = person.getEmployment();
    cout << "  nationality: " << person.getNationality().cStr() << endl;
    cout << "  address: " << person.getAddress().cStr() << endl;
    cout << "  birthday: " << person.getBitrthday().cStr() << endl;
    switch (employment.which()) {
        case Person::Employment::UNEMPLOYED:
        std::cout << "  unemployed" << std::endl;
        break;
        case Person::Employment::EMPLOYER:
        std::cout << "  employer: "
                    << employment.getEmployer().cStr() << std::endl;
        break;
        case Person::Employment::SCHOOL:
        std::cout << "  student at: "
                    << employment.getSchool().cStr() << std::endl;
        break;
        case Person::Employment::SELF_EMPLOYED:
        std::cout << "  self-employed" << std::endl;
        break;
    }
  
}


int main() { 
    int fd = open("/tmp/ramdisk/capnp.data", O_CREAT | O_WRONLY);
    if (fd == -1)  {
        fprintf(stderr, "open() error: %s\n", strerror(errno));
        return -1;
    }
    clock_t start = clock(), diff;
    printAddressBook(fd);
    diff = clock() - start;
    close(fd);
    return 0;
}