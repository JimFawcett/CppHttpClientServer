///////////////////////////////////////////////////////////////////////////
// Message.cpp - defines message structure used in communication channel //
// ver 2.1                                                               //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017          //
///////////////////////////////////////////////////////////////////////////

#include "Message.h"
#include <iostream>

using namespace HttpCommunication;
using SUtils = Utilities::StringHelper;

///////////////////////////////////////////////////////////////////////
// HttpRequest methods

//----< HttpRequest default constructor >------------------------------

HttpRequest::HttpRequest()
{
  cmd_ = GET;
  fileSpec_ = "foobar.htm";
}
//----< HttpRequest constructor >--------------------------------------

HttpRequest::HttpRequest(HttpCommand command, const std::string& fileSpec)
{
  cmd_ = command;
  fileSpec_ = fileSpec;
}
//----< promotion constructor >----------------------------------------

HttpRequest HttpRequest::fromString(const std::string& commStr)
{
  HttpRequest cmd;
  std::vector<std::string> splits = Utilities::StringHelper::split(commStr, ' ');
  std::string cmdStr;
  if (splits.size() > 1)
  {
    cmdStr = Utilities::StringHelper::trim(splits[0]);
    if (cmdStr == "GET") cmd.cmd_ = GET;
    if (cmdStr == "PUT") cmd.cmd_ = PUT;
    if (cmdStr == "POST") cmd.cmd_ = POST;
    if (cmdStr == "DELETE") cmd.cmd_ = DELETE;
    if (cmdStr == "HEAD") cmd.cmd_ = HEAD;
    cmd.fileSpec_ = Utilities::StringHelper::trim(splits[1]);
  }
  else
  {
    cmd.cmd_ = GET;
    cmd.fileSpec_ = "";
  }
  return cmd;
}

//----< return command string >----------------------------------------

std::string HttpRequest::toString(bool full) const
{
  std::string commandString;
  switch (cmd_)
  {
  case GET:
    commandString = "GET";
    break;
  case PUT:
    commandString = "PUT";
    break;
  case POST:
    commandString = "POST";
    break;
  case DELETE:
    commandString = "DELETE";
    break;
  case HEAD:
    commandString = "HEAD";
    break;
  default:
    commandString = "GET";
  }
  if (full)
  {
    commandString += " ";
    commandString += fileSpec_;
    commandString += " HTTP/1.1";
  }
  return commandString;
}

//----< return command type >------------------------------------------

HttpRequest::HttpCommand HttpRequest::command() const
{
  return cmd_;
}
//----< set command type >---------------------------------------------

void HttpRequest::command(HttpCommand cmd)
{
  cmd_ = cmd;
}
//----< return command's file specification >--------------------------

std::string HttpRequest::fileSpec() const
{
  return fileSpec_;
}
//----< set command's file specification >-----------------------------

void HttpRequest::fileSpec(const std::string& fileSpec)
{
  fileSpec_ = fileSpec;
}
///////////////////////////////////////////////////////////////////////
// HttpReply methods

//----< constructor >--------------------------------------------------

HttpReply::HttpReply(size_t status)
{
  status_ = status;
}
//----< return status >------------------------------------------------

size_t HttpReply::status()
{
  return status_;
}
//----< set status >---------------------------------------------------

void HttpReply::status(size_t status)
{
  status_ = status;
}
//----< get status message >-------------------------------------------

std::string HttpReply::message() const
{
  auto iter = statusType.find(status_);
  if(iter != statusType.end())
    return iter->second;
  return "";
}
//----< convert to string >--------------------------------------------

std::string HttpReply::toString() const
{
  std::string statusStr = Utilities::Converter<size_t>::toString(status_);
  return "HTTP/1.1 " + statusStr + " " + message();
}
//----< convert string representation to HttpReply instance >----------

HttpReply HttpReply::fromString(const std::string& cmdStr)
{
  HttpReply reply;
  std::vector<std::string> splits = Utilities::StringHelper::split(cmdStr, ' ');
  if (splits.size() > 2)
  {
    size_t st = Utilities::Converter<size_t>::toValue(splits[1]);
    reply.status(st);
  }
  else
  {
    reply.status_ = 400;
  }
  return reply;
}

///////////////////////////////////////////////////////////////////////
// MessageBody methods

