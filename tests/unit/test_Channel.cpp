#include "CppUTest/TestHarness.h"
#include "channel/Channel.hpp"

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

TEST_GROUP(Channel){};

// Name

TEST(Channel, GetNameReturnsNameGivenAtConstruction_AC_019) {
    Channel channel("#general");

    STRCMP_EQUAL("#general", channel.getName().c_str());
}

TEST(Channel, NameIsPreservedExactlyAsGiven_AC_019) {
    Channel channel("#General");

    STRCMP_EQUAL("#General", channel.getName().c_str());
}

TEST(Channel, NameDoesNotChangeWhenOtherStateChanges_AC_019) {
    Channel channel("#general");

    channel.setTopic("topic");
    channel.setKey("secret");
    channel.setLimit(10);
    channel.addMember(5);

    STRCMP_EQUAL("#general", channel.getName().c_str());
}

// Default state

TEST(Channel, NewChannelHasDefaultState_AC_019) {
    Channel channel("#general");

    STRCMP_EQUAL("", channel.getTopic().c_str());
    STRCMP_EQUAL("", channel.getKey().c_str());
    CHECK_EQUAL(0, channel.getLimit());
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());
    CHECK_TRUE(channel.getMembers().empty());
}

// Topic

TEST(Channel, SetTopicStoresTopic_AC_019) {
    Channel channel("#general");

    channel.setTopic("Welcome to the channel");

    STRCMP_EQUAL("Welcome to the channel", channel.getTopic().c_str());
}

TEST(Channel, SetTopicReplacesPreviousTopic_AC_019) {
    Channel channel("#general");

    channel.setTopic("first");
    channel.setTopic("second");

    STRCMP_EQUAL("second", channel.getTopic().c_str());
}

TEST(Channel, SetTopicWithEmptyStringClearsTopic_AC_019) {
    Channel channel("#general");
    channel.setTopic("first");

    channel.setTopic("");

    STRCMP_EQUAL("", channel.getTopic().c_str());
}

TEST(Channel, TopicMayContainSpacesAndColons_AC_019) {
    Channel channel("#general");

    channel.setTopic("a topic: with spaces");

    STRCMP_EQUAL("a topic: with spaces", channel.getTopic().c_str());
}

// Key

TEST(Channel, SetKeyStoresKey_AC_019) {
    Channel channel("#general");

    channel.setKey("secret");

    STRCMP_EQUAL("secret", channel.getKey().c_str());
}

TEST(Channel, SetKeyReplacesPreviousKey_AC_019) {
    Channel channel("#general");

    channel.setKey("first");
    channel.setKey("second");

    STRCMP_EQUAL("second", channel.getKey().c_str());
}

TEST(Channel, SetKeyWithEmptyStringRemovesKey_AC_019) {
    Channel channel("#general");
    channel.setKey("secret");

    channel.setKey("");

    STRCMP_EQUAL("", channel.getKey().c_str());
}

// Limit

TEST(Channel, SetLimitStoresLimit_AC_019) {
    Channel channel("#general");

    channel.setLimit(25);

    CHECK_EQUAL(25, channel.getLimit());
}

TEST(Channel, SetLimitReplacesPreviousLimit_AC_019) {
    Channel channel("#general");

    channel.setLimit(25);
    channel.setLimit(5);

    CHECK_EQUAL(5, channel.getLimit());
}

TEST(Channel, SetLimitToZeroRemovesLimit_AC_019) {
    Channel channel("#general");
    channel.setLimit(25);

    channel.setLimit(0);

    CHECK_EQUAL(0, channel.getLimit());
}

TEST(Channel, LimitDoesNotAffectMembership_AC_019) {
    Channel channel("#general");
    channel.setLimit(1);

    // The entity only stores the limit; enforcing it is the caller's job.
    channel.addMember(5);
    channel.addMember(6);

    CHECK_EQUAL((size_t)2, channel.getMembers().size());
}

// Modes

TEST(Channel, SetInviteOnlyUpdatesValue_AC_019) {
    Channel channel("#general");

    channel.setInviteOnly(true);

    CHECK_TRUE(channel.isInviteOnly());
}

TEST(Channel, SetInviteOnlyCanBeResetToFalse_AC_019) {
    Channel channel("#general");
    channel.setInviteOnly(true);

    channel.setInviteOnly(false);

    CHECK_FALSE(channel.isInviteOnly());
}

TEST(Channel, SetTopicProtectedUpdatesValue_AC_019) {
    Channel channel("#general");

    channel.setTopicProtected(true);

    CHECK_TRUE(channel.isTopicProtected());
}

TEST(Channel, SetTopicProtectedCanBeResetToFalse_AC_019) {
    Channel channel("#general");
    channel.setTopicProtected(true);

    channel.setTopicProtected(false);

    CHECK_FALSE(channel.isTopicProtected());
}

