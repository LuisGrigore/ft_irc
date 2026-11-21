# Implementation Plan

Traceability plan for the **ft_irc** IRC server project, derived from:

---

## 1. Features Table

| ID | Name | Related Elements |
|----|------|------------------|
| FEAT-001 | BaseException and Business exceptions (one per IRC error code) | None |
| FEAT-002 | Client entity and ClientRegistry | `client_subsystem.mmd` |
| FEAT-003 | Client authentication management | `client_subsystem.mmd` |
| FEAT-004 | Client registration management | `client_subsystem.mmd` |
| FEAT-039 | Client pending I/O management (PendingIOManager) | `client_subsystem.mmd` |
| FEAT-005 | Channel entity and ChannelRegistry | `channel_subsystem.mmd` |
| FEAT-041 | Channel membership and settings management (ChannelMembershipManager, ChannelSettingsManager) | `channel_subsystem.mmd` |
| FEAT-042 | Channel broadcast management (ChannelBroadcastManager) | `channel_subsystem.mmd`, `client_subsystem.mmd` |
| FEAT-006 | Channel management (ChannelSystem) | `channel_subsystem.mmd` |
| FEAT-040 | Client lifecycle management (ClientLifecycleManager) | `client_subsystem.mmd`, `channel_subsystem.mmd` |
| FEAT-007 | Client management (ClientSystem) | `client_subsystem.mmd` |
| FEAT-008 | IRCDto | None |
| FEAT-009 | ISecurityFilter and SecurityFilterFactory | `security_filtering.mmd` |
| FEAT-010 | ICommand and CommandFactory | `command_subsystem.mmd` |
| FEAT-011 | Command Parser | `command_subsystem.mmd` |
| FEAT-012 | Command management (CommandSystem) | `command_subsystem.mmd` |
| FEAT-013 | ISocketService | `server.mmd` |
| FEAT-014 | Socket server | `server.mmd` |
| FEAT-015 | IRCService | `server.mmd` |
| FEAT-016 | PASS command | `client_subsystem.mmd` |
| FEAT-017 | QUIT command | None |
| FEAT-018 | PING command | None |
| FEAT-019 | AuthenticatedFilter | `security_filtering.mmd` |
| FEAT-020 | NICK command | `client_subsystem.mmd` |
| FEAT-021 | USER command | `client_subsystem.mmd` |
| FEAT-022 | RegisteredFilter | `security_filtering.mmd` |
| FEAT-023 | ChainFilter | `security_filtering.mmd` |
| FEAT-024 | getCommonAccessFilter in factory | `client_subsystem.mmd` |
| FEAT-025 | JOIN command | `channel_subsystem.mmd` |
| FEAT-026 | LIST command | `channel_subsystem.mmd` |
| FEAT-027 | ChannelMemberFilter | `security_filtering.mmd` |
| FEAT-028 | getChannelCommonAccessFilter in factory | `channel_subsystem.mmd` |
| FEAT-029 | PRIVMSG command | `command_subsystem.mmd` |
| FEAT-030 | PART command | `channel_subsystem.mmd` |
| FEAT-031 | ChannelOperatorFilter | `security_filtering.mmd` |
| FEAT-032 | getChannelOperatorAccessFilter in factory | `channel_subsystem.mmd` |
| FEAT-033 | KICK command | `channel_subsystem.mmd` |
| FEAT-034 | MODE command | `channel_subsystem.mmd` |
| FEAT-035 | TOPIC command | `channel_subsystem.mmd` |
| FEAT-036 | INVITE command | `channel_subsystem.mmd` |

---

## 2. Tasks Table