HttpMessageBody::HttpMessageBody(size_t size)
{
  body_.reserve(size);
}
//----< initialize from array of bytes >-------------------------------

HttpMessageBody::HttpMessageBody(size_t size, byte* buffer)
{
  body_.resize(size);
  std::memcpy(&body_[0], buffer, size);
  //body_.insert(body_.end(), buffer, buffer + size);
}
//----< promotion assignment >-----------------------------------------

void HttpMessageBody::load(size_t size, HttpMessageBody::byte* buffer)
{
  body_.resize(size);
  std::memcpy(&body_[0], buffer, size);
}
//----< initialize from std::string >----------------------------------

HttpMessageBody::HttpMessageBody(const std::string& bodyStr)
{
  body_.resize(0);
  body_.insert(body_.end(), bodyStr.begin(), bodyStr.end());
}
//----< promotion assignment >-----------------------------------------

HttpMessageBody& HttpMessageBody::operator=(const std::string& bodyStr)
{
  body_.resize(0);
  body_.insert(body_.end(), bodyStr.begin(), bodyStr.end());
  return *this;
}
//----< non-const indexer >--------------------------------------------

HttpMessageBody::byte& HttpMessageBody::operator[](size_t i)
{
  if (i < 0 || body_.size() <= i)
    throw std::invalid_argument("index out of range");
  return body_[i];
}
//----< const indexer >------------------------------------------------

HttpMessageBody::byte HttpMessageBody::operator[](size_t i) const
{
  if (i < 0 || body_.size() <= i)
    throw std::invalid_argument("index out of range");
  return body_[i];
}
//----< return body size >---------------------------------------------

size_t HttpMessageBody::size() const
{
  return body_.size();
}
//----< reset body size >----------------------------------------------

void HttpMessageBody::size(size_t size)
{
  body_.resize(size);
}
//----< return iterator pointing to first byte >-----------------------

HttpMessageBody::iterator HttpMessageBody::begin()
{
  return body_.begin();
}
//----< return iterator pointing to one past the last byte >-----------

HttpMessageBody::iterator HttpMessageBody::end()
{
  return body_.end();
}
//----< clear body contents >------------------------------------------

void HttpMessageBody::clear()
{
  body_.clear();
}
//----< convert to std::string >---------------------------------------

std::string HttpMessageBody::toString() const
{
  std::string temp;
  for (size_t i = 0; i < body_.size(); ++i)
  {
    temp += body_[i];
  }
  return temp;
}
//----< build HttpMessageBody instance from string >-------------------

HttpMessageBody HttpMessageBody::fromString(const std::string& bodyStr)
{
  HttpMessageBody body(bodyStr);
  return body;
}

//----< display message body >-----------------------------------------

void HttpMessageBody::show(std::ostream& out) const
{
  out << "\nbody:";
  for (auto ch : body_)
  {
    out << ch;
  }
}
///////////////////////////////////////////////////////////////////////
// EndPoint methods

//----< initialize address and port >----------------------------------

EndPoint::EndPoint(Address anAddress, Port aPort) : address(anAddress), port(aPort) {}

//----< convert to EndPoint string >-----------------------------------

std::string EndPoint::toString() const
{
  return address + ":" + Utilities::Converter<size_t>::toString(port);
}
//----< build EndPoint from address:port string >----------------------

EndPoint EndPoint::fromString(const std::string& str)
{
  EndPoint ep;
  size_t pos = str.find_first_of(':');
  if (pos == str.length())
    return ep;
  ep.address = str.substr(0, pos);
  std::string portStr = str.substr(pos + 1);
  ep.port = Utilities::Converter<size_t>::toValue(portStr);
  return ep;
}

//----< test stub >----------------------------------------------------

#ifdef TEST_MESSAGE

