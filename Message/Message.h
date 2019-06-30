#pragma once
/////////////////////////////////////////////////////////////////////////
// Message.h - defines HTTP request and reply messages                 //
// ver 2.1                                                             //
// Jim Fawcett, CSE687 Object Oriented Design, Spring 2018             //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines an EndPoint struct and several Message-related classes.
*  It's purpose is to participate in both traditional synchronous HTTP operations
*  and non-traditional asynchronous, one-way, communications.
*  - Endpoints define a message source or destination with an address and port number.
*  - HttpRequest defines the command line for HttpMessage<HttpRequest> instances.
*  - HttpReply defines the command line for HttpMessage<HttpReply> instances.
*  - HttpMessageBody manages message body contents.
*  - HttpMessage<T> has an HTTP style structure with a set of attribute lines containing
*    name:value pairs.
*  - Message have a number of getter, setter methods for common attributes, and allow
*    definition of other "custom" attributes.
*
*  Required Files:
*  ---------------
*  Message.h, Message.cpp, Utilities.h, Utilities.cpp
*
*  Maintenance History:
*  --------------------
*  ver 2.1 : 15 Jan 2018
*  - added HttpMessage<T>::toHeaderString() method
*  ver 2.0 : 04 Jan 2018
*  - major refactoring of classes
*  - introduced template classes to avoid a lot of code repetition
*  ver 1.1 : 28 Dec 2017
*  - added #include <iostream>
*  ver 1.0 : 03 Oct 2017
*  - first release
*
*/
#include "../Utilities/Utilities.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace HttpCommunication
{
  ///////////////////////////////////////////////////////////////////
  // EndPoint struct

  struct EndPoint
  {
    using Address = std::string;
    using Port = size_t;
    Address address;
    Port port;
    EndPoint(Address anAddress = "", Port aPort = 0);
    std::string toString() const;
    static EndPoint fromString(const std::string& str);
  };

  /////////////////////////////////////////////////////////////////////
  // HttpRequest class
  //
#undef DELETE // defined in a  header used by Sockets

  class HttpRequest  // represents first line of HTTP Message
  {
  public:
    enum HttpCommand { GET, PUT, POST, DELETE, HEAD };

    HttpRequest();
    HttpRequest(HttpCommand command, const std::string& fileSpec);
    std::string toString(bool full = true) const;
    static HttpRequest fromString(const std::string& cmdStr);
    HttpCommand command() const;
    void command(HttpCommand cmd);
    std::string fileSpec() const;
    void fileSpec(const std::string& fileSpec);
  private:
    HttpCommand cmd_;
    std::string fileSpec_;
  };

  /////////////////////////////////////////////////////////////////////
  // HttpReply class
  //
  class HttpReply
  {
  public:
    using StatusType = std::unordered_map<size_t, std::string>;

    HttpReply(size_t status = 400);
    size_t status();
    void status(size_t st);
    std::string message() const;
    std::string toString() const;
    static HttpReply fromString(const std::string& cmdStr);
  private:
    StatusType statusType { 
      {100, "info"}, {200, "OK"}, {300, "redirect"}, 
      {400, "error"}, {404, "not found"}, {500, "server error"}
    };
    size_t status_ = 200;
  };

  ///////////////////////////////////////////////////////////////////
  // MessageBody class
  // - provides correct value semantics

  class HttpMessageBody
  {
  public:
    using byte = unsigned char;
    using iterator = std::vector<byte>::iterator;

    HttpMessageBody() = default;
    HttpMessageBody(size_t size);
    HttpMessageBody(size_t size, byte* buffer);
    HttpMessageBody(const std::string& bodyStr);
    HttpMessageBody& operator=(const std::string& bodyStr);
    HttpMessageBody::byte& operator[](size_t i);
    HttpMessageBody::byte operator[](size_t i) const;
    size_t size() const;
    void size(size_t size);
    HttpMessageBody::iterator begin();
    HttpMessageBody::iterator end();
    void load(size_t sz, byte*);
    void clear();
    std::vector<byte>& value() { return body_; }

    std::string toString() const;
    static HttpMessageBody fromString(const std::string& bodyStr);
    void show(std::ostream& out = std::cout) const;
  private:
    std::vector<byte> body_;
  };
  ///////////////////////////////////////////////////////////////////
  // HttpMessage class
  //
  template <typename T>
  class HttpMessage
  {
  public:
    using Key = std::string;
    using Value = std::string;
    using Attribute = std::string;
    using Attributes = std::unordered_map<Key, Value>;
    using Keys = std::vector<Key>;
    using FileSpec = std::string;
    using CommandString = std::string;
    using byte = unsigned char;

    HttpMessage() = default;

    T& type();
    Attributes& attributes();
    void attribute(const Key& key, const Value& value);
    Keys keys() const;
    static Key attribKey(const Attribute& attr);
    static Value attribValue(const Attribute& attr);
    bool containsKey(const Key& key) const;

    size_t contentLength();
    void contentLength(size_t ln);
    HttpMessageBody& body();
    HttpMessageBody body() const;
    std::string name();
    void name(const std::string& nm);
    std::string action();
    void action(const std::string& cmd);
    EndPoint to();
    void to(EndPoint ep);
    EndPoint from();
    void from(EndPoint ep);
    void clearBody();
    void clearAttributes();
    void clear();
    std::string toHeaderString() const;
    std::string toString() const;
    static HttpMessage<T> fromString(const std::string& src);
    void show(std::ostream& out = std::cout, bool suppressTrailingNewLine = true) const;
  protected:
    T type_;
    Attributes attributes_;
    HttpMessageBody body_;
  };

  //----< factory for HttpRequestMessages >----------------------------
  
  inline HttpMessage<HttpRequest> makeHttpRequestMessage(
    HttpRequest::HttpCommand cmd, const std::string& fileSpec
  )
  {
    HttpMessage<HttpRequest> msg;
    HttpRequest request = msg.type();
    request.command(cmd);
    request.fileSpec(fileSpec);
    msg.type() = request;
    return msg;
  }
  //----< factory for HttpReplyMessages >------------------------------

  inline HttpMessage<HttpReply> makeHttpReplyMessage(size_t status)
  {
    HttpMessage<HttpReply> msg;
    HttpReply reply = msg.type();
    reply.status(status);
    msg.type() = reply;
    return msg;
  }

  /////////////////////////////////////////////////////////////////////
  // HttpMessage methods

  //----< return reference to message type instance >------------------

  template <typename T>
  T& HttpMessage<T>::type()
  {
    return type_;
  }
  //----< return reference to message attributes >---------------------

  template <typename T>
  typename HttpMessage<T>::Attributes& HttpMessage<T>::attributes()
  {
    return attributes_;
  }
  //----< set message attribute >--------------------------------------

  template <typename T>
  void HttpMessage<T>::attribute(const Key& key, const Value& value)
  {
    attributes_[key] = value;
  }
  //----< return vector of attribute keys >----------------------------

  template <typename T>
  typename HttpMessage<T>::Keys HttpMessage<T>::keys() const
  {
    Keys keys;
    keys.reserve(attributes_.size());
    for (auto kv : attributes_)
    {
      keys.push_back(kv.first);
    }
    return keys;
  }
  //----< does message contain attribute with this key? >--------------

  template <typename T>
  bool HttpMessage<T>::containsKey(const Key& key) const
  {
    if (attributes_.find(key) == attributes_.end())
      return false;
    return true;
  }
  //----< find key from attribute string "key:value" >-----------------

  template <typename T>
  typename HttpMessage<T>::Key HttpMessage<T>::attribKey(const Attribute& attrib)
  {
    size_t pos = attrib.find_first_of(':');
    if (0 <= pos && pos < attrib.length())
      return attrib.substr(0, pos);
    return "";
  }
  //----< find value from attribute string "key:value" >---------------

  template <typename T>
  typename HttpMessage<T>::Value HttpMessage<T>::attribValue(const Attribute& attrib)
  {
    size_t pos = attrib.find_first_of(':');
    if (0 <= pos && pos < attrib.length())
      return attrib.substr(pos + 1, attrib.length() - pos);
    return "";
  }
  //----< return value of content-length >-----------------------------

  template <typename T>
  size_t HttpMessage<T>::contentLength()
  {
    if (containsKey("content-length"))
    {
      std::string lenStr = attributes_["content-length"];
      return Utilities::Converter<size_t>::toValue(lenStr);
    }
    return 0;
  }
  //----< set content-length value >-----------------------------------

  template <typename T>
  void HttpMessage<T>::contentLength(size_t ln)
  {
    attributes_["content-length"] = Utilities::Converter<size_t>::toString(ln);
  }
  //----< retrieve reference to body >---------------------------------

  template<typename T>
  HttpMessageBody& HttpMessage<T>::body()
  {
    return body_;
  }
  //----< set body >---------------------------------------------------

  template <typename T>
  HttpMessageBody HttpMessage<T>::body() const
  {
    return body_;
  }
  //----< get name attribute >-------------------------------------------

  template <typename T>
  std::string HttpMessage<T>::name()
  {
    if (containsKey("name"))
    {
      return attributes_["name"];
    }
    return "";
  }
  //----< set name attribute >-------------------------------------------

  template <typename T>
  void HttpMessage<T>::name(const std::string& nm)
  {
    attributes_["name"] = nm;
  }
  //----< get action attribute >-----------------------------------------

  template <typename T>
  std::string HttpMessage<T>::action()
  {
    if (containsKey("action"))
    {
      return attributes_["action"];
    }
    return "";
  }
  //----< set action attribute >-----------------------------------------

  template <typename T>
  void HttpMessage<T>::action(const std::string& cmd)
  {
    attributes_["action"] = cmd;
  }
  //----< get to attribute >---------------------------------------------

  template <typename T>
  EndPoint HttpMessage<T>::to()
  {
    if (containsKey("to"))
    {
      return EndPoint::fromString(attributes_["to"]);
    }
    return EndPoint();
  }
  //----< set to attribute >---------------------------------------------

  template <typename T>
  void HttpMessage<T>::to(EndPoint ep)
  {
    attributes_["to"] = ep.toString();
  }
  //----< get from attribute >-------------------------------------------

  template <typename T>
  EndPoint HttpMessage<T>::from()
  {
    if (containsKey("from"))
    {
      return EndPoint::fromString(attributes_["from"]);
    }
    return EndPoint();
  }
  //----< set from attribute >-------------------------------------------

  template <typename T>
  void HttpMessage<T>::from(EndPoint ep)
  {
    attributes_["from"] = ep.toString();
  }
  //----< clears all attributes >----------------------------------------

  template <typename T>
  void HttpMessage<T>::clearAttributes()
  {
    attributes_.clear();
  }
  //----< clears body bytes >--------------------------------------------

  template <typename T>
  void HttpMessage<T>::clearBody()
  {
    body_.clear();
  }
  //----< clears HttpMessage attributes and body >---------------------------

  template <typename T>
  void HttpMessage<T>::clear()
  {
    clearAttributes();
    clearBody();
  }
  //----< return string representation of Message header >-------------

  template<typename T>
  std::string HttpMessage<T>::toHeaderString() const
  {
    std::string temp = type_.toString();

    if (attributes_.size() > 0)
      temp += "\n";
    for (auto kv : attributes_)
    {
      temp += kv.first + ":" + kv.second + "\n";
    }
    temp += "\n";
    return temp;
  }
  //----< convert HttpMessage to string representation >---------------

  template <typename T>
  std::string HttpMessage<T>::toString() const
  {
    std::string temp = toHeaderString();
    if (body_.size() > 0)
    {
      //temp += "\n";
      temp += (body_.toString() + "\n");
    }
    return temp;
  }
  //----< build HttpMessage<HttpRequest> from string rep >-------------

  template <>
  inline HttpMessage<HttpRequest> HttpMessage<HttpRequest>::fromString(const std::string& src)
  {
    std::vector<std::string> splits = Utilities::StringHelper::split(src, '\n');
    if (splits.size() > 1)
    {
      std::vector<std::string> cmdSplits = Utilities::StringHelper::split(splits[0], ' ');
      HttpMessage<HttpRequest> msg;
      if (cmdSplits.size() > 1)
      {
        msg.type() = HttpRequest::fromString(splits[0]);
        msg.type().fileSpec(cmdSplits[1]);
      }
      for (size_t i = 1; i < splits.size(); ++i)
      {
        if (splits[i].size() == 0)
          continue;
        Key key = attribKey(splits[i]);
        Value value = attribValue(splits[i]);
        if (key.size() > 0)
        {
          msg.attributes_[key] = value;
        }
        else
        {
          HttpMessageBody body = HttpMessageBody::fromString(splits[i]);
          msg.body() = body;
          msg.attributes_["content-length"] = Utilities::Converter<size_t>::toString(splits[i].size());
        }
      }
      return msg;
    }
    return HttpMessage<HttpRequest>();
  }
  //----< build HttpMessage<HttpReply> from string rep >---------------

  template <>
  inline HttpMessage<HttpReply> HttpMessage<HttpReply>::fromString(const std::string& src)
  {
    std::vector<std::string> splits = Utilities::StringHelper::split(src, '\n');
    if (splits.size() > 1)
    {
      std::vector<std::string> cmdSplits = Utilities::StringHelper::split(splits[0], ' ');
      HttpMessage<HttpReply> msg;
      if (cmdSplits.size() > 1)
      {
        msg.type() = HttpReply::fromString(splits[0]);
        size_t statusNum = Utilities::Converter<size_t>::toValue(cmdSplits[1]);
        msg.type().status(statusNum);
      }
      for (size_t i = 1; i < splits.size(); ++i)
      {
        if (splits[i].size() == 0)
          continue;
        Key key = attribKey(splits[i]);
        Value value = attribValue(splits[i]);
        if (key.size() > 0)
        {
          msg.attributes_[key] = value;
        }
        else
        {
          HttpMessageBody body = HttpMessageBody::fromString(splits[i]);
          msg.body() = body;
          msg.attributes_["content-length"] = Utilities::Converter<size_t>::toString(splits[i].size());
        }
      }
      return msg;
    }
    return HttpMessage<HttpReply>();
  }

  //----< displays HttpMessage on std::ostream >-----------------------------
  /*
  *  - adds beginning newline and removes trailing newline
  *  - by default stream is std::cout
  *  - can be replaced by std::ostringstream to get display string
  */
  template <typename T>
  void HttpMessage<T>::show(std::ostream& out, bool suppressTrailingNewLine) const
  {
    std::string temp = toString();  // convert this HttpRequestMessage to string
    size_t pos = temp.find_last_of('\n');
    if (pos < temp.size())
    {
      temp = Utilities::StringHelper::trim(temp);
    }
    out << "\n" << temp; 
  }
}