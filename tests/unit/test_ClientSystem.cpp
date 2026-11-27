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

TEST(ClientSystem, AddClientCreatesNewClientInRegistry) {
    system->addClient(5, "host");

    try {
        clientRegistry->findClient(5);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, NewClientAreAddedWithHostname) {
    system->addClient(5, "host");

    try {
        CHECK_EQUAL("host", clientRegistry->findClient(5).getHostname())
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, NewClientHasDefaultValues) {
    system->addClient(5, "host");
    try {
        CHECK_TRUE(clientRegistry->findClient(5).getChannels().empty())
        CHECK_TRUE(clientRegistry->findClient(5).getRealname().empty())
        CHECK_TRUE(clientRegistry->findClient(5).getNick().empty())
        CHECK_TRUE(clientRegistry->findClient(5).getUsername().empty())
        CHECK_TRUE(clientRegistry->findClient(5).isInputBufferEmpty())
        CHECK_TRUE(clientRegistry->findClient(5).isOutputBufferEmpty())
        CHECK_FALSE(clientRegistry->findClient(5).isAuthenticated())
        CHECK_FALSE(clientRegistry->findClient(5).isQuitting())
        CHECK_FALSE(clientRegistry->findClient(5).isRegistered())
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// RemoveClient

TEST(ClientSystem, RemoveClientRemovesClientFromRegistry) {
    system->addClient(5, "host");
    try {
        system->removeClient(5);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, clientRegistry->findClient(5));
}

// PushInput

TEST(ClientSystem, PushInputAddsToTheInputBuffer) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input");
        CHECK_FALSE(clientRegistry->findClient(5).isInputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// GetNextInputLine

TEST(ClientSystem, GetNextInputLineReturnsTrueAndLineOnCompleteLine) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input\r\n");
        std::string line;
        CHECK_TRUE(system->getNextInputLine(5, line));
        CHECK_EQUAL("input", line);
        CHECK_TRUE(clientRegistry->findClient(5).isInputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetNextInputLineReturnsFalseAndNoLineOnEmptyBuffer) {
    system->addClient(5, "host");
    try {
        std::string line;
        CHECK_FALSE(system->getNextInputLine(5, line));
        CHECK_TRUE(line.empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetNextInputLineReturnsFalseAndNoLineOnIncompleteLine) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input");
        std::string line;
        CHECK_FALSE(system->getNextInputLine(5, line));
        CHECK_TRUE(line.empty());
        CHECK_FALSE(clientRegistry->findClient(5).isInputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetNextInputLineReturnsTrueAndCompleteLineOnPartialLineAfterLine) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input\r\ninp");
        std::string line;
        CHECK_TRUE(system->getNextInputLine(5, line));
        CHECK_EQUAL("input", line);
        CHECK_FALSE(clientRegistry->findClient(5).isInputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// PushOutput

TEST(ClientSystem, PushOutputAddsToTheInputBuffer) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        CHECK_FALSE(clientRegistry->findClient(5).isOutputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// PeekOutput

TEST(ClientSystem, PeekOutputReturnsCorrectOutputLessLen) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        CHECK_EQUAL("out", system->peekOutput(5, 3));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, PeekOutputReturnsCorrectOutputExactLen) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        CHECK_EQUAL("output", system->peekOutput(5, 6));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, PeekOutputReturnsCorrectOutputMoreLen) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        CHECK_EQUAL("output", system->peekOutput(5, 10));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// ConsumeOutput

TEST(ClientSystem, ConsumeOutputConsumesLessLen) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->consumeOutput(5, 3);
        CHECK_EQUAL("put", system->peekOutput(5, 10));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, ConsumeOutputConsumesExactLen) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->consumeOutput(5, 6);
        CHECK_TRUE(clientRegistry->findClient(5).isOutputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, ConsumeOutputConsumesMoreLen) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->consumeOutput(5, 10);
        CHECK_TRUE(clientRegistry->findClient(5).isOutputBufferEmpty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// SetIsQuitting

TEST(ClientSystem, SetIsQuittingSetsClientQuittingTrue) {
    system->addClient(5, "host");
    try {
        system->setIsQuitting(5);
        CHECK_TRUE(clientRegistry->findClient(5).isQuitting());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// Authenticate

TEST(ClientSystem, AuthenticateAuthenticatesClientWithCorrectSecret) {
    system->addClient(5, "host");
    try {
        system->authenticate(5, secret);
        CHECK_TRUE(clientRegistry->findClient(5).isAuthenticated());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, AuthenticateNotAuthenticatesClientWithInCorrectSecret) {
    system->addClient(5, "host");
    try {
        CHECK_THROWS(AuthenticationManager::AuthenticationFailedException,
                     system->authenticate(5, secret + "incorrect"));
        CHECK_FALSE(clientRegistry->findClient(5).isAuthenticated());
    } catch (std::exception& e) {
        FAIL(e.what());
    }
}

// IsAuthenticated

TEST(ClientSystem, IsAuthenticatedReturnsTrueIfAuthenticated) {
    system->addClient(5, "host");
    try {
        system->authenticate(5, secret);
        CHECK_TRUE(system->isAuthenticated(5));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, IsAuthenticatedReturnsFalseIfNotAuthenticated) {
    system->addClient(5, "host");
    try {
        CHECK_THROWS(AuthenticationManager::AuthenticationFailedException,
                     system->authenticate(5, secret + "incorrect"));
        CHECK_FALSE(system->isAuthenticated(5));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// SetNick

TEST(ClientSystem, SetNickSetsIfValidAndAvailibe) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "nick");
        CHECK_EQUAL("nick", clientRegistry->findClient(5).getNick());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, SetNickSetsNormalized) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "Nick");
        CHECK_EQUAL("nick", clientRegistry->findClient(5).getNick());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, SetNickThrowsIfNotValid) {
    system->addClient(5, "host");
    try {
        CHECK_THROWS(RegistrationManager::InvalidNicknameException,
                     system->setNick(5, "nick$%&/(/&%$)"));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, SetNickThrowsIfNotAvailibe) {
    system->addClient(5, "host");
	system->addClient(6, "host1");
    try {
        system->setNick(5, "Nick");
        CHECK_THROWS(RegistrationManager::NicknameAlreadyInUseException,
                     system->setNick(6, "nick"));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// SetUsername

TEST(ClientSystem, SetUsernameSetsUsername) {
    system->addClient(5, "host");
    try {
        system->setUsername(5, "user");
        CHECK_EQUAL("user", clientRegistry->findClient(5).getUsername());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// SetRealname

TEST(ClientSystem, SetRealnameSetsRealname) {
    system->addClient(5, "host");
    try {
        system->setRealname(5, "name");
        CHECK_EQUAL("name", clientRegistry->findClient(5).getRealname());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// IsRegistered

TEST(ClientSystem, IsRegisteredReturnsTrueIfRegistered) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "Nick");
        system->setRealname(5, "name");
        system->setUsername(5, "user");
        CHECK_TRUE(system->isRegistered(5));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, IsRegisteredReturnsFalseMissingNick) {
    system->addClient(5, "host");
    try {
        system->setRealname(5, "name");
        system->setUsername(5, "user");
        CHECK_FALSE(system->isRegistered(5));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, IsRegisteredReturnsFalseMissingUser) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "Nick");
        system->setRealname(5, "name");
        CHECK_FALSE(system->isRegistered(5));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, IsRegisteredReturnsFalseMissingRealname) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "Nick");
        system->setUsername(5, "user");
        CHECK_FALSE(system->isRegistered(5));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// GetFdByNick

TEST(ClientSystem, GetFdByNickReturnsCorrectFd) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "Nick");
        CHECK_EQUAL(5, system->getFdByNick("nick"));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetFdByNickReturnsCorrectFdNormalized) {
    system->addClient(5, "host");
    try {
        system->setNick(5, "Nick");
        CHECK_EQUAL(5, system->getFdByNick("Nick"));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetFdByNickTrowdOnNotFoundNick) {
    system->addClient(5, "host");
    try {
        CHECK_THROWS(NickManager::NickNotFoundException, system->getFdByNick("Nick"));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// GetClientDetails
#include <sstream>

SimpleString StringFrom(const ClientSystem::ClientDetails& d) {
    std::ostringstream oss;
    oss << "{fd=" << d.fd << ", nick=" << d.nick << ", username=" << d.username
        << ", realname=" << d.realname << ", hostname=" << d.hostname
        << ", authenticated=" << d.authenticated << ", registered=" << d.registered
        << ", quitting=" << d.quitting << ", channels=[";
    for (std::set<std::string>::const_iterator it = d.channels.begin(); it != d.channels.end();
         ++it) {
        if (it != d.channels.begin())
            oss << ", ";
        oss << *it;
    }
    oss << "]}";
    return SimpleString(oss.str().c_str());
}

TEST(ClientSystem, ClientDetailsReturndCorrectData) {
    ClientSystem::ClientDetails clientDetails(5, "nick", "user", "realname", "host", true, true,
                                              false, std::set<std::string>());
    system->addClient(clientDetails.fd, clientDetails.hostname);
    try {
        system->authenticate(clientDetails.fd, secret);
        system->setNick(clientDetails.fd, clientDetails.nick);
        system->setUsername(clientDetails.fd, clientDetails.username);
        system->setRealname(clientDetails.fd, clientDetails.realname);
        CHECK_EQUAL(clientDetails, system->getClientDetails(clientDetails.fd));
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// GetClientsToClose

TEST(ClientSystem, GetClientsToCloseIsEmptyWithNoClients) {
    try {
        CHECK_TRUE(system->getClientsToClose().empty());
    } catch (std::exception& e) {
        FAIL("Unexpected exception");
    }
}

TEST(ClientSystem, GetClientsToCloseDoesNotIncludeNonQuittingClient) {
    system->addClient(5, "host");
    try {
        CHECK_TRUE(system->getClientsToClose().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetClientsToCloseIncludesQuittingClientWithEmptyOutput) {
    system->addClient(5, "host");
    try {
        system->setIsQuitting(5);
        std::set<int> toClose = system->getClientsToClose();
        LONGS_EQUAL(1, toClose.size());
        CHECK_TRUE(toClose.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetClientsToCloseDoesNotIncludeQuittingClientWithPendingOutput) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->setIsQuitting(5);
        CHECK_TRUE(system->getClientsToClose().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetClientsToCloseIncludesQuittingClientOnceOutputIsFullyConsumed) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->setIsQuitting(5);
        CHECK_TRUE(system->getClientsToClose().empty());

        system->consumeOutput(5, 6);
        std::set<int> toClose = system->getClientsToClose();
        LONGS_EQUAL(1, toClose.size());
        CHECK_TRUE(toClose.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetClientsToCloseDoesNotIncludeClientWithPartiallyConsumedOutput) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->setIsQuitting(5);
        system->consumeOutput(5, 3);
        CHECK_TRUE(system->getClientsToClose().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetClientsToCloseOnlyIncludesQuittingClientsWithEmptyOutput) {
    system->addClient(5, "host"); // quitting, empty output -> included
    system->addClient(6, "host"); // quitting, pending output -> excluded
    system->addClient(7, "host"); // not quitting, empty output -> excluded
    system->addClient(8, "host"); // not quitting, pending output -> excluded
    system->addClient(9, "host"); // quitting, empty output -> included
    try {
        system->setIsQuitting(5);

        system->pushOutput(6, "output");
        system->setIsQuitting(6);

        system->pushOutput(8, "output");

        system->setIsQuitting(9);

        std::set<int> toClose = system->getClientsToClose();
        LONGS_EQUAL(2, toClose.size());
        CHECK_TRUE(toClose.count(5) > 0);
        CHECK_TRUE(toClose.count(9) > 0);
        CHECK_FALSE(toClose.count(6) > 0);
        CHECK_FALSE(toClose.count(7) > 0);
        CHECK_FALSE(toClose.count(8) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

// GetHasPendingInput

TEST(ClientSystem, GetHasPendingInputIsEmptyWithNoClients) {
    try {
        CHECK_TRUE(system->getHasPendingInput().empty());
    } catch (std::exception& e) {
        FAIL("Unexpected exception");
    }
}

TEST(ClientSystem, GetHasPendingInputIsEmptyIfNoInputWasPushed) {
    system->addClient(5, "host");
    try {
        CHECK_TRUE(system->getHasPendingInput().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingInputIncludesClientWithCompleteLine) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input\r\n");
        std::set<int> pending = system->getHasPendingInput();
        LONGS_EQUAL(1, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingInputIncludesClientWithIncompleteLine) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input");
        std::set<int> pending = system->getHasPendingInput();
        LONGS_EQUAL(1, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingInputExcludesClientOnceInputIsFullyRead) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input\r\n");
        std::string line;
        CHECK_TRUE(system->getNextInputLine(5, line));
        CHECK_TRUE(system->getHasPendingInput().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingInputIncludesClientWithRemainingPartialInput) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input\r\ninp");
        std::string line;
        CHECK_TRUE(system->getNextInputLine(5, line));
        std::set<int> pending = system->getHasPendingInput();
        LONGS_EQUAL(1, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingInputOnlyIncludesClientsWithInput) {
    system->addClient(5, "host");
    system->addClient(6, "host");
    system->addClient(7, "host");
    try {
        system->pushInput(5, "input\r\n");
        system->pushInput(7, "inp");
        std::set<int> pending = system->getHasPendingInput();
        LONGS_EQUAL(2, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
        CHECK_FALSE(pending.count(6) > 0);
        CHECK_TRUE(pending.count(7) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingInputDoesNotIncludeRemovedClient) {
    system->addClient(5, "host");
    try {
        system->pushInput(5, "input\r\n");
        system->removeClient(5);
        CHECK_TRUE(system->getHasPendingInput().empty());
    } catch (std::exception& e) {
        FAIL("Unexpected exception");
    }
}

// GetHasPendingOutput

TEST(ClientSystem, GetHasPendingOutputIsEmptyWithNoClients) {
    try {
        CHECK_TRUE(system->getHasPendingOutput().empty());
    } catch (std::exception& e) {
        FAIL("Unexpected exception");
    }
}

TEST(ClientSystem, GetHasPendingOutputIsEmptyIfNoOutputWasPushed) {
    system->addClient(5, "host");
    try {
        CHECK_TRUE(system->getHasPendingOutput().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingOutputIncludesClientWithOutput) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        std::set<int> pending = system->getHasPendingOutput();
        LONGS_EQUAL(1, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingOutputExcludesClientOnceOutputIsFullyConsumed) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->consumeOutput(5, 6);
        CHECK_TRUE(system->getHasPendingOutput().empty());
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingOutputIncludesClientWithPartiallyConsumedOutput) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->consumeOutput(5, 3);
        std::set<int> pending = system->getHasPendingOutput();
        LONGS_EQUAL(1, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingOutputOnlyIncludesClientsWithOutput) {
    system->addClient(5, "host");
    system->addClient(6, "host");
    system->addClient(7, "host");
    try {
        system->pushOutput(6, "output");
        system->pushOutput(7, "output");
        std::set<int> pending = system->getHasPendingOutput();
        LONGS_EQUAL(2, pending.size());
        CHECK_FALSE(pending.count(5) > 0);
        CHECK_TRUE(pending.count(6) > 0);
        CHECK_TRUE(pending.count(7) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingOutputIncludesQuittingClientWithOutput) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->setIsQuitting(5);
        std::set<int> pending = system->getHasPendingOutput();
        LONGS_EQUAL(1, pending.size());
        CHECK_TRUE(pending.count(5) > 0);
    } catch (std::exception& e) {
        FAIL("Client not registered correctly in client registry");
    }
}

TEST(ClientSystem, GetHasPendingOutputDoesNotIncludeRemovedClient) {
    system->addClient(5, "host");
    try {
        system->pushOutput(5, "output");
        system->removeClient(5);
        CHECK_TRUE(system->getHasPendingOutput().empty());
    } catch (std::exception& e) {
        FAIL("Unexpected exception");
    }
}