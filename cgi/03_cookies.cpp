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

int main(){

    if (getenv("REQUEST_METHOD") == NULL) {
        // This check is necessary; otherwise strcmp(NULL, str) triggers
        // a segment fault.
        cout << "Content-type:text/plain\n" << endl;
        cout << "Environment variable REQUEST_METHOD not set. "
        << "It is most likely that the binary is NOT called by an CGI server!"
        << endl;
        return 1;
    } else if (strcmp(getenv("REQUEST_METHOD"), "GET") != 0) {
        cout << "Content-type:text/plain\n" << endl;
        cout << "Method [" << getenv("REQUEST_METHOD") << "] not supported";
        return 1;
    }

    string queryString = getenv("QUERY_STRING");
    
    cout << "Content-type:application/json" << endl;
    vector<string> paras = split(queryString, ";");
    if (paras.size() > 1) {
        cout << "Set-Cookie:user=" << paras[0] << ";" << endl;
        cout << "Set-Cookie:password=" << paras[1] << ";" << endl;
        // the key difference between \n and endl:
        // endl causes a flushing of the output buffer every time it is called
        // whereas \n does not.
    }
    cout << "\n";
    // HTTP messages are composed of textual information encoded in ASCII.
    // An empty line marks the end of the header section
    

    Json::Value root;
    root["REQUEST_METHOD"] = getenv("REQUEST_METHOD");
    std::time_t t = std::time(0);  // t is an integer type
    root["unix_epoch"] = t;
    #ifdef _WIN32 // Works even for 64bit Windows lol
    root["is_windows"] = true;
    # else
    root["is_windows"] = false;
    #endif
    root["Other CGI environment variables"]["REMOTE"]["ADDR"] = getenv("REMOTE_ADDR") == NULL ? "" : getenv("REMOTE_ADDR");
    root["Other CGI environment variables"]["REMOTE"]["HOST"] = getenv("REMOTE_HOST") == NULL ? "" : getenv("REMOTE_HOST");
    root["Other CGI environment variables"]["HTTP"]["COOKIE"] = getenv("HTTP_COOKIE") == NULL ? "" : getenv("HTTP_COOKIE");
    root["Other CGI environment variables"]["HTTP"]["UA"] = getenv("HTTP_USER_AGENT") == NULL ? "" : getenv("HTTP_USER_AGENT");
    cout << root << endl;
    
    return 0;
}
