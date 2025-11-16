# FOS - Educational Operating System

An educational operating system developed as part of an Operating Systems course. This project implements core OS components including memory management, process scheduling, system calls, and concurrency primitives.

## 🏆 Achievement

**Top 10 placement across all projects** - Successfully implemented and passed all test cases for the assigned components.

## 📋 Project Overview

FOS (FCIS Operating System) is a 32-bit x86 operating system built from scratch. The project covers fundamental OS concepts including:

- **Boot Process**: Custom bootloader that loads the kernel from disk
- **Memory Management**: Virtual memory, paging, heap allocation, and page replacement algorithms
- **Process Management**: User environments, context switching, and scheduling
- **System Calls**: Interface between user programs and kernel
- **Concurrency**: Semaphores, locks, and inter-process communication
- **I/O Management**: Console, keyboard, and disk interfaces

## 🏗️ Architecture

### Memory Layout

The system uses a sophisticated virtual memory layout:
- **Kernel Space** (0xF0000000+): Kernel code, data, and heap
- **User Space** (0x00000000-0xEF800000): User programs, stack, and heap
- **Shared Memory**: Inter-process shared memory regions
- **Page Tables**: Virtual-to-physical address translation

### Key Components

```
FOS/
├── boot/          # Bootloader (boot.S, main.c)
├── kern/          # Kernel implementation
│   ├── cpu/       # CPU management, scheduling
│   ├── mem/       # Memory management
│   ├── proc/      # Process/user environment management
│   ├── trap/      # Interrupts, exceptions, system calls
│   ├── conc/      # Concurrency primitives
│   ├── cons/      # Console I/O
│   ├── cmd/       # Command prompt
│   └── disk/      # Disk/pagefile management
├── lib/           # User-space library functions
├── user/          # User programs and test cases
└── inc/           # Header files and definitions
```

## ✅ Implemented Features

All functions marked with `TODO:` comments were successfully implemented and passed test cases. The implementation includes:

### 1. System Calls (`kern/trap/syscall.c`, `lib/syscall.c`)
- System call parameter validation
- System call dispatch mechanism
- User-space system call wrappers

### 2. Dynamic Allocator (`lib/dynamic_allocator.c`)
- `initialize_dynamic_allocator()` - Initialize the allocator
- `set_block_data()` - Set block metadata
- `alloc_block_FF()` - First-fit allocation algorithm
- `alloc_block_BF()` - Best-fit allocation (bonus)

### 3. Kernel Heap (`kern/mem/kheap.c`)
- `initialize_kheap_dynamic_allocator()` - Initialize kernel heap
- `sbrk()` - Adjust heap size
- `kmalloc()` - Kernel memory allocation
- `kfree()` - Kernel memory deallocation
- `kheap_physical_address()` - Physical address translation
- `kheap_virtual_address()` - Virtual address translation
- `krealloc()` - Reallocation (bonus)

### 4. User Heap (`lib/uheap.c`, `kern/mem/chunk_operations.c`)
- `malloc()` - User-space memory allocation
- `free()` - User-space memory deallocation
- `sys_sbrk()` - System call for heap growth
- `allocate_user_mem()` - Kernel-side allocation
- `free_user_mem()` - Kernel-side deallocation
- O(1) free implementation (bonus)

### 5. Shared Memory (`lib/uheap.c`, `kern/mem/shared_memory_manager.c`)
- `smalloc()` - Shared memory allocation
- `sget()` - Get shared memory object
- `sfree()` - Free shared memory (bonus)
- `create_frames_storage()` - Frame storage management
- `create_share()` - Create shared object
- `get_share()` - Retrieve shared object
- `createSharedObject()` - Kernel-side creation
- `getSharedObject()` - Kernel-side retrieval
- `freeSharedObject()` - Kernel-side deallocation (bonus)

### 6. Fault Handler (`kern/trap/fault_handler.c`)
- Invalid pointer detection
- Page placement algorithm
- Page fault handling

### 7. Working Set Manager (`kern/mem/working_set_manager.c`)
- Create new working set elements
- Track page access patterns

### 8. User Environment (`kern/proc/user_environment.c`)
- `create_user_kern_stack()` - Create kernel stack for user process
- `initialize_uheap_dynamic_allocator()` - Initialize user heap

### 9. Concurrency Primitives

