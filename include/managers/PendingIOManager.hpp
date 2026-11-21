#ifndef PENDING_IO_MANAGER_HPP
#define PENDING_IO_MANAGER_HPP

#include <cstddef>
#include <set>
#include <string>

#include "../client/ClientRegistry.hpp"

/**
 * @brief Reads and writes the I/O buffers of the clients in a ClientRegistry and keeps track of
 * which file descriptors have data to process or to send.
 *
 * The manager does not own the ClientRegistry, and it does not create or remove clients or touch
 * their sockets. It only operates on clients that already exist.
 *
 * An fd is reported as having pending input while its input buffer holds at least one complete
 * line, and as having pending output while its output buffer is not empty. Both sets are updated
 * by the operations below, so callers (for example the poll loop) never have to inspect the
 * buffers themselves.
 *
 * Every operation on an fd that is not in the ClientRegistry throws
 * ClientRegistry::ClientNotFoundException and leaves the pending state unchanged.
 *
 * The manager is not copyable.
 */
class PendingIOManager {
  private:
    ClientRegistry& clientRegistry;
    std::set<int> hasPendingInput;
    std::set<int> hasPendingOutput;

    PendingIOManager(const PendingIOManager&);
    PendingIOManager& operator=(const PendingIOManager&);

  public:
    /**
     * @brief Constructs a PendingIOManager.
     * @param clientRegistry Registry containing the clients whose buffers are managed.
     */
    PendingIOManager(ClientRegistry& clientRegistry);
    ~PendingIOManager();

    /**
     * @brief Appends data received from a client to its input buffer.
     *
     * The data may contain partial lines; they are kept until the rest arrives.
     *
     * @param fd The file descriptor of the client.
     * @param data The data to append.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void pushInput(int fd, const std::string& data);

    /**
     * @brief Extracts the next complete line from the input buffer of a client, consuming it.
     *
     * The line terminator ("\n" or "\r\n") is stripped. Exactly one line is extracted per call.
     * If no complete line is available, `line` and the buffer are left untouched. The fd stops
     * being reported as having pending input once no complete line remains.
     *
     * @param fd The file descriptor of the client.
     * @param line The string to store the extracted line.
     * @return true if a line was extracted, false if there is no complete line.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    bool getNextInputLine(int fd, std::string& line);

    /**
     * @brief Appends data to be sent to a client to its output buffer.
     * @param fd The file descriptor of the client.
     * @param data The data to append.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void pushOutput(int fd, const std::string& data);

    /**
     * @brief Retrieves up to `length` bytes from the output buffer without consuming them.
     *
     * Intended for writes to a socket: peek, send, then consumeOutput() with the number of
     * bytes actually sent.
     *
     * @param fd The file descriptor of the client.
     * @param length The maximum number of bytes to retrieve.
     * @return The retrieved bytes, shorter than `length` if fewer are available, empty if the
     * buffer is empty.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    std::string peekOutput(int fd, size_t length) const;

    /**
     * @brief Removes the first `length` bytes from the output buffer, keeping the rest in order.
     *
     * Consuming more bytes than available empties the buffer. The fd stops being reported as
     * having pending output once its buffer is empty.
     *
     * @param fd The file descriptor of the client.
     * @param length The number of bytes that were actually sent.
     * @throws ClientRegistry::ClientNotFoundException if the client does not exist.
     */
    void consumeOutput(int fd, size_t length);

    /**
     * @brief Retrieves the file descriptors that have input to process.
     * @return A snapshot (copy) of the set; later operations do not modify it.
     */
    std::set<int> getHasPendingInput() const;

    /**
     * @brief Retrieves the file descriptors that have output to send.
     * @return A snapshot (copy) of the set; later operations do not modify it.
     */
    std::set<int> getHasPendingOutput() const;
};

#endif // PENDING_IO_MANAGER_HPP