#include "client/ClientRegistry.hpp"
#include "CppUTest/TestHarness.h"

/*
 * CppUTest forces `#define new new(__FILE__, __LINE__)` through
 * MemoryLeakDetectorNewMacros.h (included from TestHarness.h). As the
 * project compiles with CppUTest leak detection disabled
 * (MEMORY_LEAK_DETECTION OFF), no one provides the placement `new` operator
 * required by that macro. It is undefined here, only in test translation
 * units, to use the ordinary `new` from the standard library.
 */
#undef new

/*
 * Checks that evaluating `expression` throws an exception of type `ExceptionType`.
 */
#define CHECK_THROWS_TYPE(ExceptionType, expression) \
    do {                                             \
        bool thrown = false;                         \
        try {                                        \
            expression;                              \
        } catch (const ExceptionType&) {             \
            thrown = true;                           \
        }                                            \
        CHECK_TRUE(thrown);                          \
    } while (0)

TEST_GROUP(ClientRegistry){};

// Add Client

TEST(ClientRegistry, AddClientMakesClientFindableByFd_AC_003) {
    ClientRegistry registry;

    registry.addClient(5);

    CHECK_EQUAL(5, registry.findClient(5).getFd());
}

TEST(ClientRegistry, AddClientWithExistingFdThrowsAlreadyExists_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    CHECK_THROWS_TYPE(ClientRegistry::ClientAlreadyExistsException, registry.addClient(5));
}

TEST(ClientRegistry, AddClientWithExistingFdKeepsOriginalClient_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);
    registry.findClient(5).setNick("original");

    CHECK_THROWS_TYPE(ClientRegistry::ClientAlreadyExistsException, registry.addClient(5));

    STRCMP_EQUAL("original", registry.findClient(5).getNick().c_str());
}

TEST(ClientRegistry, AddClientAllowsMultipleDistinctFds_AC_003) {
    ClientRegistry registry;

    registry.addClient(5);
    registry.addClient(6);
    registry.addClient(7);

    CHECK_EQUAL(5, registry.findClient(5).getFd());
    CHECK_EQUAL(6, registry.findClient(6).getFd());
    CHECK_EQUAL(7, registry.findClient(7).getFd());
}

TEST(ClientRegistry, AddedClientStartsWithDefaultState_AC_003) {
    ClientRegistry registry;

    registry.addClient(5);

    Client& client = registry.findClient(5);
    STRCMP_EQUAL("", client.getNick().c_str());
    CHECK_FALSE(client.isAuthenticated());
    CHECK_FALSE(client.isRegistered());
    CHECK_FALSE(client.isQuitting());
    CHECK_TRUE(client.isInputBufferEmpty());
    CHECK_TRUE(client.isOutputBufferEmpty());
}

// Find Client

TEST(ClientRegistry, FindClientThrowsNotFoundWhenRegistryIsEmpty_AC_003) {
    ClientRegistry registry;

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, registry.findClient(5));
}

TEST(ClientRegistry, FindClientThrowsNotFoundWhenFdDoesNotExist_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, registry.findClient(6));
}

TEST(ClientRegistry, FindClientReturnsSameObjectOnEveryCall_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    Client& first = registry.findClient(5);
    Client& second = registry.findClient(5);

    CHECK_TRUE(&first == &second);
}

TEST(ClientRegistry, FindClientReturnsReferenceThatModifiesStoredClient_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    registry.findClient(5).setNick("testuser");

    STRCMP_EQUAL("testuser", registry.findClient(5).getNick().c_str());
}

TEST(ClientRegistry, FindClientReturnsDistinctObjectsForDistinctFds_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);
    registry.addClient(6);

    registry.findClient(5).setNick("first");
    registry.findClient(6).setNick("second");

    STRCMP_EQUAL("first", registry.findClient(5).getNick().c_str());
    STRCMP_EQUAL("second", registry.findClient(6).getNick().c_str());
}

TEST(ClientRegistry, FindClientWorksOnConstRegistry_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);
    registry.findClient(5).setNick("testuser");

    const ClientRegistry& constRegistry = registry;

    CHECK_EQUAL(5, constRegistry.findClient(5).getFd());
    STRCMP_EQUAL("testuser", constRegistry.findClient(5).getNick().c_str());
}

TEST(ClientRegistry, FindClientOnConstRegistryThrowsNotFoundWhenFdDoesNotExist_AC_003) {
    ClientRegistry registry;
    const ClientRegistry& constRegistry = registry;

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, constRegistry.findClient(5));
}

// Remove Client

TEST(ClientRegistry, RemoveClientMakesClientNotFound_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    registry.removeClient(5);

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, registry.findClient(5));
}

TEST(ClientRegistry, RemoveClientThrowsNotFoundWhenRegistryIsEmpty_AC_003) {
    ClientRegistry registry;

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, registry.removeClient(5));
}

TEST(ClientRegistry, RemoveClientThrowsNotFoundWhenFdDoesNotExist_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, registry.removeClient(6));
    CHECK_EQUAL(5, registry.findClient(5).getFd());
}

TEST(ClientRegistry, RemoveClientTwiceThrowsNotFoundOnSecondCall_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);

    registry.removeClient(5);

    CHECK_THROWS_TYPE(ClientRegistry::ClientNotFoundException, registry.removeClient(5));
}

TEST(ClientRegistry, RemoveClientDoesNotAffectOtherClients_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);
    registry.addClient(6);
    registry.findClient(6).setNick("second");

    registry.removeClient(5);

    CHECK_EQUAL(6, registry.findClient(6).getFd());
    STRCMP_EQUAL("second", registry.findClient(6).getNick().c_str());
}

TEST(ClientRegistry, AddClientAfterRemoveClientWithSameFdCreatesFreshClient_AC_003) {
    ClientRegistry registry;
    registry.addClient(5);
    registry.findClient(5).setNick("testuser");
    registry.findClient(5).pushOutput("hello");
    registry.findClient(5).setRegistered(true);

    registry.removeClient(5);
    registry.addClient(5);

    Client& client = registry.findClient(5);
    STRCMP_EQUAL("", client.getNick().c_str());
    CHECK_FALSE(client.isRegistered());
    CHECK_TRUE(client.isOutputBufferEmpty());
}