TEST(Channel, ModesAreIndependent_AC_019) {
    Channel channel("#general");

    channel.setInviteOnly(true);

    CHECK_TRUE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());

    channel.setTopicProtected(true);
    channel.setInviteOnly(false);

    CHECK_FALSE(channel.isInviteOnly());
    CHECK_TRUE(channel.isTopicProtected());
}

TEST(Channel, ConfigurationFieldsAreIndependent_AC_019) {
    Channel channel("#general");

    channel.setKey("secret");

    STRCMP_EQUAL("", channel.getTopic().c_str());
    CHECK_EQUAL(0, channel.getLimit());

    channel.setLimit(10);

    STRCMP_EQUAL("secret", channel.getKey().c_str());
    STRCMP_EQUAL("", channel.getTopic().c_str());
}

TEST(Channel, ChannelsDoNotShareState_AC_019) {
    Channel first("#first");
    Channel second("#second");

    first.setTopic("topic");
    first.setKey("secret");
    first.setLimit(10);
    first.setInviteOnly(true);
    first.setTopicProtected(true);
    first.addMember(5);

    STRCMP_EQUAL("", second.getTopic().c_str());
    STRCMP_EQUAL("", second.getKey().c_str());
    CHECK_EQUAL(0, second.getLimit());
    CHECK_FALSE(second.isInviteOnly());
    CHECK_FALSE(second.isTopicProtected());
    CHECK_FALSE(second.isMember(5));
}

TEST(Channel, CopiedChannelKeepsStateAndIsIndependent_AC_019) {
    Channel original("#general");
    original.setTopic("topic");
    original.addMember(5);
    original.addOperator(5);

    Channel copy(original);
    copy.setTopic("changed");
    copy.addMember(6);

    STRCMP_EQUAL("#general", copy.getName().c_str());
    CHECK_TRUE(copy.isOperator(5));
    STRCMP_EQUAL("topic", original.getTopic().c_str());
    CHECK_FALSE(original.isMember(6));
}

// Membership

TEST(Channel, AddMemberMakesClientMember_AC_020) {
    Channel channel("#general");

    channel.addMember(5);

    CHECK_TRUE(channel.isMember(5));
}

TEST(Channel, IsMemberReturnsFalseForUnknownFd_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    CHECK_FALSE(channel.isMember(6));
}

TEST(Channel, IsMemberReturnsFalseOnEmptyChannel_AC_020) {
    Channel channel("#general");

    CHECK_FALSE(channel.isMember(5));
}

TEST(Channel, AddMemberStoresMultipleMembers_AC_020) {
    Channel channel("#general");

    channel.addMember(5);
    channel.addMember(6);
    channel.addMember(7);

    std::set<int> members = channel.getMembers();
    CHECK_EQUAL((size_t)3, members.size());
    CHECK_EQUAL((size_t)1, members.count(5));
    CHECK_EQUAL((size_t)1, members.count(6));
    CHECK_EQUAL((size_t)1, members.count(7));
}

TEST(Channel, AddMemberTwiceKeepsSingleEntry_AC_020) {
    Channel channel("#general");

    channel.addMember(5);
    channel.addMember(5);

    CHECK_EQUAL((size_t)1, channel.getMembers().size());
}

TEST(Channel, RemoveMemberRemovesMembership_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    channel.removeMember(5);

    CHECK_FALSE(channel.isMember(5));
}

TEST(Channel, RemoveMemberRemovesOnlyThatClient_AC_020) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);

    channel.removeMember(5);

    CHECK_FALSE(channel.isMember(5));
    CHECK_TRUE(channel.isMember(6));
    CHECK_EQUAL((size_t)1, channel.getMembers().size());
}

TEST(Channel, RemoveLastMemberLeavesChannelWithNoMembers_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    channel.removeMember(5);

    CHECK_TRUE(channel.getMembers().empty());
}

TEST(Channel, RemoveMemberThatIsNotMemberHasNoEffect_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    channel.removeMember(6);

    CHECK_TRUE(channel.isMember(5));
    CHECK_EQUAL((size_t)1, channel.getMembers().size());
}

TEST(Channel, RemoveMemberOnEmptyChannelHasNoEffect_AC_020) {
    Channel channel("#general");

    channel.removeMember(5);

    CHECK_TRUE(channel.getMembers().empty());
}

TEST(Channel, RemoveMemberTwiceHasNoEffectTheSecondTime_AC_020) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);

    channel.removeMember(5);
    channel.removeMember(5);

    CHECK_TRUE(channel.isMember(6));
    CHECK_EQUAL((size_t)1, channel.getMembers().size());
}

TEST(Channel, MemberCanBeAddedAgainAfterRemoval_AC_020) {
    Channel channel("#general");
    channel.addMember(5);
    channel.removeMember(5);

    channel.addMember(5);

    CHECK_TRUE(channel.isMember(5));
    CHECK_EQUAL((size_t)1, channel.getMembers().size());
}

