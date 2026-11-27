#include "CppUTest/TestHarness.h"
#include "channel/ChannelRegistry.hpp"
#include "client/ClientRegistry.hpp"
#include "managers/ChannelMembershipManager.hpp"

#include <cstddef>
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

TEST_GROUP(ChannelMembershipManager) {
    ClientRegistry* clients;
    ChannelRegistry* channels;
    ChannelMembershipManager* manager;

    void setup() {
        clients = new ClientRegistry();
        channels = new ChannelRegistry();
        manager = new ChannelMembershipManager(*channels, *clients);
        clients->addClient(5);
        clients->addClient(6);
        clients->addClient(7);
    }

    void teardown() {
        delete manager;
        delete channels;
        delete clients;
    }

    // True if the client's own set of joined channels lists the channel.
    bool clientListsChannel(int fd, const std::string& name) {
        return clients->findClient(fd).getChannels().count(name) == 1;
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

// Create channel

TEST(ChannelMembershipManager, CreateChannelRegistersChannelInRegistry_AC_218) {
    manager->createChannel(5, "#general");

    STRCMP_EQUAL("#general", channels->findChannel("#general").getName().c_str());
}

TEST(ChannelMembershipManager, CreateChannelAddsCreatorAsFirstMember_AC_218) {
    manager->createChannel(5, "#general");

    CHECK_TRUE(channels->findChannel("#general").isMember(5));
    CHECK_EQUAL((size_t)1, channels->findChannel("#general").getMembers().size());
}

TEST(ChannelMembershipManager, CreateChannelMakesCreatorOperator_AC_218) {
    manager->createChannel(5, "#general");

    CHECK_TRUE(channels->findChannel("#general").isOperator(5));
}

TEST(ChannelMembershipManager, CreateChannelAddsChannelToCreatorJoinedChannels_AC_218) {
    manager->createChannel(5, "#general");

    CHECK_TRUE(clientListsChannel(5, "#general"));
}

TEST(ChannelMembershipManager, CreateChannelDoesNotAffectOtherClients_AC_218) {
    manager->createChannel(5, "#general");

    CHECK_FALSE(channels->findChannel("#general").isMember(6));
    CHECK_FALSE(channels->findChannel("#general").isOperator(6));
    CHECK_TRUE(clients->findClient(6).getChannels().empty());
}

TEST(ChannelMembershipManager, CreateChannelStartsWithDefaultSettings_AC_218) {
    manager->createChannel(5, "#general");

    Channel& channel = channels->findChannel("#general");
    STRCMP_EQUAL("", channel.getTopic().c_str());
    STRCMP_EQUAL("", channel.getKey().c_str());
    CHECK_EQUAL(0, channel.getLimit());
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());
}

TEST(ChannelMembershipManager, CreateChannelPreservesCapitalizationOfName_AC_218) {
    manager->createChannel(5, "#General");

    STRCMP_EQUAL("#General", channels->findChannel("#general").getName().c_str());
}

TEST(ChannelMembershipManager, CreateSeveralChannelsWithSameClient_AC_218) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");

    CHECK_TRUE(manager->isOperator(5, "#first"));
    CHECK_TRUE(manager->isOperator(5, "#second"));
    CHECK_EQUAL((size_t)2, clients->findClient(5).getChannels().size());
}

TEST(ChannelMembershipManager, DifferentClientsCanCreateDifferentChannels_AC_218) {
    manager->createChannel(5, "#first");
    manager->createChannel(6, "#second");

    CHECK_TRUE(manager->isMember(5, "#first"));
    CHECK_FALSE(manager->isMember(5, "#second"));
    CHECK_TRUE(manager->isMember(6, "#second"));
    CHECK_FALSE(manager->isMember(6, "#first"));
}

// Add member and remove client (consistency of both sides)

TEST(ChannelMembershipManager, AddMemberMakesClientMemberOfChannel_AC_219) {
    manager->createChannel(5, "#general");

    manager->addMemberToChannel(6, "#general");

    CHECK_TRUE(channels->findChannel("#general").isMember(6));
}

TEST(ChannelMembershipManager, AddMemberAddsChannelToClientJoinedChannels_AC_219) {
    manager->createChannel(5, "#general");

    manager->addMemberToChannel(6, "#general");

    CHECK_TRUE(clientListsChannel(6, "#general"));
}

