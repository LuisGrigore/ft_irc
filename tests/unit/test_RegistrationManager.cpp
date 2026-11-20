
#include "managers/RegistrationManager.hpp"
#include "client/ClientRegistry.hpp"
#include "managers/NickManager.hpp"
#include "CppUTest/TestHarness.h"

#include <string>

#undef new

TEST_GROUP(RegistrationManager){};

// Registration status

TEST(RegistrationManager, NewClientIsNotRegistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    CHECK_FALSE(manager.isRegistered(5));
}

TEST(RegistrationManager, ClientWithOnlyNickIsNotRegistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    manager.setNick(5, nick);

    CHECK_FALSE(manager.isRegistered(5));
}

TEST(RegistrationManager, ClientWithNickAndUsernameIsNotRegistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";

    manager.setNick(5, nick);
    manager.setUsername(5, username);

    CHECK_FALSE(manager.isRegistered(5));
}

TEST(RegistrationManager, ClientWithNickAndRealnameIsNotRegistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string realname = "Alice Smith";

    manager.setNick(5, nick);
    manager.setRealname(5, realname);

    CHECK_FALSE(manager.isRegistered(5));
}

TEST(RegistrationManager, ClientWithUsernameAndRealnameIsNotRegistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string username = "alice123";
    std::string realname = "Alice Smith";

    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    CHECK_FALSE(manager.isRegistered(5));
}

TEST(RegistrationManager, ClientWithAllRequiredFieldsIsRegistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    CHECK_TRUE(manager.isRegistered(5));
}

TEST(RegistrationManager, HostnameIsNotRequiredForRegistration_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    CHECK_TRUE(manager.isRegistered(5));
}

TEST(RegistrationManager, RegistrationDoesNotDependOnFieldAssignmentOrder_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";

    manager.setRealname(5, realname);
    manager.setUsername(5, username);

    CHECK_FALSE(manager.isRegistered(5));

    manager.setNick(5, nick);

    CHECK_TRUE(manager.isRegistered(5));
}

// Nickname validation

TEST(RegistrationManager, SetNickAssignsNicknameToClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    manager.setNick(5, nick);

    STRCMP_EQUAL("alice", registry.findClient(5).getNick().c_str());
}

TEST(RegistrationManager, SetNickReservesNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    manager.setNick(5, nick);

    CHECK_FALSE(nickManager.isAvailable(nick));
}

TEST(RegistrationManager, SetNickWithInvalidNicknameThrowsException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "invalid nickname";

    CHECK_THROWS(
        RegistrationManager::InvalidNicknameException,
        manager.setNick(5, nick)
    );
}

TEST(RegistrationManager, SetNickWithEmptyNicknameThrowsException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick;

    CHECK_THROWS(
        RegistrationManager::InvalidNicknameException,
        manager.setNick(5, nick)
    );
}

TEST(RegistrationManager, InvalidNicknameDoesNotModifyClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "invalid nickname";

    CHECK_THROWS(
        RegistrationManager::InvalidNicknameException,
        manager.setNick(5, nick)
    );

    CHECK_TRUE(registry.findClient(5).getNick().empty());
}

TEST(RegistrationManager, InvalidNicknameDoesNotReserveNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "invalid nickname";

    CHECK_THROWS(
        RegistrationManager::InvalidNicknameException,
        manager.setNick(5, nick)
    );

    CHECK_TRUE(nickManager.isAvailable(nick));
}

TEST(RegistrationManager, SetNickWithAlreadyReservedNicknameThrowsException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick = "alice";

    manager.setNick(5, nick);

    CHECK_THROWS(
        RegistrationManager::NicknameAlreadyInUseException,
        manager.setNick(6, nick)
    );
}

TEST(RegistrationManager, DuplicateNicknameDoesNotModifySecondClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick = "alice";

    manager.setNick(5, nick);

    CHECK_THROWS(
        RegistrationManager::NicknameAlreadyInUseException,
        manager.setNick(6, nick)
    );

    CHECK_TRUE(registry.findClient(6).getNick().empty());
}

TEST(RegistrationManager, SetNickWithSameNicknameIsIdempotent_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";

    manager.setNick(5, nick);
    manager.setNick(5, nick);

    STRCMP_EQUAL("alice", registry.findClient(5).getNick().c_str());
    CHECK_FALSE(nickManager.isAvailable(nick));
}

