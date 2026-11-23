#include "CppUTest/TestHarness.h"
#include "client/ClientRegistry.hpp"
#include "managers/PendingIOManager.hpp"

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

namespace {

    bool contains(const std::set<int>& fds, int fd) { return fds.find(fd) != fds.end(); }

} // namespace

TEST_GROUP(PendingIOManager) {
    ClientRegistry* registry;
    PendingIOManager* manager;

    void setup() {
        registry = new ClientRegistry();
        manager = new PendingIOManager(*registry);
    }

    void teardown() {
        delete manager;
        delete registry;
    }
};

// Buffer clear

TEST(PendingIOManager, ClientInputClearBufferClearsBuffer) {
    registry->addClient(5);

    manager->pushInput(5, "hello\r\n");

    manager->clearInput(5);

    CHECK_TRUE(registry->findClient(5).isInputBufferEmpty());
}

TEST(PendingIOManager, ClientOutputClearBufferClearsBuffer) {
    registry->addClient(5);

    manager->pushOutput(5, "hello\r\n");

    manager->clearOutput(5);

    CHECK_TRUE(registry->findClient(5).isOutputBufferEmpty());
}

// Push Input

TEST(PendingIOManager, PushInputMarksFdAsHavingPendingInput_AC_207) {
    registry->addClient(5);

    manager->pushInput(5, "hello\r\n");

    std::set<int> pending = manager->getHasPendingInput();
    CHECK_EQUAL((size_t)1, pending.size());
    CHECK_TRUE(contains(pending, 5));
}

TEST(PendingIOManager, PushInputAppendsDataToClientInputBuffer_AC_207) {
    registry->addClient(5);

    manager->pushInput(5, "hello\r\n");

    CHECK_FALSE(registry->findClient(5).isInputBufferEmpty());
}

TEST(PendingIOManager, PushInputAppendsDataToExistingBuffer_AC_207) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hel");
    manager->pushInput(5, "lo\r\n");

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("hello", line.c_str());
}

TEST(PendingIOManager, PushInputDoesNotMarkFdAsHavingPendingOutput_AC_207) {
    registry->addClient(5);

    manager->pushInput(5, "hello\r\n");

    CHECK_TRUE(manager->getHasPendingOutput().empty());
}

TEST(PendingIOManager, PushInputOnlyMarksTargetFd_AC_207) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushInput(5, "hello\r\n");

    std::set<int> pending = manager->getHasPendingInput();
    CHECK_TRUE(contains(pending, 5));
    CHECK_FALSE(contains(pending, 6));
}

TEST(PendingIOManager, PushInputTwiceReportsFdOnce_AC_207) {
    registry->addClient(5);

    manager->pushInput(5, "first\r\n");
    manager->pushInput(5, "second\r\n");

    CHECK_EQUAL((size_t)1, manager->getHasPendingInput().size());
}

TEST(PendingIOManager, PushInputKeepsDataOfDifferentClientsSeparate_AC_207) {
    registry->addClient(5);
    registry->addClient(6);
    std::string line;

    manager->pushInput(5, "from five\r\n");
    manager->pushInput(6, "from six\r\n");

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("from five", line.c_str());
    CHECK_TRUE(manager->getNextInputLine(6, line));
    STRCMP_EQUAL("from six", line.c_str());
}

TEST(PendingIOManager, PushInputSplitBetweenCarriageReturnAndNewlineYieldsLine_AC_207) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hello\r");
    CHECK_FALSE(manager->getNextInputLine(5, line));

    manager->pushInput(5, "\n");
    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("hello", line.c_str());
}

// Push Output

TEST(PendingIOManager, PushOutputMarksFdAsHavingPendingOutput_AC_207) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    std::set<int> pending = manager->getHasPendingOutput();
    CHECK_EQUAL((size_t)1, pending.size());
    CHECK_TRUE(contains(pending, 5));
}