| ID | Description | TDD Step | Feature ID | Related Elements |
|----|-------------|----------|------------|------------------|
| T-001 | Tests BaseException | — | FEAT-001 | None |
| T-002 | Implement BaseException | — | FEAT-001 | None |
| T-003 | Refactor BaseException | — | FEAT-001 | None |
| T-004 | Tests Client and ClientBuffer | — | FEAT-002 | None |
| T-005 | Implement Client and ClientBuffer | — | FEAT-002 | None |
| T-006 | Refactor Client and ClientBuffer | — | FEAT-002 | None |
| T-007 | Tests ClientRegistry | — | FEAT-002 | None |
| T-008 | Implement ClientRegistry | — | FEAT-002 | None |
| T-009 | Refactor ClientRegistry | — | FEAT-002 | None |
| T-010 | Tests AuthenticationManager | — | FEAT-003 | None |
| T-011 | Implement AuthenticationManager | — | FEAT-003 | None |
| T-012 | Refactor AuthenticationManager | — | FEAT-003 | None |
| T-013 | Tests NickManager | — | FEAT-004 | None |
| T-014 | Implement NickManager | — | FEAT-004 | None |
| T-015 | Refactor NickManager | — | FEAT-004 | None |
| T-016 | Tests RegistrationManager | — | FEAT-004 | None |
| T-017 | Implement RegistrationManager | — | FEAT-004 | None |
| T-018 | Refactor RegistrationManager | — | FEAT-004 | None |
| T-179 | Tests PendingIOManager | — | FEAT-039 | None |
| T-180 | Implement PendingIOManager | — | FEAT-039 | None |
| T-181 | Refactor PendingIOManager | — | FEAT-039 | None |
| T-019 | Tests Channel | — | FEAT-005 | None |
| T-020 | Implement Channel | — | FEAT-005 | None |
| T-021 | Refactor Channel | — | FEAT-005 | None |
| T-022 | Tests ChannelRegistry | — | FEAT-005 | None |
| T-023 | Implement ChannelRegistry | — | FEAT-005 | None |
| T-024 | Refactor ChannelRegistry | — | FEAT-005 | None |
| T-185 | Tests ChannelMembershipManager | — | FEAT-041 | None |
| T-186 | Implement ChannelMembershipManager | — | FEAT-041 | None |
| T-187 | Refactor ChannelMembershipManager | — | FEAT-041 | None |
| T-188 | Tests ChannelSettingsManager | — | FEAT-041 | None |
| T-189 | Implement ChannelSettingsManager | — | FEAT-041 | None |
| T-190 | Refactor ChannelSettingsManager | — | FEAT-041 | None |
| T-191 | Tests ChannelBroadcastManager | — | FEAT-042 | None |
| T-192 | Implement ChannelBroadcastManager | — | FEAT-042 | None |
| T-193 | Refactor ChannelBroadcastManager | — | FEAT-042 | None |
| T-025 | Tests ChannelSystem | — | FEAT-006 | None |
| T-026 | Implement ChannelSystem and ChannelDetails struct | — | FEAT-006 | None |
| T-027 | Refactor ChannelSystem | — | FEAT-006 | None |
| T-182 | Tests ClientLifecycleManager | — | FEAT-040 | None |
| T-183 | Implement ClientLifecycleManager | — | FEAT-040 | None |
| T-184 | Refactor ClientLifecycleManager | — | FEAT-040 | None |
| T-028 | Tests ClientSystem | — | FEAT-007 | None |
| T-029 | Implement ClientSystem and ClientDetails struct | — | FEAT-007 | None |
| T-030 | Refactor ClientSystem | — | FEAT-007 | None |
| T-031 | Implement IRCDto struct | — | FEAT-008 | None |
| T-032 | Implement ISecurityFilter | — | FEAT-009 | None |
| T-033 | Tests SecurityFilterFactory(throws) | — | FEAT-009 | None |
| T-034 | Implement SecurityFilterFactory | — | FEAT-009 | None |
| T-035 | Refactor SecurityFilterFactory | — | FEAT-009 | None |
| T-036 | Implement ICommand | — | FEAT-010 | None |
| T-037 | Tests CommandFactory(throws) | — | FEAT-010 | None |
| T-038 | Implement CommandFactory | — | FEAT-010 | None |
| T-039 | Refactor CommandFactory | — | FEAT-010 | None |
| T-040 | Tests CommandParser | — | FEAT-011 | None |
| T-041 | Implement CommandParser | — | FEAT-011 | None |
| T-042 | Refactor CommandParser | — | FEAT-011 | None |
| T-043 | Tests Command System | Red | FEAT-012 | `command_subsystem.mmd` |
| T-044 | Implement Command System | Green | FEAT-012 | `command_subsystem.mmd` |
| T-045 | Refactor Command System | Refactor | FEAT-012 | `command_subsystem.mmd` |
| T-046 | Tests TickResult | Red | FEAT-013 | `server.mmd` |
| T-047 | Implement TickResult | Green | FEAT-013 | `server.mmd` |
| T-048 | Refactor TickResult | Refactor | FEAT-013 | `server.mmd` |
| T-049 | Tests ISocketService | Red | FEAT-013 | `server.mmd` |
| T-050 | Implement ISocketService | Green | FEAT-013 | `server.mmd` |
| T-051 | Refactor ISocketService | Refactor | FEAT-013 | `server.mmd` |
| T-052 | Tests SocketServer | Red | FEAT-014 | `server.mmd` |
| T-053 | Implement SocketServer | Green | FEAT-014 | `server.mmd` |
| T-054 | Refactor SocketServer | Refactor | FEAT-014 | `server.mmd` |
| T-055 | Tests IRCService | Red | FEAT-015 | `server.mmd` |
| T-056 | Implement IRCService | Green | FEAT-015 | `server.mmd` |
| T-057 | Refactor IRCService | Refactor | FEAT-015 | `server.mmd` |
| T-058 | Tests PASS command | Red | FEAT-016 | `client_subsystem.mmd` |
| T-059 | Implement PASS command | Green | FEAT-016 | `client_subsystem.mmd` |
| T-060 | Refactor PASS command | Refactor | FEAT-016 | `client_subsystem.mmd` |
| T-061 | Tests PASS command in CommandFactory | Red | FEAT-016 | `command_subsystem.mmd` |
| T-062 | Implement PASS command in CommandFactory | Green | FEAT-016 | `command_subsystem.mmd` |
| T-063 | Refactor PASS command in CommandFactory | Refactor | FEAT-016 | `command_subsystem.mmd` |
| T-064 | Tests QUIT command | Red | FEAT-017 | None |
| T-065 | Implement QUIT command | Green | FEAT-017 | None |
| T-066 | Refactor QUIT command | Refactor | FEAT-017 | None |
| T-067 | Tests QUIT command in CommandFactory | Red | FEAT-017 | `command_subsystem.mmd` |
| T-068 | Implement QUIT command in CommandFactory | Green | FEAT-017 | `command_subsystem.mmd` |
| T-069 | Refactor QUIT command in CommandFactory | Refactor | FEAT-017 | `command_subsystem.mmd` |
| T-070 | Tests PING command | Red | FEAT-018 | None |
| T-071 | Implement PING command | Green | FEAT-018 | None |
| T-072 | Refactor PING command | Refactor | FEAT-018 | None |
| T-073 | Tests PING command in CommandFactory | Red | FEAT-018 | `command_subsystem.mmd` |
| T-074 | Implement PING command in CommandFactory | Green | FEAT-018 | `command_subsystem.mmd` |
| T-075 | Refactor PING command in CommandFactory | Refactor | FEAT-018 | `command_subsystem.mmd` |
| T-076 | Tests AuthenticatedFilter | Red | FEAT-019 | `client_subsystem.mmd` |
| T-077 | Implement AuthenticatedFilter | Green | FEAT-019 | `client_subsystem.mmd` |
| T-078 | Refactor AuthenticatedFilter | Refactor | FEAT-019 | `client_subsystem.mmd` |
| T-079 | Tests AuthenticatedFilter in SecurityFilterFactory | Red | FEAT-019 | `client_subsystem.mmd` |
| T-080 | Implement AuthenticatedFilter in SecurityFilterFactory | Green | FEAT-019 | `client_subsystem.mmd` |
| T-081 | Refactor AuthenticatedFilter in SecurityFilterFactory | Refactor | FEAT-019 | `client_subsystem.mmd` |
| T-082 | Tests NICK command | Red | FEAT-020 | `client_subsystem.mmd` |
| T-083 | Implement NICK command | Green | FEAT-020 | `client_subsystem.mmd` |
| T-084 | Refactor NICK command | Refactor | FEAT-020 | `client_subsystem.mmd` |
| T-085 | Tests NICK command in CommandFactory | Red | FEAT-020 | `command_subsystem.mmd` |
| T-086 | Implement NICK command in CommandFactory | Green | FEAT-020 | `command_subsystem.mmd` |
| T-087 | Refactor NICK command in CommandFactory | Refactor | FEAT-020 | `command_subsystem.mmd` |
| T-088 | Tests USER command | Red | FEAT-021 | `client_subsystem.mmd` |
| T-089 | Implement USER command | Green | FEAT-021 | `client_subsystem.mmd` |
| T-090 | Refactor USER command | Refactor | FEAT-021 | `client_subsystem.mmd` |
| T-091 | Tests USER command in CommandFactory | Red | FEAT-021 | `command_subsystem.mmd` |
| T-092 | Implement USER command in CommandFactory | Green | FEAT-021 | `command_subsystem.mmd` |
| T-093 | Refactor USER command in CommandFactory | Refactor | FEAT-021 | `command_subsystem.mmd` |
| T-094 | Tests RegisteredFilter | Red | FEAT-022 | `client_subsystem.mmd` |
| T-095 | Implement RegisteredFilter | Green | FEAT-022 | `client_subsystem.mmd` |
| T-096 | Refactor RegisteredFilter | Refactor | FEAT-022 | `client_subsystem.mmd` |
| T-097 | Tests RegisteredFilter in SecurityFilterFactory | Red | FEAT-022 | `client_subsystem.mmd` |
| T-098 | Implement RegisteredFilter in SecurityFilterFactory | Green | FEAT-022 | `client_subsystem.mmd` |
| T-099 | Refactor RegisteredFilter in SecurityFilterFactory | Refactor | FEAT-022 | `client_subsystem.mmd` |
| T-100 | Tests ChainFilter | Red | FEAT-023 | `client_subsystem.mmd` |
| T-101 | Implement ChainFilter | Green | FEAT-023 | `client_subsystem.mmd` |
| T-102 | Refactor ChainFilter | Refactor | FEAT-023 | `client_subsystem.mmd` |
| T-103 | Tests ChainFilter in SecurityFilterFactory | Red | FEAT-023 | `client_subsystem.mmd` |
| T-104 | Implement ChainFilter in SecurityFilterFactory | Green | FEAT-023 | `client_subsystem.mmd` |
| T-105 | Refactor ChainFilter in SecurityFilterFactory | Refactor | FEAT-023 | `client_subsystem.mmd` |
| T-106 | Tests getCommonAccessFilter in SecurityFilterFactory | Red | FEAT-024 | `client_subsystem.mmd` |
| T-107 | Implement getCommonAccessFilter in SecurityFilterFactory | Green | FEAT-024 | `client_subsystem.mmd` |
| T-108 | Refactor getCommonAccessFilter in SecurityFilterFactory | Refactor | FEAT-024 | `client_subsystem.mmd` |
| T-109 | Tests JOIN command | Red | FEAT-025 | `channel_subsystem.mmd` |
| T-110 | Implement JOIN command | Green | FEAT-025 | `channel_subsystem.mmd` |
| T-111 | Refactor JOIN command | Refactor | FEAT-025 | `channel_subsystem.mmd` |
| T-112 | Tests JOIN command in CommandFactory | Red | FEAT-025 | `command_subsystem.mmd` |
| T-113 | Implement JOIN command in CommandFactory | Green | FEAT-025 | `command_subsystem.mmd` |
| T-114 | Refactor JOIN command in CommandFactory | Refactor | FEAT-025 | `command_subsystem.mmd` |
| T-115 | Tests LIST command | Red | FEAT-026 | `channel_subsystem.mmd` |
| T-116 | Implement LIST command | Green | FEAT-026 | `channel_subsystem.mmd` |
| T-117 | Refactor LIST command | Refactor | FEAT-026 | `channel_subsystem.mmd` |
| T-118 | Tests LIST command in CommandFactory | Red | FEAT-026 | `command_subsystem.mmd` |
| T-119 | Implement LIST command in CommandFactory | Green | FEAT-026 | `command_subsystem.mmd` |
| T-120 | Refactor LIST command in CommandFactory | Refactor | FEAT-026 | `command_subsystem.mmd` |
| T-121 | Tests ChannelMemberFilter | Red | FEAT-027 | `channel_subsystem.mmd` |
| T-122 | Implement ChannelMemberFilter | Green | FEAT-027 | `channel_subsystem.mmd` |
| T-123 | Refactor ChannelMemberFilter | Refactor | FEAT-027 | `channel_subsystem.mmd` |
| T-124 | Tests ChannelMemberFilter in SecurityFilterFactory | Red | FEAT-027 | `channel_subsystem.mmd` |
| T-125 | Implement ChannelMemberFilter in SecurityFilterFactory | Green | FEAT-027 | `channel_subsystem.mmd` |
| T-126 | Refactor ChannelMemberFilter in SecurityFilterFactory | Refactor | FEAT-027 | `channel_subsystem.mmd` |
| T-127 | Tests getChannelCommonAccessFilter in SecurityFilterFactory | Red | FEAT-028 | `channel_subsystem.mmd` |
| T-128 | Implement getChannelCommonAccessFilter in SecurityFilterFactory | Green | FEAT-028 | `channel_subsystem.mmd` |
| T-129 | Refactor getChannelCommonAccessFilter in SecurityFilterFactory | Refactor | FEAT-028 | `channel_subsystem.mmd` |
| T-130 | Tests PRIVMSG command | Red | FEAT-029 | `command_subsystem.mmd` |
| T-131 | Implement PRIVMSG command | Green | FEAT-029 | `command_subsystem.mmd` |
| T-132 | Refactor PRIVMSG command | Refactor | FEAT-029 | `command_subsystem.mmd` |
| T-133 | Tests PRIVMSG command in CommandFactory | Red | FEAT-029 | `command_subsystem.mmd` |
| T-134 | Implement PRIVMSG command in CommandFactory | Green | FEAT-029 | `command_subsystem.mmd` |
| T-135 | Refactor PRIVMSG command in CommandFactory | Refactor | FEAT-029 | `command_subsystem.mmd` |
| T-136 | Tests PART command | Red | FEAT-030 | `channel_subsystem.mmd` |
| T-137 | Implement PART command | Green | FEAT-030 | `channel_subsystem.mmd` |
| T-138 | Refactor PART command | Refactor | FEAT-030 | `channel_subsystem.mmd` |
| T-139 | Tests PART command in CommandFactory | Red | FEAT-030 | `command_subsystem.mmd` |
| T-140 | Implement PART command in CommandFactory | Green | FEAT-030 | `command_subsystem.mmd` |
| T-141 | Refactor PART command in CommandFactory | Refactor | FEAT-030 | `command_subsystem.mmd` |
| T-142 | Tests ChannelOperatorFilter | Red | FEAT-031 | `channel_subsystem.mmd` |
| T-143 | Implement ChannelOperatorFilter | Green | FEAT-031 | `channel_subsystem.mmd` |
| T-144 | Refactor ChannelOperatorFilter | Refactor | FEAT-031 | `channel_subsystem.mmd` |
| T-145 | Tests ChannelOperatorFilter in SecurityFilterFactory | Red | FEAT-031 | `channel_subsystem.mmd` |
| T-146 | Implement ChannelOperatorFilter in SecurityFilterFactory | Green | FEAT-031 | `channel_subsystem.mmd` |
| T-147 | Refactor ChannelOperatorFilter in SecurityFilterFactory | Refactor | FEAT-031 | `channel_subsystem.mmd` |
| T-148 | Tests getChannelOperatorAccessFilter in SecurityFilterFactory | Red | FEAT-032 | `channel_subsystem.mmd` |
| T-149 | Implement getChannelOperatorAccessFilter in SecurityFilterFactory | Green | FEAT-032 | `channel_subsystem.mmd` |
| T-150 | Refactor getChannelOperatorAccessFilter in SecurityFilterFactory | Refactor | FEAT-032 | `channel_subsystem.mmd` |
| T-151 | Tests KICK command | Red | FEAT-033 | `channel_subsystem.mmd` |
| T-152 | Implement KICK command | Green | FEAT-033 | `channel_subsystem.mmd` |
| T-153 | Refactor KICK command | Refactor | FEAT-033 | `channel_subsystem.mmd` |
| T-154 | Tests KICK command in CommandFactory | Red | FEAT-033 | `command_subsystem.mmd` |
| T-155 | Implement KICK command in CommandFactory | Green | FEAT-033 | `command_subsystem.mmd` |
| T-156 | Refactor KICK command in CommandFactory | Refactor | FEAT-033 | `command_subsystem.mmd` |
| T-157 | Tests MODE command | Red | FEAT-034 | `channel_subsystem.mmd` |
| T-158 | Implement MODE command | Green | FEAT-034 | `channel_subsystem.mmd` |
| T-159 | Refactor MODE command | Refactor | FEAT-034 | `channel_subsystem.mmd` |
| T-160 | Tests MODE command in CommandFactory | Red | FEAT-034 | `command_subsystem.mmd` |
| T-161 | Implement MODE command in CommandFactory | Green | FEAT-034 | `command_subsystem.mmd` |
| T-162 | Refactor MODE command in CommandFactory | Refactor | FEAT-034 | `command_subsystem.mmd` |
| T-163 | Tests TOPIC command | Red | FEAT-035 | `channel_subsystem.mmd` |
| T-164 | Implement TOPIC command | Green | FEAT-035 | `channel_subsystem.mmd` |
| T-165 | Refactor TOPIC command | Refactor | FEAT-035 | `channel_subsystem.mmd` |
| T-166 | Tests TOPIC command in CommandFactory | Red | FEAT-035 | `command_subsystem.mmd` |
| T-167 | Implement TOPIC command in CommandFactory | Green | FEAT-035 | `command_subsystem.mmd` |
| T-168 | Refactor TOPIC command in CommandFactory | Refactor | FEAT-035 | `command_subsystem.mmd` |
| T-169 | Tests INVITE command | Red | FEAT-036 | `channel_subsystem.mmd` |
| T-170 | Implement INVITE command | Green | FEAT-036 | `channel_subsystem.mmd` |
| T-171 | Refactor INVITE command | Refactor | FEAT-036 | `channel_subsystem.mmd` |
| T-172 | Tests INVITE command in CommandFactory | Red | FEAT-036 | `command_subsystem.mmd` |
| T-173 | Implement INVITE command in CommandFactory | Green | FEAT-036 | `command_subsystem.mmd` |
| T-174 | Refactor INVITE command in CommandFactory | Refactor | FEAT-036 | `command_subsystem.mmd` |

