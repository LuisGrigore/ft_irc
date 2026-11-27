#include "CppUTest/TestHarness.h"
#include "channel/ChannelRegistry.hpp"
#include "client/ClientRegistry.hpp"
#include "managers/AuthenticationManager.hpp"
#include "managers/ChannelMembershipManager.hpp"
#include "managers/ClientLifecycleManager.hpp"
#include "managers/NickManager.hpp"
#include "managers/PendingIOManager.hpp"
#include "managers/RegistrationManager.hpp"

#include <cstddef>
#include <set>
#include <string>

/*
 * CppUTest forces `#define new new(__FILE__, __LINE__)` through
 * MemoryLeakDetectorNewMacros.h (included from TestHarness.h). As the
 * project compiles with CppUTest leak detection disabled
 * (MEMORY_LEAK_DETECTION OFF), no one provides the placement `new` operator
 * required by that macro. It is undefined here, only in test translation
 * units, to use the ordinary `new` from the standard library.
 */
#undef new

TEST_GROUP(ClientLifecycleManager) {
    ClientRegistry* clients;
    NickManager* nicks;
    ChannelRegistry* channels;
    AuthenticationManager* auth;
    RegistrationManager* registration;
    ChannelMembershipManager* membership;
    PendingIOManager* pendingIO;
    ClientLifecycleManager* manager;

    void setup() {
        clients = new ClientRegistry();
        nicks = new NickManager();
        channels = new ChannelRegistry();
        auth = new AuthenticationManager(*clients, "secret");
        registration = new RegistrationManager(*clients, *nicks);
        membership = new ChannelMembershipManager(*channels, *clients);
        pendingIO = new PendingIOManager(*clients);
        manager =
            new ClientLifecycleManager(*clients, *auth, *registration, *membership, *pendingIO);
    }

    void teardown() {
        delete manager;
        delete pendingIO;
        delete membership;
        delete registration;
        delete auth;
        delete channels;
        delete nicks;
        delete clients;
    }

    static bool contains(const std::set<int>& fds, int fd) { return fds.find(fd) != fds.end(); }

    // Adds a client that is authenticated and fully registered with the given nick.
    void addRegisteredClient(int fd, const std::string& hostname, const std::string& nick) {
        manager->addClient(fd, hostname);
        auth->authenticate(fd, "secret");
        registration->setNick(fd, nick);
        registration->setUsername(fd, nick + "user");
        registration->setRealname(fd, nick + " real");
    }

    bool clientExists(int fd) {
        try {
            clients->findClient(fd);
            return true;
        } catch (const ClientRegistry::ClientNotFoundException&) {
            return false;
        }
    }

    bool channelExists(const std::string& name) {
        try {
            channels->findChannel(name);
            return true;
        } catch (const ChannelRegistry::ChannelNotFoundException&) {
            return false;
        }
    }
};

// Add client

TEST(ClientLifecycleManager, AddClientAddsClientToRegistry_AC_213) {
    manager->addClient(5, "host");

    CHECK_EQUAL(5, clients->findClient(5).getFd());
}

TEST(ClientLifecycleManager, AddClientSetsClientHostname_AC_213) {
    manager->addClient(5, "host");

    STRCMP_EQUAL("host", clients->findClient(5).getHostname().c_str());
}

TEST(ClientLifecycleManager, AddClientAcceptsEmptyHostname_AC_213) {
    manager->addClient(5, "");

    STRCMP_EQUAL("", clients->findClient(5).getHostname().c_str());
}

TEST(ClientLifecycleManager, AddedClientIsNotAuthenticated_AC_213) {
    manager->addClient(5, "host");

    CHECK_FALSE(clients->findClient(5).isAuthenticated());
    CHECK_FALSE(auth->isAuthenticated(5));
}

