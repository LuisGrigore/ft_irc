#include "CppUTest/TestHarness.h"
#include "channel/ChannelRegistry.hpp"

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

TEST_GROUP(ChannelRegistry){};

// Add and find

TEST(ChannelRegistry, AddChannelMakesChannelFindableByName_AC_022) {
    ChannelRegistry registry;

    registry.addChannel("#general");

    STRCMP_EQUAL("#general", registry.findChannel("#general").getName().c_str());
}

TEST(ChannelRegistry, AddedChannelStartsWithDefaultState_AC_023) {
    ChannelRegistry registry;

    registry.addChannel("#general");

    Channel& channel = registry.findChannel("#general");
    STRCMP_EQUAL("", channel.getTopic().c_str());
    STRCMP_EQUAL("", channel.getKey().c_str());
    CHECK_EQUAL(0, channel.getLimit());
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isTopicProtected());
    CHECK_TRUE(channel.getMembers().empty());
}

TEST(ChannelRegistry, AddChannelAllowsMultipleDistinctChannels_AC_022) {
    ChannelRegistry registry;

    registry.addChannel("#first");
    registry.addChannel("#second");
    registry.addChannel("&third");

    STRCMP_EQUAL("#first", registry.findChannel("#first").getName().c_str());
    STRCMP_EQUAL("#second", registry.findChannel("#second").getName().c_str());
    STRCMP_EQUAL("&third", registry.findChannel("&third").getName().c_str());
}

TEST(ChannelRegistry, SamePrefixlessNameWithDifferentPrefixesAreDistinctChannels_AC_022) {
    ChannelRegistry registry;

    registry.addChannel("#general");
    registry.addChannel("&general");

    registry.findChannel("#general").setTopic("hash");
    registry.findChannel("&general").setTopic("amp");

    STRCMP_EQUAL("hash", registry.findChannel("#general").getTopic().c_str());
    STRCMP_EQUAL("amp", registry.findChannel("&general").getTopic().c_str());
}

TEST(ChannelRegistry, FindChannelReturnsSameObjectOnEveryCall_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    Channel& first = registry.findChannel("#general");
    Channel& second = registry.findChannel("#general");

    CHECK_TRUE(&first == &second);
}

TEST(ChannelRegistry, FindChannelReturnsReferenceThatModifiesStoredChannel_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    registry.findChannel("#general").setTopic("topic");
    registry.findChannel("#general").addMember(5);

    STRCMP_EQUAL("topic", registry.findChannel("#general").getTopic().c_str());
    CHECK_TRUE(registry.findChannel("#general").isMember(5));
}

TEST(ChannelRegistry, FindChannelReturnsDistinctObjectsForDistinctNames_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#first");
    registry.addChannel("#second");

    registry.findChannel("#first").setTopic("one");
    registry.findChannel("#second").setTopic("two");

    STRCMP_EQUAL("one", registry.findChannel("#first").getTopic().c_str());
    STRCMP_EQUAL("two", registry.findChannel("#second").getTopic().c_str());
}

TEST(ChannelRegistry, FindChannelDoesNotMatchNamesThatOnlyShareAPrefix_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#gen"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#generals"));
}

// Case insensitivity

TEST(ChannelRegistry, FindChannelIgnoresCaseOfTheName_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    STRCMP_EQUAL("#General", registry.findChannel("#general").getName().c_str());
    STRCMP_EQUAL("#General", registry.findChannel("#GENERAL").getName().c_str());
    STRCMP_EQUAL("#General", registry.findChannel("#gEnErAl").getName().c_str());
}

TEST(ChannelRegistry, FindChannelWithDifferentCaseReturnsSameObject_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    Channel& lower = registry.findChannel("#general");
    Channel& upper = registry.findChannel("#GENERAL");

    CHECK_TRUE(&lower == &upper);
}