TEST(ChannelMembershipManager, AddMemberAddsClientAsOrdinaryMember_AC_219) {
    manager->createChannel(5, "#general");

    manager->addMemberToChannel(6, "#general");

    CHECK_FALSE(channels->findChannel("#general").isOperator(6));
}

TEST(ChannelMembershipManager, AddMemberKeepsExistingMembersAndOperators_AC_219) {
    manager->createChannel(5, "#general");

    manager->addMemberToChannel(6, "#general");
    manager->addMemberToChannel(7, "#general");

    Channel& channel = channels->findChannel("#general");
    CHECK_EQUAL((size_t)3, channel.getMembers().size());
    CHECK_TRUE(channel.isOperator(5));
}

TEST(ChannelMembershipManager, AddMemberDoesNotAffectJoinedChannelsOfOtherClients_AC_219) {
    manager->createChannel(5, "#general");

    manager->addMemberToChannel(6, "#general");

    CHECK_EQUAL((size_t)1, clients->findClient(5).getChannels().size());
    CHECK_TRUE(clients->findClient(7).getChannels().empty());
}

TEST(ChannelMembershipManager, AddMemberToSeveralChannelsTracksAllOfThem_AC_219) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");

    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");

    CHECK_TRUE(clientListsChannel(6, "#first"));
    CHECK_TRUE(clientListsChannel(6, "#second"));
}

TEST(ChannelMembershipManager, AddMemberIgnoresCaseOfChannelName_AC_219) {
    manager->createChannel(5, "#General");

    manager->addMemberToChannel(6, "#GENERAL");

    CHECK_TRUE(channels->findChannel("#general").isMember(6));
    CHECK_EQUAL((size_t)2, channels->findChannel("#general").getMembers().size());
}

TEST(ChannelMembershipManager, RemoveClientRemovesItFromChannelMembers_AC_219) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->removeClientFromChannel(6, "#general");

    CHECK_FALSE(channels->findChannel("#general").isMember(6));
}

TEST(ChannelMembershipManager, RemoveClientRemovesChannelFromClientJoinedChannels_AC_219) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->removeClientFromChannel(6, "#general");

    CHECK_FALSE(clientListsChannel(6, "#general"));
    CHECK_TRUE(clients->findClient(6).getChannels().empty());
}

TEST(ChannelMembershipManager, RemoveClientKeepsOtherMembers_AC_219) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addMemberToChannel(7, "#general");

    manager->removeClientFromChannel(6, "#general");

    CHECK_TRUE(channels->findChannel("#general").isMember(5));
    CHECK_TRUE(channels->findChannel("#general").isMember(7));
    CHECK_TRUE(clientListsChannel(7, "#general"));
}

TEST(ChannelMembershipManager, RemoveClientOnlyAffectsTheGivenChannel_AC_219) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");

    manager->removeClientFromChannel(6, "#first");

    CHECK_FALSE(clientListsChannel(6, "#first"));
    CHECK_TRUE(clientListsChannel(6, "#second"));
    CHECK_TRUE(channels->findChannel("#second").isMember(6));
}

TEST(ChannelMembershipManager, RemoveClientIgnoresCaseOfChannelName_AC_219) {
    manager->createChannel(5, "#General");
    manager->addMemberToChannel(6, "#General");

    manager->removeClientFromChannel(6, "#GENERAL");

    CHECK_FALSE(channels->findChannel("#general").isMember(6));
    CHECK_TRUE(clients->findClient(6).getChannels().empty());
}

TEST(ChannelMembershipManager, ClientCanRejoinChannelAfterLeaving_AC_219) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromChannel(6, "#general");

    manager->addMemberToChannel(6, "#general");

    CHECK_TRUE(channels->findChannel("#general").isMember(6));
    CHECK_TRUE(clientListsChannel(6, "#general"));
}

TEST(ChannelMembershipManager, ChannelAndClientSidesStayConsistent_AC_219) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addMemberToChannel(7, "#general");
    manager->removeClientFromChannel(6, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromChannel(7, "#general");

    for (int fd = 5; fd <= 7; ++fd) {
        CHECK_EQUAL(channels->findChannel("#general").isMember(fd),
                    clientListsChannel(fd, "#general"));
    }
}