---

## 3. Acceptance Criteria Table

| ID | Description | Associated Element |
|----|-------------|--------------------|
| AC-001 | BaseException exposes the IRC numeric/error code, a stable message, and any context required to build an IRC error reply. | FEAT-001 |
| AC-002 | Every concrete business exception derives from BaseException and maps to exactly one documented IRC error condition. | FEAT-001 |
| AC-003 | Business exceptions preserve actionable context (such as nickname, channel, command, or parameter) without depending on transport-layer state. | FEAT-001 |
| AC-004 | Client stores and exposes its connection identity, registration/authentication state, nickname, username, real name, and connection metadata without inconsistent duplicate state. | FEAT-002 |
| AC-005 | Client input and output buffers preserve byte order, support partial reads/writes, and retain unsent or unprocessed bytes between event-loop iterations. | FEAT-002 |
| AC-006 | ClientRegistry indexes each live client by file descriptor and supports lookup by fd without returning stale or disconnected clients. | FEAT-002 |
| AC-007 | Registering, retrieving, and removing clients keeps the registry consistent and prevents duplicate fd ownership. | FEAT-002 |
| AC-008 | Operations on unknown, already-registered, or already-removed clients raise the corresponding typed business exception. | FEAT-002 |
| AC-009 | AuthenticationManager validates the configured server password using the supplied PASS value and does not authenticate a client when the password is absent or incorrect. | FEAT-003 |
| AC-010 | Authentication state is tracked independently for each client and changes to authenticated only after successful validation. | FEAT-003 |
| AC-011 | Repeated PASS attempts do not corrupt authentication state, and successful authentication is not revoked by an unrelated client's attempt. | FEAT-003 |
| AC-012 | Missing, malformed, or incorrect credentials raise the appropriate business exception so the command layer can return the corresponding IRC error. | FEAT-003 |
| AC-013 | NickManager validates nickname syntax and length according to the server's supported IRC nickname rules. | FEAT-004 |
| AC-014 | NickManager reserves nicknames case-insensitively and prevents two connected clients from owning equivalent nicknames. | FEAT-004 |
| AC-015 | Nickname changes release the previous nickname and reserve the new nickname atomically, without leaving stale reservations. | FEAT-004 |
| AC-016 | RegistrationManager stores the USER registration fields and determines whether the required registration fields have been supplied. | FEAT-004 |
| AC-017 | Registration completion is derived consistently from the required authentication, nickname, and USER state; partial registration remains pending. | FEAT-004 |
| AC-018 | Invalid, missing, or already-in-use nicknames and invalid registration transitions raise the appropriate business exceptions. | FEAT-004 |
| AC-019 | Channel stores its name, topic, modes, key/limit configuration, and membership/operator state required by supported IRC commands. | FEAT-005 |
| AC-020 | Channel membership supports adding and removing clients while maintaining a single consistent membership relationship. | FEAT-005 |
| AC-021 | Channel membership distinguishes ordinary members from channel operators and exposes membership/operator checks. | FEAT-005 |
| AC-022 | ChannelRegistry identifies channels by their IRC case-insensitive channel name and supports lookup by name. | FEAT-005 |
| AC-023 | ChannelRegistry creates a channel when required, reuses an existing channel, and removes it when its final member leaves, according to server lifecycle rules. | FEAT-005 |
| AC-024 | Duplicate creation, invalid channel names, missing channels, and inconsistent membership operations raise the corresponding business exceptions. | FEAT-005 |
| AC-025 | ChannelSystem validates channel names and coordinates channel creation, joining, leaving, and membership updates through ChannelRegistry. | FEAT-006 |
| AC-026 | A client can join an existing channel or cause a valid channel to be created, and receives the channel's current membership/topic state as required by the protocol. | FEAT-006 |
| AC-027 | A client leaving a channel is removed from membership, and an empty channel is destroyed when the server's lifecycle rules require it. | FEAT-006 |
| AC-028 | Channel messages are delivered exactly once to all current channel members except the sender, without being sent to non-members. | FEAT-006 |
| AC-029 | Supported channel modes are parsed, validated, stored, and enforced consistently by channel operations. | FEAT-006 |
| AC-030 | Invalid channel names, attempts to join inaccessible channels, duplicate membership, or attempts to leave a channel the client has not joined raise appropriate business exceptions. | FEAT-006 |
| AC-031 | ClientSystem tracks connected, pending-registration, and quitting clients without placing a client in conflicting lifecycle states. | FEAT-007 |
| AC-032 | ClientSystem provides consistent access to client records by the supported identifiers and delegates ownership/index consistency to ClientRegistry. | FEAT-007 |
| AC-033 | Disconnecting a client removes it from active management and initiates cleanup of nickname and channel memberships. | FEAT-007 |
| AC-034 | Repeated disconnect or lifecycle transitions are handled idempotently or rejected with a documented business exception, without corrupting state. | FEAT-007 |
| AC-035 | Requests involving unknown clients or invalid client lifecycle transitions raise the appropriate business exception. | FEAT-007 |
| AC-036 | IRCDto represents a parsed IRC message with command, ordered middle parameters, and an optional trailing parameter while preserving parameter contents. | FEAT-008 |
| AC-037 | IRCDto distinguishes absent parameters from empty values and supports the maximum parameter count defined by the parser contract. | FEAT-008 |
| AC-038 | ISecurityFilter defines a consistent contract for evaluating whether a client may execute a command in the current server state. | FEAT-009 |
| AC-039 | SecurityFilterFactory constructs individual filters and composes multiple filters into a deterministic chain. | FEAT-009 |
| AC-040 | Filter-chain evaluation stops at the first rejection and allows the command only when every configured filter accepts it. | FEAT-009 |
| AC-041 | Invalid filter configuration or an attempted filter evaluation with an invalid client/context raises the appropriate business exception. | FEAT-009 |
| AC-042 | ICommand defines a consistent execution contract receiving the parsed IRC message and the client/server context required by command handlers. | FEAT-010 |
| AC-043 | CommandFactory resolves supported command names case-insensitively and creates the corresponding command implementation. | FEAT-010 |
| AC-044 | Unknown or unsupported command names are represented by the appropriate business exception or protocol-level unknown-command result, never by a null command or undefined behavior. | FEAT-010 |
| AC-045 | Invalid command construction arguments or inconsistent factory registration raise an appropriate business exception. | FEAT-010 |
| AC-046 | CommandParser converts a raw IRC input line into a command name, ordered middle parameters, and an optional trailing parameter. | FEAT-011 |
| AC-047 | Parsing preserves spaces inside the trailing parameter, removes only protocol delimiters, and handles CRLF framing according to the network input contract. | FEAT-011 |
| AC-048 | Parser rejects malformed lines, empty command names, invalid parameter layouts, and messages exceeding IRC protocol length/parameter limits. | FEAT-011 |
| AC-049 | Malformed or incomplete input raises a typed parsing/business exception that lets the caller decide whether to send an IRC error or close the connection. | FEAT-011 |
| AC-050 | CommandSystem obtains complete IRC messages from the input buffer, parses them, resolves commands through CommandFactory, and dispatches them in input order. | FEAT-012 |
| AC-051 | CommandSystem applies the configured security-filter chain before executing a command and does not execute rejected commands. | FEAT-012 |
| AC-052 | Multiple complete messages in one input buffer are dispatched independently, while incomplete trailing data remains buffered for the next read. | FEAT-012 |
| AC-053 | Command execution failures are isolated to the relevant message/client and are converted into the appropriate IRC error response without terminating the whole server loop. | FEAT-012 |
| AC-054 | Parser, command lookup, authorization, and command execution failures propagate as the appropriate typed business exceptions to the protocol error-handling boundary. | FEAT-012 |
| AC-055 | TickResult carries outbound bytes/messages and the socket descriptors scheduled for closure without conflating the two result categories. | FEAT-013 |
| AC-056 | ISocketService defines the event-loop tick contract and exposes the inputs/outputs needed by the server without prescribing a concrete socket implementation. | FEAT-013 |
| AC-057 | SocketServer binds and listens on the configured address and port, and reports startup failures without leaving partially initialized sockets. | FEAT-014 |
| AC-058 | New connections are accepted non-blockingly and each accepted connection is assigned a valid descriptor and initialized client state. | FEAT-014 |
| AC-059 | A single poll-based event loop monitors listening, readable, and writable sockets without blocking on an individual client. | FEAT-014 |
| AC-060 | Readable events append received bytes to the corresponding client input buffer; writable events send queued bytes and preserve unsent data after partial writes. | FEAT-014 |
| AC-061 | Hangups, peer disconnects, and socket errors close the affected connection and trigger client cleanup without stopping the server. | FEAT-014 |
| AC-062 | Socket setup, accept, poll, read, and write failures are surfaced as appropriate typed business/system exceptions and handled without corrupting other clients. | FEAT-014 |
| AC-063 | IRCService implements the ISocketService tick contract and coordinates socket events with ClientSystem and CommandSystem. | FEAT-015 |
| AC-064 | Each tick processes newly accepted connections, readable client data, pending IRC commands, queued output, and requested socket closures in a deterministic order. | FEAT-015 |
| AC-065 | IRCService queues protocol replies for the correct client and does not perform blocking writes or lose data after partial writes. | FEAT-015 |
| AC-066 | Disconnects and command failures are isolated to the affected client while the service continues processing other clients. | FEAT-015 |
| AC-067 | Invalid tick state, unknown client references, and unrecoverable service-level failures raise appropriate typed business exceptions. | FEAT-015 |
| AC-068 | PASS accepts the server password only with the required parameter and records successful authentication for the issuing connection. | FEAT-016 |
| AC-069 | PASS rejects missing or incorrect passwords and does not mark the client authenticated on failure. | FEAT-016 |
| AC-070 | PASS is processed before registration completion and is not accepted as a way to change credentials after registration has completed. | FEAT-016 |
| AC-071 | The server sends the appropriate numeric error for missing parameters or an invalid password and does not expose the configured password in replies or logs. | FEAT-016 |
| AC-072 | PASS validation and authentication-state failures raise the appropriate business exceptions for translation into IRC numeric replies. | FEAT-016 |
| AC-073 | QUIT terminates only the issuing client's connection and accepts an optional trailing quit message. | FEAT-017 |
| AC-074 | QUIT removes the client from all joined channels, releases its nickname, removes it from client registries, and schedules its socket for closure. | FEAT-017 |
| AC-075 | QUIT broadcasts one correctly formatted QUIT message to the client's former channel peers before membership cleanup makes that information unavailable. | FEAT-017 |
| AC-076 | QUIT cleanup is safe when invoked more than once or when the client has already disconnected. | FEAT-017 |
| AC-077 | Unknown-client or invalid lifecycle conditions encountered during QUIT raise the appropriate business exception and do not corrupt other clients' state. | FEAT-017 |
| AC-078 | PING requires a token and returns a PONG containing the same token and the correct server prefix/target information. | FEAT-018 |
| AC-079 | PING can be processed for a connected client without changing its registration, nickname, or channel state. | FEAT-018 |
| AC-080 | PING with missing or malformed parameters produces the appropriate IRC numeric error rather than silently succeeding. | FEAT-018 |
| AC-081 | Invalid client context or invalid PING state raises the appropriate business exception for protocol-level error handling. | FEAT-018 |
| AC-082 | AuthenticatedFilter allows commands only when the issuing client has successfully authenticated with the configured server password. | FEAT-019 |
| AC-083 | AuthenticatedFilter rejects unauthenticated clients for commands that require authentication while permitting explicitly allowed registration/authentication commands. | FEAT-019 |
| AC-084 | Filter decisions are based on the current client state and cannot be bypassed by stale cached authentication data. | FEAT-019 |
| AC-085 | Invalid client references or inconsistent authentication state raise the appropriate business exception rather than being treated as authenticated. | FEAT-019 |
| AC-086 | NICK sets a valid nickname for the issuing client and broadcasts the correctly formatted nickname change to the client and relevant channel peers. | FEAT-020 |
| AC-087 | NICK enforces nickname syntax, length, and case-insensitive uniqueness across connected clients. | FEAT-020 |
| AC-088 | Changing a nickname updates Client, NickManager, ClientRegistry, and any nickname-based lookup consistently and atomically. | FEAT-020 |
| AC-089 | NICK supports the server's defined registration behavior for clients that have not yet completed registration. | FEAT-020 |
| AC-090 | Missing, invalid, or already-in-use nicknames raise the corresponding business exceptions and produce the appropriate IRC numeric replies. | FEAT-020 |
| AC-091 | USER accepts the required username, mode, unused parameter, and trailing real name in the protocol-defined parameter positions. | FEAT-021 |
| AC-092 | USER stores the username and real name on the issuing client and contributes to registration completion without overwriting unrelated registration state. | FEAT-021 |
| AC-093 | USER rejects missing parameters and disallows changes after registration when required by the server's registration policy. | FEAT-021 |
| AC-094 | Successful USER processing triggers registration completion exactly once when all required registration conditions are satisfied. | FEAT-021 |
| AC-095 | Missing parameters, duplicate registration, or invalid registration state raise the appropriate business exceptions and map to the corresponding IRC numeric replies. | FEAT-021 |
| AC-096 | RegisteredFilter allows commands only when the client has completed the server's full registration sequence. | FEAT-022 |
| AC-097 | RegisteredFilter rejects commands from partially registered clients while allowing the commands explicitly permitted before registration. | FEAT-022 |
| AC-098 | Registration status is evaluated from the authoritative current client state. | FEAT-022 |
| AC-099 | Invalid client references or impossible registration states raise the appropriate business exception instead of allowing the command. | FEAT-022 |
| AC-100 | ChainFilter evaluates configured filters in their declared order and stops immediately when a filter rejects the request. | FEAT-023 |
| AC-101 | ChainFilter allows execution only when every filter in the chain accepts the same client and command context. | FEAT-023 |
| AC-102 | An empty chain follows a documented deterministic policy and does not produce undefined behavior. | FEAT-023 |
| AC-103 | Exceptions raised by a filter are propagated to the command error boundary and are not converted into an accidental allow decision. | FEAT-023 |
| AC-104 | Invalid filter entries or invalid evaluation context raise the appropriate business exception. | FEAT-023 |
| AC-105 | getCommonAccessFilter returns a chain containing the common authentication and registration checks in the documented order. | FEAT-024 |
| AC-106 | The factory returns an equivalent configured chain for repeated calls and does not share mutable per-client evaluation state between clients. | FEAT-024 |
| AC-107 | Every filter returned by the factory is applicable to the command/client context supported by the security-filter contract. | FEAT-024 |
| AC-108 | Missing dependencies or invalid common-filter configuration raise an appropriate business exception instead of returning a partially configured chain. | FEAT-024 |
| AC-109 | JOIN accepts one or more valid channel names and joins the issuing client to each channel according to the server's supported parameter rules. | FEAT-025 |
| AC-110 | JOIN creates a channel when it does not exist and adds the client exactly once when it does exist. | FEAT-025 |
| AC-111 | JOIN enforces channel key, invite-only, user-limit, and other implemented access modes before changing membership. | FEAT-025 |
| AC-112 | After a successful join, the server broadcasts JOIN to channel members and sends the joining client the channel topic and names list. | FEAT-025 |
| AC-113 | JOIN rejects invalid channel names, duplicate/ineligible membership, incorrect keys, invite restrictions, and full channels with the corresponding business exceptions and IRC numeric replies. | FEAT-025 |
| AC-114 | LIST returns the channels visible to the requesting client and includes each channel's name, user count, and topic. | FEAT-026 |
| AC-115 | LIST supports the server's documented optional channel-list parameter and does not disclose channels hidden by applicable visibility rules. | FEAT-026 |
| AC-116 | LIST emits the appropriate start, per-channel, and end numeric replies in protocol order, including an empty result when no channels match. | FEAT-026 |
| AC-117 | Invalid channel-list parameters or invalid client context raise the appropriate business exception and produce a protocol-level error where applicable. | FEAT-026 |
| AC-118 | ChannelMemberFilter allows a command only when the issuing client is a member of the target channel. | FEAT-027 |
| AC-119 | ChannelMemberFilter rejects non-members for member-only operations and identifies the target channel consistently from the command context. | FEAT-027 |
| AC-120 | Commands explicitly permitted for non-members are not blocked by this filter. | FEAT-027 |
| AC-121 | Missing channels, unknown clients, or invalid command/channel context raise the appropriate business exception rather than granting access. | FEAT-027 |
| AC-122 | getChannelCommonAccessFilter returns the documented channel-level access chain and includes the channel-membership check in the correct order. | FEAT-028 |
| AC-123 | The returned chain evaluates the target channel and issuing client from the current command context. | FEAT-028 |
| AC-124 | Repeated factory calls do not leak mutable state or alter the behavior of previously created chains. | FEAT-028 |
| AC-125 | Missing filter dependencies or invalid channel-filter configuration raise an appropriate business exception. | FEAT-028 |
| AC-126 | PRIVMSG delivers a message addressed to a nickname to exactly that connected recipient and does not echo it to unrelated clients. | FEAT-029 |
| AC-127 | PRIVMSG delivers a message addressed to a channel to all eligible channel members except the sender. | FEAT-029 |
| AC-128 | PRIVMSG preserves the trailing message text, including embedded spaces, and uses the correct IRC prefix and target in the forwarded message. | FEAT-029 |
| AC-129 | PRIVMSG rejects missing recipient/text, unknown nicknames/channels, and attempts to message a channel without required membership with the corresponding IRC numeric errors. | FEAT-029 |
| AC-130 | Invalid targets, empty message text, and unauthorized channel messaging raise the appropriate typed business exceptions. | FEAT-029 |
| AC-131 | PART removes the issuing client from each specified channel and accepts an optional trailing part message. | FEAT-030 |
| AC-132 | PART broadcasts the correctly formatted PART message to channel members before removing the client from the channel. | FEAT-030 |
| AC-133 | PART supports multiple channel targets according to the server's parameter rules and does not alter membership in unrelated channels. | FEAT-030 |
| AC-134 | PART rejects unknown channels and clients that are not members with the corresponding IRC numeric replies. | FEAT-030 |
| AC-135 | Invalid channel parameters or inconsistent membership state raise the appropriate business exceptions. | FEAT-030 |
| AC-136 | ChannelOperatorFilter allows operator-only operations only when the issuing client is an operator of the target channel. | FEAT-031 |
| AC-137 | ChannelOperatorFilter rejects ordinary members and non-members for operator-only operations. | FEAT-031 |
| AC-138 | Operator status is read from authoritative channel membership state and takes effect immediately after promotion or demotion. | FEAT-031 |
| AC-139 | Missing channels, unknown clients, or invalid operator-check context raise the appropriate business exception. | FEAT-031 |
| AC-140 | getChannelOperatorAccessFilter returns a channel access chain that enforces channel membership and operator privileges in the documented order. | FEAT-032 |
| AC-141 | The returned chain denies operator-only operations to clients without operator status and permits authorized channel operators. | FEAT-032 |
| AC-142 | Factory-created chains do not retain client-specific authorization state between command executions. | FEAT-032 |
| AC-143 | Missing dependencies or invalid operator-filter configuration raise an appropriate business exception. | FEAT-032 |
| AC-144 | KICK removes the specified target client from the specified channel and accepts an optional trailing reason. | FEAT-033 |
| AC-145 | KICK broadcasts the correctly formatted KICK message to channel members before membership removal. | FEAT-033 |
| AC-146 | KICK requires the issuer to be a channel operator and validates that both issuer and target belong to the channel. | FEAT-033 |
| AC-147 | KICK handles multiple targets/channels only if supported by the server's documented parameter rules and otherwise rejects unsupported parameter combinations. | FEAT-033 |
| AC-148 | KICK reports missing parameters, unknown channel/nickname, non-membership, and insufficient privileges using the corresponding IRC numeric replies and typed business exceptions. | FEAT-033 |
| AC-149 | MODE reports the current supported channel modes when queried without a mode change. | FEAT-034 |
| AC-150 | MODE correctly enables and disables the supported modes i, t, k, o, and l, including required mode parameters. | FEAT-034 |
| AC-151 | MODE enforces channel-operator privileges for mode changes and validates keys, limits, targets, and mode parameter counts. | FEAT-034 |
| AC-152 | Successful mode changes update channel state atomically and broadcast the correctly formatted MODE message to channel members. | FEAT-034 |
| AC-153 | Unsupported modes, missing/extra parameters, unknown channels/nicknames, and insufficient privileges raise the appropriate business exceptions and IRC numeric replies. | FEAT-034 |
| AC-154 | TOPIC without a topic parameter returns the current channel topic, including the correct no-topic response when none is set. | FEAT-035 |
| AC-155 | TOPIC with a trailing parameter sets or clears the channel topic according to the server's supported semantics. | FEAT-035 |
| AC-156 | When the topic-restricted mode is enabled, only channel operators may change the topic; otherwise channel members follow the server's documented policy. | FEAT-035 |
| AC-157 | Successful topic changes update channel state and broadcast the correctly formatted TOPIC message to channel members. | FEAT-035 |
| AC-158 | Unknown channels, non-members, insufficient privileges, or malformed parameters raise the appropriate business exceptions and IRC numeric replies. | FEAT-035 |
| AC-159 | INVITE records an invitation from an authorized channel operator to a target nickname for a specific invite-only channel. | FEAT-036 |
| AC-160 | INVITE sends the appropriate confirmation to the inviter and invitation notification to the invitee. | FEAT-036 |
| AC-161 | INVITE enforces issuer membership/operator rules and validates that the target exists and is not already a channel member. | FEAT-036 |
| AC-162 | An invitation permits the invited client to pass the invite-only join check and is consumed or retained according to the server's documented invitation policy. | FEAT-036 |
| AC-163 | Missing parameters, unknown nicknames/channels, existing membership, and insufficient privileges raise the corresponding business exceptions and IRC numeric replies. | FEAT-036 |
| AC-201 | ClientBuffer appends pushed data at the end and preserves byte order across any number of consecutive pushes. | FEAT-002 |
| AC-202 | peek(length) returns at most the first length bytes without modifying the buffer, and consume(length) removes exactly those bytes from the front without ever removing more than is stored. | FEAT-002 |
| AC-203 | find(needle) returns the position of the first occurrence of the needle or std::string::npos when absent, and empty(), size() and clear() always reflect the current content. | FEAT-002 |
| AC-204 | Data that arrives fragmented across several pushes is retained so that a delimiter split between two pushes can still be found afterwards. | FEAT-002 |
| AC-205 | ClientDetails exposes a snapshot of a client's fd, nick, username, realname, hostname, authenticated, registered and quitting flags, and joined channel names. | FEAT-007 |
| AC-206 | ClientDetails is a plain value type that can be copied and remains valid after the client it was built from is removed or modified. | FEAT-007 |
| AC-207 | pushInput and pushOutput append data to the target client's input or output buffer and mark that fd as having pending input or pending output. | FEAT-039 |
| AC-208 | getNextInputLine extracts exactly one complete line, removes it from the input buffer, and returns false when no complete line is available, leaving partial data untouched. | FEAT-039 |
| AC-209 | An fd is reported as having pending input when no complete line remains but buffer is not empty, and stops being reported as having pending output once its output buffer is empty. | FEAT-039 |
| AC-210 | peekOutput does not modify the output buffer, and consumeOutput removes exactly the bytes reported as sent, keeping the remaining bytes in order for the next write. | FEAT-039 |
| AC-211 | getHasPendingInput and getHasPendingOutput return consistent snapshots of the fds that currently have data to process or to send. | FEAT-039 |
| AC-212 | Operations on an unknown fd raise the corresponding typed business exception and do not create any pending state. | FEAT-039 |
| AC-213 | addClient registers a new client for the given fd in an unauthenticated, unregistered and non-quitting state, and a duplicate fd raises the corresponding typed business exception. | FEAT-040 |
| AC-214 | removeClient releases everything owned by the client, including channel memberships, reserved nickname, authentication and registration state, and pending I/O state, before removing it from the registry. | FEAT-040 |
| AC-215 | After removeClient no subsystem keeps a stale reference to the removed fd or its nickname, and the nickname becomes available again. | FEAT-040 |
| AC-216 | Removing an unknown fd raises the corresponding typed business exception and does not affect any other client. | FEAT-040 |
| AC-217 | getIsQuitting returns the fds of clients marked as quitting, and getClientsToClose returns only those quitting clients whose pending output has already been fully sent. | FEAT-040 |
| AC-218 | createChannel registers a new channel in the ChannelRegistry and adds the creating client as its first member and operator. | FEAT-041 |
| AC-219 | addMemberToChannel and removeClientFromChannel update both the channel's member set and the client's joined channel set, keeping them consistent. | FEAT-041 |
| AC-220 | removeClientFromChannel also removes the client's operator and invite status for that channel, and removes the channel from the registry when its last member leaves. | FEAT-041 |
| AC-221 | removeClientFromAllChannels removes the client from every channel it has joined and leaves no stale membership, operator or invite state behind. | FEAT-041 |
| AC-222 | isMember, isOperator and isInvited report the current state accurately, and return false for clients with no relation to the channel. | FEAT-041 |
| AC-223 | setOperator grants or revokes operator status only for clients that are members of the channel. | FEAT-041 |
| AC-224 | addInviteToChannel records an invitation and consumeInviteToChannel removes it, so an invitation can be used only once. | FEAT-041 |
| AC-225 | ChannelSettingsManager updates invite-only, topic-protected, limit, key and topic on an existing channel and does not alter members, operators or invitations. | FEAT-041 |
| AC-226 | Operations on an unknown channel, an unknown client, or a client that is already a member or not a member raise the corresponding typed business exception. | FEAT-041 |
| AC-227 | broadcast queues the given message in the output of every current member of the channel through PendingIOManager. | FEAT-042 |
| AC-228 | The client identified by the fd argument (the sender) does not receive its own message. | FEAT-042 |
| AC-229 | Broadcasting to a channel with no other members is a no-op and does not raise an error. | FEAT-042 |
| AC-230 | Broadcasting to an unknown channel raises the corresponding typed business exception and does not queue any output. | FEAT-042 |
| AC-231 | broadcast does not modify channel membership or settings. | FEAT-042 |
| AC-232 | ChannelDetails exposes a snapshot of a channel's name, topic, key, limit, invite-only flag, topic-protected flag, member fds and operator fds. | FEAT-006 |
| AC-233 | ChannelDetails is a plain value type that can be copied and remains valid after the channel it was built from is modified or removed. | FEAT-006 |

