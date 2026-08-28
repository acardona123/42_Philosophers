<h1 align="center">philosophers</h1>

<p align="center">
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C"/>
  <img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Linux"/>
  <img src="https://img.shields.io/badge/GNU%20Make-A42E2B?style=for-the-badge&logo=gnu&logoColor=white" alt="GNU Make"/>
</p>

<p align="center"><strong>The dining philosophers, solved with POSIX threads and mutexes, without deadlock.</strong></p>

---

## 📌 Overview

Dijkstra posed the dining philosophers in 1965, as an exercise about processes competing for a limited set of devices. The setup is deliberately plain.

Philosophers sit around a table. Between each pair of neighbours lies a single fork, and eating takes two. Their life is a loop: eat, sleep, think, then try to eat again. What makes it hard is that they never speak. Nobody knows whether a neighbour is eating, sleeping or reaching for the same fork, and nobody can be asked to wait or to let go. A philosopher sees the two forks beside them and nothing else, so every decision is made blind.

Now suppose all of them reach left at the same moment. Each one is holding a fork. Each one is waiting for the fork on their right, which the neighbour is holding and will not release before eating. No rule was broken and nothing crashed. The table simply never moves again.

That is a deadlock, and here it does not merely hang: it kills. A philosopher who has not started a meal for `time_to_die` milliseconds dies, so the clock keeps running while everyone waits politely, and the whole table starves in silence. There is a quieter failure too, where the program runs fine but one philosopher keeps losing the race for a fork and dies alone. Both depend on timing, so both appear on some runs and not others.

This project rebuilds all of it in C: one thread per philosopher, one mutex per fork, and a monitor watching the deadlines. Forks here are real locks and they do the real work. What the implementation adds is a correct start, with philosophers beginning in two offset groups, three when their number is odd, so the first round of contention comes out alternating instead of simultaneous. The loop then holds that phase, and the locks carry the rest of the run.

## 🎯 Objectives

- Model concurrent processes competing for shared resources, with threads and mutexes.
- Prevent deadlock and starvation at once, and know which mechanism prevents which.
- Protect every shared value, including timestamps a monitor reads while their owner writes them.
- Stay accurate in real milliseconds, using a sleep primitive that guarantees a minimum and not a maximum.
- Detect a death within a few milliseconds, then shut every thread down with no leak and no mutex left alive.

## 📋 Constraints

Most of the difficulty comes from what the subject forbids. The allowed functions are the whole toolbox:

```
memset  printf  malloc  free  write  usleep  gettimeofday
pthread_create  pthread_detach  pthread_join
pthread_mutex_init  pthread_mutex_destroy  pthread_mutex_lock  pthread_mutex_unlock
```

No condition variables, no semaphores, no atomics, no `nanosleep`.

- No global variables. Shared state travels through a struct handed to each thread.
- One thread per philosopher, one fork per pair of neighbours, both forks needed to eat.
- A death must be reported within 10 ms. That sets the pace of the monitor.
- Log lines must not interleave, so printing is itself a critical section.
- Times in milliseconds, timestamps from the start of the run, philosophers numbered from 1.
- No leaks, and the 42 Norm: 25 lines per function, 5 functions per file, 4 parameters, no `for`, no `switch`, no ternary.

## 🧠 How it works

### Forks

Each fork is a mutex guarding an availability flag.
Taking one means locking the mutex, reading the flag, then claiming it or unlocking and retrying later.

A philosopher waiting for a fork therefore never sleeps holding another fork's mutex: after having read and potentially updated the fork availability it unlocks the mutex.

Everyone reaches in the same order, left fork then right fork. That is exactly the order that deadlocks if the whole table reaches at once, which is what the staggered start is for.

### Starting the cycle

The first round of contention comes out alternating rather than simultaneous: 
- Philosophers with even id start eating (taking the forks) first, odd ones will wait `time_to_eat` before trying to reach for forks.
- With an odd number of philosophers, the last one waits twice as long to avoid competition with the first philosopher.

The point of this scheduling is only to enter the loop in a workable phase. From there the locks handle contention normally.

### Spacing out the attempts

Retrying is correct but not free. Every failed attempt costs a lock and an unlock on a fork somebody else is busy with, plus 240 µs of a thread doing nothing, and it contends on exactly the forks that are already the most contended. A philosopher that thinks for the shortest legal time and then hammers its neighbour's fork wastes a lot of work to end up eating at the same moment it would have anyway.

So the philosophers are paced. Their loop is given a fixed period, `t_cycle`, measured from the moment one reaches for its forks to the moment it reaches again. Eating and sleeping take exactly as long as the rules say, and thinking absorbs whatever is left of the period, which sets the next attempt at roughly the time the forks should be free.