TEST(Channel, GetMembersReturnsCopyThatDoesNotAffectChannel_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    std::set<int> members = channel.getMembers();
    members.insert(99);
    members.erase(5);

    CHECK_TRUE(channel.isMember(5));
    CHECK_FALSE(channel.isMember(99));
    CHECK_EQUAL((size_t)1, channel.getMembers().size());
}

TEST(Channel, EarlierMembersSnapshotIsNotAffectedByLaterChanges_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    std::set<int> before = channel.getMembers();
    channel.addMember(6);
    channel.removeMember(5);

    CHECK_EQUAL((size_t)1, before.size());
    CHECK_EQUAL((size_t)1, before.count(5));
}

TEST(Channel, MembershipQueriesWorkOnConstChannel_AC_020) {
    Channel channel("#general");
    channel.addMember(5);

    const Channel& constChannel = channel;

    CHECK_TRUE(constChannel.isMember(5));
    CHECK_FALSE(constChannel.isMember(6));
    CHECK_EQUAL((size_t)1, constChannel.getMembers().size());
}

TEST(Channel, IsMemberAndGetMembersAreConsistent_AC_020) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);
    channel.removeMember(5);
    channel.addMember(7);

    std::set<int> members = channel.getMembers();
    for (int fd = 0; fd < 10; ++fd)
        CHECK_EQUAL(members.count(fd) == 1, channel.isMember(fd));
}

TEST(Channel, MembershipIsIndependentBetweenChannels_AC_020) {
    Channel first("#first");
    Channel second("#second");

    first.addMember(5);
    second.addMember(6);
    first.removeMember(5);

    CHECK_FALSE(first.isMember(5));
    CHECK_TRUE(second.isMember(6));
    CHECK_FALSE(second.isMember(5));
}

// Operators

TEST(Channel, NewMemberIsNotOperator_AC_021) {
    Channel channel("#general");

    channel.addMember(5);

    CHECK_FALSE(channel.isOperator(5));
}

TEST(Channel, IsOperatorReturnsFalseOnEmptyChannel_AC_021) {
    Channel channel("#general");

    CHECK_FALSE(channel.isOperator(5));
}

TEST(Channel, AddOperatorMakesMemberOperator_AC_021) {
    Channel channel("#general");
    channel.addMember(5);

    channel.addOperator(5);

    CHECK_TRUE(channel.isOperator(5));
}

TEST(Channel, AddOperatorKeepsClientAsMember_AC_021) {
    Channel channel("#general");
    channel.addMember(5);

    channel.addOperator(5);

    CHECK_TRUE(channel.isMember(5));
}

TEST(Channel, OperatorAndOrdinaryMemberAreDistinguished_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);

    channel.addOperator(5);

    CHECK_TRUE(channel.isOperator(5));
    CHECK_FALSE(channel.isOperator(6));
    CHECK_TRUE(channel.isMember(6));
}

TEST(Channel, AddOperatorTwiceKeepsClientAsOperator_AC_021) {
    Channel channel("#general");
    channel.addMember(5);

    channel.addOperator(5);
    channel.addOperator(5);

    CHECK_TRUE(channel.isOperator(5));
    channel.removeOperator(5);
    CHECK_FALSE(channel.isOperator(5));
}

TEST(Channel, ChannelSupportsSeveralOperators_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);
    channel.addMember(7);

    channel.addOperator(5);
    channel.addOperator(7);

    CHECK_TRUE(channel.isOperator(5));
    CHECK_FALSE(channel.isOperator(6));
    CHECK_TRUE(channel.isOperator(7));
}

TEST(Channel, RemoveOperatorKeepsClientAsMember_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addOperator(5);

    channel.removeOperator(5);

    CHECK_FALSE(channel.isOperator(5));
    CHECK_TRUE(channel.isMember(5));
}

TEST(Channel, RemoveOperatorAffectsOnlyThatClient_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);
    channel.addOperator(5);
    channel.addOperator(6);

    channel.removeOperator(5);

    CHECK_FALSE(channel.isOperator(5));
    CHECK_TRUE(channel.isOperator(6));
}

TEST(Channel, RemoveOperatorOfOrdinaryMemberHasNoEffect_AC_021) {
    Channel channel("#general");
    channel.addMember(5);

    channel.removeOperator(5);

    CHECK_FALSE(channel.isOperator(5));
    CHECK_TRUE(channel.isMember(5));
}

TEST(Channel, RemoveOperatorOfUnknownFdHasNoEffect_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addOperator(5);

    channel.removeOperator(99);

    CHECK_TRUE(channel.isOperator(5));
}

