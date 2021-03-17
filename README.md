# xv6-steps
Development of xv6 teaching OS in steps.

1. Booting
2. General setup (interrups, smp, paging and cprintf()). No processes yet.
3. Process support: 
   - Syscalls (`sysproc.c`): `fork()`, `sleep()`, `wait()`, `print()` and `exec()`
   - `ramfs.c`: Simple RAM filesystem for user files (programs + data)
      Files are linked at end of xv6.img (same aproach as `initcode`)
   - Test user program `/init.c`
   - Student project: Make a shell. It should be launched from `/init.c`
