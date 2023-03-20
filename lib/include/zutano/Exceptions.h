#pragma once

#include <exception>
#include <sstream>
#include <iostream>
#include <string>
#include <string_view>

class AuthenticationError : public std::exception {
 public:
  AuthenticationError() : message_{"Unknown username/password"} {}

  virtual const char *what() const noexcept override {
	return message_.c_str();
  }
 private:
  std::string message_;
};

class ServerError : public std::exception {
 public:
  ServerError(std::string m, int error_code) {
	std::ostringstream stringStream;
	stringStream << "ServerError (";
	stringStream << error_code;
	stringStream << ") : ";
	stringStream << m;

	message_ = stringStream.str();
  }

  virtual const char *what() const noexcept override {
	return message_.c_str();
  }
 private:
  std::string message_;
};

class RequestError : public std::exception {
 public:
  RequestError(std::string m) : message_{"Unknown endpoint: " + m} {}

  virtual const char *what() const noexcept override {
	return message_.c_str();
  }
 private:
  std::string message_;
};

class ClientError : public std::exception {
 public:
  ClientError(std::string m) : message_{"Client error: " + m} {}

  virtual const char *what() const noexcept override {
	return message_.c_str();
  }
 private:
  std::string message_;
};