TEST(ClientLifecycleManager, AddedClientIsNotRegistered_AC_213) {
    manager->addClient(5, "host");

    CHECK_FALSE(clients->findClient(5).isRegistered());
    CHECK_FALSE(registration->isRegistered(5));
}

TEST(ClientLifecycleManager, AddedClientIsNotQuitting_AC_213) {
    manager->addClient(5, "host");

    CHECK_FALSE(clients->findClient(5).isQuitting());
    CHECK_FALSE(contains(manager->getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, AddedClientHasNoDataChannelsOrPendingIO_AC_213) {
    manager->addClient(5, "host");

    Client& client = clients->findClient(5);
    STRCMP_EQUAL("", client.getNick().c_str());
    CHECK_TRUE(client.getChannels().empty());
    CHECK_TRUE(client.isInputBufferEmpty());
    CHECK_TRUE(client.isOutputBufferEmpty());
    CHECK_TRUE(pendingIO->getHasPendingInput().empty());
    CHECK_TRUE(pendingIO->getHasPendingOutput().empty());
}

TEST(ClientLifecycleManager, AddClientAllowsSeveralDistinctFdsWithOwnHostnames_AC_213) {
    manager->addClient(5, "host1");
    manager->addClient(6, "host2");
    manager->addClient(7, "host3");

    CHECK_EQUAL(5, clients->findClient(5).getFd());
    CHECK_EQUAL(6, clients->findClient(6).getFd());
    CHECK_EQUAL(7, clients->findClient(7).getFd());
    STRCMP_EQUAL("host1", clients->findClient(5).getHostname().c_str());
    STRCMP_EQUAL("host2", clients->findClient(6).getHostname().c_str());
    STRCMP_EQUAL("host3", clients->findClient(7).getHostname().c_str());
}

TEST(ClientLifecycleManager, AddClientWithExistingFdThrowsAlreadyExists_AC_213) {
    manager->addClient(5, "host");

    CHECK_THROWS(ClientRegistry::ClientAlreadyExistsException, manager->addClient(5, "host"));
}

TEST(ClientLifecycleManager, AddClientWithExistingFdKeepsOriginalClientState_AC_213) {
    addRegisteredClient(5, "host1", "alice");

    CHECK_THROWS(ClientRegistry::ClientAlreadyExistsException, manager->addClient(5, "host2"));

    CHECK_TRUE(auth->isAuthenticated(5));
    CHECK_TRUE(registration->isRegistered(5));
    STRCMP_EQUAL("alice", clients->findClient(5).getNick().c_str());
    STRCMP_EQUAL("host1", clients->findClient(5).getHostname().c_str());
}

TEST(ClientLifecycleManager, AddClientAfterRemoveClientCreatesFreshClient_AC_213) {
    addRegisteredClient(5, "host1", "alice");
    manager->removeClient(5);

    manager->addClient(5, "host2");

    Client& client = clients->findClient(5);
    STRCMP_EQUAL("", client.getNick().c_str());
    STRCMP_EQUAL("host2", client.getHostname().c_str());
    CHECK_FALSE(client.isAuthenticated());
    CHECK_FALSE(client.isRegistered());
    CHECK_FALSE(client.isQuitting());
}

// Remove client: everything owned by the client is released

TEST(ClientLifecycleManager, RemoveClientRemovesItFromRegistry_AC_214) {
    manager->addClient(5, "host");

    manager->removeClient(5);

    CHECK_FALSE(clientExists(5));
}

TEST(ClientLifecycleManager, RemoveClientLeavesEveryChannel_AC_214) {
    addRegisteredClient(5, "host", "alice");
    addRegisteredClient(6, "host", "bob");
    membership->createChannel(6, "#first");
    membership->createChannel(6, "#second");
    membership->addMemberToChannel(5, "#first");
    membership->addMemberToChannel(5, "#second");

    manager->removeClient(5);

    CHECK_FALSE(channels->findChannel("#first").isMember(5));
    CHECK_FALSE(channels->findChannel("#second").isMember(5));
}

TEST(ClientLifecycleManager, RemoveClientDeletesChannelsLeftEmpty_AC_214) {
    addRegisteredClient(5, "host", "alice");
    membership->createChannel(5, "#first");
    membership->createChannel(5, "#second");

    manager->removeClient(5);

    CHECK_FALSE(channelExists("#first"));
    CHECK_FALSE(channelExists("#second"));
}

TEST(ClientLifecycleManager, RemoveClientKeepsChannelsWithOtherMembers_AC_214) {
    addRegisteredClient(5, "host", "alice");
    addRegisteredClient(6, "host", "bob");
    membership->createChannel(5, "#shared");
    membership->addMemberToChannel(6, "#shared");

    manager->removeClient(5);

    CHECK_TRUE(channelExists("#shared"));
    CHECK_TRUE(channels->findChannel("#shared").isMember(6));
    CHECK_FALSE(channels->findChannel("#shared").isOperator(5));
}

TEST(ClientLifecycleManager, RemoveClientClearsInvitationsOfJoinedChannels_AC_214) {
    addRegisteredClient(5, "host", "alice");
    addRegisteredClient(6, "host", "bob");
    membership->createChannel(6, "#shared");
    membership->addMemberToChannel(5, "#shared");
    membership->addInviteToChannel(5, "#shared");

    manager->removeClient(5);

    CHECK_FALSE(channels->findChannel("#shared").isInvited(5));
}

TEST(ClientLifecycleManager, RemoveClientReleasesNickname_AC_214) {
    addRegisteredClient(5, "host", "alice");

    manager->removeClient(5);

    CHECK_TRUE(nicks->isAvailable("alice"));
}

TEST(ClientLifecycleManager, RemoveClientDiscardsPendingInput_AC_214) {
    manager->addClient(5, "host");
    pendingIO->pushInput(5, "NICK alice\r\n");

    manager->removeClient(5);

    CHECK_FALSE(contains(pendingIO->getHasPendingInput(), 5));
}

TEST(ClientLifecycleManager, RemoveClientDiscardsPendingOutput_AC_214) {
    manager->addClient(5, "host");
    pendingIO->pushOutput(5, "bye");

    manager->removeClient(5);

    CHECK_FALSE(contains(pendingIO->getHasPendingOutput(), 5));
}

TEST(ClientLifecycleManager, RemoveClientWithPendingOutputRemovesItImmediately_AC_214) {
    addRegisteredClient(5, "host", "alice");
    pendingIO->pushOutput(5, "ERROR :Closing link\r\n");

    manager->removeClient(5);

    CHECK_FALSE(clientExists(5));
    CHECK_TRUE(nicks->isAvailable("alice"));
}

TEST(ClientLifecycleManager, RemoveQuittingClientRemovesItFromRegistry_AC_214) {
    manager->addClient(5, "host");
    manager->setIsQuitting(5);

    manager->removeClient(5);

    CHECK_FALSE(clientExists(5));
}

TEST(ClientLifecycleManager, RemoveClientOnlyAffectsTheGivenClient_AC_214) {
    addRegisteredClient(5, "host", "alice");
    addRegisteredClient(6, "host", "bob");
    pendingIO->pushInput(6, "PING\r\n");
    pendingIO->pushOutput(6, "PONG");

    manager->removeClient(5);

    CHECK_TRUE(auth->isAuthenticated(6));
    CHECK_TRUE(registration->isRegistered(6));
    CHECK_FALSE(nicks->isAvailable("bob"));
    CHECK_TRUE(contains(pendingIO->getHasPendingInput(), 6));
    CHECK_TRUE(contains(pendingIO->getHasPendingOutput(), 6));
}

// No stale references after removal

TEST(ClientLifecycleManager, RemovedClientLeavesNoPendingState_AC_215) {
    manager->addClient(5, "host");
    pendingIO->pushInput(5, "a\r\n");
    pendingIO->pushOutput(5, "b");

    manager->removeClient(5);

    CHECK_FALSE(contains(pendingIO->getHasPendingInput(), 5));
    CHECK_FALSE(contains(pendingIO->getHasPendingOutput(), 5));
}

TEST(ClientLifecycleManager, NicknameIsNotResolvableAfterRemoval_AC_215) {
    addRegisteredClient(5, "host", "alice");

    manager->removeClient(5);

    CHECK_THROWS(NickManager::NickNotFoundException, nicks->getFd("alice"));
    CHECK_THROWS(NickManager::NickNotFoundException, registration->getFdByNick("alice"));
}

TEST(ClientLifecycleManager, NicknameCanBeReusedByAnotherClientAfterRemoval_AC_215) {
    addRegisteredClient(5, "host", "alice");
    manager->addClient(6, "host");

    manager->removeClient(5);
    registration->setNick(6, "alice");

    CHECK_EQUAL(6, registration->getFdByNick("alice"));
}

TEST(ClientLifecycleManager, NoChannelKeepsRemovedFdAsMemberOrOperator_AC_215) {
    addRegisteredClient(5, "host", "alice");
    addRegisteredClient(6, "host", "bob");
    membership->createChannel(5, "#first");
    membership->createChannel(6, "#second");
    membership->addMemberToChannel(5, "#second");
    membership->addMemberToChannel(6, "#first");
    membership->setOperator(5, "#second", true);

    manager->removeClient(5);

    CHECK_FALSE(channels->findChannel("#first").isMember(5));
    CHECK_FALSE(channels->findChannel("#first").isOperator(5));
    CHECK_FALSE(channels->findChannel("#second").isMember(5));
    CHECK_FALSE(channels->findChannel("#second").isOperator(5));
}

TEST(ClientLifecycleManager, RemovedQuittingClientIsNotReportedToClose_AC_215) {
    manager->addClient(5, "host");
    manager->setIsQuitting(5);
    CHECK_TRUE(contains(manager->getClientsToClose(), 5));

    manager->removeClient(5);

    CHECK_FALSE(contains(manager->getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, FdCanBeReusedCleanlyAfterRemoval_AC_215) {
    addRegisteredClient(5, "host", "alice");
    pendingIO->pushInput(5, "a\r\n");
    manager->setIsQuitting(5);
    manager->removeClient(5);

    manager->addClient(5, "host");

    CHECK_FALSE(contains(pendingIO->getHasPendingInput(), 5));
    CHECK_FALSE(contains(manager->getClientsToClose(), 5));
    CHECK_FALSE(clients->findClient(5).isQuitting());
    CHECK_FALSE(auth->isAuthenticated(5));
}

TEST(ClientLifecycleManager, OperationsOnRemovedClientThrowNotFound_AC_215) {
    manager->addClient(5, "host");
    manager->removeClient(5);

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, auth->isAuthenticated(5));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, registration->isRegistered(5));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, pendingIO->pushInput(5, "a\r\n"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->setIsQuitting(5));
}

// Unknown fd

TEST(ClientLifecycleManager, RemoveUnknownClientThrowsNotFound_AC_216) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->removeClient(99));
}

