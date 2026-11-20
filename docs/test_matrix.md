# Test Matrix — Inputs / Outputs / ACs por clase

Esta matriz refleja los **tests unitarios que se van implementando** y, para cada uno, muestra la relación entre sus **inputs**, sus **outputs / comportamiento esperado** y el **criterio de aceptación (AC)** que verifican.

Reglas:
- Una tabla por clase con lógica de negocio no trivial.
- Las clases puramente de infraestructura (`SocketServer`, `IRCService` orquestador, `main`) se verifican con build + tests de integración, no aparecen aquí con unit tests obligatorios.
- Estado: **Implementado** = el archivo de test ya existe en `tests/unit/`; **Pendiente** = la clase existe en producción pero aún no tiene test unitario.

---

## 1. ClientRegistry

Archivo de test: `tests/unit/test_ClientRegistry.cpp` — **Implementado**

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| AddClientMakesClientFindableByFd | `addClient(5)` | `findClient(5).getFd() == 5` | AC-003, AC-201 |
| AddClientWithExistingFdThrowsAlreadyExists | `addClient(5)`; luego `addClient(5)` | Lanza `AlreadyExistsException` | AC-003, AC-202 |
| AddClientWithExistingFdKeepsOriginalClient | `addClient(5)`; `findClient(5).setNick("original")`; `addClient(5)` | Lanza `AlreadyExistsException`; `findClient(5).getNick() == "original"` | AC-003, AC-202 |
| AddClientAllowsMultipleDistinctFds | `addClient(5)`, `addClient(6)`, `addClient(7)` | `findClient(5).getFd()==5`, `findClient(6).getFd()==6`, `findClient(7).getFd()==7` | AC-003, AC-201 |
| AddedClientStartsWithDefaultState | `addClient(5)` | nick `""`; `!isAuthenticated()`; `!isRegistered()`; `!isQuitting()`; `isInputBufferEmpty()`; `isOutputBufferEmpty()` | AC-003, AC-201 |
| FindClientByFdThrowsNotFoundWhenRegistryIsEmpty | `findClient(5)` sobre registro vacío | Lanza `NotFoundException` | AC-003, AC-203 |
| FindClientByFdThrowsNotFoundWhenFdDoesNotExist | `addClient(5)`; `findClient(6)` | Lanza `NotFoundException` | AC-003, AC-203 |
| FindClientByFdReturnsSameObjectOnEveryCall | `findClient(5)` dos veces | Mismo puntero (`&first == &second`) | AC-003, AC-201 |
| FindClientByFdReturnsReferenceThatModifiesStoredClient | `findClient(5).setNick("testuser")` | `findClient(5).getNick() == "testuser"` | AC-003, AC-201 |
| FindClientByFdReturnsDistinctObjectsForDistinctFds | `addClient(5)`, `addClient(6)`; setNick distinto en cada uno | Objetos distintos; cada nick se lee de su fd | AC-003, AC-201 |
| FindClientByFdWorksOnConstRegistry | `addClient(5)`; setNick; referencia const | `findClient(5)` funciona sobre const | AC-003, AC-201 |
| FindClientByFdOnConstRegistryThrowsNotFoundWhenFdDoesNotExist | registro vacío const; `findClient(5)` | Lanza `NotFoundException` | AC-003, AC-203 |
| FindClientByNickReturnsMatchingClient | `addClient(5)`; `setNick("testuser")`; `findClient("testuser")` | `getFd() == 5` | AC-003, AC-201 |
| FindClientByNickReturnsSameObjectAsFindClientByFd | `findClient(5)` y `findClient("testuser")` | Mismo puntero | AC-003, AC-201 |
| FindClientByNickThrowsNotFoundWhenRegistryIsEmpty | `findClient("testuser")` sobre vacío | Lanza `NotFoundException` | AC-003, AC-203 |
| FindClientByNickThrowsNotFoundWhenNickDoesNotExist | `addClient(5)` nick `"testuser"`; `findClient("other")` | Lanza `NotFoundException` | AC-003, AC-203 |
| FindClientByNickThrowsNotFoundWhenClientsHaveNoNick | `addClient(5)` sin nick; `findClient("testuser")` | Lanza `NotFoundException` | AC-003, AC-203 |
| FindClientByNickSelectsCorrectClientAmongSeveral | 3 clientes con nicks distintos | `findClient(nick)` devuelve el fd correcto | AC-003, AC-201 |
| FindClientByNickReflectsNickChanges | setNick `"oldnick"`; setNick `"newnick"` | `findClient("newnick")==5`; `findClient("oldnick")` lanza `NotFoundException` | AC-003, AC-201, AC-203 |
| FindClientByNickWorksOnConstRegistry | referencia const; nick seteado | `findClient("testuser")` funciona sobre const | AC-003, AC-201 |
| FindClientByNickOnConstRegistryThrowsNotFoundWhenNickDoesNotExist | referencia const; nick inexistente | Lanza `NotFoundException` | AC-003, AC-203 |
| RemoveClientMakesClientNotFoundByFd | `addClient(5)`; `removeClient(5)`; `findClient(5)` | Lanza `NotFoundException` | AC-003, AC-203 |
| RemoveClientMakesClientNotFoundByNick | `addClient(5)` nick; `removeClient(5)`; `findClient(nick)` | Lanza `NotFoundException` | AC-003, AC-203 |
| RemoveClientThrowsNotFoundWhenRegistryIsEmpty | `removeClient(5)` sobre vacío | Lanza `NotFoundException` | AC-003, AC-203 |
| RemoveClientThrowsNotFoundWhenFdDoesNotExist | `addClient(5)`; `removeClient(6)` | Lanza `NotFoundException`; cliente 5 sigue accesible | AC-003, AC-203 |
| RemoveClientTwiceThrowsNotFoundOnSecondCall | `addClient(5)`; `removeClient(5)`; `removeClient(5)` | Segunda llamada lanza `NotFoundException` | AC-003, AC-203 |
| RemoveClientDoesNotAffectOtherClients | `addClient(5)`, `addClient(6)`; `removeClient(5)` | Cliente 6 sigue accesible con sus datos | AC-003, AC-201 |
| AddClientAfterRemoveClientWithSameFdCreatesFreshClient | `addClient(5)` con estado; `removeClient(5)`; `addClient(5)` | Cliente nuevo: nick `""`, `!isRegistered()`, output buffer vacío | AC-003, AC-201 |