#### Semaphores (`lib/semaphore.c`, `kern/conc/ksemaphore.c`)
- `create_semaphore()` - Create semaphore
- `get_semaphore()` - Get semaphore by ID
- `wait_semaphore()` - Wait/P operation
- `signal_semaphore()` - Signal/V operation

#### Locks (`kern/conc/`)
- **Spinlocks**: Low-level synchronization
- **Sleep Locks**: Blocking locks for long operations
  - `acquire_sleeplock()` - Acquire sleep lock
  - `release_sleeplock()` - Release sleep lock

#### Channels (`kern/conc/channel.c`)
- `sleep()` - Put process to sleep
- `wakeup_one()` - Wake single waiting process
- `wakeup_all()` - Wake all waiting processes

### 10. Scheduler (`kern/cpu/sched.c`, `kern/cpu/sched_helpers.c`)
- Priority-based Round-Robin scheduling
- `sched_init_PRIRR()` - Initialize priority scheduler
- `fos_scheduler_PRIRR()` - Main scheduling algorithm
- `clock_interrupt_handler()` - Timer interrupt handling
- `env_set_priority()` - Set process priority
- `sched_set_starv_thresh()` - Set starvation threshold

### 11. Command Prompt (`kern/cmd/command_prompt.c`)
- `process_command()` - Command parsing and execution
- Interactive kernel shell

## 🧪 Testing

The project includes comprehensive test suites covering:
- Memory allocation and deallocation
- Heap placement strategies (First-fit, Best-fit)
- Page replacement algorithms
- Shared memory operations
- Semaphore operations
- Process scheduling
- System call functionality
- Protection and security checks

Test programs are located in the `user/` directory with naming convention `tst_*.c`.

## 🛠️ Building and Running

### Prerequisites
- i386-elf cross-compiler toolchain
- Bochs emulator
- Make (GNU Make)

### Build Instructions

```bash
# Build the entire system
make

# Run in Bochs emulator
make bochs

# Or use the provided batch file (Windows)
FOS_Developer_Console.bat
```

### Build Output
- Kernel image: `obj/kern/bochs.img`
- User programs: `obj/user/*.o`

## 📁 Project Structure

```
FOS/
├── boot/              # Bootloader code
│   ├── boot.S         # Assembly boot code
│   └── main.c         # C boot code
├── kern/              # Kernel source
│   ├── cpu/           # CPU and scheduling
│   ├── mem/           # Memory management
│   ├── proc/          # Process management
│   ├── trap/          # Interrupts and syscalls
│   ├── conc/          # Concurrency
│   ├── cons/          # Console
│   ├── cmd/           # Command prompt
│   └── disk/          # Disk management
├── lib/               # User-space library
├── user/              # User programs and tests
├── inc/               # Header files
├── conf/              # Build configuration
└── GNUmakefile        # Main build file
```

## 🔧 Key Algorithms

### Memory Allocation
- **First-Fit**: Allocate from first suitable block
- **Best-Fit**: Allocate from smallest suitable block (bonus)

### Page Replacement
- **FIFO**: First-In-First-Out
- **LRU**: Least Recently Used (approximations)
- **N-Chance Clock**: Clock algorithm variant

### Scheduling
- **Priority Round-Robin**: Priority-based with time slicing
- Starvation prevention mechanisms

## 📚 Course Context

This project was developed as part of an Operating Systems course, implementing:
- **MS1 (Milestone 1)**: System calls, dynamic allocator, concurrency primitives
- **MS2 (Milestone 2)**: Memory management, heap, shared memory, fault handling
- **MS3 (Milestone 3)**: Scheduling, semaphores, priority management

## 🎯 Highlights

- ✅ All TODO functions implemented and tested
- ✅ Comprehensive test coverage
- ✅ Bonus features implemented (Best-fit, O(1) free, shared memory deallocation)
- ✅ Clean, well-documented code
- ✅ Top 10 project ranking

## 📝 Notes

- The system runs in 32-bit protected mode
- Uses x86 architecture with paging enabled
- Supports multiple user environments (processes)
- Implements virtual memory with demand paging
- Includes page replacement algorithms for memory management

## 👥 Development

This project was developed as part of an educational Operating Systems course. All functions marked with `TODO:` comments were implemented by the development team and successfully passed all test cases.

## 📄 License

See `inc/COPYRIGHT` and `kern/COPYRIGHT` for copyright information.

---

**Note**: This is an educational project. The codebase includes both provided framework code and student implementations. All functions we've successfully implemented are marked with `TODO:` comments in the source code.

