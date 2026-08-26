# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open

- review @#https://github.com/SophistSolutions/Stroika/issues/769
wtih regards to naming of SKEL test...

- **Revisit on/after 2026-08-27: Medusa-Windows-Dev build performance.**
  Applied + verified 2026-08-25 (survived a cold boot of host and guest): Hyper-V hypervisor
  enabled, boot disk SATA -> virtio-blk, NIC e1000e -> virtio, guest RAM 10 -> 18 GB (24 GB
  ceiling, retunable live via `virsh setmem`), Defender exclusion for `C:\ProgramData\Docker`,
  and the docker Windows service (not Docker Desktop) owning the engine so builds work with
  nobody logged in. All four Windows buildvm images now build there (~4h45m, first success).
  Still open:
    - Was 18 GB the right number? Untested under a real Stroika build.
    - VSMB cost of `docker run -v c:\Sandbox:c:\Sandbox` under Hyper-V isolation: a bind mount
      goes host<->utility-VM over VSMB, which for small-file build IO may dominate everything
      above. Untested. Compare against a tree cloned inside the container.
    - Host CPU load on medusa is mostly chrome + gnome-shell rendering camera streams (~700%
      combined) rather than frigate itself (254%) - closing those windows is free performance.
    - CCD pinning + hugepages deliberately deferred; do not re-propose unprompted.
