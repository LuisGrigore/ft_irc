#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>

class Channel {
  private:
    const std::string name;
    std::string topic;
    std::set<int> members;
    std::set<int> operators;
    std::set<int> invited;
    std::string key;
    int limit;
    bool inviteOnly;
    bool topicProtected;

  public:
    /**
     * @brief Creates an empty channel with no topic, no key, no user limit and no modes set.
     * @param name The name of the channel.
     */
    Channel(const std::string& name);
    ~Channel();

    // Getters
    std::string getName() const;
    std::string getTopic() const;
    std::string getKey() const;

    /**
     * @brief Retrieves the maximum number of members allowed in the channel.
     * @return The user limit, or 0 if the channel has no limit.
     */
    int getLimit() const;

    // Setters
    void setTopic(const std::string& topic);

    /**
     * @brief Sets the channel key. An empty key means the channel has no key.
     */
    void setKey(const std::string& key);

    /**
     * @brief Sets the maximum number of members. A value of 0 removes the limit.
     */
    void setLimit(int limit);

    // Modes
    bool isInviteOnly() const;
    void setInviteOnly(bool inviteOnly);
    bool isTopicProtected() const;
    void setTopicProtected(bool topicProtected);

    // Members
    /**
     * @brief Adds a client to the channel. Adding an existing member has no effect.
     * @param fd The file descriptor of the client.
     */
    void addMember(int fd);

    /**
     * @brief Removes a client from the channel, including its operator status.
     * Removing a client that is not a member has no effect.
     * @param fd The file descriptor of the client.
     */
    void removeMember(int fd);
    bool isMember(int fd) const;
    std::set<int> getMembers() const;

    // Operators
    void addOperator(int fd);
    void removeOperator(int fd);
    bool isOperator(int fd) const;

    // Invitations
    void addInvited(int fd);

    /**
     * @brief Consumes the invitation of a client, so it cannot be used again.
     * Consuming an invitation that does not exist has no effect.
     * @param fd The file descriptor of the client.
     */
    void consumeInvited(int fd);
    bool isInvited(int fd) const;
};

#endif // CHANNEL_HPP