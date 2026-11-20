#include "managers/AuthenticationManager.hpp"
#include "client/ClientRegistry.hpp"

#include "CppUTest/TestHarness.h"


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

TEST_GROUP(AuthenticationManager){};

// Is Authenticated

TEST(AuthenticationManager, IsAuthenticatedReturnsFalseForNotAuthenticatedClient_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);

    CHECK_FALSE(auth.isAuthenticated(5));
}

// Authenticate

TEST(AuthenticationManager, AuthenticateWithCorrectPasswordMarksClientAuthenticated_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);

    auth.authenticate(5, password);

    CHECK_TRUE(auth.isAuthenticated(5));
}

TEST(AuthenticationManager, AuthenticateWithCorrectPasswordIsIdempotent_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);

    auth.authenticate(5, password);
    auth.authenticate(5, password);

    CHECK_TRUE(auth.isAuthenticated(5));
}

TEST(AuthenticationManager, AuthenticateWithWrongPasswordThrowsException_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    std::string wrong = "wrong";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);

    CHECK_THROWS(
        AuthenticationManager::AuthenticationFailedException,
        auth.authenticate(5, wrong)
    );

    CHECK_FALSE(auth.isAuthenticated(5));
}

TEST(AuthenticationManager, AuthenticateDoesNotAffectOtherClients_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);
    registry.addClient(6);

    auth.authenticate(5, password);

    CHECK_TRUE(auth.isAuthenticated(5));
    CHECK_FALSE(auth.isAuthenticated(6));
}

// Un Authenticate

TEST(AuthenticationManager, UnAuthenticateMarksAuthenticatedClientAsNotAuthenticated_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);
    auth.authenticate(5, password);
    auth.unAuthenticate(5);

    CHECK_FALSE(auth.isAuthenticated(5));
}

TEST(AuthenticationManager, UnAuthenticateOnUnauthenticatedClientIsNoOp_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);
    auth.unAuthenticate(5);

    CHECK_FALSE(auth.isAuthenticated(5));
}

TEST(AuthenticationManager, UnAuthenticateDoesNotAffectOtherClients_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);
    registry.addClient(6);
    auth.authenticate(6, password);
    auth.unAuthenticate(5);

    CHECK_FALSE(auth.isAuthenticated(5));
    CHECK_TRUE(auth.isAuthenticated(6));
}

TEST(AuthenticationManager, AuthenticateNonExistingClientThrowsException_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    CHECK_THROWS(
        BaseException,
        auth.authenticate(5, password)
    );
}

TEST(AuthenticationManager, UnAuthenticateNonExistingClientThrowsException_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    CHECK_THROWS(
        BaseException,
        auth.unAuthenticate(5)
    );
}

TEST(AuthenticationManager, IsAuthenticatedForNonExistingClientThrowsException_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    AuthenticationManager auth(registry, password);

    CHECK_THROWS(
        BaseException,
        auth.isAuthenticated(5)
    );
}

TEST(AuthenticationManager, FailedAuthenticationDoesNotRemovePreviousAuthentication_AC_005) {
    ClientRegistry registry;
    std::string password = "secret";
    std::string wrong = "wrong";
    AuthenticationManager auth(registry, password);

    registry.addClient(5);

    auth.authenticate(5, password);

    CHECK_THROWS(
        AuthenticationManager::AuthenticationFailedException,
        auth.authenticate(5, wrong)
    );

    CHECK_TRUE(auth.isAuthenticated(5));
}
