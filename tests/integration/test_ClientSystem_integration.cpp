#include "managers/ClientSystem.hpp"

#include "CppUTest/TestHarness.h"

#undef new

#include <set>
#include <string>

TEST_GROUP(ClientSystem) {
    ClientRegistry* clientRegistry;
    ClientLifecycleManager* lifecycleManager;
    PendingIOManager* pendingIOManager;
    AuthenticationManager* authManager;
    NickManager* nickManager;
    RegistrationManager* registrationManager;
    ChannelRegistry* channelRegistry;
    ChannelMembershipManager* channelMembershipManager;
    ClientSystem* system;
    std::string secret;

    static const int FD_A = 10;
    static const int FD_B = 11;

    void setup() {
        secret = "secret";
        clientRegistry = new ClientRegistry();
        channelRegistry = new ChannelRegistry();
        channelMembershipManager = new ChannelMembershipManager(*channelRegistry, *clientRegistry);
        pendingIOManager = new PendingIOManager(*clientRegistry);
        authManager = new AuthenticationManager(*clientRegistry, secret);
        nickManager = new NickManager();
        registrationManager = new RegistrationManager(*clientRegistry, *nickManager);
        lifecycleManager =
            new ClientLifecycleManager(*clientRegistry, *authManager, *registrationManager,
                                       *channelMembershipManager, *pendingIOManager);
        system = new ClientSystem(*lifecycleManager, *pendingIOManager, *authManager,
                                  *registrationManager, *clientRegistry);
    }

    void teardown() {
        // Destroy in reverse order of creation: the facade first,
        // since it holds references to the managers.
        delete system;
        delete registrationManager;
        delete nickManager;
        delete authManager;
        delete pendingIOManager;
        delete lifecycleManager;
        delete channelMembershipManager;
        delete channelRegistry;
        delete clientRegistry;

        system = NULL;
        registrationManager = NULL;
        nickManager = NULL;
        authManager = NULL;
        pendingIOManager = NULL;
        lifecycleManager = NULL;
        channelMembershipManager = NULL;
        channelRegistry = NULL;
        clientRegistry = NULL;
    }
};

// Full flows