TEST(PendingIOManager, PushOutputAppendsDataToClientOutputBuffer_AC_207) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    CHECK_FALSE(registry->findClient(5).isOutputBufferEmpty());
    STRCMP_EQUAL("hello", manager->peekOutput(5, 5).c_str());
}

TEST(PendingIOManager, PushOutputAppendsDataToExistingBuffer_AC_207) {
    registry->addClient(5);

    manager->pushOutput(5, "hel");
    manager->pushOutput(5, "lo");

    STRCMP_EQUAL("hello", manager->peekOutput(5, 5).c_str());
}

TEST(PendingIOManager, PushOutputDoesNotMarkFdAsHavingPendingInput_AC_207) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    CHECK_TRUE(manager->getHasPendingInput().empty());
}

TEST(PendingIOManager, PushOutputOnlyMarksTargetFd_AC_207) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushOutput(5, "hello");

    std::set<int> pending = manager->getHasPendingOutput();
    CHECK_TRUE(contains(pending, 5));
    CHECK_FALSE(contains(pending, 6));
}

TEST(PendingIOManager, PushOutputTwiceReportsFdOnce_AC_207) {
    registry->addClient(5);

    manager->pushOutput(5, "first");
    manager->pushOutput(5, "second");

    CHECK_EQUAL((size_t)1, manager->getHasPendingOutput().size());
}

TEST(PendingIOManager, PushOutputKeepsDataOfDifferentClientsSeparate_AC_207) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushOutput(5, "five");
    manager->pushOutput(6, "six");

    STRCMP_EQUAL("five", manager->peekOutput(5, 10).c_str());
    STRCMP_EQUAL("six", manager->peekOutput(6, 10).c_str());
}

// Get Next Input Line

TEST(PendingIOManager, GetNextInputLineExtractsCompleteLine_AC_208) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hello\r\n");

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("hello", line.c_str());
}

TEST(PendingIOManager, GetNextInputLineExtractsExactlyOneLine_AC_208) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "first\r\nsecond\r\n");

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("first", line.c_str());

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("second", line.c_str());

    CHECK_FALSE(manager->getNextInputLine(5, line));
}

TEST(PendingIOManager, GetNextInputLineRemovesExtractedLineFromBuffer_AC_208) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hello\r\n");
    manager->getNextInputLine(5, line);

    CHECK_TRUE(registry->findClient(5).isInputBufferEmpty());
}

TEST(PendingIOManager, GetNextInputLineReturnsFalseWhenBufferIsEmpty_AC_208) {
    registry->addClient(5);
    std::string line = "untouched";

    CHECK_FALSE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("untouched", line.c_str());
}

TEST(PendingIOManager, GetNextInputLineReturnsFalseAndLeavesPartialDataUntouched_AC_208) {
    registry->addClient(5);
    std::string line = "untouched";

    manager->pushInput(5, "hello");

    CHECK_FALSE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("untouched", line.c_str());
    CHECK_FALSE(registry->findClient(5).isInputBufferEmpty());

    manager->pushInput(5, "\r\n");
    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("hello", line.c_str());
}

TEST(PendingIOManager, GetNextInputLineKeepsIncompleteRemainderAfterExtractingLine_AC_208) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "first\r\nsecond");

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("first", line.c_str());
    CHECK_FALSE(registry->findClient(5).isInputBufferEmpty());

    manager->pushInput(5, "\r\n");
    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("second", line.c_str());
}

TEST(PendingIOManager, GetNextInputLineAcceptsNewlineAloneAsTerminator_AC_208) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hello\n");

    CHECK_TRUE(manager->getNextInputLine(5, line));
    STRCMP_EQUAL("hello", line.c_str());
}

TEST(PendingIOManager, GetNextInputLineDoesNotAffectPendingOutput_AC_208) {
    registry->addClient(5);
    std::string line;

    manager->pushOutput(5, "reply");
    manager->pushInput(5, "hello\r\n");
    manager->getNextInputLine(5, line);

    CHECK_TRUE(contains(manager->getHasPendingOutput(), 5));
    STRCMP_EQUAL("reply", manager->peekOutput(5, 10).c_str());
}

