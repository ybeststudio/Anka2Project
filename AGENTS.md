# AGENTS.md

## Cursor Cloud specific instructions

### Project Overview

Anka2 is a Metin2 MMORPG private server + client project. The codebase has two main build targets:

- **Server** (game + db binaries): C++20, targets **FreeBSD 13.1+** with `ccache clang++-devel`, 32-bit. Uses kqueue for I/O multiplexing.
- **Client** (gameclient.exe): C++17/C++20, targets **Windows** with Visual Studio 2022 (v143), Win32. Uses DirectX 8.

### Platform Constraints (Linux Cloud VM)

The server binaries are FreeBSD ELF executables and **cannot run on Linux**. The client requires Windows + VS2022. On the Linux cloud VM:

- **Full compilation** of the server is not possible due to FreeBSD-specific APIs (kqueue in libthecore, FreeBSD system headers).
- **Platform-independent libraries** (libpoly, libgame, liblua) can be compiled with `clang++ -m32` to verify code integrity.
- **Static analysis** via `cppcheck` works fully on all server and client source.
- The pre-compiled server/client binaries in the repo are FreeBSD ELF / Windows PE format respectively.

### Development Tools Available

| Tool | Command | Notes |
|------|---------|-------|
| Static analysis | `cppcheck --enable=warning --suppress=missingInclude --std=c++20 --language=c++ --quiet --force Source/Server/game/src/` | Works on all C++ source |
| Compile libpoly | `make CXX="clang++ -Wno-everything"` in `Source/Server/library/libpoly/` | Platform-independent |
| Compile libgame | `make CXX="clang++ -Wno-everything"` in `Source/Server/library/libgame/src/` | Platform-independent |
| Compile liblua | `make CC="clang -Wno-everything"` in `Source/Server/library/liblua/src/` | Platform-independent |
| Database | `mysql -u beststudio -p'!@#bestprodesinq#@!'` | MariaDB with account, player, common, log databases |

### Database Setup

MariaDB is installed with 4 databases (`account`, `player`, `common`, `log`) and the `beststudio` user. Database schemas (table definitions) are **not in the repo** -- they ship with the FreeBSD VDI image. Start MariaDB with:

```bash
sudo mysqld_safe &
```

### Key Directories

- `Source/Server/game/src/` -- Game server source (163 .cpp files)
- `Source/Server/db/src/` -- DB server source (29 .cpp files)
- `Source/Server/common/` -- Shared headers including `service.h` (100+ feature flags)
- `Source/Server/library/` -- Server libraries (libthecore, liblua, libsql, libgame, libpoly)
- `Source/Binary/source/` -- Client source (17 modules, 648 files)
- `Source/Binary/vs_files/` -- VS2022 project files
- `home/` -- Server deployment directory (configs, pre-compiled binaries, game data)
- `Tools/` -- Development tools (archiver, dump_proto, binary_unpack)

### Build Notes

- Server Makefiles use `CC = ccache clang++-devel` (FreeBSD package). Override with `CC="clang++"` or `CXX="clang++"` on Linux.
- The `-lmd` flag in the game Makefile is FreeBSD-specific (libmd). Not available on Linux.
- MySQL include paths in Makefiles point to `/usr/local/include/mysql` (FreeBSD). On Linux, symlinks are created to `/usr/include/mysql`.
- Depend files contain hardcoded FreeBSD paths; regenerate with `make dep` if needed.

### Encoding

All C++ source files use **Windows-1254** encoding (Turkish). Quest/Lua files use **ANSI**. See user rules for full encoding policy.