// Remove client: operator, invite and channel lifecycle

TEST(ChannelMembershipManager, RemoveClientAlsoRemovesOperatorStatus_AC_220) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->removeClientFromChannel(5, "#general");

    CHECK_FALSE(channels->findChannel("#general").isOperator(5));
}

TEST(ChannelMembershipManager, RejoiningClientDoesNotRecoverOperatorStatus_AC_220) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromChannel(5, "#general");

    manager->addMemberToChannel(5, "#general");

    CHECK_TRUE(channels->findChannel("#general").isMember(5));
    CHECK_FALSE(channels->findChannel("#general").isOperator(5));
}

TEST(ChannelMembershipManager, RemoveClientAlsoRemovesInvitation_AC_220) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addInviteToChannel(6, "#general");

    manager->removeClientFromChannel(6, "#general");

    CHECK_FALSE(channels->findChannel("#general").isInvited(6));
}

TEST(ChannelMembershipManager, RemoveClientKeepsOtherOperatorsAndInvitations_AC_220) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addInviteToChannel(7, "#general");

    manager->removeClientFromChannel(6, "#general");

    CHECK_TRUE(channels->findChannel("#general").isOperator(5));
    CHECK_TRUE(channels->findChannel("#general").isInvited(7));
}

TEST(ChannelMembershipManager, ChannelIsKeptWhileMembersRemain_AC_220) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->removeClientFromChannel(5, "#general");

    CHECK_TRUE(channelExists("#general"));
}

TEST(ChannelMembershipManager, ChannelIsRemovedWhenLastMemberLeaves_AC_220) {
    manager->createChannel(5, "#general");

    manager->removeClientFromChannel(5, "#general");

    CHECK_FALSE(channelExists("#general"));
}

TEST(ChannelMembershipManager, ChannelIsRemovedOnlyWhenLastOfSeveralMembersLeaves_AC_220) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addMemberToChannel(7, "#general");

    manager->removeClientFromChannel(5, "#general");
    manager->removeClientFromChannel(6, "#general");
    CHECK_TRUE(channelExists("#general"));

    manager->removeClientFromChannel(7, "#general");
    CHECK_FALSE(channelExists("#general"));
}

TEST(ChannelMembershipManager, ChannelIsRemovedEvenIfInvitationsArePending_AC_220) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");

    manager->removeClientFromChannel(5, "#general");

    CHECK_FALSE(channelExists("#general"));
}

TEST(ChannelMembershipManager, RemovingLastMemberClearsItsJoinedChannels_AC_220) {
    manager->createChannel(5, "#general");

    manager->removeClientFromChannel(5, "#general");

    CHECK_TRUE(clients->findClient(5).getChannels().empty());
}

TEST(ChannelMembershipManager, RemovingLastMemberDoesNotRemoveOtherChannels_AC_220) {
    manager->createChannel(5, "#first");
    manager->createChannel(6, "#second");

    manager->removeClientFromChannel(5, "#first");

    CHECK_FALSE(channelExists("#first"));
    CHECK_TRUE(channelExists("#second"));
}

TEST(ChannelMembershipManager, RemovedChannelCanBeCreatedAgainFromScratch_AC_220) {
    manager->createChannel(5, "#general");
    channels->findChannel("#general").setTopic("topic");
    channels->findChannel("#general").setKey("secret");
    manager->removeClientFromChannel(5, "#general");

    manager->createChannel(6, "#general");

    Channel& channel = channels->findChannel("#general");
    STRCMP_EQUAL("", channel.getTopic().c_str());
    STRCMP_EQUAL("", channel.getKey().c_str());
    CHECK_FALSE(channel.isMember(5));
    CHECK_TRUE(channel.isOperator(6));
}

// Remove client from all channels