TEST(ClientSystem, FlowMultipleClientsConnectAuthenticateAndRegister) {
    try {
        // Clients 5, 6 and 7 use the right secret; client 8 uses a wrong one.
        system->addClient(5, "host5");
        system->addClient(6, "host6");
        system->addClient(7, "host7");
        system->addClient(8, "host8");

        system->authenticate(5, secret);
        system->authenticate(6, secret);
        system->authenticate(7, secret);
        CHECK_THROWS(AuthenticationManager::AuthenticationFailedException,
                     system->authenticate(8, secret + "incorrect"));

        system->setNick(5, "Alice");
        system->setNick(6, "Bob");
        system->setNick(7, "Carol");

        // Error: nickname already in use (case-insensitive after normalization).
        CHECK_THROWS(RegistrationManager::NicknameAlreadyInUseException,
                     system->setNick(8, "alice"));
        // Error: invalid nickname.
        CHECK_THROWS(RegistrationManager::InvalidNicknameException,
                     system->setNick(8, "bad$%&/(nick"));
        // Failed attempts must not leave client 8 with a nick.
        CHECK_TRUE(clientRegistry->findClient(8).getNick().empty());

        // Client 8 recovers with a valid, free nick.
        system->setNick(8, "Dave");

        for (int fd = 5; fd <= 8; ++fd) {
            system->setUsername(fd, "user");
            system->setRealname(fd, "Real Name");
        }

        // Success cases.
        for (int fd = 5; fd <= 7; ++fd) {
            CHECK_TRUE(system->isAuthenticated(fd));
            CHECK_TRUE(system->isRegistered(fd));
        }
        CHECK_EQUAL(5, system->getFdByNick("alice"));
        CHECK_EQUAL(6, system->getFdByNick("Bob"));
        CHECK_EQUAL(7, system->getFdByNick("CAROL"));
        CHECK_EQUAL(8, system->getFdByNick("dave"));

        // Client 8 is registered but NOT authenticated (wrong secret).
        CHECK_FALSE(system->isAuthenticated(8));
        CHECK_TRUE(system->isRegistered(8));

        // Error: unknown nick.
        CHECK_THROWS(NickManager::NickNotFoundException, system->getFdByNick("nobody"));

        // A client that stops halfway is not registered.
        system->addClient(9, "host9");
        system->authenticate(9, secret);
        system->setNick(9, "Eve");
        system->setUsername(9, "user");
        CHECK_FALSE(system->isRegistered(9));

        // Details reflect the whole flow.
        ClientSystem::ClientDetails details = system->getClientDetails(5);
        CHECK_EQUAL(5, details.fd);
        CHECK_EQUAL(std::string("alice"), details.nick);
        CHECK_EQUAL(std::string("host5"), details.hostname);
        CHECK_TRUE(details.authenticated);
        CHECK_TRUE(details.registered);
        CHECK_FALSE(details.quitting);
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

TEST(ClientSystem, FlowMultipleRegisteredClientsReceiveInputAndSendOutput) {
    try {
        const char* nicks[] = {"Alice", "Bob", "Carol"};
        for (int i = 0; i < 3; ++i) {
            int fd = 5 + i;
            system->addClient(fd, "host");
            system->authenticate(fd, secret);
            system->setNick(fd, nicks[i]);
            system->setUsername(fd, "user");
            system->setRealname(fd, "Real Name");
            CHECK_TRUE(system->isRegistered(fd));
        }

        // Input: client 5 sends two lines, client 6 an incomplete one, client 7 one line.
        system->pushInput(5, "PING a\r\nPING b\r\n");
        system->pushInput(6, "PART");
        system->pushInput(7, "NICK x\r\n");

        std::set<int> pendingInput = system->getHasPendingInput();
        LONGS_EQUAL(3, pendingInput.size());

        std::string line;
        CHECK_TRUE(system->getNextInputLine(5, line));
        CHECK_EQUAL(std::string("PING a"), line);
        CHECK_TRUE(system->getNextInputLine(5, line));
        CHECK_EQUAL(std::string("PING b"), line);
        CHECK_FALSE(system->getNextInputLine(5, line));

        // Client 6 has no complete line yet; client 7 still has its line.
        line.clear();
        CHECK_FALSE(system->getNextInputLine(6, line));
        CHECK_TRUE(line.empty());
        CHECK_TRUE(system->getNextInputLine(7, line));
        CHECK_EQUAL(std::string("NICK x"), line);

        // Only client 6 still holds (partial) input.
        pendingInput = system->getHasPendingInput();
        LONGS_EQUAL(1, pendingInput.size());
        CHECK_TRUE(pendingInput.count(6) > 0);

        // Client 6 completes its line; inputs of different clients never mix.
        system->pushInput(6, "ial\r\n");
        CHECK_TRUE(system->getNextInputLine(6, line));
        CHECK_EQUAL(std::string("PARTial"), line);
        CHECK_TRUE(system->getHasPendingInput().empty());

        // Output: every client gets a different reply.
        system->pushOutput(5, "PONG a\r\n");
        system->pushOutput(6, "PARTED\r\n");
        system->pushOutput(7, "NICK ok\r\n");
        LONGS_EQUAL(3, system->getHasPendingOutput().size());

        CHECK_EQUAL(std::string("PONG"), system->peekOutput(5, 4));
        CHECK_EQUAL(std::string("PARTED\r\n"), system->peekOutput(6, 100));

        // Peeking does not consume.
        LONGS_EQUAL(3, system->getHasPendingOutput().size());

        // Partial consume keeps the client pending, full consume releases it.
        system->consumeOutput(5, 5);
        CHECK_EQUAL(std::string("a\r\n"), system->peekOutput(5, 100));
        system->consumeOutput(5, 100);
        system->consumeOutput(6, 8);

        std::set<int> pendingOutput = system->getHasPendingOutput();
        LONGS_EQUAL(1, pendingOutput.size());
        CHECK_TRUE(pendingOutput.count(7) > 0);

        // None of this changes registration state.
        for (int fd = 5; fd <= 7; ++fd) {
            CHECK_TRUE(system->isAuthenticated(fd));
            CHECK_TRUE(system->isRegistered(fd));
        }

        // Error: I/O on a client that does not exist.
        CHECK_THROWS(ClientRegistry::ClientNotFoundException, system->pushInput(99, "x\r\n"));
        CHECK_THROWS(ClientRegistry::ClientNotFoundException, system->pushOutput(99, "x"));
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}

TEST(ClientSystem, FlowClientsQuitConsumeOutputAndAreRemoved) {
    try {
        const char* nicks[] = {"Alice", "Bob", "Carol"};
        for (int i = 0; i < 3; ++i) {
            int fd = 5 + i;
            system->addClient(fd, "host");
            system->authenticate(fd, secret);
            system->setNick(fd, nicks[i]);
            system->setUsername(fd, "user");
            system->setRealname(fd, "Real Name");
            system->pushInput(fd, "QUIT\r\n");
            system->pushOutput(fd, "ERROR :Closing link\r\n");
        }

        // Clients 5 and 6 quit; client 7 stays connected.
        system->setIsQuitting(5);
        system->setIsQuitting(6);

        // Quitting with pending output: nobody can be closed yet.
        CHECK_TRUE(system->getClientsToClose().empty());
        CHECK_TRUE(system->getClientDetails(5).quitting);
        CHECK_FALSE(system->getClientDetails(7).quitting);

        // Client 5 flushes everything -> closable. Client 6 only flushes part -> not closable.
        system->consumeOutput(5, 100);
        system->consumeOutput(6, 5);
        std::set<int> toClose = system->getClientsToClose();
        LONGS_EQUAL(1, toClose.size());
        CHECK_TRUE(toClose.count(5) > 0);

        // Remove client 5: its state and nick are released.
        system->removeClient(5);
        CHECK_THROWS(ClientRegistry::ClientNotFoundException, clientRegistry->findClient(5));
        CHECK_THROWS(NickManager::NickNotFoundException, system->getFdByNick("alice"));
        CHECK_TRUE(system->getClientsToClose().empty());

        // Client 6 finishes flushing -> closable -> removed.
        system->consumeOutput(6, 100);
        toClose = system->getClientsToClose();
        LONGS_EQUAL(1, toClose.size());
        CHECK_TRUE(toClose.count(6) > 0);
        system->removeClient(6);
        CHECK_THROWS(ClientRegistry::ClientNotFoundException, clientRegistry->findClient(6));
        CHECK_THROWS(NickManager::NickNotFoundException, system->getFdByNick("bob"));

        // The remaining client is untouched, even though it also has pending data.
        CHECK_TRUE(system->getClientsToClose().empty());
        CHECK_TRUE(system->isRegistered(7));
        CHECK_EQUAL(7, system->getFdByNick("carol"));
        std::set<int> pendingOutput = system->getHasPendingOutput();
        LONGS_EQUAL(1, pendingOutput.size());
        CHECK_TRUE(pendingOutput.count(7) > 0);
        std::set<int> pendingInput = system->getHasPendingInput();
        LONGS_EQUAL(1, pendingInput.size());
        CHECK_TRUE(pendingInput.count(7) > 0);

        // A removed fd and its nick can be reused by a brand new client.
        system->addClient(5, "newhost");
        system->setNick(5, "Alice");
        CHECK_EQUAL(5, system->getFdByNick("alice"));
        ClientSystem::ClientDetails details = system->getClientDetails(5);
        CHECK_FALSE(details.authenticated);
        CHECK_FALSE(details.registered);
        CHECK_FALSE(details.quitting);
        CHECK_EQUAL(std::string("newhost"), details.hostname);

        // Error: nothing is left over from the old client with the same fd.
        CHECK_TRUE(clientRegistry->findClient(5).isInputBufferEmpty());
        CHECK_TRUE(clientRegistry->findClient(5).isOutputBufferEmpty());
    } catch (const std::exception& e) {
        FAIL(e.what());
    }
}