---

## 2. NickManager

Archivo de test: `tests/unit/test_NickManager.cpp` — **Implementado** (clase existe en `include/managers/NickManager.hpp`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| isAvailable_unknownNick | `isAvailable("nuevo")` | `true` | AC-004, AC-006 |
| isAvailable_takenNick | tras `reserve("nuevo")`; `isAvailable("nuevo")` | `false` | AC-004, AC-006 |
| reserve_validNick | `reserve("user1")` | `true`; nick reservado | AC-004 |
| reserve_invalidNick | `reserve("")` o nick con caracteres inválidos | `false` | AC-004, AC-006 |
| reserve_duplicateNick | `reserve("user1")` dos veces | Segunda `false` | AC-004, AC-006 |
| release_reservedNick | `reserve("user1")`; `release("user1")`; `isAvailable("user1")` | `true` (disponible de nuevo) | AC-004 |
| release_unreservedNick | `release("nuncaReservado")` | No lanza; estado consistente | AC-004 |
| isValid_validNick | `isValid("user1")` | `true` | AC-004, AC-006 |
| isValid_emptyNick | `isValid("")` | `false` | AC-004, AC-006 |
| isValid_tooLongNick | `isValid(string(31,'a'))` | `false` (supera NICK_MAX_LENGTH) | AC-004, AC-006 |
| isValid_invalidChars | `isValid("user@host")` | `false` | AC-004, AC-006 |

---

## 3. AuthenticationManager

Archivo de test: `tests/unit/test_AuthenticationManager.cpp` — **Implementado** (clase existe en `include/managers/AuthenticationManager.hpp`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| authenticate_correctPassword | `addClient(fd)` en registry; `authenticate(fd, "pass")` | Cliente marcado autenticado; `isAuthenticated(fd)==true` | AC-205, AC-206 |
| authenticate_wrongPassword | `authenticate(fd, "wrong")` | Lanza `AuthenticationFailedException`; `!isAuthenticated(fd)` | AC-205, AC-206 |
| authenticate_unknownFd | `authenticate(999, "pass")` | Lanza `ClientNotFoundException` (de ClientRegistry) | AC-205 |
| unAuthenticate_authenticated | `authenticate(fd,"pass")`; `unAuthenticate(fd)` | `isAuthenticated(fd)==false` | AC-205 |
| unAuthenticate_notAuthenticated | `unAuthenticate(fd)` sin autenticar | No lanza; `isAuthenticated(fd)==false` | AC-205 |
| isAuthenticated_afterAuth | `authenticate(fd,"pass")`; `isAuthenticated(fd)` | `true` | AC-205 |
| isAuthenticated_beforeAuth | `isAuthenticated(fd)` sin autenticar | `false` | AC-205 |

---

## 4. RegistrationManager

Archivo de test: `tests/unit/test_RegistrationManager.cpp` — **Implementado** (clase existe en `include/managers/RegistrationManager.hpp`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| setNick_validUnique | `setNick(fd, "user1")` | Nick asignado; `getNick()=="user1"` | AC-208, AC-209 |
| setNick_invalidNick | `setNick(fd, "")` | Lanza `InvalidNicknameException` | AC-208 |
| setNick_alreadyInUse | `setNick(fd1,"user1")`; `setNick(fd2,"user1")` | Segunda lanza `NicknameAlreadyInUseException` | AC-208, AC-209 |
| setNick_changeNick | `setNick(fd,"old")`; `setNick(fd,"new")` | `"old"` liberado; `"new"` asignado | AC-208, AC-209 |
| setUsername | `setUsername(fd, "username")` | Username asignado | AC-208, AC-210 |
| setRealname | `setRealname(fd, "realname")` | Realname asignado | AC-208, AC-210 |
| setHostname | `setHostname(fd, "hostname")` | Hostname asignado (opcional) | AC-208 |
| isRegistered_incomplete | tras `setNick` solo | `false` | AC-208 |
| isRegistered_complete | tras `setNick`+`setUsername`+`setRealname` | `true` | AC-208 |
| unRegister | tras registro completo; `unRegister(fd)` | Nick liberado; campos vacíos; `isRegistered(fd)==false` | AC-208 |
| setNick_unknownFd | `setNick(999, "nick")` | Lanza `ClientNotFoundException` | AC-208 |

---

## 5. ClientManager (coordinator)

Archivo de test: `tests/unit/test_ClientManager.cpp` — **Implementado** (clase existe en `include/managers/ClientManager.hpp`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| addClient | `addClient(fd)` | Cliente creado y registrado en ClientRegistry | AC-212 |
| addClient_duplicateFd | `addClient(fd)` dos veces | Lanza `ClientAlreadyExistsException` | AC-212 |
| removeClient | `addClient(fd)`; `removeClient(fd)` | Cliente eliminado de ClientRegistry | AC-212 |
| setQuitting_true | `setQuitting(fd, true)` | Cliente marcado como quitting | AC-212 |
| setQuitting_false | `setQuitting(fd, true)`; `setQuitting(fd, false)` | Marca cancelada | AC-212 |
| getClientsToClose | cliente quitting + buffers vacíos | Devuelve `fd` en el set | AC-212 |
| getClientsToClose_notQuitting | cliente no quitting | No devuelve el fd | AC-212 |
| getClientsToClose_bufferNotEmpty | cliente quitting pero output buffer no vacío | No devuelve el fd hasta vaciar | AC-212 |
| pushInput | `pushInput(fd, "data")` | Datos en input buffer del cliente | AC-212 |
| pushInput_quitting | cliente quitting; `pushInput(fd, ...)` | Lanza `ClientAlreadyQuittingException` | AC-212 |
| pushOutput | `pushOutput(fd, "data")` | Datos en output buffer del cliente | AC-212 |
| getNextInputLine | `pushInput(fd, "line\n")`; `getNextInputLine(fd, line)` | `true`; `line == "line"` | AC-212 |
| getNextInputLine_incomplete | `pushInput(fd, "partial")`; `getNextInputLine(fd, line)` | `false` (no hay línea completa) | AC-212 |
| peekOutput | `pushOutput(fd, "hello")`; `peekOutput(fd, 2)` | `"he"` (sin consumir) | AC-212 |
| consumeOutput | `pushOutput(fd, "hello")`; `consumeOutput(fd, 2)` | Output buffer avanza 2 bytes | AC-212 |
| getHasPendingInput | cliente con input buffer no vacío | fd en el set devuelto | AC-212 |
| getHasPendingOutput | cliente con output buffer no vacío | fd en el set devuelto | AC-212 |
| getIsQuitting | cliente marcado quitting | fd en el set devuelto | AC-212 |
| getClientDetails | `getClientDetails(fd)` | Estructura `ClientDetails` con fd, nick, autenticado, registrado, quitting, pending flags | AC-212 |

---

## 6. Client / ClientBuffer

Archivo de test: `tests/unit/test_Client.cpp` — **Implementado** (clase en `include/client/Client.hpp`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| getNick_default | `Client(fd)` recién creado | `""` | AC-001 |
| setNick_getNick | `setNick("user")`; `getNick()` | `"user"` | AC-001 |
| getUsername_getRealname_getHostname | setters correspondientes | Valores asignados | AC-001 |
| isAuthenticated_default | `Client(fd)` | `false` | AC-001 |
| setAuthenticated | `setAuthenticated(true)`; `isAuthenticated()` | `true` | AC-001 |
| isRegistered_default | `Client(fd)` | `false` | AC-001 |
| setRegistered | `setRegistered(true)`; `isRegistered()` | `true` | AC-001 |
| isQuitting_default | `Client(fd)` | `false` | AC-001 |
| setQuitting | `setQuitting(true)`; `isQuitting()` | `true` | AC-001 |
| pushInput_getNextInputLine | `pushInput("cmd\r\n")`; `getNextInputLine(line)` | `true`; `line=="cmd"` | AC-002 |
| pushInput_partial | `pushInput("cmd")` (sin newline) | `getNextInputLine` devuelve `false` hasta completar | AC-002 |
| pushOutput_peekOutput_consumeOutput | `pushOutput("data")`; `peekOutput(2)`; `consumeOutput(2)` | `"da"`; luego buffer avanza | AC-002 |
| isInputBufferEmpty_default | `Client(fd)` | `true` | AC-002 |
| isOutputBufferEmpty_default | `Client(fd)` | `true` | AC-002 |

---

## 7. Common Exceptions (BaseException, AlreadyExistsException, NotFoundException)

Archivo de test: `tests/unit/test_Exceptions.cpp` — **Pendiente** (clases en `include/exceptions_common/`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| BaseException_message | `BaseException("name", "msg", 1)` | `what()` contiene `"msg"`; código 1 | AC-204 |
| BaseException_cause_chaining | excepción con causa y overrides | Cadena de causas preservada | AC-204 |
| AlreadyExistsException_instantiation | `AlreadyExistsException(fd)` | Hereda de BaseException; mensaje con fd | AC-204 |
| NotFoundException_instantiation | `NotFoundException(fd)` o `NotFoundException(nick)` | Hereda de BaseException; mensaje con fd o nick | AC-204 |
| catch_by_base | lanzar derivada; capturar `BaseException&` | Se captura correctamente por referencia base | AC-204 |

---

## 8. Command Infrastructure (ICommand, CommandData, CommandParser, CommandFactory)

Archivo de test: pendiente — **Pendiente** (clases en `include/commands/`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| CommandFactory_register_create | `registerCommand("PASS", creator)`; `createCommand("PASS", data, fd)` | Devuelve `ICommand*` no nulo | AC-015 |
| CommandFactory_create_unknown | `createCommand("UNKNOWN", ...)` | Devuelve `NULL` o lanza según implementación | AC-015 |
| CommandParser_parseLine_valid | `parseLine("NICK user")` | `CommandData` con comando `"NICK"` y params | AC-016 |
| CommandParser_parseLine_empty | `parseLine("")` | `CommandData` vacío o sin comando | AC-016 |
| ICommand_execute | comando concreto; `execute()` | Efecto sobre cliente/registry según comando | AC-014 |
| PassCommand_execute | `PassCommand` con password correcta | Cliente autenticado | AC-018 |
| NickCommand_execute | `NickCommand` con nick válido | Nick asignado al cliente | AC-018 |
| RealNameCommand_execute | `RealNameCommand` (USER) | username/realname asignados | AC-018 |

---

## 9. TickResult

Archivo de test: pendiente — **Pendiente** (clase en `include/socket_server/TickResult.hpp`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| TickResult_default | `TickResult()` | `outputs` vacío; `toClose` vacío | AC-007 |
| TickResult_parametrized | `TickResult(outputs, toClose)` | Copia los mapas/sets recibidos | AC-007 |
| TickResult_copy | `TickResult a`; `TickResult b(a)` | Copia profunda equivalente | AC-007 |

---

## 10. Security Filters (ISecurityFilter, ChainFilter, SecurityFilterFactory)

Archivo de test: pendiente — **Pendiente** (clases en `include/security/`)

| Test name | Inputs | Outputs / Expected behavior | ACs verificadas |
|-----------|--------|-----------------------------|-----------------|
| AuthenticatedFilter_handle | cliente autenticado | Permite ejecución | AC-213 |
| AuthenticatedFilter_handle_notAuth | cliente no autenticado | Bloquea / rechaza | AC-213 |
| RegisteredFilter_handle | cliente registrado | Permite ejecución | AC-213 |
| RegisteredFilter_handle_notReg | cliente no registrado | Bloquea / rechaza | AC-213 |
| ChainFilter_handle | cadena de filtros | Ejecuta todos en orden; si uno falla, corta | AC-213 |
| SecurityFilterFactory_createAuthenticated | `getAuthenticatedFilter(fd)` | Devuelve filtro que chequea `isAuthenticated(fd)` | AC-213 |
| SecurityFilterFactory_createRegistered | `getRegisteredFilter(fd)` | Devuelve filtro que chequea `isRegistered(fd)` | AC-213 |
| SecurityFilterFactory_createChain | `getChainFilter(vector de filtros)` | Devuelve `ChainFilter` compuesto | AC-213 |

---

## Notas sobre cobertura

- **Implementados en disco:** `Client`, `ClientBuffer`, `ClientRegistry`, `NickManager`, `AuthenticationManager`, `RegistrationManager`, `ClientManager`.
- **Pendientes de test unitario:** `Common Exceptions`, `Command` infraestructura (`ICommand`, `CommandData`, `CommandParser`, `CommandFactory`, comandos concretos), `TickResult`, `Security Filters`.
- **Sin unit test obligatorio (infraestructura):** `SocketServer`, `IRCService`, `main` — verificados con build y tests de integración.