TEST(ChannelMembershipManager, RemoveFromAllChannelsRemovesClientFromEveryChannel_AC_221) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->createChannel(5, "#third");
    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");
    manager->addMemberToChannel(6, "#third");

    manager->removeClientFromAllChannels(6);

    CHECK_FALSE(channels->findChannel("#first").isMember(6));
    CHECK_FALSE(channels->findChannel("#second").isMember(6));
    CHECK_FALSE(channels->findChannel("#third").isMember(6));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsClearsClientJoinedChannels_AC_221) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");

    manager->removeClientFromAllChannels(6);

    CHECK_TRUE(clients->findClient(6).getChannels().empty());
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsDeletesChannelsLeftEmpty_AC_221) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");

    manager->removeClientFromAllChannels(5);

    CHECK_FALSE(channelExists("#first"));
    CHECK_FALSE(channelExists("#second"));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsKeepsChannelsWithOtherMembers_AC_221) {
    manager->createChannel(5, "#shared");
    manager->createChannel(5, "#alone");
    manager->addMemberToChannel(6, "#shared");

    manager->removeClientFromAllChannels(5);

    CHECK_TRUE(channelExists("#shared"));
    CHECK_FALSE(channelExists("#alone"));
    CHECK_TRUE(channels->findChannel("#shared").isMember(6));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsLeavesNoStaleOperatorState_AC_221) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");

    manager->removeClientFromAllChannels(5);

    CHECK_FALSE(channels->findChannel("#first").isOperator(5));
    CHECK_FALSE(channels->findChannel("#second").isOperator(5));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsLeavesNoStaleInviteState_AC_221) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");
    manager->addInviteToChannel(6, "#first");
    manager->addInviteToChannel(6, "#second");

    manager->removeClientFromAllChannels(6);

    CHECK_FALSE(channels->findChannel("#first").isInvited(6));
    CHECK_FALSE(channels->findChannel("#second").isInvited(6));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsDoesNotAffectOtherClients_AC_221) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addMemberToChannel(7, "#general");

    manager->removeClientFromAllChannels(6);

    CHECK_TRUE(channels->findChannel("#general").isMember(5));
    CHECK_TRUE(channels->findChannel("#general").isMember(7));
    CHECK_TRUE(clientListsChannel(7, "#general"));
    CHECK_TRUE(channels->findChannel("#general").isOperator(5));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsWithNoChannelsHasNoEffect_AC_221) {
    manager->createChannel(5, "#general");

    manager->removeClientFromAllChannels(6);

    CHECK_TRUE(channelExists("#general"));
    CHECK_EQUAL((size_t)1, channels->findChannel("#general").getMembers().size());
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsTwiceHasNoEffectTheSecondTime_AC_221) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->removeClientFromAllChannels(6);
    manager->removeClientFromAllChannels(6);

    CHECK_TRUE(channelExists("#general"));
    CHECK_TRUE(channels->findChannel("#general").isMember(5));
}

TEST(ChannelMembershipManager, RemoveFromAllChannelsDoesNotRemoveClientFromRegistry_AC_221) {
    manager->createChannel(5, "#general");

    manager->removeClientFromAllChannels(5);

    CHECK_EQUAL(5, clients->findClient(5).getFd());
}

TEST(ChannelMembershipManager, ClientCanJoinChannelsAgainAfterRemoveFromAll_AC_221) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromAllChannels(6);

    manager->addMemberToChannel(6, "#general");

    CHECK_TRUE(manager->isMember(6, "#general"));
    CHECK_FALSE(manager->isOperator(6, "#general"));
}

// State queries

TEST(ChannelMembershipManager, IsMemberReturnsTrueForMembers_AC_222) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    CHECK_TRUE(manager->isMember(5, "#general"));
    CHECK_TRUE(manager->isMember(6, "#general"));
}

TEST(ChannelMembershipManager, IsMemberReturnsFalseForClientWithNoRelationToChannel_AC_222) {
    manager->createChannel(5, "#general");

    CHECK_FALSE(manager->isMember(6, "#general"));
}

TEST(ChannelMembershipManager, IsMemberReturnsFalseAfterClientLeaves_AC_222) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromChannel(6, "#general");

    CHECK_FALSE(manager->isMember(6, "#general"));
}

TEST(ChannelMembershipManager, IsMemberIgnoresCaseOfChannelName_AC_222) {
    manager->createChannel(5, "#General");

    CHECK_TRUE(manager->isMember(5, "#general"));
    CHECK_TRUE(manager->isMember(5, "#GENERAL"));
}