// Pending state transitions

TEST(PendingIOManager, FdStopsHavingPendingInputAfterLastLineIsExtracted_AC_209) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hello\r\n");
    manager->getNextInputLine(5, line);

    CHECK_FALSE(contains(manager->getHasPendingInput(), 5));
}

TEST(PendingIOManager, FdKeepsPendingInputWhileCompleteLinesRemain_AC_209) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "first\r\nsecond\r\n");

    manager->getNextInputLine(5, line);
    CHECK_TRUE(contains(manager->getHasPendingInput(), 5));

    manager->getNextInputLine(5, line);
    CHECK_FALSE(contains(manager->getHasPendingInput(), 5));
}

TEST(PendingIOManager, FdHavePendingInputWhenOnlyPartialDataRemains_AC_209) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "first\r\nsecond");
    manager->getNextInputLine(5, line);

    CHECK_TRUE(contains(manager->getHasPendingInput(), 5));
}

TEST(PendingIOManager, FdPendingAfterFailedExtractionOfPartialData_AC_209) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hello");

    CHECK_FALSE(manager->getNextInputLine(5, line));
    CHECK_TRUE(contains(manager->getHasPendingInput(), 5));
}

TEST(PendingIOManager, FdBecomesPendingAgainWhenPartialLineIsCompleted_AC_209) {
    registry->addClient(5);
    std::string line;

    manager->pushInput(5, "hel");
    manager->getNextInputLine(5, line);

    manager->pushInput(5, "lo\r\n");

    CHECK_TRUE(contains(manager->getHasPendingInput(), 5));
}

TEST(PendingIOManager, ExtractingLineOfOneFdDoesNotAffectPendingInputOfAnother_AC_209) {
    registry->addClient(5);
    registry->addClient(6);
    std::string line;

    manager->pushInput(5, "hello\r\n");
    manager->pushInput(6, "world\r\n");
    manager->getNextInputLine(5, line);

    std::set<int> pending = manager->getHasPendingInput();
    CHECK_FALSE(contains(pending, 5));
    CHECK_TRUE(contains(pending, 6));
}

TEST(PendingIOManager, FdStopsHavingPendingOutputWhenBufferIsFullyConsumed_AC_209) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");
    manager->consumeOutput(5, 5);

    CHECK_FALSE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, FdKeepsPendingOutputWhileBytesRemain_AC_209) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");
    manager->consumeOutput(5, 2);

    CHECK_TRUE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, FdStopsHavingPendingOutputWhenConsumingMoreThanAvailable_AC_209) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");
    manager->consumeOutput(5, 100);

    CHECK_FALSE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, FdBecomesPendingOutputAgainAfterNewPush_AC_209) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");
    manager->consumeOutput(5, 5);
    manager->pushOutput(5, "again");

    CHECK_TRUE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, ConsumingOutputOfOneFdDoesNotAffectPendingOutputOfAnother_AC_209) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushOutput(5, "hello");
    manager->pushOutput(6, "world");
    manager->consumeOutput(5, 5);

    std::set<int> pending = manager->getHasPendingOutput();
    CHECK_FALSE(contains(pending, 5));
    CHECK_TRUE(contains(pending, 6));
}

TEST(PendingIOManager, ConsumeOutputDoesNotAffectPendingInput_AC_209) {
    registry->addClient(5);

    manager->pushInput(5, "hello\r\n");
    manager->pushOutput(5, "reply");
    manager->consumeOutput(5, 5);

    CHECK_TRUE(contains(manager->getHasPendingInput(), 5));
}

// Peek Output

TEST(PendingIOManager, PeekOutputReturnsRequestedBytes_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    STRCMP_EQUAL("hel", manager->peekOutput(5, 3).c_str());
}

