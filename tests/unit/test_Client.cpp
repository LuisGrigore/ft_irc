#include "client/Client.hpp"
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

TEST_GROUP(Client){};

// Getters

TEST(Client, GetFdReturnsCorrectValue_AC_001) {
    Client client(5);
    CHECK_EQUAL(5, client.getFd());
}

TEST(Client, GetNickReturnsCorrectValue_AC_001) {
    Client client(5);
    client.setNick("testuser");
    STRCMP_EQUAL("testuser", client.getNick().c_str());
}

TEST(Client, GetUsernameReturnsCorrectValue_AC_001) {
    Client client(5);
    client.setUsername("testuser");
    STRCMP_EQUAL("testuser", client.getUsername().c_str());
}

TEST(Client, GetRealnameReturnsCorrectValue_AC_001) {
    Client client(5);
    client.setRealname("Test User");
    STRCMP_EQUAL("Test User", client.getRealname().c_str());
}

TEST(Client, GetHostnameReturnsCorrectValue_AC_001) {
    Client client(5);
    client.setHostname("localhost");
    STRCMP_EQUAL("localhost", client.getHostname().c_str());
}

// Setters

TEST(Client, SetNickUpdatesValue_AC_001) {
    Client client(5);

    client.setNick("testuser");

    STRCMP_EQUAL("testuser", client.getNick().c_str());
}

TEST(Client, SetUsernameUpdatesValue_AC_001) {
    Client client(5);

    client.setUsername("testuser");

    STRCMP_EQUAL("testuser", client.getUsername().c_str());
}

TEST(Client, SetRealnameUpdatesValue_AC_001) {
    Client client(5);

    client.setRealname("Test User");

    STRCMP_EQUAL("Test User", client.getRealname().c_str());
}

TEST(Client, SetHostnameUpdatesValue_AC_001) {
    Client client(5);

    client.setHostname("localhost");

    STRCMP_EQUAL("localhost", client.getHostname().c_str());
}

TEST(Client, SetFdUpdatesValue_AC_001) {
    Client client(5);
    client.setFd(9);
    CHECK_EQUAL(9, client.getFd());
}

// State Flags

TEST(Client, NewClientIsNotAuthenticatedNorRegisteredNorQuitting_AC_001) {
    Client client(5);
    CHECK_FALSE(client.isAuthenticated());
    CHECK_FALSE(client.isRegistered());
    CHECK_FALSE(client.isQuitting());
}

TEST(Client, SettersUpdateStateFlags_AC_001) {
    Client client(5);
    client.setAuthenticated(true);
    client.setRegistered(true);
    client.setQuitting(true);
    CHECK_TRUE(client.isAuthenticated());
    CHECK_TRUE(client.isRegistered());
    CHECK_TRUE(client.isQuitting());
}

TEST(Client, SettersCanResetStateFlagsToFalse_AC_001) {
    Client client(5);
    client.setAuthenticated(true);
    client.setRegistered(true);
    client.setQuitting(true);

    client.setAuthenticated(false);
    client.setRegistered(false);
    client.setQuitting(false);

    CHECK_FALSE(client.isAuthenticated());
    CHECK_FALSE(client.isRegistered());
    CHECK_FALSE(client.isQuitting());
}

TEST(Client, StateFlagsAreIndependent_AC_001) {
    Client client(5);

    client.setQuitting(true);

    CHECK_FALSE(client.isAuthenticated());
    CHECK_FALSE(client.isRegistered());
    CHECK_TRUE(client.isQuitting());
}

// Channels

TEST(Client, NewClientHasNoChannels_AC_001) {
    Client client(5);

    CHECK_TRUE(client.getChannels().empty());
}

TEST(Client, AddChannelStoresChannel_AC_001) {
    Client client(5);

    client.addChannel("#general");

    std::set<std::string> channels = client.getChannels();
    CHECK_EQUAL((size_t)1, channels.size());
    CHECK_EQUAL((size_t)1, channels.count("#general"));
}

TEST(Client, AddChannelStoresMultipleChannels_AC_001) {
    Client client(5);

    client.addChannel("#general");
    client.addChannel("#random");

    std::set<std::string> channels = client.getChannels();
    CHECK_EQUAL((size_t)2, channels.size());
    CHECK_EQUAL((size_t)1, channels.count("#general"));
    CHECK_EQUAL((size_t)1, channels.count("#random"));
}

TEST(Client, AddChannelTwiceKeepsSingleEntry_AC_001) {
    Client client(5);

    client.addChannel("#general");
    client.addChannel("#general");

    CHECK_EQUAL((size_t)1, client.getChannels().size());
}

TEST(Client, RemoveChannelRemovesOnlyThatChannel_AC_001) {
    Client client(5);
    client.addChannel("#general");
    client.addChannel("#random");

    client.removeChannel("#general");

    std::set<std::string> channels = client.getChannels();
    CHECK_EQUAL((size_t)1, channels.size());
    CHECK_EQUAL((size_t)0, channels.count("#general"));
    CHECK_EQUAL((size_t)1, channels.count("#random"));
}