TEST(ChannelMembershipManager, IsMemberIsPerChannel_AC_222) {
    manager->createChannel(5, "#first");
    manager->createChannel(6, "#second");

    CHECK_TRUE(manager->isMember(5, "#first"));
    CHECK_FALSE(manager->isMember(5, "#second"));
}

TEST(ChannelMembershipManager, IsOperatorReturnsTrueOnlyForOperators_AC_222) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    CHECK_TRUE(manager->isOperator(5, "#general"));
    CHECK_FALSE(manager->isOperator(6, "#general"));
}

TEST(ChannelMembershipManager, IsOperatorReturnsFalseForClientWithNoRelationToChannel_AC_222) {
    manager->createChannel(5, "#general");

    CHECK_FALSE(manager->isOperator(6, "#general"));
}

TEST(ChannelMembershipManager, IsOperatorIsPerChannel_AC_222) {
    manager->createChannel(5, "#first");
    manager->createChannel(6, "#second");
    manager->addMemberToChannel(5, "#second");

    CHECK_TRUE(manager->isOperator(5, "#first"));
    CHECK_FALSE(manager->isOperator(5, "#second"));
}

TEST(ChannelMembershipManager, IsInvitedReturnsFalseByDefault_AC_222) {
    manager->createChannel(5, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
    CHECK_FALSE(manager->isInvited(5, "#general"));
}

TEST(ChannelMembershipManager, IsInvitedReturnsTrueOnlyForInvitedClient_AC_222) {
    manager->createChannel(5, "#general");

    manager->addInviteToChannel(6, "#general");

    CHECK_TRUE(manager->isInvited(6, "#general"));
    CHECK_FALSE(manager->isInvited(7, "#general"));
}

TEST(ChannelMembershipManager, IsInvitedIsPerChannel_AC_222) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");

    manager->addInviteToChannel(6, "#first");

    CHECK_TRUE(manager->isInvited(6, "#first"));
    CHECK_FALSE(manager->isInvited(6, "#second"));
}

TEST(ChannelMembershipManager, QueriesWorkOnConstManager_AC_222) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");

    const ChannelMembershipManager& constManager = *manager;

    CHECK_TRUE(constManager.isMember(5, "#general"));
    CHECK_TRUE(constManager.isOperator(5, "#general"));
    CHECK_TRUE(constManager.isInvited(6, "#general"));
    CHECK_FALSE(constManager.isMember(6, "#general"));
}

TEST(ChannelMembershipManager, QueriesDoNotModifyState_AC_222) {
    manager->createChannel(5, "#general");

    manager->isMember(6, "#general");
    manager->isOperator(6, "#general");
    manager->isInvited(6, "#general");

    CHECK_EQUAL((size_t)1, channels->findChannel("#general").getMembers().size());
    CHECK_TRUE(clients->findClient(6).getChannels().empty());
}

// Set operator