---

## 4. Detailed Breakdown by Feature

### FEAT-001 — BaseException and Business exceptions (one per IRC error code)

**Related Elements:** None  
**Tasks:** T-001 to T-003  
**Acceptance Criteria:** AC-001 to AC-003  
**Depends on:** None

### FEAT-002 — Client entity and ClientRegistry

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-004 to T-009  
**Acceptance Criteria:** AC-004 to AC-204  
**Depends on:** FEAT-001

### FEAT-003 — Client authentication management

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-010 to T-012  
**Acceptance Criteria:** AC-009 to AC-012  
**Depends on:** FEAT-002

### FEAT-004 — Client registration management

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-013 to T-018  
**Acceptance Criteria:** AC-013 to AC-018  
**Depends on:** FEAT-002

### FEAT-039 — Client pending I/O management (PendingIOManager)

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-179 to T-181  
**Acceptance Criteria:** AC-207 to AC-212  
**Depends on:** FEAT-002

### FEAT-005 — Channel entity and ChannelRegistry

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-019 to T-024  
**Acceptance Criteria:** AC-019 to AC-024  
**Depends on:** FEAT-001

### FEAT-041 — Channel membership and settings management (ChannelMembershipManager, ChannelSettingsManager)

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-185 to T-190  
**Acceptance Criteria:** AC-218 to AC-226  
**Depends on:** FEAT-002, FEAT-005