TEST(RegistrationManager, ChangingNicknameReleasesPreviousNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string oldNick = "alice";
    std::string newNick = "bob";

    manager.setNick(5, oldNick);
    manager.setNick(5, newNick);

    STRCMP_EQUAL("bob", registry.findClient(5).getNick().c_str());

    CHECK_TRUE(nickManager.isAvailable(oldNick));
    CHECK_FALSE(nickManager.isAvailable(newNick));
}

TEST(RegistrationManager, ChangingToReservedNicknamePreservesPreviousNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick1 = "alice";
    std::string nick2 = "bob";

    manager.setNick(5, nick1);
    manager.setNick(6, nick2);

    CHECK_THROWS(
        RegistrationManager::NicknameAlreadyInUseException,
        manager.setNick(5, nick2)
    );

    STRCMP_EQUAL("alice", registry.findClient(5).getNick().c_str());

    CHECK_FALSE(nickManager.isAvailable(nick1));
    CHECK_FALSE(nickManager.isAvailable(nick2));
}

TEST(RegistrationManager, ChangingToInvalidNicknamePreservesPreviousNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string invalidNick = "invalid nickname";

    manager.setNick(5, nick);

    CHECK_THROWS(
        RegistrationManager::InvalidNicknameException,
        manager.setNick(5, invalidNick)
    );

    STRCMP_EQUAL("alice", registry.findClient(5).getNick().c_str());
    CHECK_FALSE(nickManager.isAvailable(nick));
}

TEST(RegistrationManager, DifferentClientsCanHaveDifferentNicknames_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick1 = "alice";
    std::string nick2 = "bob";

    manager.setNick(5, nick1);
    manager.setNick(6, nick2);

    STRCMP_EQUAL("alice", registry.findClient(5).getNick().c_str());
    STRCMP_EQUAL("bob", registry.findClient(6).getNick().c_str());
}

// Username

TEST(RegistrationManager, SetUsernameAssignsUsernameToClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string username = "alice123";
    manager.setUsername(5, username);

    STRCMP_EQUAL(
        "alice123",
        registry.findClient(5).getUsername().c_str()
    );
}

TEST(RegistrationManager, SetUsernameCanUpdateExistingUsername_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string username1 = "alice123";
    std::string username2 = "alice456";

    manager.setUsername(5, username1);
    manager.setUsername(5, username2);

    STRCMP_EQUAL(
        "alice456",
        registry.findClient(5).getUsername().c_str()
    );
}

TEST(RegistrationManager, EmptyUsernameMakesClientUnregistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";
    std::string empty;

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    CHECK_TRUE(manager.isRegistered(5));

    manager.setUsername(5, empty);

    CHECK_FALSE(manager.isRegistered(5));
}

// Realname

TEST(RegistrationManager, SetRealnameAssignsRealnameToClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string realname = "Alice Smith";
    manager.setRealname(5, realname);

    STRCMP_EQUAL(
        "Alice Smith",
        registry.findClient(5).getRealname().c_str()
    );
}

TEST(RegistrationManager, SetRealnameCanUpdateExistingRealname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string realname1 = "Alice Smith";
    std::string realname2 = "Alice Jones";

    manager.setRealname(5, realname1);
    manager.setRealname(5, realname2);

    STRCMP_EQUAL(
        "Alice Jones",
        registry.findClient(5).getRealname().c_str()
    );
}

TEST(RegistrationManager, EmptyRealnameMakesClientUnregistered_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";
    std::string empty;

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    CHECK_TRUE(manager.isRegistered(5));

    manager.setRealname(5, empty);

    CHECK_FALSE(manager.isRegistered(5));
}

// Hostname

TEST(RegistrationManager, SetHostnameAssignsHostnameToClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string hostname = "localhost";
    manager.setHostname(5, hostname);

    STRCMP_EQUAL(
        "localhost",
        registry.findClient(5).getHostname().c_str()
    );
}

TEST(RegistrationManager, SetHostnameCanUpdateExistingHostname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string hostname1 = "localhost";
    std::string hostname2 = "server.local";

    manager.setHostname(5, hostname1);
    manager.setHostname(5, hostname2);

    STRCMP_EQUAL(
        "server.local",
        registry.findClient(5).getHostname().c_str()
    );
}

