
#ifndef CLIENT_SYSTEM_HPP
#define CLIENT_SYSTEM_HPP

#include <cstddef>
#include <set>
#include <string>

#include "./AuthenticationManager.hpp"
#include "./ClientLifecycleManager.hpp"
#include "./PendingIOManager.hpp"
#include "./RegistrationManager.hpp"

/**
 * @brief Facade that provides a unified interface for client lifecycle, pending I/O,
 * authentication, registration and client queries.
 *
 * It coordinates the existing managers without owning them. ClientSystem delegates each
 * operation to the manager responsible for that functionality, providing a single entry
 * point for the rest of the application.
 *
 * It does not own the managers it works with, and it does not directly manage client state.
 *
 * The system is not copyable.
 */
class ClientSystem {
  private:
    ClientLifecycleManager& lifecycleManager;
    PendingIOManager& pendingIOManager;
    AuthenticationManager& authManager;
    RegistrationManager& registrationManager;
    ClientRegistry& clientRegistry;

    ClientSystem(const ClientSystem&);
    ClientSystem& operator=(const ClientSystem&);

  public:
    struct ClientDetails {
        const int fd;
        const std::string nick;
        const std::string username;
        const std::string realname;
        const std::string hostname;
        const bool authenticated;
        const bool registered;
        const bool quitting;
        const std::set<std::string> channels;
        ClientDetails(int fd, std::string nick, std::string username, std::string realname,
                      std::string hostname, bool authenticated, bool registered, bool quitting,
                      std::set<std::string> channels);
        bool operator==(const ClientDetails& other) const;
        bool operator!=(const ClientDetails& other) const;
    };
    /**
     * @brief Constructs a ClientSystem.
     * @param lifecycleManager Manager responsible for client creation and disconnection.
     * @param pendingIOManager Manager responsible for pending input and output.
     * @param authManager Manager responsible for client authentication.
     * @param registrationManager Manager responsible for client registration.
     */
    ClientSystem(ClientLifecycleManager& lifecycleManager, PendingIOManager& pendingIOManager,
                 AuthenticationManager& authManager, RegistrationManager& registrationManager,
                 ClientRegistry& clientRegistry);
    ~ClientSystem();

    /**
     * @brief Retrieves the clients that are quitting and have no pending output.
     * @return A set containing the file descriptors of clients ready to be closed.
     */
    std::set<int> getClientsToClose() const;

    /**
     * @brief Retrieves the clients with pending input data.
     * @return A set containing the file descriptors of clients with pending input.
     */
    std::set<int> getHasPendingInput() const;

    /**
     * @brief Retrieves the clients with pending output data.
     * @return A set containing the file descriptors of clients with pending output.
     */
    std::set<int> getHasPendingOutput() const;

    /**
     * @brief Adds a newly connected client.
     * @param fd The file descriptor of the client.
     * @param hostname The hostname of the client.
     */
    void addClient(int fd, const std::string& hostname);

    /**
     * @brief Removes a client and clears all state associated with it.
     * @param fd The file descriptor of the client.
     */
    void removeClient(int fd);

    /**
     * @brief Adds received data to a client's input buffer.
     * @param fd The file descriptor of the client.
     * @param data The input data to append.
     */
    void pushInput(int fd, const std::string& data);

    /**
     * @brief Retrieves the next complete line from a client's input buffer.
     * @param fd The file descriptor of the client.
     * @param line Output parameter that receives the extracted line.
     * @return true if a complete line was retrieved, false otherwise.
     */
    bool getNextInputLine(int fd, std::string& line);

    /**
     * @brief Adds data to a client's output buffer.
     * @param fd The file descriptor of the client.
     * @param data The output data to append.
     */
    void pushOutput(int fd, const std::string& data);

    /**
     * @brief Retrieves data from a client's output buffer without consuming it.
     * @param fd The file descriptor of the client.
     * @param length Maximum number of bytes to retrieve.
     * @return The requested output data.
     */
    std::string peekOutput(int fd, std::size_t length) const;

    /**
     * @brief Removes sent data from a client's output buffer.
     * @param fd The file descriptor of the client.
     * @param length Number of bytes to consume.
     */
    void consumeOutput(int fd, std::size_t length);

    /**
     * @brief Marks a client as quitting.
     * @param fd The file descriptor of the client.
     */
    void setIsQuitting(int fd);

    /**
     * @brief Authenticates a client using the supplied password.
     * @param fd The file descriptor of the client.
     * @param password The password provided by the client.
     */
    void authenticate(int fd, const std::string& password);

    /**
     * @brief Checks whether a client is authenticated.
     * @param fd The file descriptor of the client.
     * @return true if the client is authenticated.
     */
    bool isAuthenticated(int fd) const;

    /**
     * @brief Sets a client's nickname.
     * @param fd The file descriptor of the client.
     * @param nick The nickname to assign.
     */
    void setNick(int fd, const std::string& nick);

    /**
     * @brief Sets a client's username.
     * @param fd The file descriptor of the client.
     * @param username The username to assign.
     */
    void setUsername(int fd, const std::string& username);

    /**
     * @brief Sets a client's real name.
     * @param fd The file descriptor of the client.
     * @param realname The real name to assign.
     */
    void setRealname(int fd, const std::string& realname);

    /**
     * @brief Checks whether a client is fully registered.
     * @param fd The file descriptor of the client.
     * @return true if the client is registered.
     */
    bool isRegistered(int fd) const;

    /**
     * @brief Retrieves the file descriptor associated with a nickname.
     * @param nick The nickname to search for.
     * @return The file descriptor of the client using that nickname.
     */
    int getFdByNick(const std::string& nick) const;

    /**
     * @brief Retrieves the details of a client.
     * @param fd The file descriptor of the client.
     * @return The details associated with the client.
     */
    ClientDetails getClientDetails(int fd) const;
};

#endif // CLIENT_SYSTEM_HPP