### FEAT-042 — Channel broadcast management (ChannelBroadcastManager)

**Related Elements:** `channel_subsystem.mmd`, `client_subsystem.mmd`  
**Tasks:** T-191 to T-193  
**Acceptance Criteria:** AC-227 to AC-231  
**Depends on:** FEAT-005, FEAT-039

### FEAT-006 — Channel management (ChannelSystem)

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-025 to T-027  
**Acceptance Criteria:** AC-025 to AC-233  
**Depends on:** FEAT-005, FEAT-041, FEAT-042

### FEAT-040 — Client lifecycle management (ClientLifecycleManager)

**Related Elements:** `client_subsystem.mmd`, `channel_subsystem.mmd`  
**Tasks:** T-182 to T-184  
**Acceptance Criteria:** AC-213 to AC-217  
**Depends on:** FEAT-002, FEAT-003, FEAT-004, FEAT-039, FEAT-041

### FEAT-007 — Client management (ClientSystem)

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-028 to T-030  
**Acceptance Criteria:** AC-031 to AC-206  
**Depends on:** FEAT-002, FEAT-003, FEAT-004, FEAT-039, FEAT-040

### FEAT-008 — IRCDto

**Related Elements:** None  
**Tasks:** T-031  
**Acceptance Criteria:** AC-036 to AC-037  
**Depends on:** None

