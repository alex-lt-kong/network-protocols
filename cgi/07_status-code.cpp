#include<iostream>
#include<chrono>
#include<cstring>
#include <ctime>
#include <fstream>
#include "json/json.h"

using namespace std;

vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

int handleGET404Status() {

    cout << "Content-Type: text/plain;\n"
         << "Status: 404\n" << endl;
    cout << "Not found" << endl;
    return 0; 
}


int handleGET202Status() {

    cout << "Content-Type: text/plain;\n"
         << "Status: 202 Accepted\n" << endl;
    cout << "Good" << endl;
    return 0; 
}

int main(){

    if (getenv("REQUEST_METHOD") == NULL) {
        // This check is necessary; otherwise strcmp(NULL, str) triggers
        // a segment fault.
        cout << "Content-type:text/plain\n" << endl;
        cout << "Environment variable REQUEST_METHOD not set. "
        << "It is most likely that the binary is NOT called by an CGI server!"
        << endl;
        return 1;
    }
    if (strcmp(getenv("REQUEST_METHOD"), "GET") != 0) {
        cout << "Content-type:text/plain\n" << endl;
        cout << "Method [" << getenv("REQUEST_METHOD") << "] not supported";
        return 1;
    }
    string query = getenv("QUERY_STRING") == NULL ? "" : getenv("QUERY_STRING");

    if (strcmp(query.c_str(), "404") == 0) {
        return handleGET404Status();
    } else 
        return handleGET202Status();
}