TEST(ClientLifecycleManager, RemoveUnknownClientDoesNotAffectOtherClients_AC_216) {
    addRegisteredClient(5, "host", "alice");
    membership->createChannel(5, "#general");
    pendingIO->pushOutput(5, "hello");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->removeClient(99));

    CHECK_TRUE(clientExists(5));
    CHECK_TRUE(auth->isAuthenticated(5));
    CHECK_TRUE(registration->isRegistered(5));
    CHECK_TRUE(channels->findChannel("#general").isMember(5));
    CHECK_FALSE(nicks->isAvailable("alice"));
    CHECK_TRUE(contains(pendingIO->getHasPendingOutput(), 5));
}

TEST(ClientLifecycleManager, RemoveUnknownClientDoesNotMarkItAsQuitting_AC_216) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->removeClient(99));

    CHECK_TRUE(manager->getClientsToClose().empty());
}

TEST(ClientLifecycleManager, RemoveClientTwiceThrowsNotFoundOnSecondCall_AC_216) {
    manager->addClient(5, "host");
    manager->removeClient(5);

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->removeClient(5));
}

// Set quitting

TEST(ClientLifecycleManager, SetIsQuittingMarksClientAsQuitting_AC_217) {
    manager->addClient(5, "host");

    manager->setIsQuitting(5);

    CHECK_TRUE(clients->findClient(5).isQuitting());
}