TEST(PendingIOManager, PeekOutputDoesNotModifyBuffer_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    STRCMP_EQUAL("hello", manager->peekOutput(5, 5).c_str());
    STRCMP_EQUAL("hello", manager->peekOutput(5, 5).c_str());
    CHECK_FALSE(registry->findClient(5).isOutputBufferEmpty());
}

TEST(PendingIOManager, PeekOutputKeepsFdPending_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");
    manager->peekOutput(5, 5);

    CHECK_TRUE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, PeekOutputReturnsAvailableDataWhenLengthExceedsBuffer_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    STRCMP_EQUAL("hello", manager->peekOutput(5, 100).c_str());
}

TEST(PendingIOManager, PeekOutputReturnsEmptyStringWhenBufferIsEmpty_AC_210) {
    registry->addClient(5);

    STRCMP_EQUAL("", manager->peekOutput(5, 5).c_str());
}

TEST(PendingIOManager, PeekOutputWithZeroLengthReturnsEmptyStringAndKeepsData_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");

    STRCMP_EQUAL("", manager->peekOutput(5, 0).c_str());
    STRCMP_EQUAL("hello", manager->peekOutput(5, 5).c_str());
}

TEST(PendingIOManager, PeekOutputDoesNotReturnDataOfOtherClients_AC_210) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushOutput(6, "other");

    STRCMP_EQUAL("", manager->peekOutput(5, 10).c_str());
}

// Consume Output

TEST(PendingIOManager, ConsumeOutputRemovesExactlyTheRequestedBytes_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello world");
    manager->consumeOutput(5, 6);

    STRCMP_EQUAL("world", manager->peekOutput(5, 20).c_str());
}

TEST(PendingIOManager, ConsumeOutputKeepsRemainingBytesInOrder_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "abcdef");
    manager->consumeOutput(5, 2);
    manager->pushOutput(5, "gh");
    manager->consumeOutput(5, 3);

    STRCMP_EQUAL("fgh", manager->peekOutput(5, 20).c_str());
}

TEST(PendingIOManager, ConsumeOutputWithZeroDoesNotModifyBuffer_AC_210) {
    registry->addClient(5);

    manager->pushOutput(5, "hello");
    manager->consumeOutput(5, 0);

    STRCMP_EQUAL("hello", manager->peekOutput(5, 5).c_str());
    CHECK_TRUE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, ConsumeOutputOnEmptyBufferDoesNotCreatePendingOutput_AC_210) {
    registry->addClient(5);

    manager->consumeOutput(5, 5);

    CHECK_TRUE(manager->getHasPendingOutput().empty());
    CHECK_TRUE(registry->findClient(5).isOutputBufferEmpty());
}

TEST(PendingIOManager, ConsumeOutputDoesNotModifyOtherClients_AC_210) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushOutput(5, "hello");
    manager->pushOutput(6, "world");
    manager->consumeOutput(5, 3);

    STRCMP_EQUAL("world", manager->peekOutput(6, 10).c_str());
}

TEST(PendingIOManager, PartialWritesDrainLargeOutputInOrder_AC_210) {
    registry->addClient(5);
    std::string expected;
    for (int i = 0; i < 1000; ++i)
        expected += "0123456789";
    std::string received;

    manager->pushOutput(5, expected);

    // Simulates a socket that accepts at most 7 bytes per write.
    while (contains(manager->getHasPendingOutput(), 5)) {
        std::string chunk = manager->peekOutput(5, 7);
        received += chunk;
        manager->consumeOutput(5, chunk.size());
    }

    CHECK_TRUE(expected == received);
    CHECK_TRUE(registry->findClient(5).isOutputBufferEmpty());
}

// Pending snapshots

TEST(PendingIOManager, SnapshotsAreEmptyInitially_AC_211) {
    CHECK_TRUE(manager->getHasPendingInput().empty());
    CHECK_TRUE(manager->getHasPendingOutput().empty());
}