int main()
{
  SUtils::Title("Testing Message Class");

  SUtils::title("testing HTTP commands");
  HttpMessage<HttpRequest> get = makeHttpRequestMessage(HttpRequest::GET, "foobar.html");
  get.show(std::cout, true);
  HttpMessage<HttpRequest> put = makeHttpRequestMessage(HttpRequest::PUT, "foobar.html");
  put.show(std::cout, true);
  HttpMessage<HttpRequest> post = makeHttpRequestMessage(HttpRequest::POST, "foobar.html");
  post.show(std::cout, true);
  HttpMessage<HttpRequest> del = makeHttpRequestMessage(HttpRequest::DELETE, "foobar.html");
  del.show(std::cout, true);
  HttpMessage<HttpRequest> head = makeHttpRequestMessage(HttpRequest::HEAD, "foobar.html");
  head.show(std::cout, true);
  Utilities::putline();

  SUtils::title("testing endpoints");
  EndPoint ep("localhost", 8080);
  std::cout << "\n  address = " << ep.address;
  std::cout << "\n  port = " << ep.port;
  std::string epStr = ep.toString();
  std::cout << "\n  " << epStr;

  EndPoint newEp = EndPoint::fromString(epStr);
  std::cout << "\n  " << newEp.toString();
  Utilities::putline();

  SUtils::title("testing messages");
  Utilities::putline();

  SUtils::title("creating message from Message::methods");
  HttpMessage<HttpRequest> msg;
  msg.name("msg#1");
  msg.to(EndPoint("localhost", 8080));
  msg.from(EndPoint("localhost", 8081));
  msg.action("doIt");
  msg.contentLength(42);
  HttpRequest req = msg.type();
  req.command(HttpRequest::POST);
  req.fileSpec("feebar.html");
  msg.type() = req;
  msg.body() = "abc123";
  msg.contentLength(strlen("abc123"));
  msg.show();
  Utilities::putline();

  SUtils::title("testing Message msg = fromString(msg.toString())");
  std::string test = msg.toString();
  HttpMessage<HttpRequest> newMsg = HttpMessage<HttpRequest>::fromString(msg.toString());
  newMsg.show();
  Utilities::putline();

  SUtils::title("retrieving attributes from message");
  std::cout << "\n  msg name          : " << newMsg.name();
  std::cout << "\n  msg action        : " << newMsg.action();
  std::cout << "\n  msg to            : " << newMsg.to().toString();
  std::cout << "\n  msg from          : " << newMsg.from().toString();
  std::cout << "\n  msg file          : " << newMsg.type().fileSpec();
  std::cout << "\n  msg content-Length: " << newMsg.contentLength();
  Utilities::putline();

  SUtils::title("adding custom attribute");
  newMsg.attribute("customKey", "customValue");
  newMsg.show();
  Utilities::putline();

  SUtils::title("testing assignment");
  HttpMessage<HttpRequest> srcMsg = makeHttpRequestMessage(HttpRequest::POST, "someFile");
  srcMsg.name("srcMsg");
  srcMsg.attribute("someKey", "someValue");
  srcMsg.body() = "aeiou";
  srcMsg.show();
  std::cout << "\n\n  assigning srcMsg to msg #1\n";
  newMsg = srcMsg;
  newMsg.show();
  Utilities::putline();

  SUtils::title("loading new message body contents");
  size_t j = 0;
  newMsg.body().size(10);
  for (auto& ch : newMsg.body())
  {
    ch = char(int('a') + j++);
  }
  newMsg.show();
  Utilities::putline();

  HttpMessage<HttpRequest> newerMsg;
  newerMsg.name("newerMsg");
  newerMsg.body().load(newMsg.body().size(), &newMsg.body()[0]);
  newerMsg.show();
  Utilities::putline();

  SUtils::title("testing with browser request string");
  std::string chromeStr =
    std::string("GET /foobar.htm?name=value HTTP/1.1\n") + 
    "Host: localhost:36895\n" +
    "Connection: keep-alive\n" +
    "Cache-Control: max-age=0\n" +
    "Upgrade-Insecure-Requests: 1\n" +
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.94 Safari/537.36\n" +
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\n" +
    "Accept-Encoding: gzip, deflate, br\n" +
    "Accept-Language: en-US,en;q=0.9\n\n";
  std::cout << "\n" + chromeStr;
  Utilities::putline();

  HttpMessage<HttpRequest> testMsg = HttpMessage<HttpRequest>::fromString(chromeStr);
  testMsg.show();
  Utilities::putline();

  SUtils::title("testing reply message");
  HttpMessage<HttpReply> reply = makeHttpReplyMessage(200);
  std::string bodyStr = "<h3>Got a message for you</h3>";
  reply.body() = HttpMessageBody::fromString(bodyStr);
  reply.contentLength(bodyStr.size());
  reply.show();
  Utilities::putline();

  reply = HttpMessage<HttpReply>::fromString(reply.toString());
  reply.show();

  std::cout << "\n\n";
  return 0;
}
#endif
