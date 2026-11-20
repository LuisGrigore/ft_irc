#ifndef AUTHENTICATION_MANAGER_HPP
#define AUTHENTICATION_MANAGER_HPP

#include <cstddef>
#include <string>

#include "../client/ClientRegistry.hpp"
#include "../exceptions_common/BaseException.hpp"

/**
 * @brief Manages authentication for clients already registered in ClientRegistry.
 *
 * AuthenticationManager verifies client passwords and maintains each client's
 * authentication state.
 *
 * It does not create, register, remove, or destroy clients. Client lifecycle
 * management remains entirely under the responsibility of ClientRegistry.
 *
 * The expected password is immutable after construction.
 *
 * @note The class does not own the ClientRegistry instance.
 */
class AuthenticationManager {
  private:
    ClientRegistry& clientRegistry;
    const std::string password;

  public:
    /**
     * @brief Thrown when a client fails authentication.
     *
     * This exception is raised when the provided password does not match
     * the expected password.
     */
    class AuthenticationFailedException : public BaseException {
      public:
        AuthenticationFailedException(const std::exception* cause = NULL,
                                      const std::exception* overrides = NULL)
            : BaseException("AuthenticationFailedException", "Incorrect password", -1, cause,
                            overrides) {}
    };

    /**
     * @brief Constructs an AuthenticationManager.
     *
     * @param clientRegistry Reference to the registry containing existing clients.
     * @param password Expected password required for client authentication.
     */
    AuthenticationManager(ClientRegistry& clientRegistry, const std::string& password);

    /**
     * @brief Destroys the AuthenticationManager.
     */
    ~AuthenticationManager();

    /**
     * @brief Authenticates an existing client.
     *
     * Verifies the provided password and marks the client as authenticated
     * if the password is correct.
     *
     * The client must already exist in ClientRegistry.
     *
     * @param fd File descriptor identifying the client.
     * @param password Password provided for authentication.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     * @throws AuthenticationFailedException if the password is incorrect.
     */
    void authenticate(int fd, const std::string& password);

    /**
     * @brief Removes the authenticated state of an existing client.
     *
     * The client remains registered in ClientRegistry.
     *
     * @param fd File descriptor identifying the client.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void unAuthenticate(int fd);

    /**
     * @brief Checks whether an existing client is authenticated.
     *
     * @param fd File descriptor identifying the client.
     * @return true if the client is authenticated, false otherwise.
     *
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    bool isAuthenticated(int fd) const;
};

#endif // AUTHENTICATION_MANAGER_HPP