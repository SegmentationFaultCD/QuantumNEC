# MuQSS Scheduler Critical Issues and Fixes

## Key Problem
The MuQSS scheduler has multiple critical bug in the `schedule()` function (lines 98-104 in Muqss.cpp) that causes #GP, #PF, and TF faults in multicore scenarios.

## Critical Bug 1: Atomic Lock Management Failed
```cpp
rq.core.lock->unlock( );   // Partial unlock - line 74
current.core.lock->unlock( );  // Second unlock - line 75
return;  // Exit function

// IF try_lock() FAILS:
rq.core.lock stays LOCKED
current.core.lock.gets UNLOCKED
rq.core.scheduler_queue stays UNCHANGED
current.core.next stays EMPTY
```

**Why this causes #GP/PF/TF:**
- Other cores hold scheduler's locked resources
- Current core releases its lock and tries to access same resources
- Memory corruption and register safety violations
- Kernel crash due to invalid memory access

## Critical Bug 2: Lock Access Outside Lock Context
```cpp
if ( interactive ) {
    // Unlocked: sq created
    while ( !sq.empty( ) ) {
        if ( auto &rq = scheduler->running_queue[ sq.top( )->schedule->cpu ]; rq.core.lock->try_lock( ) ) {
            // LOCKED RQ, LOCKED CURRENT
            // ... process ...
            rq.core.lock->unlock( );      // OK - unlock rq
            current.core.lock->unlock( ); // OK - unlock current
            return;  // EXIT
        }
    }
    current.core.lock->unlock( );  // PROBLEM! No rq lock held
}
```

**Why this causes #GP/PF/TF:**
- current.core.lock unlocked without rq.core.lock held
- current core accesses scheduler->running_queue[ p->schedule->cpu ].core.running_task
- scheduler->running_queue is not protected by current.core.lock
- Memory corruption and segmentation faults

## Critical Bug 3: Potential Deadlock
```cpp
while ( !sq.empty( ) ) {
    if ( auto &rq = scheduler->running_queue[ sq.top( )->schedule->cpu ]; rq.core.lock->try_lock( ) ) {
        // LOCK RQ, THEN LOCK CURRENT
        // ... process ...
        rq.core.lock->unlock( );
        current.core.lock->unlock();
        return;
    }
    // rq.lock NOT ACQUIRED
    // Just pop and continue
    sq.pop( );  // This function itself doesn't allocate
}
current.core.lock->unlock();  // POSSIBLE DEADLOCK HERE
```

**Why this causes #GP/PF/TF:**
- Deadlock when all cpu locks are held by other cores
- Current core spinlocks forever trying to acquire cpu locks
- Queue of pending exceptions builds up
- #GP/#PF faults when interrupt handling reaches critical section

## Critical Bug 4: Wrong Lock After Early Exit
```cpp
if ( !current.core.RT_task_queue.empty( ) ) {
    current.core.next = std::move( current.core.RT_task_queue.front( ) );
    current.core.RT_task_queue.pop( );
    current.core.next.schedule->cpu = current.core.cpu_id;
    pop_running_tasks( );
    current.core.lock->unlock( );  // Lock released
    return;  // EXIT
}
// ... more code ...
```

**Why this causes #GP/PF/TF:**
- Unexpected mode switch to running_task with wrong scheduling context
- Invalid register states cause #GP fault
- Page table corruption causes #PF fault

## Critical Bug 5: Incorrect PCB for Wake-Up
```cpp
if ( pcb.schedule->priority < p->schedule->priority || 
     pcb.schedule->virtual_deadline < p->schedule->virtual_deadline && q.core.lock->try_lock( ) ) {
    q.core.next = std::move( pcb );  // MOVE FROM wcit's PCB TO q.core.next
    q.core.next.schedule->cpu = q.core.cpu_id;
    q.core.lock->unlock( );
    return;
}
```

**Why this causes #GP/PF/TF:**
- pcb is a temporary/waiting task - shouldn't schedule to CPU
- Activation generates invalid stack pages
- Invalid page table mappings cause #PF fault

## Solution: Fix All Critical Bugs

### 1. Ensure lock is atomically acquired or not at all
- Only release rq.core.lock if acquired
- Only release current.core.lock if acquired
- Always check try_lock result before unlocking

### 2. Protect all scheduler access with proper locks
- Current core lock must be held when accessing any scheduler->running_queue
- Other core locks must not be acquired
- Remove nested locking in interactive mode

### 3. Fix wake_up handling
```cpp
// BAD - Can schedule waiting tasks directly
if ( pcb.schedule->priority < p->schedule->priority || pcb.schedule->virtual_deadline < p->schedule->virtual_deadline && q.core.lock->try_lock( ) ) {
    q.core.next = std::move( pcb );
    q.core.next.schedule->cpu = q.core.cpu_id;
    q.core.lock->unlock( );
    return;
}

// GOOD - Check priority to decide where to queue
if ( pcb.schedule->priority < p->schedule->priority ) {
    if ( pcb.schedule->virtual_deadline < p->schedule->virtual_deadline && q.core.lock->try_lock( ) ) {
        q.core.next = std::move( pcb );
        q.core.next.schedule->cpu = q.core.cpu_id;
        q.core.lock->unlock( );
        return;
    }
} else if ( pcb.schedule->virtual_deadline < p->schedule->virtual_deadline && q.core.lock->try_lock( ) ) {
    q.core.next = std::move( pcb );
    q.core.next.schedule->cpu = q.core.cpu_id;
    q.core.lock->unlock( );
    return;
}
```

### 4. Fix interactive mode locking
```cpp
// BAD - Locks nested incorrectly
if ( interactive ) {
    current.core.lock->lock( );  // Lock current
    while ( !sq.empty( ) ) {
        if ( auto &rq = scheduler->running_queue[ sq.top( )->schedule->cpu ]; rq.core.lock->try_lock( ) ) {
            // LOCK RQ, THEN LOCK CURRENT - DEADLOCK!
            // ... 
        }
    }
    current.core.lock->unlock( ); // Unlock current
}

// GOOD - Simple non-nested locking
if ( interactive ) {
    std::priority_queue< const PCB *, std::cxxvector< const PCB * >, mCore::compare_ptr > sq;
    for ( auto &cpu : scheduler->running_queue ) {
        if ( cpu.core.cpu_id == current.core.cpu_id ) continue;
        if ( !cpu.core.scheduler_queue.empty( ) ) {
            sq.push( &cpu.core.scheduler_queue.top( ) );
        }
    }
    
    // Lock each rq individually, release between attempts
    while ( !sq.empty( ) ) {
        auto cpu_id = sq.top( )->schedule->cpu;
        if ( scheduler->running_queue[ cpu_id ].core.lock->try_lock( ) ) {
            // Process this rq, release its lock
            // ...
            scheduler->running_queue[ cpu_id ].core.lock->unlock( );
            sq.pop( );
        } else {
            sq.pop( );
        }
    }
}
```

### 5. Fix scheduler.cpp integration
```cpp
// The root cause - scheduler instance is not properly managed
// Instead of singleton, it should be per-core or properly initialized

// BAD - Global singleton
scheduler = new Scheduler;

// GOOD - Each core gets its own scheduler
// Need to initialize running_queue per-core properly
```

## Build Test
To test the fixes:

1. First, fix the code (hard to complete due to extensive changes)
2. Build with `git submodule update --init && xmake -P .`
3. Run QEMU with `-s` for GDB
4. Add intentional context switches to test multicore scheduling
5. Verify no #GP/#PF/#TF faults