TEST(ChannelRegistry, ChannelKeepsCapitalizationUsedAtCreation_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#MixedCase");

    STRCMP_EQUAL("#MixedCase", registry.findChannel("#mixedcase").getName().c_str());
}

TEST(ChannelRegistry, ModifyingChannelThroughDifferentCaseAffectsTheSameChannel_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    registry.findChannel("#GENERAL").setTopic("topic");

    STRCMP_EQUAL("topic", registry.findChannel("#General").getTopic().c_str());
}

TEST(ChannelRegistry, AddChannelWithDifferentCaseThrowsAlreadyExists_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException, registry.addChannel("#general"));
    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException, registry.addChannel("#GENERAL"));
}

TEST(ChannelRegistry, DifferentNamesAreNotEquivalentWhenIgnoringCase_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    registry.addChannel("#Other");

    STRCMP_EQUAL("#Other", registry.findChannel("#other").getName().c_str());
}

// Missing channels

TEST(ChannelRegistry, FindChannelThrowsNotFoundWhenRegistryIsEmpty_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#general"));
}

TEST(ChannelRegistry, FindChannelThrowsNotFoundWhenNameDoesNotExist_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#other"));
}

TEST(ChannelRegistry, FindChannelWithInvalidNameThrowsNotFound_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel(""));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("general"));
    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#gen eral"));
}

TEST(ChannelRegistry, NotFoundExceptionHasExpectedNameAndMessage_AC_024) {
    ChannelRegistry registry;

    try {
        registry.findChannel("#missing");
        FAIL("Expected ChannelNotFoundException");
    } catch (const ChannelRegistry::ChannelNotFoundException& e) {
        STRCMP_EQUAL("ChannelNotFoundException", e.getName().c_str());
        CHECK_TRUE(std::string(e.what()).find("#missing") != std::string::npos);
    }
}

// Duplicate creation

TEST(ChannelRegistry, AddChannelWithExistingNameThrowsAlreadyExists_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException, registry.addChannel("#general"));
}

TEST(ChannelRegistry, AddChannelWithExistingNameKeepsOriginalChannel_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");
    registry.findChannel("#general").setTopic("original");
    registry.findChannel("#general").addMember(5);

    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException, registry.addChannel("#general"));

    STRCMP_EQUAL("original", registry.findChannel("#general").getTopic().c_str());
    CHECK_TRUE(registry.findChannel("#general").isMember(5));
}

TEST(ChannelRegistry, DuplicateWithDifferentCaseKeepsOriginalCapitalization_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    CHECK_THROWS(ChannelRegistry::ChannelAlreadyExistsException, registry.addChannel("#GENERAL"));

    STRCMP_EQUAL("#General", registry.findChannel("#general").getName().c_str());
}

TEST(ChannelRegistry, AlreadyExistsExceptionHasExpectedNameAndMessage_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    try {
        registry.addChannel("#general");
        FAIL("Expected ChannelAlreadyExistsException");
    } catch (const ChannelRegistry::ChannelAlreadyExistsException& e) {
        STRCMP_EQUAL("ChannelAlreadyExistsException", e.getName().c_str());
        CHECK_TRUE(std::string(e.what()).find("#general") != std::string::npos);
    }
}

// Invalid names: accepted

TEST(ChannelRegistry, AddChannelAcceptsHashAndAmpersandPrefixes_AC_024) {
    ChannelRegistry registry;

    registry.addChannel("#general");
    registry.addChannel("&local");

    STRCMP_EQUAL("#general", registry.findChannel("#general").getName().c_str());
    STRCMP_EQUAL("&local", registry.findChannel("&local").getName().c_str());
}

TEST(ChannelRegistry, AddChannelAcceptsSingleCharacterAfterPrefix_AC_024) {
    ChannelRegistry registry;

    registry.addChannel("#a");

    STRCMP_EQUAL("#a", registry.findChannel("#a").getName().c_str());
}