TEST(Client, RemoveChannelEmptiesSetWhenLastChannelIsRemoved_AC_001) {
    Client client(5);
    client.addChannel("#general");

    client.removeChannel("#general");

    CHECK_TRUE(client.getChannels().empty());
}

TEST(Client, RemoveChannelNotJoinedHasNoEffect_AC_001) {
    Client client(5);
    client.addChannel("#general");

    client.removeChannel("#random");

    std::set<std::string> channels = client.getChannels();
    CHECK_EQUAL((size_t)1, channels.size());
    CHECK_EQUAL((size_t)1, channels.count("#general"));
}

TEST(Client, RemoveChannelCanBeCalledWithNoChannels_AC_001) {
    Client client(5);

    client.removeChannel("#general");

    CHECK_TRUE(client.getChannels().empty());
}

TEST(Client, GetChannelReturnsCopyThatDoesNotAffectClient_AC_001) {
    Client client(5);
    client.addChannel("#general");

    std::set<std::string> channels = client.getChannels();
    channels.insert("#intruder");
    channels.erase("#general");

    std::set<std::string> current = client.getChannels();
    CHECK_EQUAL((size_t)1, current.size());
    CHECK_EQUAL((size_t)1, current.count("#general"));
    CHECK_EQUAL((size_t)0, current.count("#intruder"));
}

TEST(Client, ChannelNamesAreCaseSensitive_AC_001) {
    Client client(5);

    client.addChannel("#General");
    client.addChannel("#general");

    CHECK_EQUAL((size_t)2, client.getChannels().size());
}

// Buffer clear

TEST(Client, ClientInputClearBufferClearsBuffer) {
    Client client(5);

	client.pushInput("input");
	client.clearInput();

    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, ClientOutputClearBufferClearsBuffer) {
    Client client(5);

	client.pushOutput("input");
	client.clearOutput();

    CHECK_TRUE(client.isOutputBufferEmpty());
}

// Buffer Empty

TEST(Client, NewClientHasEmptyInputAndOutputBuffers_AC_002) {
    Client client(5);

    CHECK_TRUE(client.isInputBufferEmpty());
    CHECK_TRUE(client.isOutputBufferEmpty());
}

TEST(Client, InputBufferIsNotEmptyAfterPushInput_AC_002) {
    Client client(5);

    client.pushInput("hello");

    CHECK_FALSE(client.isInputBufferEmpty());
}

TEST(Client, OutputBufferIsNotEmptyAfterPushOutput_AC_002) {
    Client client(5);

    client.pushOutput("hello");

    CHECK_FALSE(client.isOutputBufferEmpty());
}

// Push Input

TEST(Client, PushInputWithStringAppendsData_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput(std::string("hello\r\n"));

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, PushInputWithSingleCharacterStringsAppendsData_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput(std::string(1, 'h'));
    client.pushInput(std::string(1, 'i'));
    client.pushInput(std::string(1, '\r'));
    client.pushInput(std::string(1, '\n'));

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hi", line.c_str());
}

TEST(Client, PushInputWithCStringAppendsData_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());
}

