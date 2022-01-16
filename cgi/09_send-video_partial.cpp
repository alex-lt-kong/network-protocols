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

int handleGET_old() {

    cout << "Content-Type: video/mp4;\n"
         << "Accept-Ranges: bytes;\n"
         << "Content-Length: none;" << "\n" << endl;
    ifstream is;
    is.open("./06_send-video.mp4", ios::in | ios::binary);
    if (is.is_open())
    {
        cout << is.rdbuf();
        // basically we read the image as bytes and directly send to cout
    }
    return 0;
}


int handleGET() {

    ifstream in("./06_send-video.mp4", ios::binary | ios::ate);
    size_t fileSize = in.tellg();
    cout << "Content-Type: video/mp4\n"
         << "Accept-Ranges: bytes\n";

    int pos = 0;
    if (getenv("HTTP_RANGE") == NULL)  {
        pos = 0;
    } else {
        string range = getenv("HTTP_RANGE");
        range.erase(0, 6);
        range.pop_back();
        pos = stoi(range);
    }
    size_t bufferSize = 1 * 1024 * 1024;    
    cout << "Status: 206 Partial Content\n"
         << "Content-Range: bytes " << pos << "-" << pos + bufferSize  << "/" << fileSize << "\n"
         << "Content-Length: " << bufferSize  << "\n\n";
    
    char *buffer = new char[bufferSize];
    if (pos + bufferSize > fileSize) 
        bufferSize = fileSize - pos - 1;
    in.seekg(pos, ios::beg);
    
    in.read(buffer, bufferSize);
    cout.write(buffer, bufferSize);
    
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

    return handleGET();
}
