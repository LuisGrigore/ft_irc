#ifndef NICKMANAGER_HPP
#define NICKMANAGER_HPP

#include <cstddef>
#include <map>
#include <string>

#include "../exceptions_common/BaseException.hpp"

/**
 * @brief Keeps track of the nicknames in use and the file descriptor that owns each one.
 *
 * Nicknames are compared case-insensitively: they are normalized before being stored or looked up.
 *
 * The manager only maps nicknames to file descriptors. It does not know about Client objects and
 * does not modify them.
 */
class NickManager {
  private:
    std::map<std::string, int> nicks;

    static const size_t NICK_MAX_LENGTH = 30;

    static bool isLetter(char c);
    static bool isDigit(char c);
    static bool isSpecial(char c);

    static std::string normalize(const std::string& nick);

  public:
    /**
     * @brief Thrown when a nickname is not reserved by any client.
     */
    class NickNotFoundException : public BaseException {
      public:
        NickNotFoundException(const std::string& nick, const std::exception* cause = NULL,
                              const std::exception* overrides = NULL)
            : BaseException("NickNotFoundException", "Nick: \"" + nick + "\" not found", -1,
                            cause, overrides) {}
    };

    NickManager();
    ~NickManager();

    /**
     * @brief Checks if a nickname is available.
     * @param nick The nickname to check.
     * @return true if no client has reserved the nickname, false otherwise.
     */
    bool isAvailable(const std::string& nick) const;

    /**
     * @brief Reserves a nickname for a client.
     * @param nick The nickname to reserve.
     * @param fd The file descriptor of the client that will own the nickname.
     * @return true if the nickname was reserved, false if it is invalid or already taken.
     */
    bool reserve(const std::string& nick, int fd);

    /**
     * @brief Retrieves the file descriptor of the client that owns a nickname.
     * @param nick The nickname to look up.
     * @return The file descriptor of the owner.
     * @throws NickNotFoundException if the nickname is not reserved.
     */
    int getFd(const std::string& nick) const;

    /**
     * @brief Releases a nickname so it becomes available again.
     * @param nick The nickname to release. Releasing a nickname that is not reserved has no
     * effect.
     */
    void release(const std::string& nick);

    /**
     * @brief Checks if a nickname is valid.
     * @param nick The nickname to check.
     * @return true if the nickname is valid, false otherwise.
     */
    bool isValid(const std::string& nick) const;
};

#endif // NICKMANAGER_HPP