TEST(RegistrationManager, EmptyHostnameDoesNotAffectRegistration_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";
    std::string hostname;

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);
    manager.setHostname(5, hostname);

    CHECK_TRUE(manager.isRegistered(5));
}

// Unregister

TEST(RegistrationManager, UnRegisterClearsAllRegistrationFields_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";
    std::string hostname = "localhost";

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);
    manager.setHostname(5, hostname);

    manager.unRegister(5);

    CHECK_TRUE(registry.findClient(5).getNick().empty());
    CHECK_TRUE(registry.findClient(5).getUsername().empty());
    CHECK_TRUE(registry.findClient(5).getRealname().empty());
    CHECK_TRUE(registry.findClient(5).getHostname().empty());

    CHECK_FALSE(manager.isRegistered(5));
}

TEST(RegistrationManager, UnRegisterReleasesNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";

    manager.setNick(5, nick);
    manager.unRegister(5);

    CHECK_TRUE(nickManager.isAvailable(nick));
}

TEST(RegistrationManager, UnRegisterAllowsNicknameReuseByAnotherClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick = "alice";

    manager.setNick(5, nick);
    manager.unRegister(5);
    manager.setNick(6, nick);

    STRCMP_EQUAL("alice", registry.findClient(6).getNick().c_str());
}

TEST(RegistrationManager, UnRegisterDoesNotRemoveClientFromRegistry_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    manager.setNick(5, nick);

    manager.unRegister(5);

    CHECK_FALSE(manager.isRegistered(5));
    CHECK_TRUE(registry.findClient(5).getNick().empty());
}

TEST(RegistrationManager, UnRegisterIsIdempotent_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    manager.unRegister(5);
    manager.unRegister(5);

    CHECK_FALSE(manager.isRegistered(5));
    CHECK_TRUE(nickManager.isAvailable(nick));
}

TEST(RegistrationManager, UnRegisterDoesNotReleaseAnotherClientsNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick1 = "alice";
    std::string nick2 = "bob";

    manager.setNick(5, nick1);
    manager.setNick(6, nick2);

    manager.unRegister(5);

    CHECK_TRUE(nickManager.isAvailable(nick1));
    CHECK_FALSE(nickManager.isAvailable(nick2));
    STRCMP_EQUAL("bob", registry.findClient(6).getNick().c_str());
}

// Client independence

TEST(RegistrationManager, RegistrationOfOneClientDoesNotAffectAnother_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick = "alice";
    std::string username = "alice123";
    std::string realname = "Alice Smith";

    manager.setNick(5, nick);
    manager.setUsername(5, username);
    manager.setRealname(5, realname);

    CHECK_TRUE(manager.isRegistered(5));
    CHECK_FALSE(manager.isRegistered(6));
}

// Exception inheritance and error codes

TEST(RegistrationManager, InvalidNicknameExceptionHasExpectedErrorCode_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    std::string nick = "invalid nickname";

    try {
        manager.setNick(5, nick);
        FAIL("Expected InvalidNicknameException");
    }
    catch (const RegistrationManager::InvalidNicknameException& e) {
        CHECK_EQUAL(2001, e.getErrorCode());
        STRCMP_EQUAL(
            "InvalidNicknameException",
            e.getName().c_str()
        );
    }
}

TEST(RegistrationManager, NicknameAlreadyInUseExceptionHasExpectedErrorCode_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    std::string nick = "alice";
    manager.setNick(5, nick);

    try {
        manager.setNick(6, nick);
        FAIL("Expected NicknameAlreadyInUseException");
    }
    catch (const RegistrationManager::NicknameAlreadyInUseException& e) {
        CHECK_EQUAL(2002, e.getErrorCode());
        STRCMP_EQUAL(
            "NicknameAlreadyInUseException",
            e.getName().c_str()
        );
    }
}

// Missing clients

TEST(RegistrationManager, SetNickForNonExistingClientThrowsBaseException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    std::string nick = "alice";

    CHECK_THROWS(BaseException, manager.setNick(5, nick));
}

TEST(RegistrationManager, SetUsernameForNonExistingClientThrowsBaseException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    std::string username = "alice123";

    CHECK_THROWS(BaseException, manager.setUsername(5, username));
}

