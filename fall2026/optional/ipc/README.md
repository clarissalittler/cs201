# Optional ipc clinic

[Teaching outline](Agenda.org) · [Optional material](../README.md)

## Preparation and pacing

Topic: Optional ipc clinic

### Optional two-hour clinic: from file descriptors to IPC
After meetings 13–14; not a prerequisite for the HTTP assignment.
- 10 min: recall descriptor lifetime and read returning zero at EOF.
- 20 min: trace [anonPipe.c](<anonPipe.c>) with parent/child descriptor tables.
- 25 min: compare [anonPipeLoopFix.c](<anonPipeLoopFix.c>); account for every copy of the write end.
- 10 min: break.
- 25 min: student diagram/exercise: close unused ends and explain why EOF can be delayed.
- 15 min: one optional contrast, either named pipes or mmapFile.c, not both.
- 5 min: exit check: distinguish shared bytes from synchronized access.
- 10 min: flex or project help.
Named pipes, domainClient/domainServer, mmapShared/mmapSem, signalDuel,
and the line-editor connections remain further examples. Do not tour them all.
Process-shared synchronization needs appropriate shared storage/configuration;
ordinary process-local mutex initialization is not sufficient. Review these
older sketches before running; this pass did not certify every IPC program.

