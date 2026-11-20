#ifndef CLIENT_REGISTRY_HPP
#define CLIENT_REGISTRY_HPP

#include <cstddef>
#include <map>
#include <sstream>
#include <string>

#include "../exceptions_common/BaseException.hpp"

#include "./Client.hpp"

/**
 * @brief Owns every connected Client and allows looking them up by file descriptor.
 *
 * The registry is the sole owner of the Client objects it stores: they are created in addClient()
 * and destroyed in removeClient() or when the registry itself is destroyed. References returned by
 * the findClient() overloads are therefore invalidated as soon as the corresponding client is
 * removed.
 *
 * The registry does not close the file descriptors of its clients; that is the caller's
 * responsibility.
 *
 * The registry is not copyable.
 */
class ClientRegistry {
  private:
    std::map<int, Client*> clientsByFd;

    ClientRegistry(const ClientRegistry&);
    ClientRegistry& operator=(const ClientRegistry&);

  public:
    class ClientNotFoundException : public BaseException {
      private:
        static std::string buildMessage(int fd) {
            std::ostringstream oss;
            oss << "Client with fd: \"" << fd << "\" not found";
            return oss.str();
        }

      public:
        ClientNotFoundException(int fd, const std::exception* cause = NULL,
                                const std::exception* overrides = NULL)
            : BaseException("ClientNotFoundException", buildMessage(fd), -1, cause, overrides) {}

        ClientNotFoundException(const std::string& nick, const std::exception* cause = NULL,
                                const std::exception* overrides = NULL)
            : BaseException("ClientNotFoundException",
                            "Client with nick: \"" + nick + "\" not found", -1, cause,
                            overrides) {}
    };

    class ClientAlreadyExistsException : public BaseException {
      private:
        static std::string buildMessage(int fd) {
            std::ostringstream oss;
            oss << "Client with fd: \"" << fd << "\" already exists";
            return oss.str();
        }

      public:
        ClientAlreadyExistsException(int fd, const std::exception* cause = NULL,
                                     const std::exception* overrides = NULL)
            : BaseException("ClientAlreadyExistsException", buildMessage(fd), -1, cause,
                            overrides) {}
    };

    ClientRegistry();
    ~ClientRegistry();

    /**
     * @brief Finds a client by its file descriptor.
     * @param fd The file descriptor of the client to find.
     * @return A reference to the Client owned by the registry.
     * @throws ClientNotFoundException if no client with the given file descriptor exists.
     */
    Client& findClient(int fd);

    /**
     * @brief Finds a client by its file descriptor (read-only access).
     * @param fd The file descriptor of the client to find.
     * @return A const reference to the Client owned by the registry.
     * @throws ClientNotFoundException if no client with the given file descriptor exists.
     */
    const Client& findClient(int fd) const;

    /**
     * @brief Creates a new client and adds it to the registry.
     * @param fd The file descriptor of the new client.
     * @throws ClientAlreadyExistsException if a client with the given file descriptor already
     * exists.
     */
    void addClient(int fd);

    /**
     * @brief Removes a client from the registry and destroys it.
     *
     * The client's file descriptor is not closed. Any reference previously obtained for this
     * client becomes invalid.
     *
     * @param fd The file descriptor of the client to remove.
     * @throws ClientNotFoundException if no client with the given file descriptor exists.
     */
    void removeClient(int fd);
};

#endif // CLIENT_REGISTRY_HPP