TEST(RegistrationManager, SetRealnameForNonExistingClientThrowsBaseException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    std::string realname = "Alice Smith";

    CHECK_THROWS(BaseException, manager.setRealname(5, realname));
}

TEST(RegistrationManager, SetHostnameForNonExistingClientThrowsBaseException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    std::string hostname = "localhost";

    CHECK_THROWS(BaseException, manager.setHostname(5, hostname));
}

TEST(RegistrationManager, UnRegisterForNonExistingClientThrowsBaseException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    CHECK_THROWS(BaseException, manager.unRegister(5));
}

TEST(RegistrationManager, IsRegisteredForNonExistingClientThrowsBaseException_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    CHECK_THROWS(BaseException, manager.isRegistered(5));
}

// Nickname reservation ownership

TEST(RegistrationManager, SetNickReservesNicknameForTheClientFd_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "alice");

    CHECK_EQUAL(5, nickManager.getFd("alice"));
}

TEST(RegistrationManager, ChangingNicknameMovesReservationToNewNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "alice");
    manager.setNick(5, "bob");

    CHECK_EQUAL(5, nickManager.getFd("bob"));
    CHECK_THROWS(NickManager::NickNotFoundException, nickManager.getFd("alice"));
}

TEST(RegistrationManager, ChangingNicknameSeveralTimesLeavesOnlyLatestReserved_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "alice");
    manager.setNick(5, "bob");
    manager.setNick(5, "carol");

    CHECK_TRUE(nickManager.isAvailable("alice"));
    CHECK_TRUE(nickManager.isAvailable("bob"));
    CHECK_FALSE(nickManager.isAvailable("carol"));
}

TEST(RegistrationManager, PreviousNicknameCanBeTakenByAnotherClientAfterChange_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(5, "alice");
    manager.setNick(5, "bob");
    manager.setNick(6, "alice");

    STRCMP_EQUAL("alice", registry.findClient(6).getNick().c_str());
    CHECK_EQUAL(6, nickManager.getFd("alice"));
    CHECK_EQUAL(5, nickManager.getFd("bob"));
}

TEST(RegistrationManager, FailedNicknameChangeKeepsPreviousReservationOwner_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(5, "alice");
    manager.setNick(6, "bob");

    CHECK_THROWS(RegistrationManager::NicknameAlreadyInUseException, manager.setNick(5, "bob"));

    CHECK_EQUAL(5, nickManager.getFd("alice"));
    CHECK_EQUAL(6, nickManager.getFd("bob"));
}

// Nickname case-insensitivity

TEST(RegistrationManager, SetNickWithDifferentCaseOfAnotherClientsNickThrows_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(5, "Alice");

    CHECK_THROWS(RegistrationManager::NicknameAlreadyInUseException, manager.setNick(6, "alice"));
    CHECK_THROWS(RegistrationManager::NicknameAlreadyInUseException, manager.setNick(6, "ALICE"));
    CHECK_TRUE(registry.findClient(6).getNick().empty());
}

TEST(RegistrationManager, FailedChangeToCaseVariantOfAnotherNickKeepsPreviousNick_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(5, "Alice");
    manager.setNick(6, "bob");

    CHECK_THROWS(RegistrationManager::NicknameAlreadyInUseException, manager.setNick(6, "ALICE"));

    STRCMP_EQUAL("bob", registry.findClient(6).getNick().c_str());
    CHECK_FALSE(nickManager.isAvailable("bob"));
    CHECK_EQUAL(5, nickManager.getFd("alice"));
}

TEST(RegistrationManager, ChangingNicknameOnlyInCaseIsAllowed_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "Alice");
    manager.setNick(5, "alice");

    STRCMP_EQUAL("alice", registry.findClient(5).getNick().c_str());
}

TEST(RegistrationManager, CaseOnlyNicknameChangeKeepsNicknameReservedForClient_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "Alice");
    manager.setNick(5, "ALICE");

    CHECK_FALSE(nickManager.isAvailable("alice"));
    CHECK_EQUAL(5, nickManager.getFd("alice"));
}

TEST(RegistrationManager, CaseOnlyNicknameChangeDoesNotAffectOtherClients_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(6, "bob");
    manager.setNick(5, "Alice");
    manager.setNick(5, "alice");

    STRCMP_EQUAL("bob", registry.findClient(6).getNick().c_str());
    CHECK_EQUAL(6, nickManager.getFd("bob"));
}

