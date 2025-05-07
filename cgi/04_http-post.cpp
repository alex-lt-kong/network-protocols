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

int handleGET() {
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
    root["is_server_windows"] = true;
    # else
    root["is_server_windows"] = false;
    #endif
    root["Other CGI environment variables"]["REMOTE"]["ADDR"] = getenv("REMOTE_ADDR") == NULL ? "" : getenv("REMOTE_ADDR");
    root["Other CGI environment variables"]["REMOTE"]["HOST"] = getenv("REMOTE_HOST") == NULL ? "" : getenv("REMOTE_HOST");
    root["Other CGI environment variables"]["HTTP"]["COOKIE"] = getenv("HTTP_COOKIE") == NULL ? "" : getenv("HTTP_COOKIE");
    root["Other CGI environment variables"]["HTTP"]["UA"] = getenv("HTTP_USER_AGENT") == NULL ? "" : getenv("HTTP_USER_AGENT");
    cout << root << endl;
    
    return 0;
}

int handlePOST() {
    // To test POST:
    // curl --request POST --data "username=test-user" --data "password=Passw0rd" "http://localhost/cgi-test/04_http-post.cgi"
    cout << "Content-type:text/plain\n" << endl;
    int len;
    char* lenstr = getenv("CONTENT_LENGTH");

    /*
    Reading the “post” data is a bit more complicated.
    The data needs to be read from the standard input,
    but the program won’t receive an EOF when it reaches the end of the data
    but instead it should stop reading after reading a specified amount of bytes,
    which is defined in the environment variable "CONTENT_LENGTH".
    So you should read getenv("CONTENT_LENGTH") bytes from the standard input
    to receive the “post” data. 
    */
    if(lenstr != NULL && (len = atoi(lenstr)) != 0){
        // atoi : Parse the C-string str interpreting its content as an integral number
        char* post_data = new char[len];
        fgets(post_data,len+1,stdin);
        cout << "Data received from POST request:\n" << post_data << endl;;
    } else {
        cout << "No data are received from POST request" << endl;
    }
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
    if (strcmp(getenv("REQUEST_METHOD"), "GET") * strcmp(getenv("REQUEST_METHOD"), "POST") != 0) {
        cout << "Content-type:text/plain\n" << endl;
        cout << "Method [" << getenv("REQUEST_METHOD") << "] not supported";
        return 1;
    }

    if (strcmp(getenv("REQUEST_METHOD"), "GET") == 0)  {
        return handleGET();
    } else {
        return handlePOST();
    }

}
