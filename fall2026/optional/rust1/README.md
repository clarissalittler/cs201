# Optional rust1 clinic

[Teaching outline](Agenda.org) · [Optional material](../README.md)

## Preparation and pacing

Topic: Optional rust1 clinic

### Optional two-hour clinic: ownership as a contrast to C
Use an extra confirmed meeting or a separate recording after core project work.
- 10 min: recall one C dangling-pointer/ownership problem.
- 20 min: [hello.rs](<hello.rs>) then [echo.rs](<echo.rs>); introduce only the syntax used.
- 25 min: inspect [badborrow.moveme](<badborrow.moveme>) and [fixborrow.rs](<fixborrow.rs>). Copy the bad file to a scratch .rs filename to see the diagnostic; keep the original unchanged.
- 10 min: break.
- 25 min: students repair a move/borrow example, using [func2.rs](<func2.rs>) as reference.
- 15 min: choose either [fileLines.rs](<fileLines.rs>) for resource cleanup or [thread2.rs](<thread2.rs>) for argument ownership, not both.
- 5 min: explain what the compiler rejected and which lifetime caused it.
- 10 min: questions.
The iteration catalog, vectors, bitmunger, line editor, Arc/Mutex, and channels
are optional follow-up. No Rust implementation is required for a C assignment.