TEST(ChannelMembershipManager, SetOperatorGrantsOperatorStatusToMember_AC_223) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->setOperator(6, "#general", true);

    CHECK_TRUE(manager->isOperator(6, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorRevokesOperatorStatusFromMember_AC_223) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->setOperator(6, "#general", true);

    manager->setOperator(6, "#general", false);

    CHECK_FALSE(manager->isOperator(6, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorKeepsClientAsMember_AC_223) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->setOperator(6, "#general", true);
    CHECK_TRUE(manager->isMember(6, "#general"));

    manager->setOperator(6, "#general", false);
    CHECK_TRUE(manager->isMember(6, "#general"));
    CHECK_TRUE(clientListsChannel(6, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorAffectsOnlyTheGivenClient_AC_223) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->addMemberToChannel(7, "#general");

    manager->setOperator(6, "#general", true);

    CHECK_TRUE(manager->isOperator(5, "#general"));
    CHECK_TRUE(manager->isOperator(6, "#general"));
    CHECK_FALSE(manager->isOperator(7, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorAffectsOnlyTheGivenChannel_AC_223) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->addMemberToChannel(6, "#first");
    manager->addMemberToChannel(6, "#second");

    manager->setOperator(6, "#first", true);

    CHECK_TRUE(manager->isOperator(6, "#first"));
    CHECK_FALSE(manager->isOperator(6, "#second"));
}

TEST(ChannelMembershipManager, SetOperatorTrueTwiceKeepsOperatorStatus_AC_223) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->setOperator(6, "#general", true);
    manager->setOperator(6, "#general", true);

    CHECK_TRUE(manager->isOperator(6, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorFalseOnOrdinaryMemberHasNoEffect_AC_223) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    manager->setOperator(6, "#general", false);

    CHECK_FALSE(manager->isOperator(6, "#general"));
    CHECK_TRUE(manager->isMember(6, "#general"));
}

TEST(ChannelMembershipManager, CreatorCanLoseOperatorStatus_AC_223) {
    manager->createChannel(5, "#general");

    manager->setOperator(5, "#general", false);

    CHECK_FALSE(manager->isOperator(5, "#general"));
    CHECK_TRUE(manager->isMember(5, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorIgnoresCaseOfChannelName_AC_223) {
    manager->createChannel(5, "#General");
    manager->addMemberToChannel(6, "#General");

    manager->setOperator(6, "#GENERAL", true);

    CHECK_TRUE(manager->isOperator(6, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorOnNonMemberThrowsNotMember_AC_223) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelMembershipManager::NotMemberException,
                 manager->setOperator(6, "#general", true));
}

TEST(ChannelMembershipManager, SetOperatorOnNonMemberDoesNotCreateAnyState_AC_223) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelMembershipManager::NotMemberException,
                 manager->setOperator(6, "#general", true));

    CHECK_FALSE(channels->findChannel("#general").isOperator(6));
    CHECK_FALSE(channels->findChannel("#general").isMember(6));
    CHECK_TRUE(clients->findClient(6).getChannels().empty());
}

TEST(ChannelMembershipManager, RevokingOperatorOnNonMemberThrowsNotMember_AC_223) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelMembershipManager::NotMemberException,
                 manager->setOperator(6, "#general", false));
}

// Invitations

TEST(ChannelMembershipManager, AddInviteRecordsInvitation_AC_224) {
    manager->createChannel(5, "#general");

    manager->addInviteToChannel(6, "#general");

    CHECK_TRUE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, AddInviteDoesNotMakeClientMember_AC_224) {
    manager->createChannel(5, "#general");

    manager->addInviteToChannel(6, "#general");

    CHECK_FALSE(manager->isMember(6, "#general"));
    CHECK_FALSE(manager->isOperator(6, "#general"));
    CHECK_FALSE(clientListsChannel(6, "#general"));
}

TEST(ChannelMembershipManager, AddInviteTwiceRequiresOnlyOneConsume_AC_224) {
    manager->createChannel(5, "#general");

    manager->addInviteToChannel(6, "#general");
    manager->addInviteToChannel(6, "#general");
    manager->consumeInviteToChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, ConsumeInviteRemovesInvitation_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");

    manager->consumeInviteToChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, ConsumeInviteAffectsOnlyThatClient_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");
    manager->addInviteToChannel(7, "#general");

    manager->consumeInviteToChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
    CHECK_TRUE(manager->isInvited(7, "#general"));
}

TEST(ChannelMembershipManager, ConsumeInviteAffectsOnlyThatChannel_AC_224) {
    manager->createChannel(5, "#first");
    manager->createChannel(5, "#second");
    manager->addInviteToChannel(6, "#first");
    manager->addInviteToChannel(6, "#second");

    manager->consumeInviteToChannel(6, "#first");

    CHECK_FALSE(manager->isInvited(6, "#first"));
    CHECK_TRUE(manager->isInvited(6, "#second"));
}

TEST(ChannelMembershipManager, ConsumeInviteWithoutInvitationHasNoEffect_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(7, "#general");

    manager->consumeInviteToChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
    CHECK_TRUE(manager->isInvited(7, "#general"));
}

TEST(ChannelMembershipManager, ConsumeInviteTwiceHasNoEffectTheSecondTime_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");

    manager->consumeInviteToChannel(6, "#general");
    manager->consumeInviteToChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, JoiningWithInvitationConsumesIt_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");

    manager->addMemberToChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, InvitationCannotBeReusedToJoinAgain_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromChannel(6, "#general");

    CHECK_FALSE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, ClientCanBeInvitedAgainAfterConsumingInvitation_AC_224) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");
    manager->consumeInviteToChannel(6, "#general");

    manager->addInviteToChannel(6, "#general");

    CHECK_TRUE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, InviteIgnoresCaseOfChannelName_AC_224) {
    manager->createChannel(5, "#General");

    manager->addInviteToChannel(6, "#GENERAL");
    CHECK_TRUE(manager->isInvited(6, "#general"));

    manager->consumeInviteToChannel(6, "#General");
    CHECK_FALSE(manager->isInvited(6, "#general"));
}

