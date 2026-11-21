#ifndef PENDING_IO_MANAGER_HPP
#define PENDING_IO_MANAGER_HPP

#include "../client/ClientRegistry.hpp"
#include <set>

class PendingIOManager {
  private:
    ClientRegistry& clientRegistry;
    std::set<int> hasPendingInput;
    std::set<int> hasPendingOutput;

  public:
    PendingIOManager(ClientRegistry& clientRegistry);
    ~PendingIOManager();
    void pushInput(int fd, const std::string& data);
    bool getNextInputLine(int fd, std::string& line);
    void pushOutput(int fd, const std::string& data);
    std::string peekOutput(int fd, size_t length) const;
    void consumeOutput(int fd, size_t length);
    std::set<int> getHasPendingInput() const;
    std::set<int> getHasPendingOutput() const;
};

#endif // PENDING_IO_MANAGER_HPP