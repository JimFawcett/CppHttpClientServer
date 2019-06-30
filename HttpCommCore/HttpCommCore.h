#pragma once
/////////////////////////////////////////////////////////////////////////
// HttpCommCore.h - Provides core HTTP Message services                //
// ver 1.1                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
// Application: OOD Projects                                           //
// Platform:    Visual Studio 2017, Dell XPS 8920, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* - This package implements a class HttpCommCore that provides shared
*   code for its derived classes, HttpClient and HttpServer.
* - It's purpose is to provide a prototype for communication message for
*   OOD and DO projects.
*
* Required Files:
* ---------------
*   HttpCommCore.h, HttpCommCore.cpp
*   Message.h, Message.cpp
*   Sockets.h, Sockets.cpp
*
* Maintenance History:
* --------------------
*   ver 1.1 : 15 Jan 2018
*   - changed message body processing to accomodate binary data
*   ver 1.0 : 06 Jan 2018
*   - first release
*
* Note:
* -----
*   No test stub is provided as it only makes sense to test as part of
*   HttpClient and HttpServer.
*
*/
#include <functional>
#include "../Message/Message.h"
#include "../Sockets/Sockets.h"

namespace HttpCommunication
{
  class HttpCommCore
  {
  public:
    HttpCommCore(Sockets::Socket* pSocket) : pSocket_(pSocket) {};
    HttpCommCore() : pSocket_(nullptr) {}
    virtual ~HttpCommCore() {};
    void setSocket(Sockets::Socket* pSocket) { pSocket_ = pSocket; }
    template <typename T>
    HttpMessage<T> getMessage();
    template <typename T>
    void postMessage(HttpMessage<T> msg);
  protected:
    Sockets::Socket* pSocket_;
  };

  //----< pull HttpMessage from socket >-------------------------------

  template<typename T>
  HttpMessage<T> HttpCommCore::getMessage()
  {
    // read HTTP message header lines

    Sockets::Socket& socket = *pSocket_;
    std::string msgString;
    while (socket.validState())
    {
      std::string temp = socket.recvString('\n');
      msgString += temp;
      if (temp.length() < 3 || !socket.validState())  // temp = "\r\n" terminates headers
        break;
    }

    HttpMessage<T> msg = HttpMessage<T>::fromString(msgString);

    // read message body

    size_t bodyLen = msg.contentLength();
    HttpMessageBody body;
    std::string temp;
    if (bodyLen > 0)
    {
      body.size(bodyLen);
      socket.recv(bodyLen, (Sockets::Socket::byte*)(body.value().data()));
    }
    msg.body() = body;
    return msg;
  }
  //----< push HttpMessage into socket >-------------------------------

  template<typename T>
  void HttpCommCore::postMessage(HttpMessage<T> msg)
  {
    std::string sendHeader = msg.toHeaderString();
    pSocket_->send(sendHeader.size(), (Sockets::Socket::byte*)sendHeader.c_str());
    size_t bodyLen = msg.contentLength();
    if (bodyLen > 0)
    {
      HttpMessageBody body = msg.body();
      pSocket_->send(bodyLen, (Sockets::Socket::byte*)body.value().data());
    }
    //char terminal[1];
    //terminal[0] = '\n';
    pSocket_->send(1, (Sockets::Socket::byte*)"\n");
  }
}