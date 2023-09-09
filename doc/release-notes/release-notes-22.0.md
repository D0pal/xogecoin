22.0 Release Notes
==================

Qogecoin Core version 22.0 is now available from:

  <https://xogecoincore.org/bin/xogecoin-core-22.0/>

This release includes new features, various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/xogecoin/xogecoin/issues>

To receive security and update notifications, please subscribe to:

  <https://xogecoincore.org/en/list/announcements/join/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Qogecoin-Qt` (on Mac)
or `xogecoind`/`xogecoin-qt` (on Linux).

Upgrading directly from a version of Qogecoin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of Qogecoin Core are generally supported.

Compatibility
==============

Qogecoin Core is supported and extensively tested on operating systems
using the Linux kernel, macOS 10.14+, and Windows 7 and newer.  Qogecoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them.  It is not recommended to use Qogecoin Core on
unsupported systems.

From Qogecoin Core 22.0 onwards, macOS versions earlier than 10.14 are no longer supported.

Notable changes
===============

P2P and network changes
-----------------------
- Added support for running Qogecoin Core as an
  [I2P (Invisible Internet Project)](https://en.wikipedia.org/wiki/I2P) service
  and connect to such services. See [i2p.md](https://github.com/xogecoin/xogecoin/blob/22.x/doc/i2p.md) for details. (#20685)
- This release removes support for Tor version 2 hidden services in favor of Tor
  v3 only, as the Tor network [dropped support for Tor
  v2](https://blog.torproject.org/v2-deprecation-timeline) with the release of
  Tor version 0.4.6.  Henceforth, Qogecoin Core ignores Tor v2 addresses; it
  neither rumors them over the network to other peers, nor stores them in memory
  or to `peers.dat`.  (#22050)

- Added NAT-PMP port mapping support via
  [`libnatpmp`](https://miniupnp.tuxfamily.org/libnatpmp.html). (#18077)

New and Updated RPCs
--------------------

- Due to [BIP 350](https://github.com/xogecoin/bips/blob/master/bip-0350.mediawiki)
  being implemented, behavior for all RPCs that accept addresses is changed when
  a native witness version 1 (or higher) is passed. These now require a Bech32m
  encoding instead of a Bech32 one, and Bech32m encoding will be used for such
  addresses in RPC output as well. No version 1 addresses should be created
  for mainnet until consensus rules are adopted that give them meaning
  (as will happen through [BIP 341](https://github.com/xogecoin/bips/blob/master/bip-0341.mediawiki)).
  Once that happens, Bech32m is expected to be used for them, so this shouldn't
  affect any production systems, but may be observed on other networks where such
  addresses already have meaning (like signet). (#20861)

- The `getpeerinfo` RPC returns two new boolean fields, `bip152_hb_to` and
  `bip152_hb_from`, that respectively indicate whether we selected a peer to be
  in compact blocks high-bandwidth mode or whether a peer selected us as a
  compact blocks high-bandwidth peer. High-bandwidth peers send new block
  announcements via a `cmpctblock` message rather than the usual inv/headers
  announcements. See BIP 152 for more details. (#19776)

- `getpeerinfo` no longer returns the following fields: `addnode`, `banscore`,
  and `whitelisted`, which were previously deprecated in 0.21. Instead of
  `addnode`, the `connection_type` field returns manual. Instead of
  `whitelisted`, the `permissions` field indicates if the peer has special
  privileges. The `banscore` field has simply been removed. (#20755)

- The following RPCs:  `gettxout`, `getrawtransaction`, `decoderawtransaction`,
  `decodescript`, `gettransaction`, and REST endpoints: `/rest/tx`,
  `/rest/getutxos`, `/rest/block` deprecated the following fields (which are no
  longer returned in the responses by default): `addresses`, `reqSigs`.
  The `-deprecatedrpc=addresses` flag must be passed for these fields to be
  included in the RPC response. This flag/option will be available only for this major release, after which
  the deprecation will be removed entirely. Note that these fields are attributes of
  the `scriptPubKey` object returned in the RPC response. However, in the response
  of `decodescript` these fields are top-level attributes, and included again as attributes
  of the `scriptPubKey` object. (#20286)

- When creating a hex-encoded xogecoin transaction using the `xogecoin-tx` utility
  with the `-json` option set, the following fields: `addresses`, `reqSigs` are no longer
  returned in the tx output of the response. (#20286)

- The `listbanned` RPC now returns two new numeric fields: `ban_duration` and `time_remaining`.
  Respectively, these new fields indicate the duration of a ban and the time remaining until a ban expires,
  both in seconds. Additionally, the `ban_created` field is repositioned to come before `banned_until`. (#21602)

- The `setban` RPC can ban onion addresses again. This fixes a regression introduced in version 0.21.0. (#20852)

- The `getnodeaddresses` RPC now returns a "network" field indicating the
  network type (ipv4, ipv6, onion, or i2p) for each address.  (#21594)

- `getnodeaddresses` now also accepts a "network" argument (ipv4, ipv6, onion,
  or i2p) to return only addresses of the specified network.  (#21843)

- The `testmempoolaccept` RPC now accepts multiple transactions (still experimental at the moment,
  API may be unstable). This is intended for testing transaction packages with dependency
  relationships; it is not recommended for batch-validating independent transactions. In addition to
  mempool policy, package policies apply: the list cannot contain more than 25 transactions or have a
  total size exceeding 101K virtual bytes, and cannot conflict with (spend the same inputs as) each other or
  the mempool, even if it would be a valid BIP125 replace-by-fee. There are some known limitations to
  the accuracy of the test accept: it's possible for `testmempoolaccept` to return "allowed"=True for a
  group of transactions, but "too-long-mempool-chain" if they are actually submitted. (#20833)

- `addmultisigaddress` and `createmultisig` now support up to 20 keys for
  Segwit addresses. (#20867)

Changes to Wallet or GUI related RPCs can be found in the GUI or Wallet section below.

Build System
------------

- Release binaries are now produced using the new `guix`-based build system.
  The [/doc/release-process.md](/doc/release-process.md) document has been updated accordingly.

Files
-----

- The list of banned hosts and networks (via `setban` RPC) is now saved on disk
  in JSON format in `banlist.json` instead of `banlist.dat`. `banlist.dat` is
  only read on startup if `banlist.json` is not present. Changes are only written to the new
  `banlist.json`. A future version of Qogecoin Core may completely ignore
  `banlist.dat`. (#20966)

New settings
------------

- The `-natpmp` option has been added to use NAT-PMP to map the listening port.
  If both UPnP and NAT-PMP are enabled, a successful allocation from UPnP
  prevails over one from NAT-PMP. (#18077)

Updated settings
----------------

Changes to Wallet or GUI related settings can be found in the GUI or Wallet section below.

- Passing an invalid `-rpcauth` argument now cause xogecoind to fail to start.  (#20461)

Tools and Utilities
-------------------

- A new CLI `-addrinfo` command returns the number of addresses known to the
  node per network type (including Tor v2 versus v3) and total. This can be
  useful to see if the node knows enough addresses in a network to use options
  like `-onlynet=<network>` or to upgrade to this release of Qogecoin Core 22.0
  that supports Tor v3 only.  (#21595)

- A new `-rpcwaittimeout` argument to `xogecoin-cli` sets the timeout
  in seconds to use with `-rpcwait`. If the timeout expires,
  `xogecoin-cli` will report a failure. (#21056)

Wallet
------

- External signers such as hardware wallets can now be used through the new RPC methods `enumeratesigners` and `displayaddress`. Support is also added to the `send` RPC call. This feature is experimental. See [external-signer.md](https://github.com/xogecoin/xogecoin/blob/22.x/doc/external-signer.md) for details. (#16546)

- A new `listdescriptors` RPC is available to inspect the contents of descriptor-enabled wallets.
  The RPC returns public versions of all imported descriptors, including their timestamp and flags.
  For ranged descriptors, it also returns the range boundaries and the next index to generate addresses from. (#20226)

- The `bumpfee` RPC is not available with wallets that have private keys
  disabled. `psbtbumpfee` can be used instead. (#20891)

- The `fundrawtransaction`, `send` and `walletcreatefundedpsbt` RPCs now support an `include_unsafe` option
  that when `true` allows using unsafe inputs to fund the transaction.
  Note that the resulting transaction may become invalid if one of the unsafe inputs disappears.
  If that happens, the transaction must be funded with different inputs and republished. (#21359)

- We now support up to 20 keys in `multi()` and `sortedmulti()` descriptors
  under `wsh()`. (#20867)

- Taproot descriptors can be imported into the wallet only after activation has occurred on the network (e.g. mainnet, testnet, signet) in use. See [descriptors.md](https://github.com/xogecoin/xogecoin/blob/22.x/doc/descriptors.md) for supported descriptors.

GUI changes
-----------

- External signers such as hardware wallets can now be used. These require an external tool such as [HWI](https://github.com/xogecoin-core/HWI) to be installed and configured under Options -> Wallet. When creating a new wallet a new option "External signer" will appear in the dialog. If the device is detected, its name is suggested as the wallet name. The watch-only keys are then automatically imported. Receive addresses can be verified on the device. The send dialog will automatically use the connected device. This feature is experimental and the UI may freeze for a few seconds when performing these actions.

Low-level changes
=================

RPC
---

- The RPC server can process a limited number of simultaneous RPC requests.
  Previously, if this limit was exceeded, the RPC server would respond with
  [status code 500 (`HTTP_INTERNAL_SERVER_ERROR`)](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes#5xx_server_errors).
  Now it returns status code 503 (`HTTP_SERVICE_UNAVAILABLE`). (#18335)

- Error codes have been updated to be more accurate for the following error cases (#18466):
  - `signmessage` now returns RPC_INVALID_ADDRESS_OR_KEY (-5) if the
    passed address is invalid. Previously returned RPC_TYPE_ERROR (-3).
  - `verifymessage` now returns RPC_INVALID_ADDRESS_OR_KEY (-5) if the
    passed address is invalid. Previously returned RPC_TYPE_ERROR (-3).
  - `verifymessage` now returns RPC_TYPE_ERROR (-3) if the passed signature
    is malformed. Previously returned RPC_INVALID_ADDRESS_OR_KEY (-5).

Tests
-----

22.0 change log
===============

A detailed list of changes in this version follows. To keep the list to a manageable length, small refactors and typo fixes are not included, and similar changes are sometimes condensed into one line.

### Consensus
- xogecoin/xogecoin#19438 Introduce deploymentstatus (ajtowns)
- xogecoin/xogecoin#20207 Follow-up extra comments on taproot code and tests (sipa)
- xogecoin/xogecoin#21330 Deal with missing data in signature hashes more consistently (sipa)

### Policy
- xogecoin/xogecoin#18766 Disable fee estimation in blocksonly mode (by removing the fee estimates global) (darosior)
- xogecoin/xogecoin#20497 Add `MAX_STANDARD_SCRIPTSIG_SIZE` to policy (sanket1729)
- xogecoin/xogecoin#20611 Move `TX_MAX_STANDARD_VERSION` to policy (MarcoFalke)

### Mining
- xogecoin/xogecoin#19937, xogecoin/xogecoin#20923 Signet mining utility (ajtowns)

### Block and transaction handling
- xogecoin/xogecoin#14501 Fix possible data race when committing block files (luke-jr)
- xogecoin/xogecoin#15946 Allow maintaining the blockfilterindex when using prune (jonasschnelli)
- xogecoin/xogecoin#18710 Add local thread pool to CCheckQueue (hebasto)
- xogecoin/xogecoin#19521 Coinstats Index (fjahr)
- xogecoin/xogecoin#19806 UTXO snapshot activation (jamesob)
- xogecoin/xogecoin#19905 Remove dead CheckForkWarningConditionsOnNewFork (MarcoFalke)
- xogecoin/xogecoin#19935 Move SaltedHashers to separate file and add some new ones (achow101)
- xogecoin/xogecoin#20054 Remove confusing and useless "unexpected version" warning (MarcoFalke)
- xogecoin/xogecoin#20519 Handle rename failure in `DumpMempool(…)` by using the `RenameOver(…)` return value (practicalswift)
- xogecoin/xogecoin#20749, xogecoin/xogecoin#20750, xogecoin/xogecoin#21055, xogecoin/xogecoin#21270, xogecoin/xogecoin#21525, xogecoin/xogecoin#21391, xogecoin/xogecoin#21767, xogecoin/xogecoin#21866 Prune `g_chainman` usage (dongcarl)
- xogecoin/xogecoin#20833 rpc/validation: enable packages through testmempoolaccept (glozow)
- xogecoin/xogecoin#20834 Locks and docs in ATMP and CheckInputsFromMempoolAndCache (glozow)
- xogecoin/xogecoin#20854 Remove unnecessary try-block (amitiuttarwar)
- xogecoin/xogecoin#20868 Remove redundant check on pindex (jarolrod)
- xogecoin/xogecoin#20921 Don't try to invalidate genesis block in CChainState::InvalidateBlock (theStack)
- xogecoin/xogecoin#20972 Locks: Annotate CTxMemPool::check to require `cs_main` (dongcarl)
- xogecoin/xogecoin#21009 Remove RewindBlockIndex logic (dhruv)
- xogecoin/xogecoin#21025 Guard chainman chainstates with `cs_main` (dongcarl)
- xogecoin/xogecoin#21202 Two small clang lock annotation improvements (amitiuttarwar)
- xogecoin/xogecoin#21523 Run VerifyDB on all chainstates (jamesob)
- xogecoin/xogecoin#21573 Update libsecp256k1 subtree to latest master (sipa)
- xogecoin/xogecoin#21582, xogecoin/xogecoin#21584, xogecoin/xogecoin#21585 Fix assumeutxo crashes (MarcoFalke)
- xogecoin/xogecoin#21681 Fix ActivateSnapshot to use hardcoded nChainTx (jamesob)
- xogecoin/xogecoin#21796 index: Avoid async shutdown on init error (MarcoFalke)
- xogecoin/xogecoin#21946 Document and test lack of inherited signaling in RBF policy (ariard)
- xogecoin/xogecoin#22084 Package testmempoolaccept followups (glozow)
- xogecoin/xogecoin#22102 Remove `Warning:` from warning message printed for unknown new rules (prayank23)
- xogecoin/xogecoin#22112 Force port 0 in I2P (vasild)
- xogecoin/xogecoin#22135 CRegTestParams: Use `args` instead of `gArgs` (kiminuo)
- xogecoin/xogecoin#22146 Reject invalid coin height and output index when loading assumeutxo (MarcoFalke)
- xogecoin/xogecoin#22253 Distinguish between same tx and same-nonwitness-data tx in mempool (glozow)
- xogecoin/xogecoin#22261 Two small fixes to node broadcast logic (jnewbery)
- xogecoin/xogecoin#22415 Make `m_mempool` optional in CChainState (jamesob)
- xogecoin/xogecoin#22499 Update assumed chain params (sriramdvt)
- xogecoin/xogecoin#22589 net, doc: update I2P hardcoded seeds and docs for 22.0 (jonatack)

### P2P protocol and network code
- xogecoin/xogecoin#18077 Add NAT-PMP port forwarding support (hebasto)
- xogecoin/xogecoin#18722 addrman: improve performance by using more suitable containers (vasild)
- xogecoin/xogecoin#18819 Replace `cs_feeFilter` with simple std::atomic (MarcoFalke)
- xogecoin/xogecoin#19203 Add regression fuzz harness for CVE-2017-18350. Add FuzzedSocket (practicalswift)
- xogecoin/xogecoin#19288 fuzz: Add fuzzing harness for TorController (practicalswift)
- xogecoin/xogecoin#19415 Make DNS lookup mockable, add fuzzing harness (practicalswift)
- xogecoin/xogecoin#19509 Per-Peer Message Capture (troygiorshev)
- xogecoin/xogecoin#19763 Don't try to relay to the address' originator (vasild)
- xogecoin/xogecoin#19771 Replace enum CConnMan::NumConnections with enum class ConnectionDirection (luke-jr)
- xogecoin/xogecoin#19776 net, rpc: expose high bandwidth mode state via getpeerinfo (theStack)
- xogecoin/xogecoin#19832 Put disconnecting logs into BCLog::NET category (hebasto)
- xogecoin/xogecoin#19858 Periodically make block-relay connections and sync headers (sdaftuar)
- xogecoin/xogecoin#19884 No delay in adding fixed seeds if -dnsseed=0 and peers.dat is empty (dhruv)
- xogecoin/xogecoin#20079 Treat handshake misbehavior like unknown message (MarcoFalke)
- xogecoin/xogecoin#20138 Assume that SetCommonVersion is called at most once per peer (MarcoFalke)
- xogecoin/xogecoin#20162 p2p: declare Announcement::m_state as uint8_t, add getter/setter (jonatack)
- xogecoin/xogecoin#20197 Protect onions in AttemptToEvictConnection(), add eviction protection test coverage (jonatack)
- xogecoin/xogecoin#20210 assert `CNode::m_inbound_onion` is inbound in ctor, add getter, unit tests (jonatack)
- xogecoin/xogecoin#20228 addrman: Make addrman a top-level component (jnewbery)
- xogecoin/xogecoin#20234 Don't bind on 0.0.0.0 if binds are restricted to Tor (vasild)
- xogecoin/xogecoin#20477 Add unit testing of node eviction logic (practicalswift)
- xogecoin/xogecoin#20516 Well-defined CAddress disk serialization, and addrv2 anchors.dat (sipa)
- xogecoin/xogecoin#20557 addrman: Fix new table bucketing during unserialization (jnewbery)
- xogecoin/xogecoin#20561 Periodically clear `m_addr_known` (sdaftuar)
- xogecoin/xogecoin#20599 net processing: Tolerate sendheaders and sendcmpct messages before verack (jnewbery)
- xogecoin/xogecoin#20616 Check CJDNS address is valid (lontivero)
- xogecoin/xogecoin#20617 Remove `m_is_manual_connection` from CNodeState (ariard)
- xogecoin/xogecoin#20624 net processing: Remove nStartingHeight check from block relay (jnewbery)
- xogecoin/xogecoin#20651 Make p2p recv buffer timeout 20 minutes for all peers (jnewbery)
- xogecoin/xogecoin#20661 Only select from addrv2-capable peers for torv3 address relay (sipa)
- xogecoin/xogecoin#20685 Add I2P support using I2P SAM (vasild)
- xogecoin/xogecoin#20690 Clean up logging of outbound connection type (sdaftuar)
- xogecoin/xogecoin#20721 Move ping data to `net_processing` (jnewbery)
- xogecoin/xogecoin#20724 Cleanup of -debug=net log messages (ajtowns)
- xogecoin/xogecoin#20747 net processing: Remove dropmessagestest (jnewbery)
- xogecoin/xogecoin#20764 cli -netinfo peer connections dashboard updates 🎄 ✨ (jonatack)
- xogecoin/xogecoin#20788 add RAII socket and use it instead of bare SOCKET (vasild)
- xogecoin/xogecoin#20791 remove unused legacyWhitelisted in AcceptConnection() (jonatack)
- xogecoin/xogecoin#20816 Move RecordBytesSent() call out of `cs_vSend` lock (jnewbery)
- xogecoin/xogecoin#20845 Log to net debug in MaybeDiscourageAndDisconnect except for noban and manual peers (MarcoFalke)
- xogecoin/xogecoin#20864 Move SocketSendData lock annotation to header (MarcoFalke)
- xogecoin/xogecoin#20965 net, rpc:  return `NET_UNROUTABLE` as `not_publicly_routable`, automate helps (jonatack)
- xogecoin/xogecoin#20966 banman: save the banlist in a JSON format on disk (vasild)
- xogecoin/xogecoin#21015 Make all of `net_processing` (and some of net) use std::chrono types (dhruv)
- xogecoin/xogecoin#21029 xogecoin-cli: Correct docs (no "generatenewaddress" exists) (luke-jr)
- xogecoin/xogecoin#21148 Split orphan handling from `net_processing` into txorphanage (ajtowns)
- xogecoin/xogecoin#21162 Net Processing: Move RelayTransaction() into PeerManager (jnewbery)
- xogecoin/xogecoin#21167 make `CNode::m_inbound_onion` public, initialize explicitly (jonatack)
- xogecoin/xogecoin#21186 net/net processing: Move addr data into `net_processing` (jnewbery)
- xogecoin/xogecoin#21187 Net processing: Only call PushAddress() from `net_processing` (jnewbery)
- xogecoin/xogecoin#21198 Address outstanding review comments from PR20721 (jnewbery)
- xogecoin/xogecoin#21222 log: Clarify log message when file does not exist (MarcoFalke)
- xogecoin/xogecoin#21235 Clarify disconnect log message in ProcessGetBlockData, remove send bool (MarcoFalke)
- xogecoin/xogecoin#21236 Net processing: Extract `addr` send functionality into MaybeSendAddr() (jnewbery)
- xogecoin/xogecoin#21261 update inbound eviction protection for multiple networks, add I2P peers (jonatack)
- xogecoin/xogecoin#21328 net, refactor: pass uint16 CService::port as uint16 (jonatack)
- xogecoin/xogecoin#21387 Refactor sock to add I2P fuzz and unit tests (vasild)
- xogecoin/xogecoin#21395 Net processing: Remove unused CNodeState.address member (jnewbery)
- xogecoin/xogecoin#21407 i2p: limit the size of incoming messages (vasild)
- xogecoin/xogecoin#21506 p2p, refactor: make NetPermissionFlags an enum class (jonatack)
- xogecoin/xogecoin#21509 Don't send FEEFILTER in blocksonly mode (mzumsande)
- xogecoin/xogecoin#21560 Add Tor v3 hardcoded seeds (laanwj)
- xogecoin/xogecoin#21563 Restrict period when `cs_vNodes` mutex is locked (hebasto)
- xogecoin/xogecoin#21564 Avoid calling getnameinfo when formatting IPv4 addresses in CNetAddr::ToStringIP (practicalswift)
- xogecoin/xogecoin#21631 i2p: always check the return value of Sock::Wait() (vasild)
- xogecoin/xogecoin#21644 p2p, bugfix: use NetPermissions::HasFlag() in CConnman::Bind() (jonatack)
- xogecoin/xogecoin#21659 flag relevant Sock methods with [[nodiscard]] (vasild)
- xogecoin/xogecoin#21750 remove unnecessary check of `CNode::cs_vSend` (vasild)
- xogecoin/xogecoin#21756 Avoid calling `getnameinfo` when formatting IPv6 addresses in `CNetAddr::ToStringIP` (practicalswift)
- xogecoin/xogecoin#21775 Limit `m_block_inv_mutex` (MarcoFalke)
- xogecoin/xogecoin#21825 Add I2P hardcoded seeds (jonatack)
- xogecoin/xogecoin#21843 p2p, rpc: enable GetAddr, GetAddresses, and getnodeaddresses by network (jonatack)
- xogecoin/xogecoin#21845 net processing: Don't require locking `cs_main` before calling RelayTransactions() (jnewbery)
- xogecoin/xogecoin#21872 Sanitize message type for logging (laanwj)
- xogecoin/xogecoin#21914 Use stronger AddLocal() for our I2P address (vasild)
- xogecoin/xogecoin#21985 Return IPv6 scope id in `CNetAddr::ToStringIP()` (laanwj)
- xogecoin/xogecoin#21992 Remove -feefilter option (amadeuszpawlik)
- xogecoin/xogecoin#21996 Pass strings to NetPermissions::TryParse functions by const ref (jonatack)
- xogecoin/xogecoin#22013 ignore block-relay-only peers when skipping DNS seed (ajtowns)
- xogecoin/xogecoin#22050 Remove tor v2 support (jonatack)
- xogecoin/xogecoin#22096 AddrFetch - don't disconnect on self-announcements (mzumsande)
- xogecoin/xogecoin#22141 net processing: Remove hash and fValidatedHeaders from QueuedBlock (jnewbery)
- xogecoin/xogecoin#22144 Randomize message processing peer order (sipa)
- xogecoin/xogecoin#22147 Protect last outbound HB compact block peer (sdaftuar)
- xogecoin/xogecoin#22179 Torv2 removal followups (vasild)
- xogecoin/xogecoin#22211 Relay I2P addresses even if not reachable (by us) (vasild)
- xogecoin/xogecoin#22284 Performance improvements to ProtectEvictionCandidatesByRatio() (jonatack)
- xogecoin/xogecoin#22387 Rate limit the processing of rumoured addresses (sipa)
- xogecoin/xogecoin#22455 addrman: detect on-disk corrupted nNew and nTried during unserialization (vasild)

### Wallet
- xogecoin/xogecoin#15710 Catch `ios_base::failure` specifically (Bushstar)
- xogecoin/xogecoin#16546 External signer support - Wallet Box edition (Sjors)
- xogecoin/xogecoin#17331 Use effective values throughout coin selection (achow101)
- xogecoin/xogecoin#18418 Increase `OUTPUT_GROUP_MAX_ENTRIES` to 100 (fjahr)
- xogecoin/xogecoin#18842 Mark replaced tx to not be in the mempool anymore (MarcoFalke)
- xogecoin/xogecoin#19136 Add `parent_desc` to `getaddressinfo` (achow101)
- xogecoin/xogecoin#19137 wallettool: Add dump and createfromdump commands (achow101)
- xogecoin/xogecoin#19651 `importdescriptor`s update existing (S3RK)
- xogecoin/xogecoin#20040 Refactor OutputGroups to handle fees and spending eligibility on grouping (achow101)
- xogecoin/xogecoin#20202 Make BDB support optional (achow101)
- xogecoin/xogecoin#20226, xogecoin/xogecoin#21277, - xogecoin/xogecoin#21063 Add `listdescriptors` command (S3RK)
- xogecoin/xogecoin#20267 Disable and fix tests for when BDB is not compiled (achow101)
- xogecoin/xogecoin#20275 List all wallets in non-SQLite and non-BDB builds (ryanofsky)
- xogecoin/xogecoin#20365 wallettool: Add parameter to create descriptors wallet (S3RK)
- xogecoin/xogecoin#20403 `upgradewallet` fixes, improvements, test coverage (jonatack)
- xogecoin/xogecoin#20448 `unloadwallet`: Allow specifying `wallet_name` param matching RPC endpoint wallet (luke-jr)
- xogecoin/xogecoin#20536 Error with "Transaction too large" if the funded tx will end up being too large after signing (achow101)
- xogecoin/xogecoin#20687 Add missing check for -descriptors wallet tool option (MarcoFalke)
- xogecoin/xogecoin#20952 Add BerkeleyDB version sanity check at init time (laanwj)
- xogecoin/xogecoin#21127 Load flags before everything else (Sjors)
- xogecoin/xogecoin#21141 Add new format string placeholders for walletnotify (maayank)
- xogecoin/xogecoin#21238 A few descriptor improvements to prepare for Taproot support (sipa)
- xogecoin/xogecoin#21302 `createwallet` examples for descriptor wallets (S3RK)
- xogecoin/xogecoin#21329 descriptor wallet: Cache last hardened xpub and use in normalized descriptors (achow101)
- xogecoin/xogecoin#21365 Basic Taproot signing support for descriptor wallets (sipa)
- xogecoin/xogecoin#21417 Misc external signer improvement and HWI 2 support (Sjors)
- xogecoin/xogecoin#21467 Move external signer out of wallet module (Sjors)
- xogecoin/xogecoin#21572 Fix wrong wallet RPC context set after #21366 (ryanofsky)
- xogecoin/xogecoin#21574 Drop JSONRPCRequest constructors after #21366 (ryanofsky)
- xogecoin/xogecoin#21666 Miscellaneous external signer changes (fanquake)
- xogecoin/xogecoin#21759 Document coin selection code (glozow)
- xogecoin/xogecoin#21786 Ensure sat/vB feerates are in range (mantissa of 3) (jonatack)
- xogecoin/xogecoin#21944 Fix issues when `walletdir` is root directory (prayank23)
- xogecoin/xogecoin#22042 Replace size/weight estimate tuple with struct for named fields (instagibbs)
- xogecoin/xogecoin#22051 Basic Taproot derivation support for descriptors (sipa)
- xogecoin/xogecoin#22154 Add OutputType::BECH32M and related wallet support for fetching bech32m addresses (achow101)
- xogecoin/xogecoin#22156 Allow tr() import only when Taproot is active (achow101)
- xogecoin/xogecoin#22166 Add support for inferring tr() descriptors (sipa)
- xogecoin/xogecoin#22173 Do not load external signers wallets when unsupported (achow101)
- xogecoin/xogecoin#22308 Add missing BlockUntilSyncedToCurrentChain (MarcoFalke)
- xogecoin/xogecoin#22334 Do not spam about non-existent spk managers (S3RK)
- xogecoin/xogecoin#22379 Erase spkmans rather than setting to nullptr (achow101)
- xogecoin/xogecoin#22421 Make IsSegWitOutput return true for taproot outputs (sipa)
- xogecoin/xogecoin#22461 Change ScriptPubKeyMan::Upgrade default to True (achow101)
- xogecoin/xogecoin#22492 Reorder locks in dumpwallet to avoid lock order assertion (achow101)
- xogecoin/xogecoin#22686 Use GetSelectionAmount in ApproximateBestSubset (achow101)

### RPC and other APIs
- xogecoin/xogecoin#18335, xogecoin/xogecoin#21484 cli: Print useful error if xogecoind rpc work queue exceeded (LarryRuane)
- xogecoin/xogecoin#18466 Fix invalid parameter error codes for `{sign,verify}message` RPCs (theStack)
- xogecoin/xogecoin#18772 Calculate fees in `getblock` using BlockUndo data (robot-visions)
- xogecoin/xogecoin#19033 http: Release work queue after event base finish (promag)
- xogecoin/xogecoin#19055 Add MuHash3072 implementation (fjahr)
- xogecoin/xogecoin#19145 Add `hash_type` MUHASH for gettxoutsetinfo (fjahr)
- xogecoin/xogecoin#19847 Avoid duplicate set lookup in `gettxoutproof` (promag)
- xogecoin/xogecoin#20286 Deprecate `addresses` and `reqSigs` from RPC outputs (mjdietzx)
- xogecoin/xogecoin#20459 Fail to return undocumented return values (MarcoFalke)
- xogecoin/xogecoin#20461 Validate `-rpcauth` arguments (promag)
- xogecoin/xogecoin#20556 Properly document return values (`submitblock`, `gettxout`, `getblocktemplate`, `scantxoutset`) (MarcoFalke)
- xogecoin/xogecoin#20755 Remove deprecated fields from `getpeerinfo` (amitiuttarwar)
- xogecoin/xogecoin#20832 Better error messages for invalid addresses (eilx2)
- xogecoin/xogecoin#20867 Support up to 20 keys for multisig under Segwit context (darosior)
- xogecoin/xogecoin#20877 cli: `-netinfo` user help and argument parsing improvements (jonatack)
- xogecoin/xogecoin#20891 Remove deprecated bumpfee behavior (achow101)
- xogecoin/xogecoin#20916 Return wtxid from `testmempoolaccept` (MarcoFalke)
- xogecoin/xogecoin#20917 Add missing signet mentions in network name lists (theStack)
- xogecoin/xogecoin#20941 Document `RPC_TRANSACTION_ALREADY_IN_CHAIN` exception (jarolrod)
- xogecoin/xogecoin#20944 Return total fee in `getmempoolinfo` (MarcoFalke)
- xogecoin/xogecoin#20964 Add specific error code for "wallet already loaded" (laanwj)
- xogecoin/xogecoin#21053 Document {previous,next}blockhash as optional (theStack)
- xogecoin/xogecoin#21056 Add a `-rpcwaittimeout` parameter to limit time spent waiting (cdecker)
- xogecoin/xogecoin#21192 cli: Treat high detail levels as maximum in `-netinfo` (laanwj)
- xogecoin/xogecoin#21311 Document optional fields for `getchaintxstats` result (theStack)
- xogecoin/xogecoin#21359 `include_unsafe` option for fundrawtransaction (t-bast)
- xogecoin/xogecoin#21426 Remove `scantxoutset` EXPERIMENTAL warning (jonatack)
- xogecoin/xogecoin#21544 Missing doc updates for bumpfee psbt update (MarcoFalke)
- xogecoin/xogecoin#21594 Add `network` field to `getnodeaddresses` (jonatack)
- xogecoin/xogecoin#21595, xogecoin/xogecoin#21753 cli: Create `-addrinfo` (jonatack)
- xogecoin/xogecoin#21602 Add additional ban time fields to `listbanned` (jarolrod)
- xogecoin/xogecoin#21679 Keep default argument value in correct type (promag)
- xogecoin/xogecoin#21718 Improve error message for `getblock` invalid datatype (klementtan)
- xogecoin/xogecoin#21913 RPCHelpMan fixes (kallewoof)
- xogecoin/xogecoin#22021 `bumpfee`/`psbtbumpfee` fixes and updates (jonatack)
- xogecoin/xogecoin#22043 `addpeeraddress` test coverage, code simplify/constness (jonatack)
- xogecoin/xogecoin#22327 cli: Avoid truncating `-rpcwaittimeout` (MarcoFalke)

### GUI
- xogecoin/xogecoin#18948 Call setParent() in the parent's context (hebasto)
- xogecoin/xogecoin#20482 Add depends qt fix for ARM macs (jonasschnelli)
- xogecoin/xogecoin#21836 scripted-diff: Replace three dots with ellipsis in the ui strings (hebasto)
- xogecoin/xogecoin#21935 Enable external signer support for GUI builds (Sjors)
- xogecoin/xogecoin#22133 Make QWindowsVistaStylePlugin available again (regression) (hebasto)
- xogecoin-core/gui#4 UI external signer support (e.g. hardware wallet) (Sjors)
- xogecoin-core/gui#13 Hide peer detail view if multiple are selected (promag)
- xogecoin-core/gui#18 Add peertablesortproxy module (hebasto)
- xogecoin-core/gui#21 Improve pruning tooltip (fluffypony, QogecoinErrorLog)
- xogecoin-core/gui#72 Log static plugins meta data and used style (hebasto)
- xogecoin-core/gui#79 Embed monospaced font (hebasto)
- xogecoin-core/gui#85 Remove unused "What's This" button in dialogs on Windows OS (hebasto)
- xogecoin-core/gui#115 Replace "Hide tray icon" option with positive "Show tray icon" one (hebasto)
- xogecoin-core/gui#118 Remove BDB version from the Information tab (hebasto)
- xogecoin-core/gui#121 Early subscribe core signals in transaction table model (promag)
- xogecoin-core/gui#123 Do not accept command while executing another one (hebasto)
- xogecoin-core/gui#125 Enable changing the autoprune block space size in intro dialog (luke-jr)
- xogecoin-core/gui#138 Unlock encrypted wallet "OK" button bugfix (mjdietzx)
- xogecoin-core/gui#139 doc: Improve gui/src/qt README.md (jarolrod)
- xogecoin-core/gui#154 Support macOS Dark mode (goums, Uplab)
- xogecoin-core/gui#162 Add network to peers window and peer details (jonatack)
- xogecoin-core/gui#163, xogecoin-core/gui#180 Peer details: replace Direction with Connection Type (jonatack)
- xogecoin-core/gui#164 Handle peer addition/removal in a right way (hebasto)
- xogecoin-core/gui#165 Save QSplitter state in QSettings (hebasto)
- xogecoin-core/gui#173 Follow Qt docs when implementing rowCount and columnCount (hebasto)
- xogecoin-core/gui#179 Add Type column to peers window, update peer details name/tooltip (jonatack)
- xogecoin-core/gui#186 Add information to "Confirm fee bump" window (prayank23)
- xogecoin-core/gui#189 Drop workaround for QTBUG-42503 which was fixed in Qt 5.5.0 (prusnak)
- xogecoin-core/gui#194 Save/restore RPCConsole geometry only for window (hebasto)
- xogecoin-core/gui#202 Fix right panel toggle in peers tab (RandyMcMillan)
- xogecoin-core/gui#203 Display plain "Inbound" in peer details (jonatack)
- xogecoin-core/gui#204 Drop buggy TableViewLastColumnResizingFixer class (hebasto)
- xogecoin-core/gui#205, xogecoin-core/gui#229 Save/restore TransactionView and recentRequestsView tables column sizes (hebasto)
- xogecoin-core/gui#206 Display fRelayTxes and `bip152_highbandwidth_{to, from}` in peer details (jonatack)
- xogecoin-core/gui#213 Add Copy Address Action to Payment Requests (jarolrod)
- xogecoin-core/gui#214 Disable requests context menu actions when appropriate (jarolrod)
- xogecoin-core/gui#217 Make warning label look clickable (jarolrod)
- xogecoin-core/gui#219 Prevent the main window popup menu (hebasto)
- xogecoin-core/gui#220 Do not translate file extensions (hebasto)
- xogecoin-core/gui#221 RPCConsole translatable string fixes and improvements (jonatack)
- xogecoin-core/gui#226 Add "Last Block" and "Last Tx" rows to peer details area (jonatack)
- xogecoin-core/gui#233 qt test: Don't bind to regtest port (achow101)
- xogecoin-core/gui#243 Fix issue when disabling the auto-enabled blank wallet checkbox (jarolrod)
- xogecoin-core/gui#246 Revert "qt: Use "fusion" style on macOS Big Sur with old Qt" (hebasto)
- xogecoin-core/gui#248 For values of "Bytes transferred" and "Bytes/s" with 1000-based prefix names use 1000-based divisor instead of 1024-based (wodry)
- xogecoin-core/gui#251 Improve URI/file handling message (hebasto)
- xogecoin-core/gui#256 Save/restore column sizes of the tables in the Peers tab (hebasto)
- xogecoin-core/gui#260 Handle exceptions isntead of crash (hebasto)
- xogecoin-core/gui#263 Revamp context menus (hebasto)
- xogecoin-core/gui#271 Don't clear console prompt when font resizing (jarolrod)
- xogecoin-core/gui#275 Support runtime appearance adjustment on macOS (hebasto)
- xogecoin-core/gui#276 Elide long strings in their middle in the Peers tab (hebasto)
- xogecoin-core/gui#281 Set shortcuts for console's resize buttons (jarolrod)
- xogecoin-core/gui#293 Enable wordWrap for Services (RandyMcMillan)
- xogecoin-core/gui#296 Do not use QObject::tr plural syntax for numbers with a unit symbol (hebasto)
- xogecoin-core/gui#297 Avoid unnecessary translations (hebasto)
- xogecoin-core/gui#298 Peertableview alternating row colors (RandyMcMillan)
- xogecoin-core/gui#300 Remove progress bar on modal overlay (brunoerg)
- xogecoin-core/gui#309 Add access to the Peers tab from the network icon (hebasto)
- xogecoin-core/gui#311 Peers Window rename 'Peer id' to 'Peer' (jarolrod)
- xogecoin-core/gui#313 Optimize string concatenation by default (hebasto)
- xogecoin-core/gui#325 Align numbers in the "Peer Id" column to the right (hebasto)
- xogecoin-core/gui#329 Make console buttons look clickable (jarolrod)
- xogecoin-core/gui#330 Allow prompt icon to be colorized (jarolrod)
- xogecoin-core/gui#331 Make RPC console welcome message translation-friendly (hebasto)
- xogecoin-core/gui#332 Replace disambiguation strings with translator comments (hebasto)
- xogecoin-core/gui#335 test: Use QSignalSpy instead of QEventLoop (jarolrod)
- xogecoin-core/gui#343 Improve the GUI responsiveness when progress dialogs are used (hebasto)
- xogecoin-core/gui#361 Fix GUI segfault caused by xogecoin/xogecoin#22216 (ryanofsky)
- xogecoin-core/gui#362 Add keyboard shortcuts to context menus (luke-jr)
- xogecoin-core/gui#366 Dark Mode fixes/portability (luke-jr)
- xogecoin-core/gui#375 Emit dataChanged signal to dynamically re-sort Peers table (hebasto)
- xogecoin-core/gui#393 Fix regression in "Encrypt Wallet" menu item (hebasto)
- xogecoin-core/gui#396 Ensure external signer option remains disabled without signers (achow101)
- xogecoin-core/gui#406 Handle new added plurals in `xogecoin_en.ts` (hebasto)

### Build system
- xogecoin/xogecoin#17227 Add Android packaging support (icota)
- xogecoin/xogecoin#17920 guix: Build support for macOS (dongcarl)
- xogecoin/xogecoin#18298 Fix Qt processing of configure script for depends with DEBUG=1 (hebasto)
- xogecoin/xogecoin#19160 multiprocess: Add basic spawn and IPC support (ryanofsky)
- xogecoin/xogecoin#19504 Bump minimum python version to 3.6 (ajtowns)
- xogecoin/xogecoin#19522 fix building libconsensus with reduced exports for Darwin targets (fanquake)
- xogecoin/xogecoin#19683 Pin clang search paths for darwin host (dongcarl)
- xogecoin/xogecoin#19764 Split boost into build/host packages + bump + cleanup (dongcarl)
- xogecoin/xogecoin#19817 libtapi 1100.0.11 (fanquake)
- xogecoin/xogecoin#19846 enable unused member function diagnostic (Zero-1729)
- xogecoin/xogecoin#19867 Document and cleanup Qt hacks (fanquake)
- xogecoin/xogecoin#20046 Set `CMAKE_INSTALL_RPATH` for native packages (ryanofsky)
- xogecoin/xogecoin#20223 Drop the leading 0 from the version number (achow101)
- xogecoin/xogecoin#20333 Remove `native_biplist` dependency (fanquake)
- xogecoin/xogecoin#20353 configure: Support -fdebug-prefix-map and -fmacro-prefix-map (ajtowns)
- xogecoin/xogecoin#20359 Various config.site.in improvements and linting (dongcarl)
- xogecoin/xogecoin#20413 Require C++17 compiler (MarcoFalke)
- xogecoin/xogecoin#20419 Set minimum supported macOS to 10.14 (fanquake)
- xogecoin/xogecoin#20421 miniupnpc 2.2.2 (fanquake)
- xogecoin/xogecoin#20422 Mac deployment unification (fanquake)
- xogecoin/xogecoin#20424 Update univalue subtree (MarcoFalke)
- xogecoin/xogecoin#20449 Fix Windows installer build (achow101)
- xogecoin/xogecoin#20468 Warn when generating man pages for binaries built from a dirty branch (tylerchambers)
- xogecoin/xogecoin#20469 Avoid secp256k1.h include from system (dergoegge)
- xogecoin/xogecoin#20470 Replace genisoimage with xorriso (dongcarl)
- xogecoin/xogecoin#20471 Use C++17 in depends (fanquake)
- xogecoin/xogecoin#20496 Drop unneeded macOS framework dependencies (hebasto)
- xogecoin/xogecoin#20520 Do not force Precompiled Headers (PCH) for building Qt on Linux (hebasto)
- xogecoin/xogecoin#20549 Support make src/xogecoin-node and src/xogecoin-gui (promag)
- xogecoin/xogecoin#20565 Ensure PIC build for bdb on Android (BlockMechanic)
- xogecoin/xogecoin#20594 Fix getauxval calls in randomenv.cpp (jonasschnelli)
- xogecoin/xogecoin#20603 Update crc32c subtree (MarcoFalke)
- xogecoin/xogecoin#20609 configure: output notice that test binary is disabled by fuzzing (apoelstra)
- xogecoin/xogecoin#20619 guix: Quality of life improvements (dongcarl)
- xogecoin/xogecoin#20629 Improve id string robustness (dongcarl)
- xogecoin/xogecoin#20641 Use Qt top-level build facilities (hebasto)
- xogecoin/xogecoin#20650 Drop workaround for a fixed bug in Qt build system (hebasto)
- xogecoin/xogecoin#20673 Use more legible qmake commands in qt package (hebasto)
- xogecoin/xogecoin#20684 Define .INTERMEDIATE target once only (hebasto)
- xogecoin/xogecoin#20720 more robustly check for fcf-protection support (fanquake)
- xogecoin/xogecoin#20734 Make platform-specific targets available for proper platform builds only (hebasto)
- xogecoin/xogecoin#20936 build fuzz tests by default (danben)
- xogecoin/xogecoin#20937 guix: Make nsis reproducible by respecting SOURCE-DATE-EPOCH (dongcarl)
- xogecoin/xogecoin#20938 fix linking against -latomic when building for riscv (fanquake)
- xogecoin/xogecoin#20939 fix `RELOC_SECTION` security check for xogecoin-util (fanquake)
- xogecoin/xogecoin#20963 gitian-linux: Build binaries for 64-bit POWER (continued) (laanwj)
- xogecoin/xogecoin#21036 gitian: Bump descriptors to focal for 22.0 (fanquake)
- xogecoin/xogecoin#21045 Adds switch to enable/disable randomized base address in MSVC builds (EthanHeilman)
- xogecoin/xogecoin#21065 make macOS HOST in download-osx generic (fanquake)
- xogecoin/xogecoin#21078 guix: only download sources for hosts being built (fanquake)
- xogecoin/xogecoin#21116 Disable --disable-fuzz-binary for gitian/guix builds (hebasto)
- xogecoin/xogecoin#21182 remove mostly pointless `BOOST_PROCESS` macro (fanquake)
- xogecoin/xogecoin#21205 actually fail when Boost is missing (fanquake)
- xogecoin/xogecoin#21209 use newer source for libnatpmp (fanquake)
- xogecoin/xogecoin#21226 Fix fuzz binary compilation under windows (danben)
- xogecoin/xogecoin#21231 Add /opt/homebrew to path to look for boost libraries (fyquah)
- xogecoin/xogecoin#21239 guix: Add codesignature attachment support for osx+win (dongcarl)
- xogecoin/xogecoin#21250 Make `HAVE_O_CLOEXEC` available outside LevelDB (bugfix) (theStack)
- xogecoin/xogecoin#21272 guix: Passthrough `SDK_PATH` into container (dongcarl)
- xogecoin/xogecoin#21274 assumptions:  Assume C++17 (fanquake)
- xogecoin/xogecoin#21286 Bump minimum Qt version to 5.9.5 (hebasto)
- xogecoin/xogecoin#21298 guix: Bump time-machine, glibc, and linux-headers (dongcarl)
- xogecoin/xogecoin#21304 guix: Add guix-clean script + establish gc-root for container profiles (dongcarl)
- xogecoin/xogecoin#21320 fix libnatpmp macos cross compile (fanquake)
- xogecoin/xogecoin#21321 guix: Add curl to required tool list (hebasto)
- xogecoin/xogecoin#21333 set Unicode true for NSIS installer (fanquake)
- xogecoin/xogecoin#21339 Make `AM_CONDITIONAL([ENABLE_EXTERNAL_SIGNER])` unconditional (hebasto)
- xogecoin/xogecoin#21349 Fix fuzz-cuckoocache cross-compiling with DEBUG=1 (hebasto)
- xogecoin/xogecoin#21354 build, doc: Drop no longer required packages from macOS cross-compiling dependencies (hebasto)
- xogecoin/xogecoin#21363 build, qt: Improve Qt static plugins/libs check code (hebasto)
- xogecoin/xogecoin#21375 guix: Misc feedback-based fixes + hier restructuring (dongcarl)
- xogecoin/xogecoin#21376 Qt 5.12.10 (fanquake)
- xogecoin/xogecoin#21382 Clean remnants of QTBUG-34748 fix (hebasto)
- xogecoin/xogecoin#21400 Fix regression introduced in #21363 (hebasto)
- xogecoin/xogecoin#21403 set --build when configuring packages in depends (fanquake)
- xogecoin/xogecoin#21421 don't try and use -fstack-clash-protection on Windows (fanquake)
- xogecoin/xogecoin#21423 Cleanups and follow ups after bumping Qt to 5.12.10 (hebasto)
- xogecoin/xogecoin#21427 Fix `id_string` invocations (dongcarl)
- xogecoin/xogecoin#21430 Add -Werror=implicit-fallthrough compile flag (hebasto)
- xogecoin/xogecoin#21457 Split libtapi and clang out of `native_cctools` (fanquake)
- xogecoin/xogecoin#21462 guix: Add guix-{attest,verify} scripts (dongcarl)
- xogecoin/xogecoin#21495 build, qt: Fix static builds on macOS Big Sur (hebasto)
- xogecoin/xogecoin#21497 Do not opt-in unused CoreWLAN stuff in depends for macOS (hebasto)
- xogecoin/xogecoin#21543 Enable safe warnings for msvc builds (hebasto)
- xogecoin/xogecoin#21565 Make `xogecoin_qt.m4` more generic (fanquake)
- xogecoin/xogecoin#21610 remove -Wdeprecated-register from NOWARN flags (fanquake)
- xogecoin/xogecoin#21613 enable -Wdocumentation (fanquake)
- xogecoin/xogecoin#21629 Fix configuring when building depends with `NO_BDB=1` (fanquake)
- xogecoin/xogecoin#21654 build, qt: Make Qt rcc output always deterministic (hebasto)
- xogecoin/xogecoin#21655 build, qt: No longer need to set `QT_RCC_TEST=1` for determinism (hebasto)
- xogecoin/xogecoin#21658 fix make deploy for arm64-darwin (sgulls)
- xogecoin/xogecoin#21694 Use XLIFF file to provide more context to Transifex translators (hebasto)
- xogecoin/xogecoin#21708, xogecoin/xogecoin#21593 Drop pointless sed commands (hebasto)
- xogecoin/xogecoin#21731 Update msvc build to use Qt5.12.10 binaries (sipsorcery)
- xogecoin/xogecoin#21733 Re-add command to install vcpkg (dplusplus1024)
- xogecoin/xogecoin#21793 Use `-isysroot` over `--sysroot` on macOS (fanquake)
- xogecoin/xogecoin#21869 Add missing `-D_LIBCPP_DEBUG=1` to debug flags (MarcoFalke)
- xogecoin/xogecoin#21889 macho: check for control flow instrumentation (fanquake)
- xogecoin/xogecoin#21920 Improve macro for testing -latomic requirement (MarcoFalke)
- xogecoin/xogecoin#21991 libevent 2.1.12-stable (fanquake)
- xogecoin/xogecoin#22054 Bump Qt version to 5.12.11 (hebasto)
- xogecoin/xogecoin#22063 Use Qt archive of the same version as the compiled binaries (hebasto)
- xogecoin/xogecoin#22070 Don't use cf-protection when targeting arm-apple-darwin (fanquake)
- xogecoin/xogecoin#22071 Latest config.guess and config.sub (fanquake)
- xogecoin/xogecoin#22075 guix: Misc leftover usability improvements (dongcarl)
- xogecoin/xogecoin#22123 Fix qt.mk for mac arm64 (promag)
- xogecoin/xogecoin#22174 build, qt: Fix libraries linking order for Linux hosts (hebasto)
- xogecoin/xogecoin#22182 guix: Overhaul how guix-{attest,verify} works and hierarchy (dongcarl)
- xogecoin/xogecoin#22186 build, qt: Fix compiling qt package in depends with GCC 11 (hebasto)
- xogecoin/xogecoin#22199 macdeploy: minor fixups and simplifications (fanquake)
- xogecoin/xogecoin#22230 Fix MSVC linker /SubSystem option for xogecoin-qt.exe (hebasto)
- xogecoin/xogecoin#22234 Mark print-% target as phony (dgoncharov)
- xogecoin/xogecoin#22238 improve detection of eBPF support (fanquake)
- xogecoin/xogecoin#22258 Disable deprecated-copy warning only when external warnings are enabled (MarcoFalke)
- xogecoin/xogecoin#22320 set minimum required Boost to 1.64.0 (fanquake)
- xogecoin/xogecoin#22348 Fix cross build for Windows with Boost Process (hebasto)
- xogecoin/xogecoin#22365 guix: Avoid relying on newer symbols by rebasing our cross toolchains on older glibcs (dongcarl)
- xogecoin/xogecoin#22381 guix: Test security-check sanity before performing them (with macOS) (fanquake)
- xogecoin/xogecoin#22405 Remove --enable-glibc-back-compat from Guix build (fanquake)
- xogecoin/xogecoin#22406 Remove --enable-determinism configure option (fanquake)
- xogecoin/xogecoin#22410 Avoid GCC 7.1 ABI change warning in guix build (sipa)
- xogecoin/xogecoin#22436 use aarch64 Clang if cross-compiling for darwin on aarch64 (fanquake)
- xogecoin/xogecoin#22465 guix: Pin kernel-header version, time-machine to upstream 1.3.0 commit (dongcarl)
- xogecoin/xogecoin#22511 guix: Silence `getent(1)` invocation, doc fixups (dongcarl)
- xogecoin/xogecoin#22531 guix: Fixes to guix-{attest,verify} (achow101)
- xogecoin/xogecoin#22642 release: Release with separate sha256sums and sig files (dongcarl)
- xogecoin/xogecoin#22685 clientversion: No suffix `#if CLIENT_VERSION_IS_RELEASE` (dongcarl)
- xogecoin/xogecoin#22713 Fix build with Boost 1.77.0 (sizeofvoid)

### Tests and QA
- xogecoin/xogecoin#14604 Add test and refactor `feature_block.py` (sanket1729)
- xogecoin/xogecoin#17556 Change `feature_config_args.py` not to rely on strange regtest=0 behavior (ryanofsky)
- xogecoin/xogecoin#18795 wallet issue with orphaned rewards (domob1812)
- xogecoin/xogecoin#18847 compressor: Use a prevector in CompressScript serialization (jb55)
- xogecoin/xogecoin#19259 fuzz: Add fuzzing harness for LoadMempool(…) and DumpMempool(…) (practicalswift)
- xogecoin/xogecoin#19315 Allow outbound & block-relay-only connections in functional tests. (amitiuttarwar)
- xogecoin/xogecoin#19698 Apply strict verification flags for transaction tests and assert backwards compatibility (glozow)
- xogecoin/xogecoin#19801 Check for all possible `OP_CLTV` fail reasons in `feature_cltv.py` (BIP 65) (theStack)
- xogecoin/xogecoin#19893 Remove or explain syncwithvalidationinterfacequeue (MarcoFalke)
- xogecoin/xogecoin#19972 fuzz: Add fuzzing harness for node eviction logic (practicalswift)
- xogecoin/xogecoin#19982 Fix inconsistent lock order in `wallet_tests/CreateWallet` (hebasto)
- xogecoin/xogecoin#20000 Fix creation of "std::string"s with \0s (vasild)
- xogecoin/xogecoin#20047 Use `wait_for_{block,header}` helpers in `p2p_fingerprint.py` (theStack)
- xogecoin/xogecoin#20171 Add functional test `test_txid_inv_delay` (ariard)
- xogecoin/xogecoin#20189 Switch to BIP341's suggested scheme for outputs without script (sipa)
- xogecoin/xogecoin#20248 Fix length of R check in `key_signature_tests` (dgpv)
- xogecoin/xogecoin#20276, xogecoin/xogecoin#20385, xogecoin/xogecoin#20688, xogecoin/xogecoin#20692 Run various mempool tests even with wallet disabled (mjdietzx)
- xogecoin/xogecoin#20323 Create or use existing properly initialized NodeContexts (dongcarl)
- xogecoin/xogecoin#20354 Add `feature_taproot.py --previous_release` (MarcoFalke)
- xogecoin/xogecoin#20370 fuzz: Version handshake (MarcoFalke)
- xogecoin/xogecoin#20377 fuzz: Fill various small fuzzing gaps (practicalswift)
- xogecoin/xogecoin#20425 fuzz: Make CAddrMan fuzzing harness deterministic (practicalswift)
- xogecoin/xogecoin#20430 Sanitizers: Add suppression for unsigned-integer-overflow in libstdc++ (jonasschnelli)
- xogecoin/xogecoin#20437 fuzz: Avoid time-based "non-determinism" in fuzzing harnesses by using mocked GetTime() (practicalswift)
- xogecoin/xogecoin#20458 Add `is_bdb_compiled` helper (Sjors)
- xogecoin/xogecoin#20466 Fix intermittent `p2p_fingerprint` issue (MarcoFalke)
- xogecoin/xogecoin#20472 Add testing of ParseInt/ParseUInt edge cases with leading +/-/0:s (practicalswift)
- xogecoin/xogecoin#20507 sync: print proper lock order location when double lock is detected (vasild)
- xogecoin/xogecoin#20522 Fix sync issue in `disconnect_p2ps` (amitiuttarwar)
- xogecoin/xogecoin#20524 Move `MIN_VERSION_SUPPORTED` to p2p.py (jnewbery)
- xogecoin/xogecoin#20540 Fix `wallet_multiwallet` issue on windows (MarcoFalke)
- xogecoin/xogecoin#20560 fuzz: Link all targets once (MarcoFalke)
- xogecoin/xogecoin#20567 Add option to git-subtree-check to do full check, add help (laanwj)
- xogecoin/xogecoin#20569 Fix intermittent `wallet_multiwallet` issue with `got_loading_error` (MarcoFalke)
- xogecoin/xogecoin#20613 Use Popen.wait instead of RPC in `assert_start_raises_init_error` (MarcoFalke)
- xogecoin/xogecoin#20663 fuzz: Hide `script_assets_test_minimizer` (MarcoFalke)
- xogecoin/xogecoin#20674 fuzz: Call SendMessages after ProcessMessage to increase coverage (MarcoFalke)
- xogecoin/xogecoin#20683 Fix restart node race (MarcoFalke)
- xogecoin/xogecoin#20686 fuzz: replace CNode code with fuzz/util.h::ConsumeNode() (jonatack)
- xogecoin/xogecoin#20733 Inline non-member functions with body in fuzzing headers (pstratem)
- xogecoin/xogecoin#20737 Add missing assignment in `mempool_resurrect.py` (MarcoFalke)
- xogecoin/xogecoin#20745 Correct `epoll_ctl` data race suppression (hebasto)
- xogecoin/xogecoin#20748 Add race:SendZmqMessage tsan suppression (MarcoFalke)
- xogecoin/xogecoin#20760 Set correct nValue for multi-op-return policy check (MarcoFalke)
- xogecoin/xogecoin#20761 fuzz: Check that `NULL_DATA` is unspendable (MarcoFalke)
- xogecoin/xogecoin#20765 fuzz: Check that certain script TxoutType are nonstandard (mjdietzx)
- xogecoin/xogecoin#20772 fuzz: Bolster ExtractDestination(s) checks (mjdietzx)
- xogecoin/xogecoin#20789 fuzz: Rework strong and weak net enum fuzzing (MarcoFalke)
- xogecoin/xogecoin#20828 fuzz: Introduce CallOneOf helper to replace switch-case (MarcoFalke)
- xogecoin/xogecoin#20839 fuzz: Avoid extraneous copy of input data, using Span<> (MarcoFalke)
- xogecoin/xogecoin#20844 Add sanitizer suppressions for AMD EPYC CPUs (MarcoFalke)
- xogecoin/xogecoin#20857 Update documentation in `feature_csv_activation.py` (PiRK)
- xogecoin/xogecoin#20876 Replace getmempoolentry with testmempoolaccept in MiniWallet (MarcoFalke)
- xogecoin/xogecoin#20881 fuzz: net permission flags in net processing (MarcoFalke)
- xogecoin/xogecoin#20882 fuzz: Add missing muhash registration (MarcoFalke)
- xogecoin/xogecoin#20908 fuzz: Use mocktime in `process_message*` fuzz targets (MarcoFalke)
- xogecoin/xogecoin#20915 fuzz: Fail if message type is not fuzzed (MarcoFalke)
- xogecoin/xogecoin#20946 fuzz: Consolidate fuzzing TestingSetup initialization (dongcarl)
- xogecoin/xogecoin#20954 Declare `nodes` type `in test_framework.py` (kiminuo)
- xogecoin/xogecoin#20955 Fix `get_previous_releases.py` for aarch64 (MarcoFalke)
- xogecoin/xogecoin#20969 check that getblockfilter RPC fails without block filter index (theStack)
- xogecoin/xogecoin#20971 Work around libFuzzer deadlock (MarcoFalke)
- xogecoin/xogecoin#20993 Store subversion (user agent) as string in `msg_version` (theStack)
- xogecoin/xogecoin#20995 fuzz: Avoid initializing version to less than `MIN_PEER_PROTO_VERSION` (MarcoFalke)
- xogecoin/xogecoin#20998 Fix BlockToJsonVerbose benchmark (martinus)
- xogecoin/xogecoin#21003 Move MakeNoLogFileContext to `libtest_util`, and use it in bench (MarcoFalke)
- xogecoin/xogecoin#21008 Fix zmq test flakiness, improve speed (theStack)
- xogecoin/xogecoin#21023 fuzz: Disable shuffle when merge=1 (MarcoFalke)
- xogecoin/xogecoin#21037 fuzz: Avoid designated initialization (C++20) in fuzz tests (practicalswift)
- xogecoin/xogecoin#21042 doc, test: Improve `setup_clean_chain` documentation (fjahr)
- xogecoin/xogecoin#21080 fuzz: Configure check for main function (take 2) (MarcoFalke)
- xogecoin/xogecoin#21084 Fix timeout decrease in `feature_assumevalid` (brunoerg)
- xogecoin/xogecoin#21096 Re-add dead code detection (flack)
- xogecoin/xogecoin#21100 Remove unused function `xor_bytes` (theStack)
- xogecoin/xogecoin#21115 Fix Windows cross build (hebasto)
- xogecoin/xogecoin#21117 Remove `assert_blockchain_height` (MarcoFalke)
- xogecoin/xogecoin#21121 Small unit test improvements, including helper to make mempool transaction (amitiuttarwar)
- xogecoin/xogecoin#21124 Remove unnecessary assignment in bdb (brunoerg)
- xogecoin/xogecoin#21125 Change `BOOST_CHECK` to `BOOST_CHECK_EQUAL` for paths (kiminuo)
- xogecoin/xogecoin#21142, xogecoin/xogecoin#21512 fuzz: Add `tx_pool` fuzz target (MarcoFalke)
- xogecoin/xogecoin#21165 Use mocktime in `test_seed_peers` (dhruv)
- xogecoin/xogecoin#21169 fuzz: Add RPC interface fuzzing. Increase fuzzing coverage from 65% to 70% (practicalswift)
- xogecoin/xogecoin#21170 bench: Add benchmark to write json into a string (martinus)
- xogecoin/xogecoin#21178 Run `mempool_reorg.py` even with wallet disabled (DariusParvin)
- xogecoin/xogecoin#21185 fuzz: Remove expensive and redundant muhash from crypto fuzz target (MarcoFalke)
- xogecoin/xogecoin#21200 Speed up `rpc_blockchain.py` by removing miniwallet.generate() (MarcoFalke)
- xogecoin/xogecoin#21211 Move `P2WSH_OP_TRUE` to shared test library (MarcoFalke)
- xogecoin/xogecoin#21228 Avoid comparision of integers with different signs (jonasschnelli)
- xogecoin/xogecoin#21230 Fix `NODE_NETWORK_LIMITED_MIN_BLOCKS` disconnection (MarcoFalke)
- xogecoin/xogecoin#21252 Add missing wait for sync to `feature_blockfilterindex_prune` (MarcoFalke)
- xogecoin/xogecoin#21254 Avoid connecting to real network when running tests (MarcoFalke)
- xogecoin/xogecoin#21264 fuzz: Two scripted diff renames (MarcoFalke)
- xogecoin/xogecoin#21280 Bug fix in `transaction_tests` (glozow)
- xogecoin/xogecoin#21293 Replace accidentally placed bit-OR with logical-OR (hebasto)
- xogecoin/xogecoin#21297 `feature_blockfilterindex_prune.py` improvements (jonatack)
- xogecoin/xogecoin#21310 zmq test: fix sync-up by matching notification to generated block (theStack)
- xogecoin/xogecoin#21334 Additional BIP9 tests (Sjors)
- xogecoin/xogecoin#21338 Add functional test for anchors.dat (brunoerg)
- xogecoin/xogecoin#21345 Bring `p2p_leak.py` up to date (mzumsande)
- xogecoin/xogecoin#21357 Unconditionally check for fRelay field in test framework (jarolrod)
- xogecoin/xogecoin#21358 fuzz: Add missing include (`test/util/setup_common.h`) (MarcoFalke)
- xogecoin/xogecoin#21371 fuzz: fix gcc Woverloaded-virtual build warnings (jonatack)
- xogecoin/xogecoin#21373 Generate fewer blocks in `feature_nulldummy` to fix timeouts, speed up (jonatack)
- xogecoin/xogecoin#21390 Test improvements for UTXO set hash tests (fjahr)
- xogecoin/xogecoin#21410 increase `rpc_timeout` for fundrawtx `test_transaction_too_large` (jonatack)
- xogecoin/xogecoin#21411 add logging, reduce blocks, move `sync_all` in `wallet_` groups (jonatack)
- xogecoin/xogecoin#21438 Add ParseUInt8() test coverage (jonatack)
- xogecoin/xogecoin#21443 fuzz: Implement `fuzzed_dns_lookup_function` as a lambda (practicalswift)
- xogecoin/xogecoin#21445 cirrus: Use SSD cluster for speedup (MarcoFalke)
- xogecoin/xogecoin#21477 Add test for CNetAddr::ToString IPv6 address formatting (RFC 5952) (practicalswift)
- xogecoin/xogecoin#21487 fuzz: Use ConsumeWeakEnum in addrman for service flags (MarcoFalke)
- xogecoin/xogecoin#21488 Add ParseUInt16() unit test and fuzz coverage (jonatack)
- xogecoin/xogecoin#21491 test: remove duplicate assertions in util_tests (jonatack)
- xogecoin/xogecoin#21522 fuzz: Use PickValue where possible (MarcoFalke)
- xogecoin/xogecoin#21531 remove qt byteswap compattests (fanquake)
- xogecoin/xogecoin#21557 small cleanup in RPCNestedTests tests (fanquake)
- xogecoin/xogecoin#21586 Add missing suppression for signed-integer-overflow:txmempool.cpp (MarcoFalke)
- xogecoin/xogecoin#21592 Remove option to make TestChain100Setup non-deterministic (MarcoFalke)
- xogecoin/xogecoin#21597 Document `race:validation_chainstatemanager_tests` suppression (MarcoFalke)
- xogecoin/xogecoin#21599 Replace file level integer overflow suppression with function level suppression (practicalswift)
- xogecoin/xogecoin#21604 Document why no symbol names can be used for suppressions (MarcoFalke)
- xogecoin/xogecoin#21606 fuzz: Extend psbt fuzz target a bit (MarcoFalke)
- xogecoin/xogecoin#21617 fuzz: Fix uninitialized read in i2p test (MarcoFalke)
- xogecoin/xogecoin#21630 fuzz: split FuzzedSock interface and implementation (vasild)
- xogecoin/xogecoin#21634 Skip SQLite fsyncs while testing (achow101)
- xogecoin/xogecoin#21669 Remove spurious double lock tsan suppressions by bumping to clang-12 (MarcoFalke)
- xogecoin/xogecoin#21676 Use mocktime to avoid intermittent failure in `rpc_tests` (MarcoFalke)
- xogecoin/xogecoin#21677 fuzz: Avoid use of low file descriptor ids (which may be in use) in FuzzedSock (practicalswift)
- xogecoin/xogecoin#21678 Fix TestPotentialDeadLockDetected suppression (hebasto)
- xogecoin/xogecoin#21689 Remove intermittently failing and not very meaningful `BOOST_CHECK` in `cnetaddr_basic` (practicalswift)
- xogecoin/xogecoin#21691 Check that no versionbits are re-used (MarcoFalke)
- xogecoin/xogecoin#21707 Extend functional tests for addr relay (mzumsande)
- xogecoin/xogecoin#21712 Test default `include_mempool` value of gettxout (promag)
- xogecoin/xogecoin#21738 Use clang-12 for ASAN, Add missing suppression (MarcoFalke)
- xogecoin/xogecoin#21740 add new python linter to check file names and permissions (windsok)
- xogecoin/xogecoin#21749 Bump shellcheck version (hebasto)
- xogecoin/xogecoin#21754 Run `feature_cltv` with MiniWallet (MarcoFalke)
- xogecoin/xogecoin#21762 Speed up `mempool_spend_coinbase.py` (MarcoFalke)
- xogecoin/xogecoin#21773 fuzz: Ensure prevout is consensus-valid (MarcoFalke)
- xogecoin/xogecoin#21777 Fix `feature_notifications.py` intermittent issue (MarcoFalke)
- xogecoin/xogecoin#21785 Fix intermittent issue in `p2p_addr_relay.py` (MarcoFalke)
- xogecoin/xogecoin#21787 Fix off-by-ones in `rpc_fundrawtransaction` assertions (jonatack)
- xogecoin/xogecoin#21792 Fix intermittent issue in `p2p_segwit.py` (MarcoFalke)
- xogecoin/xogecoin#21795 fuzz: Terminate immediately if a fuzzing harness tries to perform a DNS lookup (belt and suspenders) (practicalswift)
- xogecoin/xogecoin#21798 fuzz: Create a block template in `tx_pool` targets (MarcoFalke)
- xogecoin/xogecoin#21804 Speed up `p2p_segwit.py` (jnewbery)
- xogecoin/xogecoin#21810 fuzz: Various RPC fuzzer follow-ups (practicalswift)
- xogecoin/xogecoin#21814 Fix `feature_config_args.py` intermittent issue (MarcoFalke)
- xogecoin/xogecoin#21821 Add missing test for empty P2WSH redeem (MarcoFalke)
- xogecoin/xogecoin#21822 Resolve bug in `interface_xogecoin_cli.py` (klementtan)
- xogecoin/xogecoin#21846 fuzz: Add `-fsanitize=integer` suppression needed for RPC fuzzer (`generateblock`) (practicalswift)
- xogecoin/xogecoin#21849 fuzz: Limit toxic test globals to their respective scope (MarcoFalke)
- xogecoin/xogecoin#21867 use MiniWallet for `p2p_blocksonly.py` (theStack)
- xogecoin/xogecoin#21873 minor fixes & improvements for files linter test (windsok)
- xogecoin/xogecoin#21874 fuzz: Add `WRITE_ALL_FUZZ_TARGETS_AND_ABORT` (MarcoFalke)
- xogecoin/xogecoin#21884 fuzz: Remove unused --enable-danger-fuzz-link-all option (MarcoFalke)
- xogecoin/xogecoin#21890 fuzz: Limit ParseISO8601DateTime fuzzing to 32-bit (MarcoFalke)
- xogecoin/xogecoin#21891 fuzz: Remove strprintf test cases that are known to fail (MarcoFalke)
- xogecoin/xogecoin#21892 fuzz: Avoid excessively large min fee rate in `tx_pool` (MarcoFalke)
- xogecoin/xogecoin#21895 Add TSA annotations to the WorkQueue class members (hebasto)
- xogecoin/xogecoin#21900 use MiniWallet for `feature_csv_activation.py` (theStack)
- xogecoin/xogecoin#21909 fuzz: Limit max insertions in timedata fuzz test (MarcoFalke)
- xogecoin/xogecoin#21922 fuzz: Avoid timeout in EncodeBase58 (MarcoFalke)
- xogecoin/xogecoin#21927 fuzz: Run const CScript member functions only once (MarcoFalke)
- xogecoin/xogecoin#21929 fuzz: Remove incorrect float round-trip serialization test (MarcoFalke)
- xogecoin/xogecoin#21936 fuzz: Terminate immediately if a fuzzing harness tries to create a TCP socket (belt and suspenders) (practicalswift)
- xogecoin/xogecoin#21941 fuzz: Call const member functions in addrman fuzz test only once (MarcoFalke)
- xogecoin/xogecoin#21945 add P2PK support to MiniWallet (theStack)
- xogecoin/xogecoin#21948 Fix off-by-one in mockscheduler test RPC (MarcoFalke)
- xogecoin/xogecoin#21953 fuzz: Add `utxo_snapshot` target (MarcoFalke)
- xogecoin/xogecoin#21970 fuzz: Add missing CheckTransaction before CheckTxInputs (MarcoFalke)
- xogecoin/xogecoin#21989 Use `COINBASE_MATURITY` in functional tests (kiminuo)
- xogecoin/xogecoin#22003 Add thread safety annotations (ajtowns)
- xogecoin/xogecoin#22004 fuzz: Speed up transaction fuzz target (MarcoFalke)
- xogecoin/xogecoin#22005 fuzz: Speed up banman fuzz target (MarcoFalke)
- xogecoin/xogecoin#22029 [fuzz] Improve transport deserialization fuzz test coverage (dhruv)
- xogecoin/xogecoin#22048 MiniWallet: introduce enum type for output mode (theStack)
- xogecoin/xogecoin#22057 use MiniWallet (P2PK mode) for `feature_dersig.py` (theStack)
- xogecoin/xogecoin#22065 Mark `CheckTxInputs` `[[nodiscard]]`. Avoid UUM in fuzzing harness `coins_view` (practicalswift)
- xogecoin/xogecoin#22069 fuzz: don't try and use fopencookie() when building for Android (fanquake)
- xogecoin/xogecoin#22082 update nanobench from release 4.0.0 to 4.3.4 (martinus)
- xogecoin/xogecoin#22086 remove BasicTestingSetup from unit tests that don't need it (fanquake)
- xogecoin/xogecoin#22089 MiniWallet: fix fee calculation for P2PK and check tx vsize (theStack)
- xogecoin/xogecoin#21107, xogecoin/xogecoin#22092 Convert documentation into type annotations (fanquake)
- xogecoin/xogecoin#22095 Additional BIP32 test vector for hardened derivation with leading zeros (kristapsk)
- xogecoin/xogecoin#22103 Fix IPv6 check on BSD systems (n-thumann)
- xogecoin/xogecoin#22118 check anchors.dat when node starts for the first time (brunoerg)
- xogecoin/xogecoin#22120 `p2p_invalid_block`: Check that a block rejected due to too-new tim… (willcl-ark)
- xogecoin/xogecoin#22153 Fix `p2p_leak.py` intermittent failure (mzumsande)
- xogecoin/xogecoin#22169 p2p, rpc, fuzz: various tiny follow-ups (jonatack)
- xogecoin/xogecoin#22176 Correct outstanding -Werror=sign-compare errors (Empact)
- xogecoin/xogecoin#22180 fuzz: Increase branch coverage of the float fuzz target (MarcoFalke)
- xogecoin/xogecoin#22187 Add `sync_blocks` in `wallet_orphanedreward.py` (domob1812)
- xogecoin/xogecoin#22201 Fix TestShell to allow running in Jupyter Notebook (josibake)
- xogecoin/xogecoin#22202 Add temporary coinstats suppressions (MarcoFalke)
- xogecoin/xogecoin#22203 Use ConnmanTestMsg from test lib in `denialofservice_tests` (MarcoFalke)
- xogecoin/xogecoin#22210 Use MiniWallet in `test_no_inherited_signaling` RBF test (MarcoFalke)
- xogecoin/xogecoin#22224 Update msvc and appveyor builds to use Qt5.12.11 binaries (sipsorcery)
- xogecoin/xogecoin#22249 Kill process group to avoid dangling processes when using `--failfast` (S3RK)
- xogecoin/xogecoin#22267 fuzz: Speed up crypto fuzz target (MarcoFalke)
- xogecoin/xogecoin#22270 Add xogecoin-util tests (+refactors) (MarcoFalke)
- xogecoin/xogecoin#22271 fuzz: Assert roundtrip equality for `CPubKey` (theStack)
- xogecoin/xogecoin#22279 fuzz: add missing ECCVerifyHandle to `base_encode_decode` (apoelstra)
- xogecoin/xogecoin#22292 bench, doc: benchmarking updates and fixups (jonatack)
- xogecoin/xogecoin#22306 Improvements to `p2p_addr_relay.py` (amitiuttarwar)
- xogecoin/xogecoin#22310 Add functional test for replacement relay fee check (ariard)
- xogecoin/xogecoin#22311 Add missing syncwithvalidationinterfacequeue in `p2p_blockfilters` (MarcoFalke)
- xogecoin/xogecoin#22313 Add missing `sync_all` to `feature_coinstatsindex` (MarcoFalke)
- xogecoin/xogecoin#22322 fuzz: Check banman roundtrip (MarcoFalke)
- xogecoin/xogecoin#22363 Use `script_util` helpers for creating P2{PKH,SH,WPKH,WSH} scripts (theStack)
- xogecoin/xogecoin#22399 fuzz: Rework CTxDestination fuzzing (MarcoFalke)
- xogecoin/xogecoin#22408 add tests for `bad-txns-prevout-null` reject reason (theStack)
- xogecoin/xogecoin#22445 fuzz: Move implementations of non-template fuzz helpers from util.h to util.cpp (sriramdvt)
- xogecoin/xogecoin#22446 Fix `wallet_listdescriptors.py` if bdb is not compiled (hebasto)
- xogecoin/xogecoin#22447 Whitelist `rpc_rawtransaction` peers to speed up tests (jonatack)
- xogecoin/xogecoin#22742 Use proper target in `do_fund_send` (S3RK)

### Miscellaneous
- xogecoin/xogecoin#19337 sync: Detect double lock from the same thread (vasild)
- xogecoin/xogecoin#19809 log: Prefix log messages with function name and source code location if -logsourcelocations is set (practicalswift)
- xogecoin/xogecoin#19866 eBPF Linux tracepoints (jb55)
- xogecoin/xogecoin#20024 init: Fix incorrect warning "Reducing -maxconnections from N to N-1, because of system limitations" (practicalswift)
- xogecoin/xogecoin#20145 contrib: Add getcoins.py script to get coins from (signet) faucet (kallewoof)
- xogecoin/xogecoin#20255 util: Add assume() identity function (MarcoFalke)
- xogecoin/xogecoin#20288 script, doc: Contrib/seeds updates (jonatack)
- xogecoin/xogecoin#20358 src/randomenv.cpp: Fix build on uclibc (ffontaine)
- xogecoin/xogecoin#20406 util: Avoid invalid integer negation in formatmoney and valuefromamount (practicalswift)
- xogecoin/xogecoin#20434 contrib: Parse elf directly for symbol and security checks (laanwj)
- xogecoin/xogecoin#20451 lint: Run mypy over contrib/devtools (fanquake)
- xogecoin/xogecoin#20476 contrib: Add test for elf symbol-check (laanwj)
- xogecoin/xogecoin#20530 lint: Update cppcheck linter to c++17 and improve explicit usage (fjahr)
- xogecoin/xogecoin#20589 log: Clarify that failure to read/write `fee_estimates.dat` is non-fatal (MarcoFalke)
- xogecoin/xogecoin#20602 util: Allow use of c++14 chrono literals (MarcoFalke)
- xogecoin/xogecoin#20605 init: Signal-safe instant shutdown (laanwj)
- xogecoin/xogecoin#20608 contrib: Add symbol check test for PE binaries (fanquake)
- xogecoin/xogecoin#20689 contrib: Replace binary verification script verify.sh with python rewrite (theStack)
- xogecoin/xogecoin#20715 util: Add argsmanager::getcommand() and use it in xogecoin-wallet (MarcoFalke)
- xogecoin/xogecoin#20735 script: Remove outdated extract-osx-sdk.sh (hebasto)
- xogecoin/xogecoin#20817 lint: Update list of spelling linter false positives, bump to codespell 2.0.0 (theStack)
- xogecoin/xogecoin#20884 script: Improve robustness of xogecoind.service on startup (hebasto)
- xogecoin/xogecoin#20906 contrib: Embed c++11 patch in `install_db4.sh` (gruve-p)
- xogecoin/xogecoin#21004 contrib: Fix docker args conditional in gitian-build (setpill)
- xogecoin/xogecoin#21007 xogecoind: Add -daemonwait option to wait for initialization (laanwj)
- xogecoin/xogecoin#21041 log: Move "Pre-allocating up to position 0x[…] in […].dat" log message to debug category (practicalswift)
- xogecoin/xogecoin#21059 Drop boost/preprocessor dependencies (hebasto)
- xogecoin/xogecoin#21087 guix: Passthrough `BASE_CACHE` into container (dongcarl)
- xogecoin/xogecoin#21088 guix: Jump forwards in time-machine and adapt (dongcarl)
- xogecoin/xogecoin#21089 guix: Add support for powerpc64{,le} (dongcarl)
- xogecoin/xogecoin#21110 util: Remove boost `posix_time` usage from `gettime*` (fanquake)
- xogecoin/xogecoin#21111 Improve OpenRC initscript (parazyd)
- xogecoin/xogecoin#21123 code style: Add EditorConfig file (kiminuo)
- xogecoin/xogecoin#21173 util: Faster hexstr => 13% faster blocktojson (martinus)
- xogecoin/xogecoin#21221 tools: Allow argument/parameter bin packing in clang-format (jnewbery)
- xogecoin/xogecoin#21244 Move GetDataDir to ArgsManager (kiminuo)
- xogecoin/xogecoin#21255 contrib: Run test-symbol-check for risc-v (fanquake)
- xogecoin/xogecoin#21271 guix: Explicitly set umask in build container (dongcarl)
- xogecoin/xogecoin#21300 script: Add explanatory comment to tc.sh (dscotese)
- xogecoin/xogecoin#21317 util: Make assume() usable as unary expression (MarcoFalke)
- xogecoin/xogecoin#21336 Make .gitignore ignore src/test/fuzz/fuzz.exe (hebasto)
- xogecoin/xogecoin#21337 guix: Update darwin native packages dependencies (hebasto)
- xogecoin/xogecoin#21405 compat: remove memcpy -> memmove backwards compatibility alias (fanquake)
- xogecoin/xogecoin#21418 contrib: Make systemd invoke dependencies only when ready (laanwj)
- xogecoin/xogecoin#21447 Always add -daemonwait to known command line arguments (hebasto)
- xogecoin/xogecoin#21471 bugfix: Fix `bech32_encode` calls in `gen_key_io_test_vectors.py` (sipa)
- xogecoin/xogecoin#21615 script: Add trusted key for hebasto (hebasto)
- xogecoin/xogecoin#21664 contrib: Use lief for macos and windows symbol & security checks (fanquake)
- xogecoin/xogecoin#21695 contrib: Remove no longer used contrib/xogecoin-qt.pro (hebasto)
- xogecoin/xogecoin#21711 guix: Add full installation and usage documentation (dongcarl)
- xogecoin/xogecoin#21799 guix: Use `gcc-8` across the board (dongcarl)
- xogecoin/xogecoin#21802 Avoid UB in util/asmap (advance a dereferenceable iterator outside its valid range) (MarcoFalke)
- xogecoin/xogecoin#21823 script: Update reviewers (jonatack)
- xogecoin/xogecoin#21850 Remove `GetDataDir(net_specific)` function (kiminuo)
- xogecoin/xogecoin#21871 scripts: Add checks for minimum required os versions (fanquake)
- xogecoin/xogecoin#21966 Remove double serialization; use software encoder for fee estimation (sipa)
- xogecoin/xogecoin#22060 contrib: Add torv3 seed nodes for testnet, drop v2 ones (laanwj)
- xogecoin/xogecoin#22244 devtools: Correctly extract symbol versions in symbol-check (laanwj)
- xogecoin/xogecoin#22533 guix/build: Remove vestigial SKIPATTEST.TAG (dongcarl)
- xogecoin/xogecoin#22643 guix-verify: Non-zero exit code when anything fails (dongcarl)
- xogecoin/xogecoin#22654 guix: Don't include directory name in SHA256SUMS (achow101)

### Documentation
- xogecoin/xogecoin#15451 clarify getdata limit after #14897 (HashUnlimited)
- xogecoin/xogecoin#15545 Explain why CheckBlock() is called before AcceptBlock (Sjors)
- xogecoin/xogecoin#17350 Add developer documentation to isminetype (HAOYUatHZ)
- xogecoin/xogecoin#17934 Use `CONFIG_SITE` variable instead of --prefix option (hebasto)
- xogecoin/xogecoin#18030 Coin::IsSpent() can also mean never existed (Sjors)
- xogecoin/xogecoin#18096 IsFinalTx comment about nSequence & `OP_CLTV` (nothingmuch)
- xogecoin/xogecoin#18568 Clarify developer notes about constant naming (ryanofsky)
- xogecoin/xogecoin#19961 doc: tor.md updates (jonatack)
- xogecoin/xogecoin#19968 Clarify CRollingBloomFilter size estimate (robot-dreams)
- xogecoin/xogecoin#20200 Rename CODEOWNERS to REVIEWERS (adamjonas)
- xogecoin/xogecoin#20329 docs/descriptors.md: Remove hardened marker in the path after xpub (dgpv)
- xogecoin/xogecoin#20380 Add instructions on how to fuzz the P2P layer using Honggfuzz NetDriver (practicalswift)
- xogecoin/xogecoin#20414 Remove generated manual pages from master branch (laanwj)
- xogecoin/xogecoin#20473 Document current boost dependency as 1.71.0 (laanwj)
- xogecoin/xogecoin#20512 Add bash as an OpenBSD dependency (emilengler)
- xogecoin/xogecoin#20568 Use FeeModes doc helper in estimatesmartfee (MarcoFalke)
- xogecoin/xogecoin#20577 libconsensus: add missing error code description, fix NQogecoin link (theStack)
- xogecoin/xogecoin#20587 Tidy up Tor doc (more stringent) (wodry)
- xogecoin/xogecoin#20592 Update wtxidrelay documentation per BIP339 (jonatack)
- xogecoin/xogecoin#20601 Update for FreeBSD 12.2, add GUI Build Instructions (jarolrod)
- xogecoin/xogecoin#20635 fix misleading comment about call to non-existing function (pox)
- xogecoin/xogecoin#20646 Refer to BIPs 339/155 in feature negotiation (jonatack)
- xogecoin/xogecoin#20653 Move addr relay comment in net to correct place (MarcoFalke)
- xogecoin/xogecoin#20677 Remove shouty enums in `net_processing` comments (sdaftuar)
- xogecoin/xogecoin#20741 Update 'Secure string handling' (prayank23)
- xogecoin/xogecoin#20757 tor.md and -onlynet help updates (jonatack)
- xogecoin/xogecoin#20829 Add -netinfo help (jonatack)
- xogecoin/xogecoin#20830 Update developer notes with signet (jonatack)
- xogecoin/xogecoin#20890 Add explicit macdeployqtplus dependencies install step (hebasto)
- xogecoin/xogecoin#20913 Add manual page generation for xogecoin-util (laanwj)
- xogecoin/xogecoin#20985 Add xorriso to macOS depends packages (fanquake)
- xogecoin/xogecoin#20986 Update developer notes to discourage very long lines (jnewbery)
- xogecoin/xogecoin#20987 Add instructions for generating RPC docs (ben-kaufman)
- xogecoin/xogecoin#21026 Document use of make-tag script to make tags (laanwj)
- xogecoin/xogecoin#21028 doc/bips: Add BIPs 43, 44, 49, and 84 (luke-jr)
- xogecoin/xogecoin#21049 Add release notes for listdescriptors RPC (S3RK)
- xogecoin/xogecoin#21060 More precise -debug and -debugexclude doc (wodry)
- xogecoin/xogecoin#21077 Clarify -timeout and -peertimeout config options (glozow)
- xogecoin/xogecoin#21105 Correctly identify script type (niftynei)
- xogecoin/xogecoin#21163 Guix is shipped in Debian and Ubuntu (MarcoFalke)
- xogecoin/xogecoin#21210 Rework internal and external links (MarcoFalke)
- xogecoin/xogecoin#21246 Correction for VerifyTaprootCommitment comments (roconnor-blockstream)
- xogecoin/xogecoin#21263 Clarify that squashing should happen before review (MarcoFalke)
- xogecoin/xogecoin#21323 guix, doc: Update default HOSTS value (hebasto)
- xogecoin/xogecoin#21324 Update build instructions for Fedora (hebasto)
- xogecoin/xogecoin#21343 Revamp macOS build doc (jarolrod)
- xogecoin/xogecoin#21346 install qt5 when building on macOS (fanquake)
- xogecoin/xogecoin#21384 doc: add signet to xogecoin.conf documentation (jonatack)
- xogecoin/xogecoin#21394 Improve comment about protected peers (amitiuttarwar)
- xogecoin/xogecoin#21398 Update fuzzing docs for afl-clang-lto (MarcoFalke)
- xogecoin/xogecoin#21444 net, doc: Doxygen updates and fixes in netbase.{h,cpp} (jonatack)
- xogecoin/xogecoin#21481 Tell howto install clang-format on Debian/Ubuntu (wodry)
- xogecoin/xogecoin#21567 Fix various misleading comments (glozow)
- xogecoin/xogecoin#21661 Fix name of script guix-build (Emzy)
- xogecoin/xogecoin#21672 Remove boostrap info from `GUIX_COMMON_FLAGS` doc (fanquake)
- xogecoin/xogecoin#21688 Note on SDK for macOS depends cross-compile (jarolrod)
- xogecoin/xogecoin#21709 Update reduce-memory.md and xogecoin.conf -maxconnections info (jonatack)
- xogecoin/xogecoin#21710 update helps for addnode rpc and -addnode/-maxconnections config options (jonatack)
- xogecoin/xogecoin#21752 Clarify that feerates are per virtual size (MarcoFalke)
- xogecoin/xogecoin#21811 Remove Visual Studio 2017 reference from readme (sipsorcery)
- xogecoin/xogecoin#21818 Fixup -coinstatsindex help, update xogecoin.conf and files.md (jonatack)
- xogecoin/xogecoin#21856 add OSS-Fuzz section to fuzzing.md doc (adamjonas)
- xogecoin/xogecoin#21912 Remove mention of priority estimation (MarcoFalke)
- xogecoin/xogecoin#21925 Update bips.md for 0.21.1 (MarcoFalke)
- xogecoin/xogecoin#21942 improve make with parallel jobs description (klementtan)
- xogecoin/xogecoin#21947 Fix OSS-Fuzz links (MarcoFalke)
- xogecoin/xogecoin#21988 note that brew installed qt is not supported (jarolrod)
- xogecoin/xogecoin#22056 describe in fuzzing.md how to reproduce a CI crash (jonatack)
- xogecoin/xogecoin#22080 add maxuploadtarget to xogecoin.conf example (jarolrod)
- xogecoin/xogecoin#22088 Improve note on choosing posix mingw32 (jarolrod)
- xogecoin/xogecoin#22109 Fix external links (IRC, …) (MarcoFalke)
- xogecoin/xogecoin#22121 Various validation doc fixups (MarcoFalke)
- xogecoin/xogecoin#22172 Update tor.md, release notes with removal of tor v2 support (jonatack)
- xogecoin/xogecoin#22204 Remove obsolete `okSafeMode` RPC guideline from developer notes (theStack)
- xogecoin/xogecoin#22208 Update `REVIEWERS` (practicalswift)
- xogecoin/xogecoin#22250 add basic I2P documentation (vasild)
- xogecoin/xogecoin#22296 Final merge of release notes snippets, mv to wiki (MarcoFalke)
- xogecoin/xogecoin#22335 recommend `--disable-external-signer` in OpenBSD build guide (theStack)
- xogecoin/xogecoin#22339 Document minimum required libc++ version (hebasto)
- xogecoin/xogecoin#22349 Repository IRC updates (jonatack)
- xogecoin/xogecoin#22360 Remove unused section from release process (MarcoFalke)
- xogecoin/xogecoin#22369 Add steps for Transifex to release process (jonatack)
- xogecoin/xogecoin#22393 Added info to xogecoin.conf doc (bliotti)
- xogecoin/xogecoin#22402 Install Rosetta on M1-macOS for qt in depends (hebasto)
- xogecoin/xogecoin#22432 Fix incorrect `testmempoolaccept` doc (glozow)
- xogecoin/xogecoin#22648 doc, test: improve i2p/tor docs and i2p reachable unit tests (jonatack)

Credits
=======

Thanks to everyone who directly contributed to this release:

- Aaron Clauson
- Adam Jonas
- amadeuszpawlik
- Amiti Uttarwar
- Andrew Chow
- Andrew Poelstra
- Anthony Towns
- Antoine Poinsot
- Antoine Riard
- apawlik
- apitko
- Ben Carman
- Ben Woosley
- benk10
- Bezdrighin
- Block Mechanic
- Brian Liotti
- Bruno Garcia
- Carl Dong
- Christian Decker
- coinforensics
- Cory Fields
- Dan Benjamin
- Daniel Kraft
- Darius Parvin
- Dhruv Mehta
- Dmitry Goncharov
- Dmitry Petukhov
- dplusplus1024
- dscotese
- Duncan Dean
- Elle Mouton
- Elliott Jin
- Emil Engler
- Ethan Heilman
- eugene
- Evan Klitzke
- Fabian Jahr
- Fabrice Fontaine
- fanquake
- fdov
- flack
- Fotis Koutoupas
- Fu Yong Quah
- fyquah
- glozow
- Gregory Sanders
- Guido Vranken
- Gunar C. Gessner
- h
- HAOYUatHZ
- Hennadii Stepanov
- Igor Cota
- Ikko Ashimine
- Ivan Metlushko
- jackielove4u
- James O'Beirne
- Jarol Rodriguez
- Joel Klabo
- John Newbery
- Jon Atack
- Jonas Schnelli
- João Barbosa
- Josiah Baker
- Karl-Johan Alm
- Kiminuo
- Klement Tan
- Kristaps Kaupe
- Larry Ruane
- lisa neigut
- Lucas Ontivero
- Luke Dashjr
- Maayan Keshet
- MarcoFalke
- Martin Ankerl
- Martin Zumsande
- Michael Dietz
- Michael Polzer
- Michael Tidwell
- Niklas Gögge
- nthumann
- Oliver Gugger
- parazyd
- Patrick Strateman
- Pavol Rusnak
- Peter Bushnell
- Pierre K
- Pieter Wuille
- PiRK
- pox
- practicalswift
- Prayank
- R E Broadley
- Rafael Sadowski
- randymcmillan
- Raul Siles
- Riccardo Spagni
- Russell O'Connor
- Russell Yanofsky
- S3RK
- saibato
- Samuel Dobson
- sanket1729
- Sawyer Billings
- Sebastian Falbesoner
- setpill
- sgulls
- sinetek
- Sjors Provoost
- Sriram
- Stephan Oeste
- Suhas Daftuar
- Sylvain Goumy
- t-bast
- Troy Giorshev
- Tushar Singla
- Tyler Chambers
- Uplab
- Vasil Dimov
- W. J. van der Laan
- willcl-ark
- William Bright
- William Casarin
- windsok
- wodry
- Yerzhan Mazhkenov
- Yuval Kogman
- Zero

As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com/xogecoin/xogecoin/).