// Unknown clients, unknown channels, already member, not member

TEST(ChannelMembershipManager, CreateChannelWithUnknownClientThrowsClientNotFound_AC_226) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->createChannel(99, "#general"));
}

TEST(ChannelMembershipManager, CreateChannelWithUnknownClientDoesNotCreateChannel_AC_226) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->createChannel(99, "#general"));

    CHECK_FALSE(channelExists("#general"));
}

TEST(ChannelMembershipManager, CreateChannelWithInvalidNameThrowsInvalidName_AC_226) {
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 manager->createChannel(5, "general"));
}

TEST(ChannelMembershipManager, CreateChannelWithInvalidNameLeavesClientUntouched_AC_226) {
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 manager->createChannel(5, "bad name"));

    CHECK_TRUE(clients->findClient(5).getChannels().empty());
}

TEST(ChannelMembershipManager, CreateChannelThatExistsThrowsAlreadyExists_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException,
                 manager->createChannel(6, "#general"));
    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException,
                 manager->createChannel(6, "#GENERAL"));
}

TEST(ChannelMembershipManager, CreateChannelThatExistsDoesNotChangeItsState_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException,
                 manager->createChannel(6, "#general"));

    CHECK_FALSE(manager->isMember(6, "#general"));
    CHECK_FALSE(manager->isOperator(6, "#general"));
    CHECK_TRUE(clients->findClient(6).getChannels().empty());
    CHECK_TRUE(manager->isOperator(5, "#general"));
}

TEST(ChannelMembershipManager, AddMemberToUnknownChannelThrowsChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->addMemberToChannel(5, "#missing"));
}

TEST(ChannelMembershipManager, AddMemberToUnknownChannelDoesNotCreateIt_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->addMemberToChannel(5, "#missing"));

    CHECK_FALSE(channelExists("#missing"));
    CHECK_TRUE(clients->findClient(5).getChannels().empty());
}

TEST(ChannelMembershipManager, AddMemberWithUnknownClientThrowsClientNotFound_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->addMemberToChannel(99, "#general"));
    CHECK_FALSE(channels->findChannel("#general").isMember(99));
}

TEST(ChannelMembershipManager, AddMemberAlreadyMemberThrowsAlreadyMember_AC_226) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");

    CHECK_THROWS(ChannelMembershipManager::AlreadyMemberException,
                 manager->addMemberToChannel(6, "#general"));
}

TEST(ChannelMembershipManager, CreatorAddedAgainThrowsAlreadyMember_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelMembershipManager::AlreadyMemberException,
                 manager->addMemberToChannel(5, "#general"));
}

TEST(ChannelMembershipManager, AlreadyMemberDoesNotChangeStateNorConsumeInvitation_AC_226) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->setOperator(6, "#general", true);
    manager->addInviteToChannel(6, "#general");

    CHECK_THROWS(ChannelMembershipManager::AlreadyMemberException,
                 manager->addMemberToChannel(6, "#general"));

    CHECK_TRUE(manager->isOperator(6, "#general"));
    CHECK_TRUE(manager->isInvited(6, "#general"));
    CHECK_EQUAL((size_t)2, channels->findChannel("#general").getMembers().size());
    CHECK_EQUAL((size_t)1, clients->findClient(6).getChannels().size());
}

TEST(ChannelMembershipManager, AlreadyMemberIgnoresCaseOfChannelName_AC_226) {
    manager->createChannel(5, "#General");

    CHECK_THROWS(ChannelMembershipManager::AlreadyMemberException,
                 manager->addMemberToChannel(5, "#GENERAL"));
}