TEST(ChannelRegistry, AddChannelAcceptsDigitsHyphensUnderscoresAndDots_AC_024) {
    ChannelRegistry registry;

    registry.addChannel("#chan-1");
    registry.addChannel("#chan_2");
    registry.addChannel("#chan.3");
    registry.addChannel("#123");

    STRCMP_EQUAL("#chan-1", registry.findChannel("#chan-1").getName().c_str());
    STRCMP_EQUAL("#chan_2", registry.findChannel("#chan_2").getName().c_str());
    STRCMP_EQUAL("#chan.3", registry.findChannel("#chan.3").getName().c_str());
    STRCMP_EQUAL("#123", registry.findChannel("#123").getName().c_str());
}

TEST(ChannelRegistry, AddChannelAcceptsNameWithMaximumLength_AC_024) {
    ChannelRegistry registry;
    const std::string name = "#" + std::string(49, 'a'); // 50 characters

    registry.addChannel(name);

    STRCMP_EQUAL(name.c_str(), registry.findChannel(name).getName().c_str());
}

// Invalid names: rejected

TEST(ChannelRegistry, AddChannelRejectsEmptyName_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel(""));
}

TEST(ChannelRegistry, AddChannelRejectsPrefixWithoutName_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("#"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("&"));
}

TEST(ChannelRegistry, AddChannelRejectsNameWithoutChannelPrefix_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("general"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("1general"));
}

TEST(ChannelRegistry, AddChannelRejectsUnsupportedPrefixes_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("+general"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("!general"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("@general"));
}

TEST(ChannelRegistry, AddChannelRejectsPrefixNotAtTheStart_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("a#general"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel(" #general"));
}

TEST(ChannelRegistry, AddChannelRejectsNameContainingSpaces_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("#gen eral"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("#general "));
}

TEST(ChannelRegistry, AddChannelRejectsNameContainingCommaOrColon_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("#gen,eral"));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("#gen:eral"));
}

TEST(ChannelRegistry, AddChannelRejectsNameContainingControlCharacters_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 registry.addChannel(std::string("#gen\x07" "eral")));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 registry.addChannel(std::string("#gen\reral")));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 registry.addChannel(std::string("#gen\neral")));
    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 registry.addChannel(std::string("#gen\0eral", 9)));
}

TEST(ChannelRegistry, AddChannelRejectsNameLongerThanMaximumLength_AC_024) {
    ChannelRegistry registry;
    const std::string name = "#" + std::string(50, 'a'); // 51 characters

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel(name));
}

TEST(ChannelRegistry, AddChannelRejectsVeryLongName_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException,
                 registry.addChannel("#" + std::string(1000, 'a')));
}

TEST(ChannelRegistry, InvalidNameDoesNotCreateChannel_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("#gen eral"));

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#gen eral"));
}

TEST(ChannelRegistry, InvalidNameDoesNotAffectExistingChannels_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");
    registry.findChannel("#general").setTopic("topic");

    CHECK_THROWS(ChannelRegistry::InvalidChannelNameException, registry.addChannel("general"));

    STRCMP_EQUAL("topic", registry.findChannel("#general").getTopic().c_str());
}

TEST(ChannelRegistry, InvalidNameExceptionHasExpectedNameAndMessage_AC_024) {
    ChannelRegistry registry;

    try {
        registry.addChannel("bad name");
        FAIL("Expected InvalidChannelNameException");
    } catch (const ChannelRegistry::InvalidChannelNameException& e) {
        STRCMP_EQUAL("InvalidChannelNameException", e.getName().c_str());
        CHECK_TRUE(std::string(e.what()).find("bad name") != std::string::npos);
    }
}

// Remove

TEST(ChannelRegistry, RemoveChannelMakesChannelNotFound_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    registry.removeChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#general"));
}

TEST(ChannelRegistry, RemoveChannelIgnoresCaseOfTheName_AC_022) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    registry.removeChannel("#GENERAL");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#general"));
}

