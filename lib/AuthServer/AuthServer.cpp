#include <iostream>
#include <string.h>

#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp> 

using std::string;
using namespace httpsserver;

#include "../Env/env.h"
#include "cert.h"
#include "private_key.h"

namespace AuthServer {
    const string spotify_redirect_uri = "https://esp.surf-pads.com/callback";

    string authorization_code;

    // import self-signed certificate 
    SSLCert cert = SSLCert(
      crt_DER, crt_DER_len,
      key_DER, key_DER_len
    );
    
    HTTPSServer secureServer = HTTPSServer(&cert);
    
    // returns basic html page
    void generateHTMLResponse(HTTPResponse * res, string additional_content) {
        res->setHeader("Content-Type", "text/html");

        res->println("<!DOCTYPE html>");
        res->println("<html>");
        res->println("<head><title>DeskThing</title></head>");
        res->printf("<h1> %s </h1>\n", additional_content.c_str());
        res->println("</body>");
        res->println("</html>");
    }

    // redirect to spotify log-in page
    void handleRoot(HTTPRequest * req, HTTPResponse * res) {
        res->setHeader(
            "Location",
            "https://accounts.spotify.com/authorize?response_type=code&client_id=" + env::client_id + "&redirect_uri=" + spotify_redirect_uri + "&scope=user-modify-playback-state"
        );
        res->setStatusCode(302);
    }       
    
    // redirect from spotify /callback endpoint
    void handleSpotifyRedirect(HTTPRequest * req, HTTPResponse * res) {
        string message = req->getParams()->getQueryParameter("code", authorization_code) ? "Success. You may close this page now." : "Something went wrong.\n";
        
        generateHTMLResponse(res, message);
    }

    void handleDefault(HTTPRequest * req, HTTPResponse * res) {
        generateHTMLResponse(res, "404, is this what you were looking for?");
    }

    void setupServer() {
        ResourceNode * nodeRoot = new ResourceNode("/", "GET", &handleRoot);
        ResourceNode * nodeSpotifyRedirect = new ResourceNode("/callback", "GET", &handleSpotifyRedirect);
        ResourceNode * defaultNode = new ResourceNode("", "GET", &handleDefault);

        secureServer.registerNode(nodeRoot);
        secureServer.registerNode(nodeSpotifyRedirect);
        secureServer.setDefaultNode(defaultNode);

        secureServer.start();
        if (secureServer.isRunning()) {
            Serial.println("Server ready.");
        }
    }

    void listener() {
        secureServer.loop();
    }

}
