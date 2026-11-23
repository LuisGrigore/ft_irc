#ifndef CLIENT_LIFECYCLE_MANAGER_HPP
#define CLIENT_LIFECYCLE_MANAGER_HPP

#include <set>

#include "../client/ClientRegistry.hpp"
#include "./AuthenticationManager.hpp"
#include "./ChannelMembershipManager.hpp"
#include "./PendingIOManager.hpp"
#include "./RegistrationManager.hpp"

/**
 * @brief Coordinates the creation and the orderly disconnection of clients across the registry
 * and the managers that hold state about them.
 *
 * Disconnection happens in two phases so that pending output (for example the reply to QUIT or an
 * ERROR message) can still be sent before the client disappears:
 * 1. removeClient() on a client with pending output marks it as quitting. It leaves its channels
 *    and loses its registration and authentication immediately, so other clients no longer see
 *    it, but the Client stays in the ClientRegistry until its output is flushed.
 * 2. Once its output buffer is empty, the fd shows up in getClientsToClose(). The caller closes
 *    the socket and calls removeClient() again, which finally removes the client.
 *
 * If the client has no pending output, the first removeClient() call performs both phases.
 *
 * The manager owns none of the objects it works with and it never closes file descriptors.
 * It is not copyable.
 */
class ClientLifecycleManager {
  private:
    ClientRegistry& clientRegistry;
    AuthenticationManager& authManager;
    RegistrationManager& registrationManager;
    ChannelMembershipManager& channelMembershipManager;
    PendingIOManager& pendingIOManager;
    std::set<int> isQuitting;

    ClientLifecycleManager(const ClientLifecycleManager&);
    ClientLifecycleManager& operator=(const ClientLifecycleManager&);

  public:
    /**
     * @brief Constructs a ClientLifecycleManager.
     * @param clientRegistry Registry containing the clients.
     * @param authManager Manager that holds the authentication state.
     * @param registrationManager Manager that holds the registration data and nicknames.
     * @param channelMembershipManager Manager that holds the channel memberships.
     * @param pendingIOManager Manager that holds the I/O buffers and pending state.
     */
    ClientLifecycleManager(ClientRegistry& clientRegistry, AuthenticationManager& authManager,
                           RegistrationManager& registrationManager,
                           ChannelMembershipManager& channelMembershipManager,
                           PendingIOManager& pendingIOManager);
    ~ClientLifecycleManager();

    /**
     * @brief Adds a newly connected client.
     * @param fd The file descriptor of the client.
     * @throws ClientRegistry::ClientAlreadyExistsException if the client already exists.
     */
    void addClient(int fd, const std::string& hostname);

    /**
     * @brief Disconnects a client (see the class description for the two phases).
     *
     * Leaves every channel (deleting those left empty), releases the nickname, clears the
     * registration and authentication state and, once the output buffer is empty, removes the
     * client from the registry and from the quitting set.
     *
     * @param fd The file descriptor of the client.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void removeClient(int fd);

	/**
     * @brief Sets a client as quitting.
     */
    void setIsQuitting(int fd);


    /**
     * @brief Retrieves the quitting clients that are ready to be closed, i.e. whose output
     * buffer is already empty.
     * @return A snapshot (copy) of the set. The caller must close each socket and then call
     * removeClient() for it.
     */
    std::set<int> getClientsToClose() const;
};

#endif // CLIENT_LIFECYCLE_MANAGER_HPP