TEST(ClientLifecycleManager, SetIsQuittingDiscardsPendingInput_AC_217) {
    manager->addClient(5, "host");
    pendingIO->pushInput(5, "PING\r\n");

    manager->setIsQuitting(5);

    CHECK_FALSE(contains(pendingIO->getHasPendingInput(), 5));
    CHECK_TRUE(clients->findClient(5).isInputBufferEmpty());
}

TEST(ClientLifecycleManager, SetIsQuittingKeepsPendingOutput_AC_217) {
    manager->addClient(5, "host");
    pendingIO->pushOutput(5, "bye");

    manager->setIsQuitting(5);

    CHECK_TRUE(contains(pendingIO->getHasPendingOutput(), 5));
    STRCMP_EQUAL("bye", pendingIO->peekOutput(5, 10).c_str());
}

TEST(ClientLifecycleManager, SetIsQuittingKeepsClientRegisteredAndInChannels_AC_217) {
    addRegisteredClient(5, "host", "alice");
    membership->createChannel(5, "#general");

    manager->setIsQuitting(5);

    CHECK_TRUE(clientExists(5));
    CHECK_TRUE(auth->isAuthenticated(5));
    CHECK_TRUE(registration->isRegistered(5));
    CHECK_TRUE(channels->findChannel("#general").isMember(5));
    CHECK_FALSE(nicks->isAvailable("alice"));
}

