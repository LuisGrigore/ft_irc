#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstddef>
#include <set>
#include <string>

class Client {
  private:
    class ClientBuffer {
      private:
        std::string buffer;

      public:
        ClientBuffer();
        ~ClientBuffer();
        void push(const std::string& data);

        std::string peek(size_t length) const;
        void consume(size_t length);

        bool empty() const;
        size_t size() const;
        void clear();

        size_t find(const std::string& needle) const;
    };

    int fd;
    std::string nick;
    std::string username;
    std::string realname;
    std::string hostname;
    std::set<std::string> channels;
    bool authenticated;
    bool registered;
    bool quitting;

    ClientBuffer inputBuffer;
    ClientBuffer outputBuffer;

  public:
    Client(int fd);
    ~Client();

    // Getters
    int getFd() const;
    const std::string& getNick() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    std::set<std::string> getChannel() const;

    // Setters
    void setFd(int fd);
    void setNick(const std::string& nick);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    void setAuthenticated(bool authenticated);
    void setRegistered(bool registered);
    void setQuitting(bool quitting);

    // Predicates
    bool isAuthenticated() const;
    bool isRegistered() const;
    bool isQuitting() const;
    bool isInputBufferEmpty() const;
    bool isOutputBufferEmpty() const;

    // Channel methods
    /**
     * @brief Registers a channel as joined by the client.
     * @param channelName The name of the channel. Adding an already joined channel has no effect.
     */
    void addChannel(const std::string& channelName);

    /**
     * @brief Removes a channel from the set of channels joined by the client.
     * @param channelName The name of the channel. Removing a channel that is not joined has no
     * effect.
     */
    void removeChannel(const std::string& channelName);

    // IO Buffer methods
    /**
     * @brief Appends data to the end of the input buffer of the client.
     * @param data The data to be appended to the input buffer.
     */
    void pushInput(const std::string& data);

    /**
     * @brief Appends data to the end of the output buffer of the client.
     * @param data The data to be appended to the output buffer.
     */
    void pushOutput(const std::string& data);

    /**
     * @brief Retrieves the next line from the input buffer of the client consuming it.
     * The trailing "\n" (and "\r" if present) is stripped from the returned line.
     * @param line The string to store the retrieved line.
     * @return true if a line was successfully retrieved, false if there are no more complete lines.
     */
    bool getNextInputLine(std::string& line);

    /**
     * @brief Retrieves a specified number of characters from the output buffer of the client
     * without consuming them.
     * @param length The number of characters to retrieve.
     * @return The retrieved string, empty if the buffer is empty.
     */
    std::string peekOutput(size_t length) const;

    /**
     * @brief Consumes a specified number of characters from the output buffer of the client.
     * @param length The number of characters to consume.
     */
    void consumeOutput(size_t length);

    /**
     * @brief Empties input buffer.
     */
    void clearInput();

    /**
     * @brief Empties output buffer.
     */
    void clearOutput();
};

#endif // CLIENT_HPP