# 🗺️ Treasure Hunt System (UNIX C Project) ⛯

This project is a multi-phase C application simulating a digital treasure hunt game system, developed in a UNIX environment. It demonstrates practical skills in **file systems**, **process management**, **signal-based inter-process communication**, and **pipe-based output handling**.

> Originally developed for a university systems programming lab project at Operating Systems.
    
---

## Features Overview

### Phase 1 – File Management (`treasure_manager.c`)
- Add, list, view, and remove treasures in named "hunt" directories
- Use of structured binary files (`treasure.dat`) to store game state
- System calls: `open`, `write`, `read`, `lseek`, `stat`, `mkdir`, `unlink`
- Creates symbolic links for hunt logs (`logged_hunt-<hunt_id>`)
- Logs all operations per hunt into `logged_hunt`

### Phase 2 – Signals & Process Management (`treasure_hub.c`)
- Interactive CLI ("hub") to manage the system
- Launches a separate background **monitor process**
- Communication via **UNIX signals** (`SIGUSR1`, `SIGCHLD`)
- Monitor responds to:
  - `list_hunts`
  - `list_treasures <hunt>`
  - `view_treasure <hunt> <id>`
  - `stop_monitor`
  - `exit`

### Phase 3 – Pipes & External Integration (`score.c`)
- Adds `calculate_score` command to the hub
- Spawns a child process for each hunt
- Uses `pipes` to capture scores of users based on treasure values
- Modular and extendable for scripting or API wrapping

---

## Compilation ⚙️

Use `gcc` to compile the programs:

```bash
gcc -Wall treasure_manager.c -o manager
gcc -Wall treasure_hub.c -o hub
gcc -Wall score.c -o score
```
---

## Usage 👾

### 1. Treasure Manager

```bash
./manager add hunt1            # Requires 'input_for_add.txt'
./manager list <hunt_name>
./manager view <hunt_name> <ID>
./manager remove_treasure <hunt_name> <ID>
./manager remove_hunt <hunt_name>

## Ensure input_for_add.txt contains:
    <id>
    <user name>
    <latitude longitude>
    <clue text>
    <value>
```
---

### 2. Treasure Hub (Interactive Mode)

```bash
    ./hub

## Within the hub, use commands:
    start_monitor
    list_hunts
    list_treasures <hunt_name>
    view_treasure <huntname> <ID>
    calculate_score
    stop_monitor
    exit
```

---

## Technologies Used 🛠
- C 
- UNIX system calls (fork, pipe, dup2, execl, kill, etc.)
- Signal handling with sigaction
- File I/O: binary formats with fixed-size structs
- Inter-process communication via signals and pipes
- Modular process orchestration

---

## Author 😎
Bunea Nicolae – github.com/Casperu20

---