TEST(Client, PushInputAppendsDataToExistingBuffer_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hel");
    client.pushInput("lo\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());
}

TEST(Client, PushInputSplitBetweenCarriageReturnAndNewlineStillYieldsLine_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\r");
    CHECK_FALSE(client.getNextInputLine(line));

    client.pushInput("\n");
    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

// Push Output

TEST(Client, PushOutputWithStringAppendsData_AC_002) {
    Client client(5);

    client.pushOutput(std::string("hello"));

    STRCMP_EQUAL("hello", client.peekOutput(5).c_str());
    CHECK_FALSE(client.isOutputBufferEmpty());
}

TEST(Client, PushOutputWithSingleCharacterStringsAppendsData_AC_002) {
    Client client(5);

    client.pushOutput(std::string(1, 'h'));
    client.pushOutput(std::string(1, 'i'));

    STRCMP_EQUAL("hi", client.peekOutput(2).c_str());
}

TEST(Client, PushOutputWithCStringAppendsData_AC_002) {
    Client client(5);

    client.pushOutput("hello");

    STRCMP_EQUAL("hello", client.peekOutput(5).c_str());
}

TEST(Client, PushOutputAppendsDataToExistingBuffer_AC_002) {
    Client client(5);

    client.pushOutput("hel");
    client.pushOutput("lo");

    STRCMP_EQUAL("hello", client.peekOutput(5).c_str());
}

// Get Next Input Line

TEST(Client, GetNextInputLineReturnsFalseWhenBufferIsEmpty_AC_002) {
    Client client(5);
    std::string line;

    CHECK_FALSE(client.getNextInputLine(line));
}

TEST(Client, GetNextInputLineReturnsFalseWhenNoNewlineExists_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello");

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_FALSE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineExtractsLineAndConsumesIt_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineExtractsMultipleLines_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("first\r\nsecond\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("first", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("second", line.c_str());

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLinePreservesIncompleteRemainingData_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("first\r\nsecond");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("first", line.c_str());

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_FALSE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineHandlesEmptyLineWithCrLf_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineHandlesEmptyLineWithLfOnly_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineAcceptsNewlineAloneAsTerminator_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineSplitsOnNewlineAndKeepsRemainder_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\nworld");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_FALSE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineDoesNotSplitOnCarriageReturnAlone_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\rworld");

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_FALSE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineDoesNotConsumeTrailingCarriageReturnWithoutNewline_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\r");

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_FALSE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineSplitsMixedLfAndCrLfTerminators_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\nworld\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("world", line.c_str());

    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLinePreservesCarriageReturnInsideLine_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\rworld\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello\rworld", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineStripsOnlyOneCarriageReturnBeforeNewline_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("hello\r\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello\r", line.c_str());
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineSplitsMultipleLinesWithMixedStandaloneCharacters_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("first\nline\rcontent\r\nsecond\rline\ncontent\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("first", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("line\rcontent", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("second\rline", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("content", line.c_str());

    CHECK_FALSE(client.getNextInputLine(line));
    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineReturnsConsecutiveEmptyLines_AC_002) {
    Client client(5);
    std::string line;

    client.pushInput("\r\n\r\nhello\r\n");

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("", line.c_str());

    CHECK_TRUE(client.getNextInputLine(line));
    STRCMP_EQUAL("hello", line.c_str());

    CHECK_TRUE(client.isInputBufferEmpty());
}

TEST(Client, GetNextInputLineDoesNotModifyLineWhenNoCompleteLineIsAvailable_AC_002) {
    Client client(5);
    std::string line = "untouched";

    client.pushInput("incomplete");

    CHECK_FALSE(client.getNextInputLine(line));
    STRCMP_EQUAL("untouched", line.c_str());
}

// Peek Output

TEST(Client, PeekOutputReturnsEmptyStringWhenBufferIsEmpty_AC_002) {
    Client client(5);

    STRCMP_EQUAL("", client.peekOutput(5).c_str());
}

TEST(Client, PeekOutputReturnsRequestedNumberOfCharacters_AC_002) {
    Client client(5);

    client.pushOutput("hello");

    STRCMP_EQUAL("hel", client.peekOutput(3).c_str());
}

TEST(Client, PeekOutputDoesNotConsumeData_AC_002) {
    Client client(5);

    client.pushOutput("hello");

    STRCMP_EQUAL("hello", client.peekOutput(5).c_str());
    CHECK_FALSE(client.isOutputBufferEmpty());

    STRCMP_EQUAL("hello", client.peekOutput(5).c_str());
}

TEST(Client, PeekOutputReturnsAvailableDataWhenLengthExceedsBufferSize_AC_002) {
    Client client(5);

    client.pushOutput("hello");

    STRCMP_EQUAL("hello", client.peekOutput(10).c_str());
}

TEST(Client, PeekOutputReturnsEmptyStringWhenLengthIsZero_AC_002) {
    Client client(5);

    client.pushOutput("hello");

    STRCMP_EQUAL("", client.peekOutput(0).c_str());
    CHECK_FALSE(client.isOutputBufferEmpty());
}

// Consume Output

TEST(Client, ConsumeOutputRemovesRequestedCharacters_AC_002) {
    Client client(5);

    client.pushOutput("hello");
    client.consumeOutput(2);

    STRCMP_EQUAL("llo", client.peekOutput(10).c_str());
}

TEST(Client, ConsumeOutputEmptiesBufferWhenAllCharactersAreConsumed_AC_002) {
    Client client(5);

    client.pushOutput("hello");
    client.consumeOutput(5);

    CHECK_TRUE(client.isOutputBufferEmpty());
    STRCMP_EQUAL("", client.peekOutput(5).c_str());
}

TEST(Client, ConsumeOutputCanConsumeMoreCharactersThanAvailable_AC_002) {
    Client client(5);

    client.pushOutput("hello");
    client.consumeOutput(10);

    CHECK_TRUE(client.isOutputBufferEmpty());
}

TEST(Client, ConsumeOutputWithZeroDoesNotModifyBuffer_AC_002) {
    Client client(5);

    client.pushOutput("hello");
    client.consumeOutput(0);

    STRCMP_EQUAL("hello", client.peekOutput(5).c_str());
    CHECK_FALSE(client.isOutputBufferEmpty());
}

TEST(Client, ConsumeOutputCanBeCalledOnEmptyBuffer_AC_002) {
    Client client(5);

    client.consumeOutput(5);

    CHECK_TRUE(client.isOutputBufferEmpty());
}

TEST(Client, ConsumeOutputAfterPartialConsumeKeepsOrder_AC_002) {
    Client client(5);

    client.pushOutput("hello world");
    client.consumeOutput(6);
    client.pushOutput("!");

    STRCMP_EQUAL("world!", client.peekOutput(20).c_str());
}