### FEAT-009 — ISecurityFilter and SecurityFilterFactory

**Related Elements:** `security_filtering.mmd`  
**Tasks:** T-032 to T-035  
**Acceptance Criteria:** AC-038 to AC-041  
**Depends on:** FEAT-001, FEAT-007, FEAT-006

### FEAT-010 — ICommand and CommandFactory

**Related Elements:** `command_subsystem.mmd`  
**Tasks:** T-036 to T-039  
**Acceptance Criteria:** AC-042 to AC-045  
**Depends on:** FEAT-001, FEAT-007, FEAT-006, FEAT-009

### FEAT-011 — Command Parser

**Related Elements:** `command_subsystem.mmd`  
**Tasks:** T-040 to T-042  
**Acceptance Criteria:** AC-046 to AC-049  
**Depends on:** FEAT-008, FEAT-001

### FEAT-012 — Command management (CommandSystem)

**Related Elements:** `command_subsystem.mmd`  
**Tasks:** T-043 to T-045  
**Acceptance Criteria:** AC-050 to AC-054  
**Depends on:** FEAT-010, FEAT-011

### FEAT-013 — ISocketService

**Related Elements:** `server.mmd`  
**Tasks:** T-046 to T-051  
**Acceptance Criteria:** AC-055 to AC-056  
**Depends on:** None