TEST(ClientLifecycleManager, SetIsQuittingOnlyAffectsTheGivenClient_AC_217) {
    manager->addClient(5, "host");
    manager->addClient(6, "host");
    pendingIO->pushInput(6, "PING\r\n");

    manager->setIsQuitting(5);

    CHECK_FALSE(clients->findClient(6).isQuitting());
    CHECK_TRUE(contains(pendingIO->getHasPendingInput(), 6));
}

TEST(ClientLifecycleManager, SetIsQuittingTwiceKeepsClientQuittingOnce_AC_217) {
    manager->addClient(5, "host");

    manager->setIsQuitting(5);
    manager->setIsQuitting(5);

    CHECK_TRUE(clients->findClient(5).isQuitting());
    CHECK_EQUAL((size_t)1, manager->getClientsToClose().size());
}

TEST(ClientLifecycleManager, SetIsQuittingOnUnknownClientThrowsNotFound_AC_217) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->setIsQuitting(99));
}

TEST(ClientLifecycleManager, SetIsQuittingOnUnknownClientDoesNotMarkAnything_AC_217) {
    manager->addClient(5, "host");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->setIsQuitting(99));

    CHECK_TRUE(manager->getClientsToClose().empty());
    CHECK_FALSE(clients->findClient(5).isQuitting());
}

// Clients to close

TEST(ClientLifecycleManager, GetClientsToCloseIsEmptyInitially_AC_217) {
    manager->addClient(5, "host");

    CHECK_TRUE(manager->getClientsToClose().empty());
}

TEST(ClientLifecycleManager, GetClientsToCloseIncludesQuittingClientWithoutPendingOutput_AC_217) {
    manager->addClient(5, "host");
    manager->setIsQuitting(5);

    std::set<int> toClose = manager->getClientsToClose();

    CHECK_EQUAL((size_t)1, toClose.size());
    CHECK_TRUE(contains(toClose, 5));
}

