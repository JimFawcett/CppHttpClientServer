#pragma once
/////////////////////////////////////////////////////////////////////////
// HttpServerProc.h - Provides application specific server processing  //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
// Application: OOD Projects                                           //
// Platform:    Visual Studio 2017, Dell XPS 8920, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
*  Processor Operations:
* -----------------------
* This package implements application specific processing, e.g., 
* transformation of HttpMessage<HttpRequest> into HttpMessage<HttpReply>
* provided as a set of functions.  Note that lambdas would work too.
*
*  Required Files:
* -----------------
*   HttpServerProc.h
*   Message.h, Message.cpp
*   Utilities.h, Utilities.cpp
*
*  Maintenance History:
* ----------------------
*   ver 1.0 : 07 Jan 2017
*   - first release
*
*/
#include "../Message/Message.h"
#include "../Utilities/Utilities.h"
#include <functional>
#include <string>
#include <fstream>
#include <sstream>

namespace HttpCommunication
{
  using RequestMsg = HttpMessage<HttpRequest>;
  using ReplyMsg = HttpMessage<HttpReply>;
  using Key = std::string;
  using MessageProcessType = std::function < ReplyMsg(RequestMsg&)>;

  /////////////////////////////////////////////////////////////////////
  // getProc: processing for GET message

  inline HttpMessage<HttpReply> getProc(HttpMessage<HttpRequest> msg)
  {
    HttpMessage<HttpReply> reply;

    const size_t bufSize = 255;
    char buffer[bufSize];
    ::GetCurrentDirectoryA(bufSize, buffer);
    std::string fileSpec = msg.type().fileSpec();
    if (fileSpec[0] == '/')
      fileSpec.insert(fileSpec.begin(), '.');
    std::ifstream in(fileSpec);
    if (in.good())
    {
      std::stringstream out;
      out << in.rdbuf();
      std::string& text = out.str();
      reply.contentLength(text.size());
      reply.body().load(text.size(), (HttpMessageBody::byte*)&text[0]);
      reply.type().status(200);
      return reply;
    }
    reply.type().status(404);
    reply.body().clear();
    return reply;
  }

  /////////////////////////////////////////////////////////////////////
  // postProc: processing for POST message

  inline HttpMessage<HttpReply> postProc(HttpMessage<HttpRequest> msg)
  {
    HttpMessage<HttpReply> reply;

    reply.type().status(200);
    reply.body().load(17, (HttpMessageBody::byte*)"POST message stub");
    reply.contentLength(17);
    return reply;
  }
  ////----< helper function for ClientHandler operator() >---------------
  ///*
  //*  - This function is provided by each application to define
  //*    its server processing.
  //*/
  //inline HttpMessage<HttpReply> makeReplyMessage(HttpMessage<HttpRequest> msg)
  //{
  //  static size_t count = 0;
  //  size_t status = 200;
  //  std::string resource = msg.type().fileSpec();
  //  if (resource == "/favicon.ico")
  //  {
  //    status = 404;
  //    --count;
  //  }
  //  HttpMessage<HttpReply> reply = makeHttpReplyMessage(status);
  //  std::string replyStr = "<!DOCTYPE html><html><head><style>";
  //  replyStr += "body{ margin:5% ; font-family: Tahoma, Consolas, Sansserif; font-size:14; }";
  //  replyStr += "</style></head><body>";
  //  replyStr += "<h3>Received server message #" + Utilities::Converter<size_t>::toString(++count) + "</h3>";
  //  replyStr += "</body></html>";
  //  reply.body() = replyStr;
  //  reply.contentLength(replyStr.size());
  //  return reply;
  //}
}