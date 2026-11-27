#include "CppUTest/TestHarness.h"
#include "managers/ChannelBroadcastManager.hpp"

/*
 * CppUTest forces `#define new new(__FILE__, __LINE__)` through
 * MemoryLeakDetectorNewMacros.h (included from TestHarness.h). As the
 * project compiles with CppUTest leak detection disabled
 * (MEMORY_LEAK_DETECTION OFF), no one provides the placement `new` operator
 * required by that macro. It is undefined here, only in test translation
 * units, to use the ordinary `new` from the standard library.
 */
#undef new

TEST_GROUP(ChannelBroadcastManager) {
    ClientRegistry* clientRegistry;
    PendingIOManager* pendingIOManager;
    ChannelRegistry* channelRegistry;
    ChannelBroadcastManager* channelBroadcastManager;

    void setup() {
        clientRegistry = new ClientRegistry();
        channelRegistry = new ChannelRegistry();
        pendingIOManager = new PendingIOManager(*clientRegistry);
        channelBroadcastManager = new ChannelBroadcastManager(*channelRegistry, *pendingIOManager);
    }

    void teardown() {
        delete channelBroadcastManager;
        delete pendingIOManager;
        delete channelRegistry;
        delete clientRegistry;

        channelBroadcastManager = NULL;
        pendingIOManager = NULL;
        channelRegistry = NULL;
        clientRegistry = NULL;
    }
};

TEST(ChannelBroadcastManager, AddsOutputToAllClientsInChannel) {
    try {
        clientRegistry->addClient(1);
        clientRegistry->addClient(2);
        clientRegistry->addClient(3);

        channelRegistry->addChannel("#channel");
        channelRegistry->findChannel("#channel").addMember(1);
        channelRegistry->findChannel("#channel").addMember(2);
        channelRegistry->findChannel("#channel").addMember(3);

        channelBroadcastManager->broadcast(1, "#channel", "Message");
        CHECK_EQUAL("Message", clientRegistry->findClient(2).peekOutput(7));
        CHECK_EQUAL("Message", clientRegistry->findClient(3).peekOutput(7));
    } catch (std::exception& e) {
        FAIL(e.what());
    }
}

TEST(ChannelBroadcastManager, DoesNotAddOutputToSender) {
    try {
        clientRegistry->addClient(1);
        clientRegistry->addClient(2);
        clientRegistry->addClient(3);

        channelRegistry->addChannel("#channel");
        channelRegistry->findChannel("#channel").addMember(1);
        channelRegistry->findChannel("#channel").addMember(2);
        channelRegistry->findChannel("#channel").addMember(3);

        channelBroadcastManager->broadcast(1, "#channel", "Message");
        CHECK_TRUE(clientRegistry->findClient(1).isOutputBufferEmpty());
    } catch (std::exception& e) {
        FAIL(e.what());
    }
}

TEST(ChannelBroadcastManager, BroadcastingToNoMembersChannelIsNoOp) {
    try {
        channelRegistry->addChannel("#channel");
        channelBroadcastManager->broadcast(1, "#channel", "Message");
    } catch (std::exception& e) {
        FAIL(e.what());
    }
}

TEST(ChannelBroadcastManager, BroadcastingToNoExistsChannelThrows) {
    try {
        CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                     channelBroadcastManager->broadcast(1, "#channel", "Message"));
    } catch (std::exception& e) {
        FAIL(e.what());
    }
}
