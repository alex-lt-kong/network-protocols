#ifndef SERIALIZATION_H
#define SERIALIZATION_H


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


using namespace std;

#define TEST_SIZE 2000000UL

struct person_struct {
    uint32_t id;
    string name;
    string email;
    int phone_type;
    uint32_t phone_number;
    string school;
    string nationality;
    string address;
    string birthday;
    string creation_date;
    string update_date;
    string self_introduction;
    float scores[16];
};

string getStringRepresentationOfPerson(person_struct p);
float getRandomFloat(float a, float b);
vector<person_struct> generateRandomData();

#endif
