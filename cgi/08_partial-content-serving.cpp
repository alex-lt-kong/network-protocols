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


int handleGET() {

    string content = "hello world!";
    cout << "Content-Type: text/plain;\n"
         << "Accept-Ranges: bytes;\n";
    if (getenv("HTTP_RANGE") != NULL)  {
        string subContent = content.substr(3,5);
        cout << "Status: 206 Partial Content;\n"
             << "Content-Range: bytes 3-5/" << content.length() 
             << "Content-Length: " << subContent.length() << ";\n\n";
        cout << subContent << endl;
    }
    else {
        cout << "Content-Length: " << content.length() << ";\n\n";
        cout << content << endl;
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
    if (strcmp(getenv("REQUEST_METHOD"), "GET") != 0) {
        cout << "Content-type:text/plain\n" << endl;
        cout << "Method [" << getenv("REQUEST_METHOD") << "] not supported";
        return 1;
    }
    string query = getenv("QUERY_STRING") == NULL ? "" : getenv("QUERY_STRING");

    return handleGET();
}
