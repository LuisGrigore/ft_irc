#ifndef BASE_EXCEPTION_HPP
#define BASE_EXCEPTION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

class BaseException : public std::runtime_error {
private:
    int errorCode;
    std::string name;
    std::string message;
    std::string cause;
    std::string overrides;
    std::string fullMessage;

    static std::string describeOf(const std::exception* e) {
        if (e == NULL)
            return "";
        const BaseException* b = dynamic_cast<const BaseException*>(e);
        return b != NULL ? b->describe() : e->what();
    }

    static std::string indent(const std::string& s) {
        std::string out;
        for (std::string::size_type i = 0; i < s.size(); ++i) {
            out += s[i];
            if (s[i] == '\n')
                out += "  ";
        }
        return out;
    }

    std::string buildFullMessage() const {
        std::string s = name + "::" + message;
        if (!cause.empty())
            s += "\n  caused by: " + indent(cause);
        if (!overrides.empty())
            s += "\n  overrides: " + indent(overrides);
        return s;
    }

public:
    BaseException(const std::string& name, const std::string& message, int code = 0,
                  const std::exception* cause = NULL, const std::exception* overrides = NULL)
        : std::runtime_error(name + "::" + message), errorCode(code), name(name),
          message(message), cause(describeOf(cause)), overrides(describeOf(overrides)),
          fullMessage(buildFullMessage()) {}

    virtual ~BaseException() throw() {}

    const char* describe() const throw() { return fullMessage.c_str(); }

    int getErrorCode() const { return errorCode; }
    const std::string& getName() const { return name; }
    const std::string& getMessage() const { return message; }
    const std::string& getCause() const { return cause; }
    const std::string& getOverrides() const { return overrides; }
};

#endif