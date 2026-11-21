#include "../include/channel/Channel.hpp"

Channel::Channel(const std::string& name)
    : name(name), topic(), members(), operators(), invited(), key(), limit(0), inviteOnly(false),
      topicProtected(false) {}

Channel::~Channel() {}

// Getters

std::string Channel::getName() const { return name; }

std::string Channel::getTopic() const { return topic; }

std::string Channel::getKey() const { return key; }

int Channel::getLimit() const { return limit; }

// Setters

void Channel::setTopic(const std::string& topic) { this->topic = topic; }

void Channel::setKey(const std::string& key) { this->key = key; }

void Channel::setLimit(int limit) { this->limit = limit; }

// Modes

bool Channel::isInviteOnly() const { return inviteOnly; }

void Channel::setInviteOnly(bool inviteOnly) { this->inviteOnly = inviteOnly; }

bool Channel::isTopicProtected() const { return topicProtected; }

void Channel::setTopicProtected(bool topicProtected) { this->topicProtected = topicProtected; }

// Members

void Channel::addMember(int fd) { members.insert(fd); }

void Channel::removeMember(int fd) {
    members.erase(fd);
    operators.erase(fd);
}

bool Channel::isMember(int fd) const { return members.find(fd) != members.end(); }

std::set<int> Channel::getMembers() const { return members; }

// Operators

void Channel::addOperator(int fd) { operators.insert(fd); }

void Channel::removeOperator(int fd) { operators.erase(fd); }

bool Channel::isOperator(int fd) const { return operators.find(fd) != operators.end(); }

// Invitations

void Channel::addInvited(int fd) { invited.insert(fd); }

void Channel::consumeInvited(int fd) { invited.erase(fd); }

bool Channel::isInvited(int fd) const { return invited.find(fd) != invited.end(); }