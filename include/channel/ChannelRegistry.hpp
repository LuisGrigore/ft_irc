#ifndef CHANNEL_REGISTRY_HPP
#define CHANNEL_REGISTRY_HPP

#include <cstddef>
#include <map>
#include <sstream>
#include <string>

#include "../exceptions_common/BaseException.hpp"
#include "./Channel.hpp"

/**
 * @brief Owns every existing Channel and allows looking them up by name.
 *
 * Channel names are case-insensitive: "#General" and "#general" identify the same channel. The
 * channel keeps the exact capitalization used when it was created.
 *
 * A valid channel name:
 * - starts with '#' or '&',
 * - has at least one character after the prefix and at most NAME_MAX_LENGTH characters in total,
 * - contains no space, comma, colon, BELL (0x07), CR, LF or NUL.
 *
 * The registry is the sole owner of the Channel objects it stores: they are created in
 * addChannel() and destroyed in removeChannel() or when the registry itself is destroyed.
 * References returned by findChannel() are invalidated as soon as the channel is removed.
 *
 * The registry does not look at channel membership. Deciding when a channel must be created or
 * removed (for example, when its last member leaves) is the caller's responsibility.
 *
 * The registry is not copyable.
 */
class ChannelRegistry {
  private:
    std::map<std::string, Channel*> channels;

    static const size_t NAME_MAX_LENGTH = 50;

    ChannelRegistry(const ChannelRegistry&);
    ChannelRegistry& operator=(const ChannelRegistry&);

    static std::string normalize(const std::string& name);
    static bool isValidName(const std::string& name);

  public:
    /**
     * @brief Thrown when no channel exists with the requested name.
     */
    class ChannelNotFoundException : public BaseException {
      private:
        static std::string buildMessage(const std::string& name) {
            std::ostringstream oss;
            oss << "Channel with name: \"" << name << "\" not found";
            return oss.str();
        }

      public:
        ChannelNotFoundException(const std::string& name, const std::exception* cause = NULL,
                                 const std::exception* overrides = NULL)
            : BaseException("ChannelNotFoundException", buildMessage(name), -1, cause, overrides) {}
    };

    /**
     * @brief Thrown when creating a channel whose name is already taken (ignoring case).
     */
    class ChannelAlreadyExistsException : public BaseException {
      private:
        static std::string buildMessage(const std::string& name) {
            std::ostringstream oss;
            oss << "Channel with name: \"" << name << "\" already exists";
            return oss.str();
        }

      public:
        ChannelAlreadyExistsException(const std::string& name, const std::exception* cause = NULL,
                                      const std::exception* overrides = NULL)
            : BaseException("ChannelAlreadyExistsException", buildMessage(name), -1, cause,
                            overrides) {}
    };

    /**
     * @brief Thrown when a channel name does not satisfy the channel name rules.
     */
    class InvalidChannelNameException : public BaseException {
      private:
        static std::string buildMessage(const std::string& name) {
            std::ostringstream oss;
            oss << "Invalid channel name: \"" << name << "\"";
            return oss.str();
        }

      public:
        InvalidChannelNameException(const std::string& name, const std::exception* cause = NULL,
                                    const std::exception* overrides = NULL)
            : BaseException("InvalidChannelNameException", buildMessage(name), -1, cause,
                            overrides) {}
    };

    ChannelRegistry();
    ~ChannelRegistry();

    /**
     * @brief Finds a channel by its name, ignoring case.
     * @param name The name of the channel to find.
     * @return A reference to the Channel owned by the registry.
     * @throws ChannelNotFoundException if no channel with that name exists. This includes
     * invalid names, which can never match a channel.
     */
    Channel& findChannel(const std::string& name);

    /**
     * @brief Creates an empty channel and adds it to the registry.
     * @param name The name of the new channel. Its capitalization is preserved.
     * @throws InvalidChannelNameException if the name is not a valid channel name.
     * @throws ChannelAlreadyExistsException if a channel with an equivalent name (ignoring
     * case) already exists.
     */
    void addChannel(const std::string& name);

    /**
     * @brief Removes a channel from the registry and destroys it, whatever its state.
     *
     * Any reference previously obtained for this channel becomes invalid.
     *
     * @param name The name of the channel to remove, ignoring case.
     * @throws ChannelNotFoundException if no channel with that name exists.
     */
    void removeChannel(const std::string& name);
};

#endif // CHANNEL_REGISTRY_HPP