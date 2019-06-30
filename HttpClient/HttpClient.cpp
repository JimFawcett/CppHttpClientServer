/////////////////////////////////////////////////////////////////////////
// HttpClient.cpp - Demonstrates simple HTTP messaging                 //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017           //
// Application: OOD Projects                                           //
// Platform:    Visual Studio 2017, Dell XPS 8920, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////

#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Message/Message.h"
#include "../HttpCommCore/HttpCommCore.h"
#include "HttpClient.h"
#include <string>
#include <iostream>
#include <thread>

using Show = StaticLogger<1>;
using namespace Utilities;
using namespace Sockets;

using namespace HttpCommunication;

HttpClient::HttpClient() : HttpCommCore(&socket) {}

//----< sends message and waits for reply >----------------------------

HttpMessage<HttpReply> HttpClient::postMessage(HttpMessage<HttpRequest> msg)
{
  HttpCommCore::postMessage<HttpRequest>(msg);
  HttpMessage<HttpReply> reply = HttpCommCore::getMessage<HttpReply>();
  return reply;
}
//----< reconnect to new target >--------------------------------------

bool HttpClient::connect(const std::string& address, size_t port)
{
  socket.shutDown();
  return socket.connect(address, port);
}

int main()
{
  SetConsoleTitle(L"HttpClient");
 
  Utilities::StringHelper::Title("HttpClient starting");
  std::cout << "\n  press Enter to post message: ";
  std::cout.flush();
  std::cin.get();

  HttpMessage<HttpRequest> getMsg;
  getMsg.type().command(HttpRequest::GET);
  getMsg.type().fileSpec("debug/Hello1.html");

  std::cout << "--waiting for server";

  HttpClient client;

  for (size_t i = 0; i < 3; ++i)
  {
    client.connect("localhost", 8080);

    std::cout << "\n--posting message to localhost:8080:";
    getMsg.show();
    Utilities::putline();

    HttpMessage<HttpReply> reply = client.postMessage(getMsg);
    std::cout << "\n--received reply:";
    reply.show();
    Utilities::putline();
  }

  HttpMessage<HttpRequest> postMsg;
  postMsg.type().command(HttpRequest::POST);
  postMsg.type().fileSpec("foobar.html");
  postMsg.action("demo");
  postMsg.body().load(11, (HttpMessage<HttpRequest>::byte*)"hello world");
  postMsg.contentLength(postMsg.body().size());

  std::string machine = "Odin";
  std::cout << "\n--posting message to \"" << machine << ":8080\"";
  if (client.connect(machine, 8080))
  {
    HttpMessage<HttpReply> reply = client.postMessage(postMsg);
    std::cout << "\n--received reply:";
    reply.show();
  }
  else
  {
    std::cout << "\n--could not connect to \"" << machine << "\"";
  }
  std::cout << "\n\n";

  Show::write("\n --------------------\n  press key to exit: \n --------------------");
  std::cout.flush();
  std::cin.get();
}