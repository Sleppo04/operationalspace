# OperationalSpace
OperationalSpace is a game about space exploration, epic battles, resource management,
and **programming**. Yes, that's correct: you have to plan your ships actions
and program them in advance. Ships can be upgraded using different *modules*,
each one having a different effect on the ship stats, adding to the strategy factor of the game.

The goal of the game is to collect and control as many ressources as efficiently as possible.
The only tool you have for that are your *ships*. However, ships are fully customizable, and
can fulfill a variety of tasks depending on the *modules* you install into them, and of course
also depending on the scripts you write.

## Playing
Currently, the game relies on an ANSI-compatible terminal emulator for display. In the future, we might add
a custom solution to remove that dependency, but we will of course keep that option. In case you are unsure
about your terminal emulators support for ANSI, there is a test program in the compiled builds (`os-termtest`).

![The os-termtest program running in xterm](/doc/OSTermtestExample.png)

On Linux, we recommend using **xterm** as it has great support for even the most niche of features, but most modern
terminal emulators on Linux support ANSI atleast to a usable degree, for example **GNOME-Terminal**. **LXTerminal**
is one of the few terminal emulators that didn't support some of the important features.

On Windows, things are more complicated. From Windows 10 1511 onward the built-in terminal emulator **conhost.exe** (often
wrongly called **cmd.exe**) parses ANSI escape codes when the application specifically requests it. OperationalSpace does that.
However, conhost lacks a lot of basic formatting like *italic text* or ~~crossedout text~~, which makes it pretty hard to play the game properly.
The new Windows Terminal App, which is the default on Windows 11, is our recommendation for Windows systems.
If you are on a version older than Windows 10 you will have to use a third party program like ConEMU or PuTTY (only for remote sessions).

## Compiling
OperationalSpace is written in the C-language, more specifically the C99-dialect, so you compiler needs to fully support that.
We use versions of the gcc compiler (also on Windows though MinGW); the oldest tested version is 8.1.0, but it should
compile on versions way older than that. We try to keep the code free from compiler-specific things and stick to the
language standard to make the game as accessible as possible.