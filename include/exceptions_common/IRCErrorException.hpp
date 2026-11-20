#ifndef IRC_ERROR_EXCEPTION_HPP
#define IRC_ERROR_EXCEPTION_HPP

#include "./BaseException.hpp"
#include <vector>

class IRCErrorException : public BaseException {
  private:
    std::string serverName;
    std::string nick;
    std::vector<std::string> parameters;

  public:
    IRCErrorException(std::string serverName, int code, std::string nick,
                      const std::string& message, const std::exception* cause = NULL,
                      const std::exception* overrides = NULL)
        : serverName(serverName), nick(nick), BaseException("IRCErrorException", message, code, cause, overrides) {};
    ~IRCErrorException();

	void addParameter(std::string& parameter){
		parameters.push_back(parameter);
	}
};

#endif IRC_ERROR_EXCEPTION_HPP
