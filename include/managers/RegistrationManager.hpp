#ifndef REGISTRATION_MANAGER_HPP
#define REGISTRATION_MANAGER_HPP

#include <cstddef>
#include <sstream>
#include <string>

#include "../client/ClientRegistry.hpp"
#include "../exceptions_common/BaseException.hpp"
#include "./NickManager.hpp"

/**
 * @brief Manages the registration data (nickname, username, realname, hostname) of clients that
 * already exist in ClientRegistry.
 *
 * It does not create, remove or destroy clients, and it does not own the ClientRegistry or the
 * NickManager it works with.
 */
class RegistrationManager {
  private:
    ClientRegistry& clientRegistry;
    NickManager& nickManager;

  public:
    /**
     * @brief Thrown when a nickname does not satisfy validation rules.
     */
    class InvalidNicknameException : public BaseException {
      public:
        explicit InvalidNicknameException(const std::string& nick,
                                          const std::exception* cause = NULL,
                                          const std::exception* overrides = NULL)
            : BaseException("InvalidNicknameException", "Invalid nickname: \"" + nick + "\"", 2001,
                            cause, overrides) {}
    };

    /**
     * @brief Thrown when a nickname is already reserved.
     */
    class NicknameAlreadyInUseException : public BaseException {
      private:
        static std::string buildMessage(const std::string& nick) {
            return "Nickname already in use: \"" + nick + "\"";
        }

      public:
        explicit NicknameAlreadyInUseException(const std::string& nick,
                                               const std::exception* cause = NULL,
                                               const std::exception* overrides = NULL)
            : BaseException("NicknameAlreadyInUseException", buildMessage(nick), 2002, cause,
                            overrides) {}
    };

    /**
     * @brief Thrown when a client registration is incomplete.
     *
     * A client is considered registered only when its nickname,
     * username, and realname are all non-empty.
     */
    class RegistrationIncompleteException : public BaseException {
      private:
        static std::string buildMessage(int fd) {
            std::ostringstream oss;
            oss << "Client registration is incomplete (fd: \"" << fd << "\")";
            return oss.str();
        }

      public:
        explicit RegistrationIncompleteException(int fd, const std::exception* cause = NULL,
                                                 const std::exception* overrides = NULL)
            : BaseException("RegistrationIncompleteException", buildMessage(fd), -1, cause,
                            overrides) {}
    };

    /**
     * @brief Constructs a RegistrationManager.
     *
     * @param clientRegistry Registry containing existing clients.
     * @param nickManager Manager responsible for nickname validation
     *                    and reservation.
     */
    RegistrationManager(ClientRegistry& clientRegistry, NickManager& nickManager);

    /**
     * @brief Destroys the RegistrationManager.
     */
    ~RegistrationManager();

    /**
     * @brief Assigns a nickname to an existing client.
     *
     * The nickname must be valid and available.
     * If the client already has a nickname, it is released after
     * successfully reserving the new one.
     *
     * @param fd File descriptor identifying the client.
     * @param nick Nickname to assign.
     *
     * @throws InvalidNicknameException if the nickname is invalid.
     * @throws NicknameAlreadyInUseException if the nickname is unavailable.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void setNick(int fd, const std::string& nick);

    /**
     * @brief Sets the username of an existing client.
     *
     * @param fd File descriptor identifying the client.
     * @param username Username to assign.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void setUsername(int fd, const std::string& username);

    /**
     * @brief Sets the realname of an existing client.
     *
     * @param fd File descriptor identifying the client.
     * @param realname Real name to assign.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void setRealname(int fd, const std::string& realname);

    /**
     * @brief Sets the hostname of an existing client.
     *
     * Hostname is optional and does not affect registration status.
     *
     * @param fd File descriptor identifying the client.
     * @param hostname Hostname to assign.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void setHostname(int fd, const std::string& hostname);

    /**
     * @brief Checks whether a client has completed registration.
     *
     * A client is considered registered when its nickname, username,
     * and realname are all non-empty.
     *
     * @param fd File descriptor identifying the client.
     * @return true if all required registration fields are present.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    bool isRegistered(int fd) const;

    /**
     * @brief Removes the registration information of an existing client.
     *
     * Releases the client's nickname and clears its nickname, username,
     * realname, and hostname.
     *
     * The client remains registered in ClientRegistry.
     *
     * @param fd File descriptor identifying the client.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void unRegister(int fd);

    /**
     * @brief Retrieves the file descriptor of the client that owns a nickname.
     *
     * The lookup is case-insensitive.
     *
     * @param nick Nickname to look up.
     * @return The file descriptor of the client that owns the nickname.
     *
     * @throws NickManager::NickNotFoundException if the nickname is not reserved.
     */
    int getFdByNick(const std::string& nick) const;
};

#endif // REGISTRATION_MANAGER_HPP