TEST(PendingIOManager, GetHasPendingInputReturnsAllFdsWithPendingInput_AC_211) {
    registry->addClient(5);
    registry->addClient(6);
    registry->addClient(7);

    manager->pushInput(5, "a\r\n");
    manager->pushInput(7, "b\r\n");

    std::set<int> pending = manager->getHasPendingInput();
    CHECK_EQUAL((size_t)2, pending.size());
    CHECK_TRUE(contains(pending, 5));
    CHECK_FALSE(contains(pending, 6));
    CHECK_TRUE(contains(pending, 7));
}

TEST(PendingIOManager, GetHasPendingOutputReturnsAllFdsWithPendingOutput_AC_211) {
    registry->addClient(5);
    registry->addClient(6);
    registry->addClient(7);

    manager->pushOutput(5, "a");
    manager->pushOutput(7, "b");

    std::set<int> pending = manager->getHasPendingOutput();
    CHECK_EQUAL((size_t)2, pending.size());
    CHECK_TRUE(contains(pending, 5));
    CHECK_FALSE(contains(pending, 6));
    CHECK_TRUE(contains(pending, 7));
}

TEST(PendingIOManager, PendingInputAndOutputAreTrackedIndependently_AC_211) {
    registry->addClient(5);
    registry->addClient(6);

    manager->pushInput(5, "a\r\n");
    manager->pushOutput(6, "b");

    std::set<int> input = manager->getHasPendingInput();
    std::set<int> output = manager->getHasPendingOutput();
    CHECK_TRUE(contains(input, 5));
    CHECK_FALSE(contains(input, 6));
    CHECK_TRUE(contains(output, 6));
    CHECK_FALSE(contains(output, 5));
}

TEST(PendingIOManager, FdWithBothPendingInputAndOutputAppearsInBothSnapshots_AC_211) {
    registry->addClient(5);

    manager->pushInput(5, "a\r\n");
    manager->pushOutput(5, "b");

    CHECK_TRUE(contains(manager->getHasPendingInput(), 5));
    CHECK_TRUE(contains(manager->getHasPendingOutput(), 5));
}

TEST(PendingIOManager, PendingInputSnapshotIsACopy_AC_211) {
    registry->addClient(5);
    manager->pushInput(5, "a\r\n");

    std::set<int> snapshot = manager->getHasPendingInput();
    snapshot.insert(99);
    snapshot.erase(5);

    std::set<int> current = manager->getHasPendingInput();
    CHECK_EQUAL((size_t)1, current.size());
    CHECK_TRUE(contains(current, 5));
}

TEST(PendingIOManager, PendingOutputSnapshotIsACopy_AC_211) {
    registry->addClient(5);
    manager->pushOutput(5, "a");

    std::set<int> snapshot = manager->getHasPendingOutput();
    snapshot.insert(99);
    snapshot.erase(5);

    std::set<int> current = manager->getHasPendingOutput();
    CHECK_EQUAL((size_t)1, current.size());
    CHECK_TRUE(contains(current, 5));
}

TEST(PendingIOManager, EarlierSnapshotIsNotAffectedByLaterOperations_AC_211) {
    registry->addClient(5);
    registry->addClient(6);
    manager->pushInput(5, "a\r\n");
    manager->pushOutput(5, "b");

    std::set<int> inputBefore = manager->getHasPendingInput();
    std::set<int> outputBefore = manager->getHasPendingOutput();

    manager->pushInput(6, "c\r\n");
    manager->pushOutput(6, "d");
    manager->consumeOutput(5, 1);

    CHECK_EQUAL((size_t)1, inputBefore.size());
    CHECK_TRUE(contains(inputBefore, 5));
    CHECK_EQUAL((size_t)1, outputBefore.size());
    CHECK_TRUE(contains(outputBefore, 5));
}

TEST(PendingIOManager, SnapshotsWorkOnConstManager_AC_211) {
    registry->addClient(5);
    manager->pushInput(5, "a\r\n");
    manager->pushOutput(5, "b");

    const PendingIOManager& constManager = *manager;

    CHECK_TRUE(contains(constManager.getHasPendingInput(), 5));
    CHECK_TRUE(contains(constManager.getHasPendingOutput(), 5));
}

