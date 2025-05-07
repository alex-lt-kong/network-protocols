/*
C++ is a language that long-predates JSON, but is still relevant for many projects.
There's no native support for JSON in C++ but there are a number of libraries
that provide support for working with JSON. Perhaps the most widely used is
JsonCpp, available from GitHub at https://github.com/open-source-parsers/jsoncpp
*/
#include<iostream>
#include<chrono>
#include<cstring>
#include <ctime>
#include "json/json.h"
/* 
To make #include "json/json.h" work on Debian, need the following statements
apt install libjsoncpp-dev
ln -s /usr/include/jsoncpp/json/ /usr/include/json
*/

using namespace std;

int main(){

    if (getenv("REQUEST_METHOD") == NULL) {
        // This check is necessary; otherwise strcmp(NULL, str) triggers
        // a segment fault.
        cout << "Environment variable REQUEST_METHOD not set. "
        << "It is most likely that the binary is NOT called by an CGI server!"
        << endl;
        return 1;
    }
    std::time_t t = std::time(0);  // t is an integer type
    cout << "Content-type:application/json" << endl;
    std::cout << "Set-Cookie:count=2;" << endl;
    cout << "\n\n";
    // HTTP messages are composed of textual information encoded in ASCII.
    // An empty line marks the end of the header section
    if(strcmp(getenv("REQUEST_METHOD"),"GET")==0){

        Json::Value root;
        root["REQUEST_METHOD"] = getenv("REQUEST_METHOD");
        
        root["unix_epoch"] = t;
        #ifdef _WIN32 
        root["is_windows"] = true;
        # else
        root["is_windows"] = false;
        #endif
        root["Other CGI environment variables"]["REMOTE"]["ADDR"] = getenv("REMOTE_ADDR") == NULL ? "" : getenv("REMOTE_ADDR");
        root["Other CGI environment variables"]["REMOTE"]["HOST"] = getenv("REMOTE_HOST") == NULL ? "" : getenv("REMOTE_HOST");
        root["Other CGI environment variables"]["HTTP"]["COOKIE"] = getenv("HTTP_COOKIE") == NULL ? "" : getenv("HTTP_COOKIE");
        cout << root << endl;

    } else {
        cout << "Method [" << getenv("REQUEST_METHOD") << "] not supported";
    }
    return 0;
}