TEST(ChannelRegistry, RemoveChannelDoesNotAffectOtherChannels_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#first");
    registry.addChannel("#second");
    registry.findChannel("#second").setTopic("topic");

    registry.removeChannel("#first");

    STRCMP_EQUAL("topic", registry.findChannel("#second").getTopic().c_str());
}

TEST(ChannelRegistry, RemoveChannelRemovesItEvenWhenItStillHasMembers_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");
    registry.findChannel("#general").addMember(5);

    registry.removeChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#general"));
}

TEST(ChannelRegistry, RemoveChannelThrowsNotFoundWhenRegistryIsEmpty_AC_024) {
    ChannelRegistry registry;

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.removeChannel("#general"));
}

TEST(ChannelRegistry, RemoveChannelThrowsNotFoundWhenNameDoesNotExist_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.removeChannel("#other"));
    STRCMP_EQUAL("#general", registry.findChannel("#general").getName().c_str());
}

TEST(ChannelRegistry, RemoveChannelTwiceThrowsNotFoundOnSecondCall_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    registry.removeChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.removeChannel("#general"));
}

TEST(ChannelRegistry, RemoveChannelWithInvalidNameThrowsNotFound_AC_024) {
    ChannelRegistry registry;
    registry.addChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.removeChannel("general"));
    STRCMP_EQUAL("#general", registry.findChannel("#general").getName().c_str());
}

TEST(ChannelRegistry, AddChannelAfterRemoveCreatesFreshChannel_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");
    registry.findChannel("#general").setTopic("topic");
    registry.findChannel("#general").setKey("secret");
    registry.findChannel("#general").setInviteOnly(true);
    registry.findChannel("#general").addMember(5);
    registry.findChannel("#general").addOperator(5);

    registry.removeChannel("#general");
    registry.addChannel("#general");

    Channel& channel = registry.findChannel("#general");
    STRCMP_EQUAL("", channel.getTopic().c_str());
    STRCMP_EQUAL("", channel.getKey().c_str());
    CHECK_FALSE(channel.isInviteOnly());
    CHECK_FALSE(channel.isMember(5));
    CHECK_FALSE(channel.isOperator(5));
}

TEST(ChannelRegistry, AddChannelAfterRemoveWithDifferentCaseUsesNewCapitalization_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#General");

    registry.removeChannel("#general");
    registry.addChannel("#GENERAL");

    STRCMP_EQUAL("#GENERAL", registry.findChannel("#general").getName().c_str());
}

// Lifecycle driven by the caller

TEST(ChannelRegistry, ChannelCanBeRemovedOnceItsLastMemberLeaves_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");
    registry.findChannel("#general").addMember(5);
    registry.findChannel("#general").addMember(6);

    // Simulates the server lifecycle rule: the caller removes the channel when it is empty.
    registry.findChannel("#general").removeMember(5);
    if (registry.findChannel("#general").getMembers().empty())
        registry.removeChannel("#general");
    STRCMP_EQUAL("#general", registry.findChannel("#general").getName().c_str());

    registry.findChannel("#general").removeMember(6);
    if (registry.findChannel("#general").getMembers().empty())
        registry.removeChannel("#general");

    CHECK_THROWS(ChannelRegistry::ChannelNotFoundException, registry.findChannel("#general"));
}

TEST(ChannelRegistry, ExistingChannelIsReusedByLaterJoinersInsteadOfBeingRecreated_AC_023) {
    ChannelRegistry registry;
    registry.addChannel("#general");
    registry.findChannel("#general").addMember(5);
    registry.findChannel("#general").addOperator(5);

    // A second client joins by looking the channel up, not by creating it again.
    registry.findChannel("#GENERAL").addMember(6);

    CHECK_TRUE(registry.findChannel("#general").isMember(5));
    CHECK_TRUE(registry.findChannel("#general").isOperator(5));
    CHECK_TRUE(registry.findChannel("#general").isMember(6));
    CHECK_FALSE(registry.findChannel("#general").isOperator(6));
}