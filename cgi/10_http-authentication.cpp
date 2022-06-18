#include<iostream>
#include<cstring>
#include<fstream>
#include<vector>
#include <algorithm>

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

string base64_decode(const std::string &in) {

    string out;

    vector<int> T(256,-1);
    for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val=0, valb=-8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
        }
    }
    return out;
}

int unauthorized(string debugInfo) {
    cout << "Content-Type: text/plain\n"
        << "WWW-Authenticate: Basic realm=\"Hi! HTTP Authentication is enabled for this page!\"\n"
        << "Info: " << debugInfo << "\n"
        << "Status: 401 Unauthorized\n" << endl;
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

    if (getenv("HTTP_AUTHORIZATION") == NULL) {
        // Environment variable HTTP_AUTHORIZATION is only available after
        // you set CGIPassAuth to On in Apache config file.
        return unauthorized("getenv(\"HTTP_AUTHORIZATION\") == NULL");
    }
    string auth = getenv("HTTP_AUTHORIZATION");
    transform(auth.begin(), auth.end(), auth.begin(), ::tolower); // just .lower(), C++'s way...
    if (auth.rfind("basic ", 0) != 0) {  // just .startsWith(), C++'s way...
        return unauthorized("auth.rfind(\"basic \", 0) != 0");
    }
    auth = getenv("HTTP_AUTHORIZATION");
    auth.erase(0, 6);
    vector<string> cred = split(base64_decode(auth), ":");
    if (cred.size() != 2) {
        return unauthorized("cred.size() != 2");
    }

    cout << "Content-Type: text/plain\n" << endl;
    cout << "You are in:\n";
    cout << "Username:" << cred[0] << "\nPassword:" << cred[1] << endl;
    return 0;
}