TEST(PendingIOManager, SnapshotsAreEmptyOnceAllClientsAreDrained_AC_211) {
    registry->addClient(5);
    registry->addClient(6);
    std::string line;

    manager->pushInput(5, "a\r\n");
    manager->pushInput(6, "b\r\n");
    manager->pushOutput(5, "c");
    manager->pushOutput(6, "d");

    manager->getNextInputLine(5, line);
    manager->getNextInputLine(6, line);
    manager->consumeOutput(5, 1);
    manager->consumeOutput(6, 1);

    CHECK_TRUE(manager->getHasPendingInput().empty());
    CHECK_TRUE(manager->getHasPendingOutput().empty());
}

// Unknown fd

TEST(PendingIOManager, PushInputOnUnknownFdThrowsNotFound_AC_212) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushInput(99, "hello\r\n"));
}

TEST(PendingIOManager, PushInputOnUnknownFdDoesNotCreatePendingInput_AC_212) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushInput(99, "hello\r\n"));

    CHECK_TRUE(manager->getHasPendingInput().empty());
}

TEST(PendingIOManager, PushOutputOnUnknownFdThrowsNotFound_AC_212) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushOutput(99, "hello"));
}

TEST(PendingIOManager, PushOutputOnUnknownFdDoesNotCreatePendingOutput_AC_212) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushOutput(99, "hello"));

    CHECK_TRUE(manager->getHasPendingOutput().empty());
}

TEST(PendingIOManager, GetNextInputLineOnUnknownFdThrowsNotFound_AC_212) {
    std::string line;

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->getNextInputLine(99, line));
}

TEST(PendingIOManager, GetNextInputLineOnUnknownFdLeavesLineUntouched_AC_212) {
    std::string line = "untouched";

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->getNextInputLine(99, line));

    STRCMP_EQUAL("untouched", line.c_str());
}

TEST(PendingIOManager, PeekOutputOnUnknownFdThrowsNotFound_AC_212) {
    const PendingIOManager& constManager = *manager;

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, constManager.peekOutput(99, 5));
}

TEST(PendingIOManager, ConsumeOutputOnUnknownFdThrowsNotFound_AC_212) {
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->consumeOutput(99, 5));
}

TEST(PendingIOManager, OperationsOnUnknownFdDoNotCreateAnyPendingState_AC_212) {
    std::string line;

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushInput(99, "a\r\n"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushOutput(99, "b"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->getNextInputLine(99, line));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->peekOutput(99, 1));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->consumeOutput(99, 1));

    CHECK_TRUE(manager->getHasPendingInput().empty());
    CHECK_TRUE(manager->getHasPendingOutput().empty());
}

TEST(PendingIOManager, UnknownFdOperationsDoNotAffectExistingPendingState_AC_212) {
    registry->addClient(5);
    manager->pushInput(5, "hello\r\n");
    manager->pushOutput(5, "reply");

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushInput(99, "a\r\n"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->consumeOutput(99, 1));

    std::set<int> input = manager->getHasPendingInput();
    std::set<int> output = manager->getHasPendingOutput();
    CHECK_EQUAL((size_t)1, input.size());
    CHECK_TRUE(contains(input, 5));
    CHECK_EQUAL((size_t)1, output.size());
    CHECK_TRUE(contains(output, 5));
    STRCMP_EQUAL("reply", manager->peekOutput(5, 10).c_str());
}

TEST(PendingIOManager, OperationsOnRemovedClientThrowNotFound_AC_212) {
    std::string line;
    registry->addClient(5);
    registry->removeClient(5);

    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushInput(5, "a\r\n"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->pushOutput(5, "b"));
    CHECK_THROWS(ClientRegistry::ClientNotFoundException, manager->getNextInputLine(5, line));

    CHECK_TRUE(manager->getHasPendingInput().empty());
    CHECK_TRUE(manager->getHasPendingOutput().empty());
}