TEST(RegistrationManager, UnRegisterAfterCaseOnlyNicknameChangeReleasesNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "Alice");
    manager.setNick(5, "alice");
    manager.unRegister(5);

    CHECK_TRUE(nickManager.isAvailable("alice"));
}

// Nickname length

TEST(RegistrationManager, SetNickAcceptsNicknameWithMaximumLength_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "abcdefghi"); // 9 characters

    STRCMP_EQUAL("abcdefghi", registry.findClient(5).getNick().c_str());
}

TEST(RegistrationManager, SetNickWithNicknameLongerThanMaximumThrowsInvalidNickname_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    CHECK_THROWS(RegistrationManager::InvalidNicknameException,
                 manager.setNick(5, "abcdefghijabcdefghijabcdefghijj")); // 31 characters
    CHECK_TRUE(registry.findClient(5).getNick().empty());
    CHECK_TRUE(nickManager.isAvailable("abcdefghij"));
}

// Get fd by nick

TEST(RegistrationManager, GetFdByNickReturnsFdOfClientOwningTheNick_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(5, "alice");
    manager.setNick(6, "bob");

    CHECK_EQUAL(5, manager.getFdByNick("alice"));
    CHECK_EQUAL(6, manager.getFdByNick("bob"));
}

TEST(RegistrationManager, GetFdByNickIgnoresCase_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "Alice");

    CHECK_EQUAL(5, manager.getFdByNick("alice"));
    CHECK_EQUAL(5, manager.getFdByNick("ALICE"));
}

TEST(RegistrationManager, GetFdByNickWorksOnConstManager_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    manager.setNick(5, "alice");

    const RegistrationManager& constManager = manager;

    CHECK_EQUAL(5, constManager.getFdByNick("alice"));
}

TEST(RegistrationManager, GetFdByNickThrowsNotFoundForUnknownNick_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    manager.setNick(5, "alice");

    CHECK_THROWS(NickManager::NickNotFoundException, manager.getFdByNick("bob"));
}

TEST(RegistrationManager, GetFdByNickThrowsNotFoundWhenNoClientHasANick_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    CHECK_THROWS(NickManager::NickNotFoundException, manager.getFdByNick("alice"));
}

TEST(RegistrationManager, GetFdByNickFollowsNicknameChanges_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "alice");
    manager.setNick(5, "bob");

    CHECK_EQUAL(5, manager.getFdByNick("bob"));
    CHECK_THROWS(NickManager::NickNotFoundException, manager.getFdByNick("alice"));
}

TEST(RegistrationManager, GetFdByNickThrowsNotFoundAfterUnRegister_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "alice");
    manager.unRegister(5);

    CHECK_THROWS(NickManager::NickNotFoundException, manager.getFdByNick("alice"));
}

TEST(RegistrationManager, GetFdByNickReturnsNewOwnerAfterNickIsReused_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);
    registry.addClient(6);

    manager.setNick(5, "alice");
    manager.unRegister(5);
    manager.setNick(6, "alice");

    CHECK_EQUAL(6, manager.getFdByNick("alice"));
}

// Const-reference parameters (literals and temporaries)

TEST(RegistrationManager, SettersAcceptStringLiteralsAndTemporaries_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    registry.addClient(5);

    manager.setNick(5, "alice");
    manager.setUsername(5, "alice123");
    manager.setRealname(5, std::string("Alice Smith"));
    manager.setHostname(5, "localhost");

    CHECK_TRUE(manager.isRegistered(5));
    STRCMP_EQUAL("alice123", registry.findClient(5).getUsername().c_str());
    STRCMP_EQUAL("Alice Smith", registry.findClient(5).getRealname().c_str());
    STRCMP_EQUAL("localhost", registry.findClient(5).getHostname().c_str());
}

// Missing clients

TEST(RegistrationManager, GetFdByNickDoesNotRequireAnExistingClientLookup_AC_006) {
    ClientRegistry registry;
    NickManager nickManager;
    RegistrationManager manager(registry, nickManager);

    // The nick is reserved directly in the NickManager, with no client behind it.
    nickManager.reserve("ghost", 42);

    CHECK_EQUAL(42, manager.getFdByNick("ghost"));
}