TEST(ChannelMembershipManager, RemoveClientFromUnknownChannelThrowsChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->removeClientFromChannel(5, "#missing"));
}

TEST(ChannelMembershipManager, RemoveUnknownClientFromChannelThrowsClientNotFound_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->removeClientFromChannel(99, "#general"));
    CHECK_TRUE(channelExists("#general"));
}

TEST(ChannelMembershipManager, RemoveNonMemberFromChannelThrowsNotMember_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ChannelMembershipManager::NotMemberException,
                 manager->removeClientFromChannel(6, "#general"));
}

TEST(ChannelMembershipManager, RemoveNonMemberDoesNotChangeChannelNorRemoveIt_AC_226) {
    manager->createChannel(5, "#general");
    manager->addInviteToChannel(6, "#general");

    CHECK_THROWS(ChannelMembershipManager::NotMemberException,
                 manager->removeClientFromChannel(6, "#general"));

    CHECK_TRUE(channelExists("#general"));
    CHECK_TRUE(manager->isMember(5, "#general"));
    CHECK_TRUE(manager->isOperator(5, "#general"));
    CHECK_TRUE(manager->isInvited(6, "#general"));
}

TEST(ChannelMembershipManager, RemoveClientTwiceThrowsNotMemberOnSecondCall_AC_226) {
    manager->createChannel(5, "#general");
    manager->addMemberToChannel(6, "#general");
    manager->removeClientFromChannel(6, "#general");

    CHECK_THROWS(ChannelMembershipManager::NotMemberException,
                 manager->removeClientFromChannel(6, "#general"));
}

TEST(ChannelMembershipManager, RemoveFromRemovedChannelThrowsChannelNotFound_AC_226) {
    manager->createChannel(5, "#general");
    manager->removeClientFromChannel(5, "#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->removeClientFromChannel(5, "#general"));
}

TEST(ChannelMembershipManager, RemoveUnknownClientFromAllChannelsThrowsClientNotFound_AC_226) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->removeClientFromAllChannels(99));
}

TEST(ChannelMembershipManager, QueriesOnUnknownChannelThrowChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->isMember(5, "#missing"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->isOperator(5, "#missing"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->isInvited(5, "#missing"));
}

TEST(ChannelMembershipManager, QueriesWithUnknownClientThrowClientNotFound_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->isMember(99, "#general"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->isOperator(99, "#general"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->isInvited(99, "#general"));
}

TEST(ChannelMembershipManager, SetOperatorOnUnknownChannelThrowsChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setOperator(5, "#missing", true));
}

TEST(ChannelMembershipManager, SetOperatorWithUnknownClientThrowsClientNotFound_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->setOperator(99, "#general", true));
}

TEST(ChannelMembershipManager, InviteOperationsOnUnknownChannelThrowChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->addInviteToChannel(5, "#missing"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->consumeInviteToChannel(5, "#missing"));
}

TEST(ChannelMembershipManager, InviteOperationsWithUnknownClientThrowClientNotFound_AC_226) {
    manager->createChannel(5, "#general");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->addInviteToChannel(99, "#general"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException,
                 manager->consumeInviteToChannel(99, "#general"));
    CHECK_FALSE(channels->findChannel("#general").isInvited(99));
}

TEST(ChannelMembershipManager, AlreadyMemberExceptionHasExpectedNameAndMessage_AC_226) {
    manager->createChannel(5, "#general");

    try {
        manager->addMemberToChannel(5, "#general");
        FAIL("Expected AlreadyMemberException");
    } catch (const ChannelMembershipManager::AlreadyMemberException& e) {
        STRCMP_EQUAL("AlreadyMemberException", e.getName().c_str());
        CHECK_TRUE(std::string(e.what()).find("#general") != std::string::npos);
    }
}

TEST(ChannelMembershipManager, NotMemberExceptionHasExpectedNameAndMessage_AC_226) {
    manager->createChannel(5, "#general");

    try {
        manager->removeClientFromChannel(6, "#general");
        FAIL("Expected NotMemberException");
    } catch (const ChannelMembershipManager::NotMemberException& e) {
        STRCMP_EQUAL("NotMemberException", e.getName().c_str());
        CHECK_TRUE(std::string(e.what()).find("#general") != std::string::npos);
    }
}