TEST(ClientLifecycleManager, GetClientsToCloseExcludesQuittingClientWithPendingOutput_AC_217) {
    manager->addClient(5, "host");
    pendingIO->pushOutput(5, "bye");
    manager->setIsQuitting(5);

    CHECK_FALSE(contains(manager->getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, GetClientsToCloseIncludesClientOnceOutputIsFullySent_AC_217) {
    manager->addClient(5, "host");
    pendingIO->pushOutput(5, "bye");
    manager->setIsQuitting(5);

    pendingIO->consumeOutput(5, 3);

    CHECK_TRUE(contains(manager->getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, GetClientsToCloseExcludesClientWithPartiallySentOutput_AC_217) {
    manager->addClient(5, "host");
    pendingIO->pushOutput(5, "goodbye");
    manager->setIsQuitting(5);

    pendingIO->consumeOutput(5, 4);

    CHECK_FALSE(contains(manager->getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, GetClientsToCloseIgnoresPendingInput_AC_217) {
    manager->addClient(5, "host");
    manager->setIsQuitting(5);

    pendingIO->pushInput(5, "PING\r\n");

    CHECK_TRUE(contains(manager->getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, GetClientsToCloseReturnsOnlyQuittingClients_AC_217) {
    manager->addClient(5, "host");
    manager->addClient(6, "host");
    manager->addClient(7, "host");
    pendingIO->pushOutput(7, "data");
    manager->setIsQuitting(5);

    std::set<int> toClose = manager->getClientsToClose();

    CHECK_TRUE(contains(toClose, 5));
    CHECK_FALSE(contains(toClose, 6));
    CHECK_FALSE(contains(toClose, 7));
}

TEST(ClientLifecycleManager, GetClientsToCloseIsSubsetOfQuittingClients_AC_217) {
    manager->addClient(5, "host");
    manager->addClient(6, "host");
    manager->addClient(7, "host");
    pendingIO->pushOutput(6, "bye");
    manager->setIsQuitting(5);
    manager->setIsQuitting(6);
    manager->setIsQuitting(7);

    std::set<int> toClose = manager->getClientsToClose();

    CHECK_EQUAL((size_t)2, toClose.size());
    CHECK_TRUE(contains(toClose, 5));
    CHECK_FALSE(contains(toClose, 6));
    CHECK_TRUE(contains(toClose, 7));
}

TEST(ClientLifecycleManager, GetClientsToCloseReturnsACopy_AC_217) {
    manager->addClient(5, "host");
    manager->setIsQuitting(5);

    std::set<int> toClose = manager->getClientsToClose();
    toClose.insert(99);
    toClose.erase(5);

    std::set<int> current = manager->getClientsToClose();
    CHECK_EQUAL((size_t)1, current.size());
    CHECK_TRUE(contains(current, 5));
}

TEST(ClientLifecycleManager, GetClientsToCloseWorksOnConstManager_AC_217) {
    manager->addClient(5, "host");
    manager->setIsQuitting(5);

    const ClientLifecycleManager& constManager = *manager;

    CHECK_TRUE(contains(constManager.getClientsToClose(), 5));
}

TEST(ClientLifecycleManager, ClientReportedToCloseCanBeRemoved_AC_217) {
    addRegisteredClient(5, "host", "alice");
    pendingIO->pushOutput(5, "bye");
    manager->setIsQuitting(5);
    pendingIO->consumeOutput(5, 3);
    CHECK_TRUE(contains(manager->getClientsToClose(), 5));

    manager->removeClient(5);

    CHECK_FALSE(clientExists(5));
    CHECK_TRUE(manager->getClientsToClose().empty());
    CHECK_TRUE(nicks->isAvailable("alice"));
}