#include<iostream>
#include<cstring>
#include <fstream>

using namespace std;

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

    if (getenv("HTTP_AUTHORIZATION") == NULL) {
        // Environment variable HTTP_AUTHORIZATION is only available after
        // you set CGIPassAuth to On in Apache config file.
        cout << "Content-Type: text/plain\n"
             << "WWW-Authenticate: Basic realm=\"Hi! HTTP Authentication is enabled for this page!\"\n"
             << "Status: 401 Unauthorized\n" << endl;
        return 0; 
    }
    cout << "Content-Type: text/plain\n" << endl;
    cout << "You are in:\n";
    cout << "HTTP_AUTHORIZATION:" << getenv("HTTP_AUTHORIZATION") << endl;
    return 0;
}
