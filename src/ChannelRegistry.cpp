#include "../include/channel/ChannelRegistry.hpp"

const size_t ChannelRegistry::NAME_MAX_LENGTH;

ChannelRegistry::ChannelRegistry() : channels() {}

ChannelRegistry::~ChannelRegistry() {
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end();
         ++it) {
        delete it->second;
    }
    channels.clear();
}

std::string ChannelRegistry::normalize(const std::string& name) {
    std::string result = name;
    for (std::string::size_type i = 0; i < result.size(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = static_cast<char>(result[i] + ('a' - 'A'));
    }
    return result;
}

// Validation logic based on RFC 2812 (only '#' and '&' prefixes are supported)
bool ChannelRegistry::isValidName(const std::string& name) {
    if (name.size() < 2 || name.size() > NAME_MAX_LENGTH)
        return false;

    if (name[0] != '#' && name[0] != '&')
        return false;

    for (std::string::size_type i = 1; i < name.size(); ++i) {
        char c = name[i];
        if (c == ' ' || c == ',' || c == ':' || c == '\a' || c == '\r' || c == '\n' || c == '\0')
            return false;
    }
    return true;
}

Channel& ChannelRegistry::findChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = channels.find(normalize(name));
    if (it == channels.end())
        throw ChannelNotFoundException(name);
    return *it->second;
}

void ChannelRegistry::addChannel(const std::string& name) {
    if (!isValidName(name))
        throw InvalidChannelNameException(name);

    const std::string key = normalize(name);
    if (channels.find(key) != channels.end())
        throw ChannelAlreadyExistsException(name);

    Channel* channel = new Channel(name);
    try {
        channels.insert(std::make_pair(key, channel));
    } catch (...) {
        delete channel;
        throw;
    }
}

void ChannelRegistry::removeChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = channels.find(normalize(name));
    if (it == channels.end())
        throw ChannelNotFoundException(name);
    delete it->second;
    channels.erase(it);
}