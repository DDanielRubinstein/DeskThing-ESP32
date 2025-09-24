#include <iostream>
#include <string.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp> 

using std::string;
using namespace httpsserver;

namespace AuthServer {
    extern string authorization_code;
    void setupServer();
    void listener();
}