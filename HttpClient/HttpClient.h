#pragma once
/////////////////////////////////////////////////////////////////////////
// HttpClient.h - Demonstrates simple HTTP messaging                   //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017           //
// Application: OOD Projects                                           //
// Platform:    Visual Studio 2017, Dell XPS 8920, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*   This package implements a client that sends HTTP messages to a
*   server that displays them.
*
*  Required Files:
* -----------------
*   StringClient.cpp, StringServer.cpp
*   Sockets.h, Sockets.cpp
*   Logger.h, Logger.cpp, Cpp11-BlockingQueue.h
*   Utilities.h, Utilities.cpp
*
*  Maintenance History:
* ----------------------
*   ver 1.0 : 07 Jan 2017
*   - first release
*/

#include "../Message/Message.h"
#include "../HttpCommCore/HttpCommCore.h"
#include "../Sockets/Sockets.h"

namespace HttpCommunication
{
  /////////////////////////////////////////////////////////////////////
  // HttpClient class
  // - posts Http message and waits for reply
  //
  class HttpClient : HttpCommCore
  {
  public:
    HttpClient();
    HttpMessage<HttpReply> postMessage(HttpMessage<HttpRequest> msg);
    bool connect(const std::string& address, size_t port);
  private:
    Sockets::SocketConnecter socket;
    Sockets::SocketSystem ss;
  };
}

