# PS4 Remote Package Installer

> **Note:** The original plain-text [`README`](README) by **flatz** is preserved untouched in this repository. This `README.md` documents the **open-source Docker / OpenOrbis build system**, **server modifications**, and **companion Rust CLI** first, followed by a Markdown-formatted version of the **original README**.

This repository is a **modified and modernized version of [flatz's `ps4_remote_pkg_installer`](https://github.com/flatz/ps4_remote_pkg_installer)** (the C server running on the PS4). It is designed and tested to work alongside [**`markmclaren/ps4-remote-pkg-installer-cli`**](https://github.com/markmclaren/ps4-remote-pkg-installer-cli), which is a **Rust CLI rewrite of the original [Java GUI client](https://github.com/BenjaminFaal/ps4-remote-pkg-installer)**.

---

## What's New: Server Modifications & OpenOrbis Build

The original project required a proprietary PS4 SDK (`SCE_ORBIS_SDK_DIR`), Windows publishing tools (`orbis-pub-cmd.exe`), and `wine` on Linux/TrueOS—even noting in the original README:
> *"SDK (oouch! need to move to open source SDK in the future...)"*

This modified server builds with the open-source [OpenOrbis PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain), adds an embedded Web Dashboard & Swagger UI on port `12800`, and fixes several stability and performance bottlenecks when installing large multi-GB `.pkg` files:

### Server Stability & Performance Improvements
- **Thread-Safe JSON Parsing & Memory Management:** Removed shared `static json_t* pool` pointers across HTTP `POST` handlers in `server.c` and `pkg.c` that caused heap corruption and double-free crashes when concurrent requests arrived (e.g., CLI progress polling overlapping with the Web UI).
- **Serialized Sony SDK & BGFT Access:** Added mutex synchronization around `sceBgft*`, `sceAppInstUtil*`, `sceHttp*`, and `sceNpUtilJsonEscape` calls, expanded internal `SceNet`, `SceHttp`, and `SceBgft` heaps to 4 MB, and added short-TTL (400 ms) task-progress caching so concurrent polling does not overwhelm `libSceBgft`.
- **Sandbird HTTP Server Hardening:** Fixed blocking-socket timeout handling, detached worker thread creation (512 KB stack), file descriptor lifecycle bugs, and buffer formatting (`vsnprintf`), and increased file-streaming chunk sizes to 64 KB.
- **OpenOrbis `struct stat` ABI Fix & Exact File Sizing:** Added a kernel-accurate `struct stat` shim (`docker/oosdk_shim/include/bits/stat.h`) and switched file length queries in `sandbird.c` and `util.c` to `lseek(..., SEEK_END)` so static assets and manifests are always served with their full `Content-Length`.
- **Bounded HTTP `Range` Metadata Reads:** Updated `http_download_file` and `http_get_file_size` to probe package size via `HEAD` first and fetch the initial PKG header using a bounded `Range: bytes=0-8191` request rather than opening an unbounded multi-GB `GET` stream.
- **Built-in Web Dashboard & Swagger UI:** Served directly from the PS4 on port `12800`:
  - `http://<PS4_IP>:12800/` — Live Web Dashboard showing active transfers, speeds, ETAs, pause/resume/stop controls, and a Title ID installation checker (with in-flight polling guards).
  - `http://<PS4_IP>:12800/docs` — Interactive OpenAPI / Swagger documentation (`/openapi.json`, `/openapi.yaml`).

### Added Files

| File / Path | Description |
| :--- | :--- |
| [`Dockerfile`](Dockerfile) | Multi-stage Ubuntu 20.04 image that downloads OpenOrbis `v0.5.2` (`/opt/openorbis`) and installs Clang 12, LLD 12, and `PkgTool.Core` runtime dependencies. |
| [`docker-compose.yml`](docker-compose.yml) | Defines the `builder` service (`pull_policy: build`) that mounts the workspace and runs `make -f Makefile.openorbis`. |
| [`Makefile.openorbis`](Makefile.openorbis) | OpenOrbis Makefile that compiles `thirdparty/sandbird`, `thirdparty/tiny-json`, and the main application, creates the fake-signed `eboot.bin` (`create-fself`), generates `param.sfo`, and packs `build/remote_pkg_installer.pkg` (`PkgTool.Core`). |
| [`docker/oosdk_shim/include/`](docker/oosdk_shim/include) | Compatibility header shims mapping the original SDK header names, `sceBgft*` symbols, and FreeBSD/Orbis `struct stat` layout onto OpenOrbis's open-source stub libraries. |
| [`web/`](web) | Embedded Web Dashboard (`index.html`), Swagger UI (`docs.html`), and OpenAPI specifications (`openapi.json`, `openapi.yaml`). |
| [`build.bat`](build.bat) / [`build.ps1`](build.ps1) / [`build.sh`](build.sh) | Cross-platform build scripts for Windows (`cmd` / PowerShell) and Linux/macOS (`bash`). |
| [`.dockerignore`](.dockerignore) | Excludes local build artifacts (`build/`, `obj/`) and `.git` from the Docker build context. |

### Building with Docker (No SDK Required)

Prerequisites: **Docker Desktop** (Windows/macOS) or **Docker Engine + Compose** (Linux).

#### Windows (Command Prompt)
```bat
build.bat          :: Clean and build build\remote_pkg_installer.pkg
build.bat pkg      :: Build pkg target
build.bat clean    :: Clean build artifacts
```

#### Windows (PowerShell)
```powershell
.\build.ps1          # Clean and build build\remote_pkg_installer.pkg
.\build.ps1 pkg      # Build pkg target
.\build.ps1 clean    # Clean build artifacts
```

#### Linux / macOS / Git Bash
```bash
./build.sh           # Clean and build build/remote_pkg_installer.pkg
./build.sh pkg       # Build pkg target
./build.sh clean     # Clean build artifacts
```

#### Direct Docker Compose Commands
```bash
docker compose run --rm builder
docker compose run --rm builder make -f Makefile.openorbis clean
```

### Build Output
After running the build, the installable PS4 fake-PKG is written to:
- `build/remote_pkg_installer.pkg` (also copied as `build/UP9000-FLTZ00003_00-REMOTEPKGINSTALL.pkg`)

---

## Original README (Formatted in Markdown)

> *Below is the original [`README`](README) text, formatted with Markdown headings and code blocks for readability.*

### Prerequisites (Original Build System)
- **Kernel payload changes & ShellCore patches:**
  https://gist.github.com/flatz/f510bc6647a33cef1532c65c7881f32d
- **SDK** *(oouch! need to move to open source SDK in the future...)*
- **Publishing tools with fPKG patches**
- **Linux & Wine** *(I'm building it on trueOS but it could be backported to Windows)*

### Requirements (PS4 Target)
- Any exploit on **4.5x+**
- **HEN 1.8** (you could get it from zecoxao's page) or any other kernel payload (it just needs to have fPKG stuff and `/data` mount patches for ShellCore that I've posted recently)

### Overview
Here is the first version of remote package installer that I made a few days ago, it have no GUI yet (it just displays a splash screen), but all the job could already be done via Web API that you could access remotely from your PC using any of available tools that you prefer: a custom web server or an application, NodeJS scripts, etc. No more need to use USB flash drives or external hard drives for your packages, everything could be done remotely.

One important thing: to be able to use this tool for receiving commands you need to have this application in focus (not in a background, because PS4 will suspend it and it won't be possible to use network anymore). After you send a command (to install game, for example), wait some time (I suppose "waiting to install" phase should be finished before you do minimize/close, anyway if you experiences it then click on "View Details" in notification window, you may see "the connection to the server has been lost", if so then just relaunch/maximize application and resume task), and then you could minimize this application freely (switch to a game, for example), because actual installing (or better saying, downloading) will be running in a background.

So, an idea is simple: you need to set up a web server that will serve package files for PS4 access, in other words, it will host package files and when a command will be received it will send `.pkg` file to PS4 using a simple HTTP protocol. Also, this PS4 application runs another web server by itself (using `12800` port) that is used to receive commands from PC/mobile or anything else you prefer. To send a command to PS4 server you need to use any tool that you want (this could be the same tool that you use for web server, for example, if it's a site then it will be some PHP/JS/Python script or you could just use a simple cURL client). Some of you could also create their own tool to communicate with it using HTML+JS/C#/NodeJS or anything else, so it's flexible and there are no restrictions. All commands are based on HTTP POST requests that receives/sends JSON data (see below).

There are two types of installation:
1. Using direct URLs to `.pkg` parts
2. Using URL to manifest JSON, so called CDN way, but it's limited due to exploit's reasons, however if it's still usable for you then you need to unblock this CDN domain (if it's blocked): `gs2.ww.prod.dl.playstation.net`

Also, manifest `.json` may be on a local server as well, an official server is not required, of course. You could even host a public server (even CDN) to store files that could be accessed from your PS4.

Because I'm lazy to create desktop application I just use a NodeJS server (I use NPM package called `serve`) as local web server on my PC, this package is very good because it supports multi-threading and PS4 actually need it:
```bash
npm install serve
serve -p <local port> <serve folder>
```
So you set a path to a folder where are your packages located and then specify proper URL for PS4 to locate them.

For client I just use command line version of `cURL`.

### Web API Examples (`cURL`)

*(These examples use `cURL`, but if you use something else then just take a look into available URLs and JSON format of data field for POST requests.)*

#### Checking if app exists (also tells its size if exists)
```bash
curl --data '{"title_id":"CUSA09311"}' 'http://<PS4 IP>:12800/api/is_exists'
```

#### Installing main package
*(If you have multiple pieces you should specify all of them in `packages` array in consecutive order; if you use a merged file then you just need to specify a single element for this array):*
```bash
curl -v 'http://<PS4 IP>:12800/api/install' --data '{"type":"direct","packages":["http://<local ip>:<local port>/UP1004-CUSA03041_00-REDEMPTION000002.pkg"]}'
```

#### Installing main package from CDN
```bash
curl -v 'http://<PS4 IP>:12800/api/install' --data '{"type":"ref_pkg_url","url":"http://gs2.ww.prod.dl.playstation.net/gs2/appkgo/prod/CUSA02299_00/2/f_b215964ca72fc114da7ed38b3a8e16ca79bd1a3538bd4160b230867b2f0a92e0/f/UP9000-CUSA02299_00-MARVELSSPIDERMAN.json"}'
```

#### Installing patch package
```bash
curl -v 'http://<PS4 IP>:12800/api/install' --data '{"type":"direct","packages":["http://<local ip>:<local port>/UP9000-CUSA02299_00-MARVELSSPIDERMAN-A0108-V0100_0.pkg","http://<local ip>:<local port>/UP9000-CUSA02299_00-MARVELSSPIDERMAN-A0108-V0100_1.pkg","http://<local ip>:<local port>/UP9000-CUSA02299_00-MARVELSSPIDERMAN-A0108-V0100_2.pkg"]}'
```

#### Installing additional content
```bash
curl -v 'http://<PS4 IP>:12800/api/install' --data '{"type":"direct","packages":["http://<local ip>:<local port>/UP0001-CUSA09311_00-ULCQUEST00000002.pkg"]}'
```

#### Installing theme
```bash
curl -v 'http://<PS4 IP>:12800/api/install' --data '{"type":"direct","packages":["http://<local ip>:<local port>/UP0001-CUSA09311_00-ACDNEWTHEME12345.pkg"]}'
```

#### Uninstalling game
```bash
curl -v 'http://<PS4 IP>:12800/api/uninstall_game' --data '{"title_id":"CUSA02299"}'
```

#### Uninstalling patch
```bash
curl -v 'http://<PS4 IP>:12800/api/uninstall_patch' --data '{"title_id":"CUSA08344"}'
```

#### Uninstalling additional content
```bash
curl -v 'http://<PS4 IP>:12800/api/uninstall_ac' --data '{"content_id":"UP0001-CUSA09311_00-ULCPACK000000004"}'
```

#### Uninstalling theme
```bash
curl -v 'http://<PS4 IP>:12800/api/uninstall_theme' --data '{"content_id":"UP9000-CUSA08344_00-DETROITCHARTHEME"}'
```

### Task Management API

*(You could also work with tasks themselves — pause, continue, remove, etc. — you just need to know `task_id`, which could be retrieved in a response when you send installation commands.)*

#### Starting task
*(Tasks are started automatically, but you could use this command if you have stopped task manually, for example):*
```bash
curl -v 'http://<PS4 IP>:12800/api/start_task' --data '{"task_id":123}'
```

#### Stopping task
```bash
curl -v 'http://<PS4 IP>:12800/api/stop_task' --data '{"task_id":123}'
```

#### Pausing task
```bash
curl -v 'http://<PS4 IP>:12800/api/pause_task' --data '{"task_id":123}'
```

#### Resuming task
```bash
curl -v 'http://<PS4 IP>:12800/api/resume_task' --data '{"task_id":123}'
```

#### Unregistering (removing) task
```bash
curl -v 'http://<PS4 IP>:12800/api/unregister_task' --data '{"task_id":123}'
```

#### Getting task progress information
```bash
curl -v 'http://<PS4 IP>:12800/api/get_task_progress' --data '{"task_id":123}'
```

#### Finding task ID by content ID and sub-type
```bash
curl -v 'http://<PS4 IP>:12800/api/find_task' --data '{"content_id":"UP1004-CUSA03041_00-REDEMPTION000002","sub_type":6}'
```

**Task sub-types:**
- `Game` = `6`
- `AC` = `7`
- `Patch` = `8`
- `License` = `9`