### FEAT-014 — Socket server

**Related Elements:** `server.mmd`  
**Tasks:** T-052 to T-054  
**Acceptance Criteria:** AC-057 to AC-062  
**Depends on:** FEAT-013, FEAT-001

### FEAT-015 — IRCService

**Related Elements:** `server.mmd`  
**Tasks:** T-055 to T-057  
**Acceptance Criteria:** AC-063 to AC-067  
**Depends on:** FEAT-013, FEAT-012, FEAT-007, FEAT-006

### FEAT-016 — PASS command

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-058 to T-063  
**Acceptance Criteria:** AC-068 to AC-072  
**Depends on:** FEAT-010, FEAT-007

### FEAT-017 — QUIT command

**Related Elements:** None  
**Tasks:** T-064 to T-069  
**Acceptance Criteria:** AC-073 to AC-077  
**Depends on:** FEAT-010, FEAT-007, FEAT-006

### FEAT-018 — PING command

**Related Elements:** None  
**Tasks:** T-070 to T-075  
**Acceptance Criteria:** AC-078 to AC-081  
**Depends on:** FEAT-010, FEAT-007

### FEAT-019 — AuthenticatedFilter

**Related Elements:** `security_filtering.mmd`  
**Tasks:** T-076 to T-081  
**Acceptance Criteria:** AC-082 to AC-085  
**Depends on:** FEAT-009, FEAT-007