`t_cycle` is how long the table needs to serve everyone once:
- With an even number of philosophers, they split into two alternating groups, and each fork is free every other turn. Two `time_to_eat` is enough.
- An odd number cannot be split in two around a circle, so it takes three groups, and three `time_to_eat`.
- Either way the period is at least `time_to_eat + time_to_sleep`, since a philosopher cannot reach for a fork again before finishing its meal and its nap.

Sleeping and thinking are timed against the start of the last meal rather than against each other, so time lost waiting on a fork is absorbed by the cycle instead of pushing the next attempt later and later. The spacing set at startup is kept, most attempts succeed on the first try, and the spin loop stays close to idle.

### The monitor

The philosophers are spread across threads and, by the rules of the problem, blind to each other. Yet someone has to notice that one of them died, and every one of them has to stop when it happens. That is the one thing the simulation knows and the philosophers do not, so it is kept outside them.

- The main thread does the watching, polling every 3 ms.
- It reads each philosopher's last meal timestamp under that philosopher's own mutex, and declares a death once `time_to_die` has passed.
- It also counts meals when the optional fifth argument caps them.
- Threads check a shared `all_alive` flag at every step, and printing takes the same lock, so nothing is logged after the death line.

### Time

Every rule in this project is a deadline, so a delay that no ordinary program would notice is enough to kill someone here.
`usleep` is not precise enough on its own: it returns no earlier than asked and promises nothing about how much later, and on tight parameters that overshoot alone can push a philosopher past `time_to_die`. Waiting is therefore not delegated to it. Every wait is a poll of `gettimeofday` in micro steps until the target timestamp is actually reached, so the simulation is paced against the clock rather than against the scheduler. The length of those steps was settled by testing, as a compromise between precision and the CPU spent checking the clock.

Those timestamps are also only comparable if they are counted from the same instant. Threads are created before the simulation starts and wait on a shared start timestamp before running their first meal, so every philosopher measures from the same origin and the log reads as one timeline instead of several. That same shared value doubles as an abort sentinel: if a thread fails to be created, setting it to a sentinel releases the ones already waiting and they exit without ever starting.

## 🛠️ Tech Stack

<p>
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C"/>
  <img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Linux"/>
  <img src="https://img.shields.io/badge/GNU%20Make-A42E2B?style=for-the-badge&logo=gnu&logoColor=white" alt="GNU Make"/>
</p>

C with the POSIX threads API, built with `cc -Wall -Werror -Wextra`, linked against `-lpthread`.

## 🚀 Getting Started

```bash
git clone https://github.com/acardona123/42_Philosophers.git
cd 42_Philosophers
make
```

## 📖 Usage

```bash
./philo nb_philosophers time_to_die time_to_eat time_to_sleep [meals_per_philosopher]
```

```bash
./philo 5 800 200 200      # runs until stopped
./philo 5 800 200 200 7    # stops once all five have eaten seven meals
```

Times are in milliseconds. Without the fifth argument the simulation runs until someone dies. Arguments are checked first: 4 or 5 of them, times of at least 60 ms, between 1 and 200 philosophers.

One event per line, `timestamp_ms philosopher_id action`:

```
0 1 has taken a fork
0 1 has taken a fork
0 1 is eating
200 1 is sleeping
```

## 🧪 Tests

There is no return value to check, the proof is the log. Four properties have to hold on every run:

- Nobody eats without two forks.
- No timestamp goes backwards, and nothing is printed after a death.
- Nobody dies while the parameters allow them to live.
- The death line lands within 10 ms of the deadline.

| Case | Command | Expected |
| --- | --- | --- |
| One philosopher | `./philo 1 800 200 200` | Dies at 800, one fork never becomes two |
| Comfortable | `./philo 5 800 200 200` | Runs forever, nobody dies |
| Tight | `./philo 4 410 200 200` | Survives only if the phase holds |
| Meal cap | `./philo 5 800 200 200 7` | Stops cleanly, all five fed seven times |
| Full table | `./philo 200 800 200 200` | Same, at the 200 philosopher ceiling |

A table that deadlocks can take minutes to do it, so the comfortable case is left running rather than glanced at.

Two switches in `philo.h` help while working. `COLOR` gives each philosopher its own colour, `DISPLAY_END_MSG` prints the elapsed time once everyone has been fed.

## 📁 Structure

```
main.c        entry point and the monitor loop
init.c        parsing, mutex and thread creation, group assignment
routine.c     the philosopher thread: eat, sleep, think
forks.c       taking and releasing a fork
time.c        clocks and the polling sleep
print_msg.c   log lines, serialised and cut off after a death
tools.c       string and number helpers
unset.c       stopping the threads, destroying every mutex
philo.h       shared structures and tuning constants
```

## 📚 Resources

- [`pthreads(7)`](https://man7.org/linux/man-pages/man7/pthreads.7.html) and [`gettimeofday(2)`](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

---

<p align="center"><sub>🏫 Project from the <strong>42</strong> common core, School 42 Paris.</sub></p>
