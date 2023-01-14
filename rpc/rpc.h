#ifndef RPC_H
#define RPC_H


#include <string>

using namespace std;

struct person_struct {
    uint32_t id;
    string name;
    string email;
    uint32_t phone_number;
    string school;
    string nationality;
    string address;
    string birthday;
    string creation_date;
    string update_date;
    string self_introduction;
};

#endif