### FEAT-020 — NICK command

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-082 to T-087  
**Acceptance Criteria:** AC-086 to AC-090  
**Depends on:** FEAT-010, FEAT-007, FEAT-019, FEAT-006

### FEAT-021 — USER command

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-088 to T-093  
**Acceptance Criteria:** AC-091 to AC-095  
**Depends on:** FEAT-010, FEAT-007, FEAT-019

### FEAT-022 — RegisteredFilter

**Related Elements:** `security_filtering.mmd`  
**Tasks:** T-094 to T-099  
**Acceptance Criteria:** AC-096 to AC-099  
**Depends on:** FEAT-009, FEAT-007

### FEAT-023 — ChainFilter

**Related Elements:** `security_filtering.mmd`  
**Tasks:** T-100 to T-105  
**Acceptance Criteria:** AC-100 to AC-104  
**Depends on:** FEAT-009

### FEAT-024 — getCommonAccessFilter in factory

**Related Elements:** `client_subsystem.mmd`  
**Tasks:** T-106 to T-108  
**Acceptance Criteria:** AC-105 to AC-108  
**Depends on:** FEAT-023, FEAT-019, FEAT-022

### FEAT-025 — JOIN command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-109 to T-114  
**Acceptance Criteria:** AC-109 to AC-113  
**Depends on:** FEAT-010, FEAT-007, FEAT-024, FEAT-006

### FEAT-026 — LIST command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-115 to T-120  
**Acceptance Criteria:** AC-114 to AC-117  
**Depends on:** FEAT-010, FEAT-007, FEAT-024, FEAT-006

### FEAT-027 — ChannelMemberFilter

**Related Elements:** `security_filtering.mmd`  
**Tasks:** T-121 to T-126  
**Acceptance Criteria:** AC-118 to AC-121  
**Depends on:** FEAT-009, FEAT-007, FEAT-006

### FEAT-028 — getChannelCommonAccessFilter in factory

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-127 to T-129  
**Acceptance Criteria:** AC-122 to AC-125  
**Depends on:** FEAT-024, FEAT-027

### FEAT-029 — PRIVMSG command

**Related Elements:** `command_subsystem.mmd`  
**Tasks:** T-130 to T-135  
**Acceptance Criteria:** AC-126 to AC-130  
**Depends on:** FEAT-010, FEAT-007, FEAT-028, FEAT-006

### FEAT-030 — PART command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-136 to T-141  
**Acceptance Criteria:** AC-131 to AC-135  
**Depends on:** FEAT-010, FEAT-007, FEAT-028, FEAT-006

### FEAT-031 — ChannelOperatorFilter

**Related Elements:** `security_filtering.mmd`  
**Tasks:** T-142 to T-147  
**Acceptance Criteria:** AC-136 to AC-139  
**Depends on:** FEAT-009, FEAT-007, FEAT-006

### FEAT-032 — getChannelOperatorAccessFilter in factory

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-148 to T-150  
**Acceptance Criteria:** AC-140 to AC-143  
**Depends on:** FEAT-028, FEAT-031

### FEAT-033 — KICK command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-151 to T-156  
**Acceptance Criteria:** AC-144 to AC-148  
**Depends on:** FEAT-010, FEAT-007, FEAT-032, FEAT-006

### FEAT-034 — MODE command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-157 to T-162  
**Acceptance Criteria:** AC-149 to AC-153  
**Depends on:** FEAT-010, FEAT-007, FEAT-032, FEAT-006

### FEAT-035 — TOPIC command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-163 to T-168  
**Acceptance Criteria:** AC-154 to AC-158  
**Depends on:** FEAT-010, FEAT-007, FEAT-032, FEAT-006

### FEAT-036 — INVITE command

**Related Elements:** `channel_subsystem.mmd`  
**Tasks:** T-169 to T-174  
**Acceptance Criteria:** AC-159 to AC-163  
**Depends on:** FEAT-010, FEAT-007, FEAT-032, FEAT-006

---

## Dependency Summary

| Feature | Depends On |
|---------|------------|
| FEAT-001 | None |
| FEAT-002 | FEAT-001 |
| FEAT-003 | FEAT-002 |
| FEAT-004 | FEAT-002 |
| FEAT-039 | FEAT-002 |
| FEAT-005 | FEAT-001 |
| FEAT-041 | FEAT-002, FEAT-005 |
| FEAT-042 | FEAT-005, FEAT-039 |
| FEAT-006 | FEAT-005, FEAT-041, FEAT-042 |
| FEAT-040 | FEAT-002, FEAT-003, FEAT-004, FEAT-039, FEAT-041 |
| FEAT-007 | FEAT-002, FEAT-003, FEAT-004, FEAT-039, FEAT-040 |
| FEAT-008 | None |
| FEAT-009 | FEAT-001, FEAT-007, FEAT-006 |
| FEAT-010 | FEAT-001, FEAT-007, FEAT-006, FEAT-009 |
| FEAT-011 | FEAT-008, FEAT-001 |
| FEAT-012 | FEAT-010, FEAT-011 |
| FEAT-013 | None |
| FEAT-014 | FEAT-013, FEAT-001 |
| FEAT-015 | FEAT-013, FEAT-012, FEAT-007, FEAT-006 |
| FEAT-016 | FEAT-010, FEAT-007 |
| FEAT-017 | FEAT-010, FEAT-007, FEAT-006 |
| FEAT-018 | FEAT-010, FEAT-007 |
| FEAT-019 | FEAT-009, FEAT-007 |
| FEAT-020 | FEAT-010, FEAT-007, FEAT-019, FEAT-006 |
| FEAT-021 | FEAT-010, FEAT-007, FEAT-019 |
| FEAT-022 | FEAT-009, FEAT-007 |
| FEAT-023 | FEAT-009 |
| FEAT-024 | FEAT-023, FEAT-019, FEAT-022 |
| FEAT-025 | FEAT-010, FEAT-007, FEAT-024, FEAT-006 |
| FEAT-026 | FEAT-010, FEAT-007, FEAT-024, FEAT-006 |
| FEAT-027 | FEAT-009, FEAT-007, FEAT-006 |
| FEAT-028 | FEAT-024, FEAT-027 |
| FEAT-029 | FEAT-010, FEAT-007, FEAT-028, FEAT-006 |
| FEAT-030 | FEAT-010, FEAT-007, FEAT-028, FEAT-006 |
| FEAT-031 | FEAT-009, FEAT-007, FEAT-006 |
| FEAT-032 | FEAT-028, FEAT-031 |
| FEAT-033 | FEAT-010, FEAT-007, FEAT-032, FEAT-006 |
| FEAT-034 | FEAT-010, FEAT-007, FEAT-032, FEAT-006 |
| FEAT-035 | FEAT-010, FEAT-007, FEAT-032, FEAT-006 |
| FEAT-036 | FEAT-010, FEAT-007, FEAT-032, FEAT-006 |
