#include "CppUTest/TestHarness.h"
#include "channel/ChannelRegistry.hpp"
#include "managers/ChannelSettingsManager.hpp"

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

TEST_GROUP(ChannelSettingsManager) {
    ChannelRegistry* channels;
    ChannelSettingsManager* manager;

    void setup() {
        channels = new ChannelRegistry();
        manager = new ChannelSettingsManager(*channels);

        channels->addChannel("#general");
        channels->addChannel("#other");

        // Membership state that settings must never alter:
        // members 5 and 6, operator 5, invited 7.
        Channel& channel = channels->findChannel("#general");
        channel.addMember(5);
        channel.addMember(6);
        channel.addOperator(5);
        channel.addInvited(7);
    }

    void teardown() {
        delete manager;
        delete channels;
    }

    bool membershipIsIntact() {
        Channel& channel = channels->findChannel("#general");
        std::set<int> members = channel.getMembers();
        return members.size() == 2 && members.count(5) == 1 && members.count(6) == 1 &&
               channel.isOperator(5) && !channel.isOperator(6) && channel.isInvited(7) &&
               !channel.isInvited(5) && !channel.isInvited(6);
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

// Invite-only

TEST(ChannelSettingsManager, SetInviteOnlyEnablesMode_AC_225) {
    manager->setInviteOnly("#general", true);

    CHECK_TRUE(channels->findChannel("#general").isInviteOnly());
}

TEST(ChannelSettingsManager, SetInviteOnlyDisablesMode_AC_225) {
    manager->setInviteOnly("#general", true);

    manager->setInviteOnly("#general", false);

    CHECK_FALSE(channels->findChannel("#general").isInviteOnly());
}

TEST(ChannelSettingsManager, SetInviteOnlyTrueTwiceKeepsModeEnabled_AC_225) {
    manager->setInviteOnly("#general", true);
    manager->setInviteOnly("#general", true);

    CHECK_TRUE(channels->findChannel("#general").isInviteOnly());
}

TEST(ChannelSettingsManager, SetInviteOnlyAffectsOnlyTheGivenChannel_AC_225) {
    manager->setInviteOnly("#general", true);

    CHECK_FALSE(channels->findChannel("#other").isInviteOnly());
}

TEST(ChannelSettingsManager, SetInviteOnlyIgnoresCaseOfChannelName_AC_225) {
    manager->setInviteOnly("#GENERAL", true);

    CHECK_TRUE(channels->findChannel("#general").isInviteOnly());
}

TEST(ChannelSettingsManager, SetInviteOnlyDoesNotAlterOtherSettings_AC_225) {
    manager->setInviteOnly("#general", true);

    Channel& channel = channels->findChannel("#general");
    CHECK_FALSE(channel.isTopicProtected());
    CHECK_EQUAL(0, channel.getLimit());
    STRCMP_EQUAL("", channel.getKey().c_str());
    STRCMP_EQUAL("", channel.getTopic().c_str());
}

TEST(ChannelSettingsManager, SetInviteOnlyDoesNotAlterMembersOperatorsOrInvitations_AC_225) {
    manager->setInviteOnly("#general", true);
    CHECK_TRUE(membershipIsIntact());

    manager->setInviteOnly("#general", false);
    CHECK_TRUE(membershipIsIntact());
}

// Topic-protected

TEST(ChannelSettingsManager, SetTopicProtectedEnablesMode_AC_225) {
    manager->setTopicProtected("#general", true);

    CHECK_TRUE(channels->findChannel("#general").isTopicProtected());
}

TEST(ChannelSettingsManager, SetTopicProtectedDisablesMode_AC_225) {
    manager->setTopicProtected("#general", true);

    manager->setTopicProtected("#general", false);

    CHECK_FALSE(channels->findChannel("#general").isTopicProtected());
}

TEST(ChannelSettingsManager, SetTopicProtectedAffectsOnlyTheGivenChannel_AC_225) {
    manager->setTopicProtected("#general", true);

    CHECK_FALSE(channels->findChannel("#other").isTopicProtected());
}

TEST(ChannelSettingsManager, SetTopicProtectedIgnoresCaseOfChannelName_AC_225) {
    manager->setTopicProtected("#GENERAL", true);

    CHECK_TRUE(channels->findChannel("#general").isTopicProtected());
}

TEST(ChannelSettingsManager, SetTopicProtectedDoesNotAlterOtherSettings_AC_225) {
    manager->setTopicProtected("#general", true);

    Channel& channel = channels->findChannel("#general");
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_EQUAL(0, channel.getLimit());
    STRCMP_EQUAL("", channel.getKey().c_str());
    STRCMP_EQUAL("", channel.getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicProtectedDoesNotAlterMembersOperatorsOrInvitations_AC_225) {
    manager->setTopicProtected("#general", true);
    CHECK_TRUE(membershipIsIntact());

    manager->setTopicProtected("#general", false);
    CHECK_TRUE(membershipIsIntact());
}

// Limit

TEST(ChannelSettingsManager, SetLimitStoresLimit_AC_225) {
    manager->setLimit("#general", 25);

    CHECK_EQUAL(25, channels->findChannel("#general").getLimit());
}

TEST(ChannelSettingsManager, SetLimitReplacesPreviousLimit_AC_225) {
    manager->setLimit("#general", 25);

    manager->setLimit("#general", 5);

    CHECK_EQUAL(5, channels->findChannel("#general").getLimit());
}

TEST(ChannelSettingsManager, SetLimitToZeroRemovesLimit_AC_225) {
    manager->setLimit("#general", 25);

    manager->setLimit("#general", 0);

    CHECK_EQUAL(0, channels->findChannel("#general").getLimit());
}

TEST(ChannelSettingsManager, SetLimitAffectsOnlyTheGivenChannel_AC_225) {
    manager->setLimit("#general", 25);

    CHECK_EQUAL(0, channels->findChannel("#other").getLimit());
}

TEST(ChannelSettingsManager, SetLimitIgnoresCaseOfChannelName_AC_225) {
    manager->setLimit("#GENERAL", 10);

    CHECK_EQUAL(10, channels->findChannel("#general").getLimit());
}

TEST(ChannelSettingsManager, SetLimitDoesNotAlterOtherSettings_AC_225) {
    manager->setLimit("#general", 10);

    Channel& channel = channels->findChannel("#general");
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());
    STRCMP_EQUAL("", channel.getKey().c_str());
    STRCMP_EQUAL("", channel.getTopic().c_str());
}

TEST(ChannelSettingsManager, SetLimitDoesNotAlterMembersOperatorsOrInvitations_AC_225) {
    manager->setLimit("#general", 10);
    CHECK_TRUE(membershipIsIntact());

    manager->setLimit("#general", 0);
    CHECK_TRUE(membershipIsIntact());
}

TEST(ChannelSettingsManager, SetLimitBelowCurrentMemberCountDoesNotRemoveMembers_AC_225) {
    manager->setLimit("#general", 1);

    CHECK_EQUAL(1, channels->findChannel("#general").getLimit());
    CHECK_TRUE(membershipIsIntact());
}

// Key

TEST(ChannelSettingsManager, SetKeyStoresKey_AC_225) {
    manager->setKey("#general", "secret");

    STRCMP_EQUAL("secret", channels->findChannel("#general").getKey().c_str());
}

TEST(ChannelSettingsManager, SetKeyReplacesPreviousKey_AC_225) {
    manager->setKey("#general", "first");

    manager->setKey("#general", "second");

    STRCMP_EQUAL("second", channels->findChannel("#general").getKey().c_str());
}

TEST(ChannelSettingsManager, SetKeyWithEmptyStringRemovesKey_AC_225) {
    manager->setKey("#general", "secret");

    manager->setKey("#general", "");

    STRCMP_EQUAL("", channels->findChannel("#general").getKey().c_str());
}

TEST(ChannelSettingsManager, SetKeyAffectsOnlyTheGivenChannel_AC_225) {
    manager->setKey("#general", "secret");

    STRCMP_EQUAL("", channels->findChannel("#other").getKey().c_str());
}

TEST(ChannelSettingsManager, SetKeyIgnoresCaseOfChannelName_AC_225) {
    manager->setKey("#GENERAL", "secret");

    STRCMP_EQUAL("secret", channels->findChannel("#general").getKey().c_str());
}

TEST(ChannelSettingsManager, SetKeyKeepsKeyCaseSensitive_AC_225) {
    manager->setKey("#general", "Secret");

    STRCMP_EQUAL("Secret", channels->findChannel("#general").getKey().c_str());
}

TEST(ChannelSettingsManager, SetKeyDoesNotAlterOtherSettings_AC_225) {
    manager->setKey("#general", "secret");

    Channel& channel = channels->findChannel("#general");
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());
    CHECK_EQUAL(0, channel.getLimit());
    STRCMP_EQUAL("", channel.getTopic().c_str());
}

TEST(ChannelSettingsManager, SetKeyDoesNotAlterMembersOperatorsOrInvitations_AC_225) {
    manager->setKey("#general", "secret");
    CHECK_TRUE(membershipIsIntact());

    manager->setKey("#general", "");
    CHECK_TRUE(membershipIsIntact());
}

// Topic

TEST(ChannelSettingsManager, SetTopicStoresTopic_AC_225) {
    manager->setTopic("#general", "Welcome");

    STRCMP_EQUAL("Welcome", channels->findChannel("#general").getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicReplacesPreviousTopic_AC_225) {
    manager->setTopic("#general", "first");

    manager->setTopic("#general", "second");

    STRCMP_EQUAL("second", channels->findChannel("#general").getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicWithEmptyStringClearsTopic_AC_225) {
    manager->setTopic("#general", "first");

    manager->setTopic("#general", "");

    STRCMP_EQUAL("", channels->findChannel("#general").getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicKeepsSpacesAndColons_AC_225) {
    manager->setTopic("#general", "a topic: with spaces");

    STRCMP_EQUAL("a topic: with spaces", channels->findChannel("#general").getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicAffectsOnlyTheGivenChannel_AC_225) {
    manager->setTopic("#general", "topic");

    STRCMP_EQUAL("", channels->findChannel("#other").getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicIgnoresCaseOfChannelName_AC_225) {
    manager->setTopic("#GENERAL", "topic");

    STRCMP_EQUAL("topic", channels->findChannel("#general").getTopic().c_str());
}

TEST(ChannelSettingsManager, SetTopicDoesNotAlterOtherSettings_AC_225) {
    manager->setTopic("#general", "topic");

    Channel& channel = channels->findChannel("#general");
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());
    CHECK_EQUAL(0, channel.getLimit());
    STRCMP_EQUAL("", channel.getKey().c_str());
}

TEST(ChannelSettingsManager, SetTopicDoesNotAlterMembersOperatorsOrInvitations_AC_225) {
    manager->setTopic("#general", "topic");
    CHECK_TRUE(membershipIsIntact());

    manager->setTopic("#general", "");
    CHECK_TRUE(membershipIsIntact());
}

// Combined settings

TEST(ChannelSettingsManager, AllSettingsCanBeSetOnTheSameChannelIndependently_AC_225) {
    manager->setInviteOnly("#general", true);
    manager->setTopicProtected("#general", true);
    manager->setLimit("#general", 10);
    manager->setKey("#general", "secret");
    manager->setTopic("#general", "topic");

    Channel& channel = channels->findChannel("#general");
    CHECK_TRUE(channel.isInviteOnly());
    CHECK_TRUE(channel.isTopicProtected());
    CHECK_EQUAL(10, channel.getLimit());
    STRCMP_EQUAL("secret", channel.getKey().c_str());
    STRCMP_EQUAL("topic", channel.getTopic().c_str());
    CHECK_TRUE(membershipIsIntact());
}

TEST(ChannelSettingsManager, ChannelNameIsNotAlteredBySettings_AC_225) {
    channels->addChannel("#MixedCase");

    manager->setInviteOnly("#mixedcase", true);
    manager->setTopic("#mixedcase", "topic");

    STRCMP_EQUAL("#MixedCase", channels->findChannel("#mixedcase").getName().c_str());
}

// Unknown channel

TEST(ChannelSettingsManager, EverySetterOnUnknownChannelThrowsChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setInviteOnly("#missing", true));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setTopicProtected("#missing", true));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->setLimit("#missing", 5));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setKey("#missing", "secret"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setTopic("#missing", "topic"));
}

TEST(ChannelSettingsManager, SettersOnUnknownChannelDoNotCreateIt_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setInviteOnly("#missing", true));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setTopic("#missing", "topic"));

    CHECK_FALSE(channelExists("#missing"));
}

TEST(ChannelSettingsManager, SettersOnUnknownChannelDoNotAffectExistingChannels_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->setLimit("#missing", 5));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setKey("#missing", "secret"));

    Channel& channel = channels->findChannel("#general");
    CHECK_EQUAL(0, channel.getLimit());
    STRCMP_EQUAL("", channel.getKey().c_str());
    CHECK_TRUE(membershipIsIntact());
}

TEST(ChannelSettingsManager, SettersWithInvalidChannelNameThrowChannelNotFound_AC_226) {
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setTopic("general", "topic"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->setTopic("", "topic"));
}

TEST(ChannelSettingsManager, SettersOnRemovedChannelThrowChannelNotFound_AC_226) {
    channels->removeChannel("#other");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException,
                 manager->setInviteOnly("#other", true));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, manager->setLimit("#other", 5));
}