TEST(Channel, RemoveMemberAlsoRemovesOperatorStatus_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addOperator(5);

    channel.removeMember(5);

    CHECK_FALSE(channel.isMember(5));
    CHECK_FALSE(channel.isOperator(5));
}

TEST(Channel, RejoiningMemberDoesNotRecoverOperatorStatus_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addOperator(5);
    channel.removeMember(5);

    channel.addMember(5);

    CHECK_TRUE(channel.isMember(5));
    CHECK_FALSE(channel.isOperator(5));
}

TEST(Channel, RemovingOperatorMemberDoesNotAffectOtherOperators_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);
    channel.addOperator(5);
    channel.addOperator(6);

    channel.removeMember(5);

    CHECK_TRUE(channel.isOperator(6));
    CHECK_TRUE(channel.isMember(6));
}

TEST(Channel, GetMembersIncludesOperatorsAndOrdinaryMembers_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addMember(6);
    channel.addOperator(5);

    std::set<int> members = channel.getMembers();
    CHECK_EQUAL((size_t)2, members.size());
    CHECK_EQUAL((size_t)1, members.count(5));
    CHECK_EQUAL((size_t)1, members.count(6));
}

TEST(Channel, OperatorQueriesWorkOnConstChannel_AC_021) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addOperator(5);

    const Channel& constChannel = channel;

    CHECK_TRUE(constChannel.isOperator(5));
    CHECK_FALSE(constChannel.isOperator(6));
}

TEST(Channel, OperatorsAreIndependentBetweenChannels_AC_021) {
    Channel first("#first");
    Channel second("#second");
    first.addMember(5);
    second.addMember(5);

    first.addOperator(5);

    CHECK_TRUE(first.isOperator(5));
    CHECK_FALSE(second.isOperator(5));
}

// Invitations (state required by the INVITE and JOIN commands)

TEST(Channel, NewChannelHasNoInvitedClients_AC_019) {
    Channel channel("#general");

    CHECK_FALSE(channel.isInvited(5));
}

TEST(Channel, AddInvitedMarksClientAsInvited_AC_019) {
    Channel channel("#general");

    channel.addInvited(5);

    CHECK_TRUE(channel.isInvited(5));
    CHECK_FALSE(channel.isInvited(6));
}

TEST(Channel, AddInvitedDoesNotMakeClientMember_AC_019) {
    Channel channel("#general");

    channel.addInvited(5);

    CHECK_FALSE(channel.isMember(5));
    CHECK_FALSE(channel.isOperator(5));
}

TEST(Channel, AddMemberDoesNotConsumeInvitation_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);

    // Consuming the invitation on join is the caller's responsibility.
    channel.addMember(5);

    CHECK_TRUE(channel.isInvited(5));
}

TEST(Channel, ConsumeInvitedRemovesInvitation_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);

    channel.consumeInvited(5);

    CHECK_FALSE(channel.isInvited(5));
}

TEST(Channel, ConsumeInvitedAffectsOnlyThatClient_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);
    channel.addInvited(6);

    channel.consumeInvited(5);

    CHECK_FALSE(channel.isInvited(5));
    CHECK_TRUE(channel.isInvited(6));
}

TEST(Channel, ConsumeInvitedWithoutInvitationHasNoEffect_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);

    channel.consumeInvited(6);

    CHECK_TRUE(channel.isInvited(5));
}

TEST(Channel, ConsumeInvitedTwiceHasNoEffectTheSecondTime_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);

    channel.consumeInvited(5);
    channel.consumeInvited(5);

    CHECK_FALSE(channel.isInvited(5));
}

TEST(Channel, AddInvitedTwiceRequiresOnlyOneConsume_AC_019) {
    Channel channel("#general");

    channel.addInvited(5);
    channel.addInvited(5);
    channel.consumeInvited(5);

    CHECK_FALSE(channel.isInvited(5));
}

TEST(Channel, ClientCanBeInvitedAgainAfterConsumingInvitation_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);
    channel.consumeInvited(5);

    channel.addInvited(5);

    CHECK_TRUE(channel.isInvited(5));
}

TEST(Channel, RemoveMemberDoesNotAffectInvitations_AC_019) {
    Channel channel("#general");
    channel.addMember(5);
    channel.addInvited(5);

    channel.removeMember(5);

    CHECK_TRUE(channel.isInvited(5));
}

TEST(Channel, InvitationsAreIndependentBetweenChannels_AC_019) {
    Channel first("#first");
    Channel second("#second");

    first.addInvited(5);

    CHECK_TRUE(first.isInvited(5));
    CHECK_FALSE(second.isInvited(5));
}

TEST(Channel, InvitationQueriesWorkOnConstChannel_AC_019) {
    Channel channel("#general");
    channel.addInvited(5);

    const Channel& constChannel = channel;

    CHECK_TRUE(constChannel.isInvited(5));
}