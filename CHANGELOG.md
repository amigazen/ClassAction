# Changelog

All notable changes to **ClassAction** and **ClassActionPrefs**.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versions are listed **newest first**.

The **amigazen project** entry documents this GitHub tree. Historical entries are taken from the ClassAction 4 AmigaGuide **History** node (`Source/ClassAction/ClassAction.guide`) and the Aminet-style `classaction.readme`, with AmigaGuide markup converted to Markdown. Source file headers stamp intermediate 4.1–4.6 builds, but no separate release notes for those versions appear in the ClassAction 4 documentation.

---

## [4.7] — 2026-07-26 (amigazen project)

**Lineage:** 68k · Reaction · NDK 3.2 · SAS/C (based on Martin Elsner **4.6** / Aminet `Classaction4_src`)

### Added

- SAS/C `smakefile`s for ClassAction and ClassActionPrefs (68020, `objects_sasc/`, NDK + vendored include paths; Prefs uses `CODE=FAR` and shares `IniFile` / `StringList` / `cautils` from ClassAction)
- `ClassAction.lnk` / `ClassActionPrefs.lnk` slink scripts for the SAS/C object set
- `embed_rct_res.py` plus generated `ClassAction_res_embed.c` / `ClassActionPrefs_res_embed.c` (Storm RCT + CatComp blob with runtime RELOC32 fixup via `CA_FixupRCTResource()`)
- `ca_begin.h` — neutralize NDK `__ASM__` / `__REG__` / … then include `<clib/compiler-specific.h>` for SAS/C C++
- `stormcompat.h` — Storm-style bare `Library` / `Window` / `Hook` / … typedefs over NDK `struct` tags
- `cadebug.h` — `CA_D()` Printf tracing behind `DEF=CA_DEBUG`
- Reconstructed missing Global sources: `IniFile.cpp` / `IniFile.h`, `StringList.cpp` / `StringList.h` (from 4.6 object API)
- Vendored `proto/` / `clib/` / `pragma[s]/` / `libraries/` / `inline/` headers for resource, popupmenu/pm, xadmaster, and xfdmaster
- C++ translation units of hybrid Storm units for SAS/C: `cadisk` / `caexec` / `cagui` / `careq` / `caxfd` and Prefs `capdisk` / `caplearn` / `caplist` / `capreq`
- `GetProgramDir()` in `cautils` (Storm runtime API that SAS/C lacks)

### Changed

- Version stamp **4.6** → **4.7** (`cavers.h`, `ClassActionPrefs_rev.h`; date 26.7.2026)
- Asset/config paths from `MRE:` to `PROGDIR:` (prefs, icon, guide, prefs app, lister/filetype images, patterns)
- Removed Storm `is` / `isnot` / `and` / `or` / `not` / `True` / `False` macros from `catypes.h` / `captypes.h`; sources use real C operators
- `main()` / `wbmain()` both call shared `CA_Startup()` / `CAP_Startup()` (SAS/C C++ cannot call `main()` from WB)
- AmigaGuide opens lazily and zero-initialized (`CA_OpenHelp`) instead of at init with a garbage `NewAmigaGuide`
- Missing prefs no longer aborts ClassAction; falls back to built-in defaults with a warning
- `RCTResource` is now `__far char *` with an explicit fixup call before `RL_OpenResource`
- BOOPSI/icon class APIs use NDK `Class *` instead of `IClass *`; `PopupMenuBase` typed as `struct PopupMenuBase *`
- `InitHook` assigns `h_Entry` via `void **` cast to avoid SAS/C C++ `__stdargs` mismatch

### Fixed

- SAS/C slink Error 611 on Storm RCT objects’ odd-address RELOC32 (embed + startup fixups instead of linking `*_res.o`)
- Prefs near-code Error 502 (image too large) via `CODE=FAR`; RCT embed compiled with `DATA=FAR`
- SAS/C C++ Errors 1522 / 1615 / 39 from NDK registerized function-pointer members (`ca_begin.h`)
- Registerized hooks/dispatchers: `IDCMPHandler` and `butclass_DISPATCH` use raw `__asm` / `__saveds` / `register __aN`
- Empty `if (success) else` and related Storm-keyword fallout so C++ compiles cleanly across CA/Prefs sources

---

## [4.6] — 2004-05-30 (Martin R. Elsner)

- Final source tree stamped in `cavers.h` / `ClassActionPrefs_rev.h` as **ClassAction 4.6** / **ClassActionPrefs 4.6** (30.05.2004). No separate History-node notes for 4.1–4.6 appear in the ClassAction 4 documentation shipped with the sources.

---

## [4.0] — 2001-03-30

- "Rewritten" the program to use Reaction and new OS3.5+ features.
- Now only 13 path buttons can be configured (should be enough!).
- Manager actions are now images!
- Devices are now shown in the normal file list.
- New buttons: MakeDir also for right file list, WbInfo, AddIcon, MoveAs, all the selecting methods.
- Added a menu(!) with help, preferences, settings, ...
- Added asynchronous help!
- Added a progress bar for copying/moving/...
- Now the class of a file is the one with the MOST fitting offsets, not the first one that matches.
- Comment optimization and Tooltype NOCOMMENT removed.
- New tooltypes: COPYICONS,CREATEICONS,DRAWERSFIRST,HELPFILE,SHOWHELP X,Y,WIDTH,HEIGHT,POPUPWIDTH,POPUPHEIGHT,more keys
- ARexx port is now CLASSACTION.1 to make handling easier

---

## [3.6] — 1998-03-28

- Added a new executable named CAStart wich can be set as project for your icons, and thus opening the CA window choices.
- ClassAction now set the filetype in file comments in order to speed up file recognition.
- Tooltype NOCOMMENT added to disable file comments.
- Main DirList of ClassAction is now keyboard usable With up/down/space/Enter keys
- Added key filestart, if you press on a key, the first file begining with this letter is selected (Shift+Key for Directories)
- Added a key to allow pattern selection (configurable via tooltype)
- Added a key to select all Files (configurable via tooltype)
- Added a key to unselect all files (configurable via tooltype)
- Added a key to Toggle files (configurable via tooltype)
- Added a key for Parent Dir (configurable via tooltype)
- Added a key for SelectFile (configurable via tooltype)
- MoveDir bug fixed
- Makedir requester bug fixed
- Some routines optimized and thus faster.
- Command Crash/Hang/Hit in ClassActionPrefs Fixed
- Some minor other bugs fixed

---

## [3.5] — 1997-06-27

- Bonus Version.
- CA_Register is no more needed, registration is done on-line on my site http://www.gasmi.net
- File Manager now handle icons as well
- Added tooltype DISPINFO
- Modifyed the tooltype GTLIKE
- Minor bugs removed

---

## [3.4] — 1997-02-07

- This was supposed to be the LAST release of ClassAction
- Corrected some minor bugs
- Added tooltype ICONSTART to force ClassAction to start iconified for people who does not know how to do this using MUI Prefs

---

## [3.3] — 1996-08-23

- ClassAction is no more ShareWare but MailWare !!!!
- Included Ca_Register to register for free
- Some Bugs removed .

---

## [3.2] — 1996-07-07

- ClassAction has been recompiled with latest release of Dice and is more stable.
- Arexx engine is now full .
- Some "bugs" removed.

---

## [3.1] — 1996-04-09

- ClassAction have now a tiny built-in File manager.
- Some bugs removed.

---

## [3.0] — 1996-03-12 (Major Update)

- ClassAction and ClassActionPrefs now use MUI 3.0+
- ClassAction can have up to 50 path buttons
- ClassAction recognize directories.
- ClassAction handle multi selection.
- ClassAction has an appwindow.
- You can set the Learning Accuracy now.
- Tooltypes CAPREFS,GTLIKE added.
- Tooltypes HEIGHT,APPSTART,WINX,WINY,ICONX,ICONY,CX_PRIORITY,CX_HOTKEY REQBUG,STARTDIR,WBFONT,PUBSCREEN removed and no more needed.

---

## [2.8] — 1995-09-25

- ClassAction have now a FULL commodtity support and a HotKey to show/hide ClassAction.
- Tooltype APPSTART can be set to HIDE (APPSTART=HIDE) if you want that ClassAction start hidden.
- Tooltype CX_HOTKEY added
- Tooltype PUBSCREEN added to allow using Public Screens
- The Internal File Selector of ClassAction was not freeing all the memory allocated, fixed now .

---

## [2.75] — 1995-09-11

- The REQD,REQF,REQV commands were not really incompatibles with the appicon mode, but incompatibles with some programs such MagicMenu (Bad luck I use MagicMenu...) A lot of users complained about the automatic swith into a REQT command even if they don't use an incompatible program. I have added a tooltype (REQBUG) to let the user choose to auto-switch into REQT or not.

---

## [2.7] — 1995-09-03

- Added commands [b],[x],[B],[X],[F],[S]
- Added the Action arexx command.
- removed some bugs in rendering routines
- removed a bug with multiple icons throwed on the AppIcon.
- The REQD,REQF,REQV commands are now swapped into REQT when using them from the AppIcon or Arexx , they are only compatible with the Window mode.

---

## [2.6] — 1995-08-28

- ClassAction window is now resizable.
- ClassAction and ClassActionPrefs are now using ReqTools.library.
- Added REQV command to request a volume.
- Added REQT command to request a text.
- Added WINX and WINY tooltypes.
- Learn requester has now an ALL button.
- Selected File is now in all the REQs requesters.
- ClassAction use now a Key file for the registerd versions, this key file is placed in S: .

---

## [2.5] — 1995-07-17

- ClassAction and ClassActionPrefs are now localised. and a French catalog is provided with the archive.
- Learn function added to ClassActionPrefs.
- To select the first action of a file, you must now Double Click on it instead of reselecting the file.
- Generic Actions are now Synchro and rescans the current directory.
- AUTOSELECT ToolType is no more used.
- REQs requesters opens now in current directory.
- minor improvements made.

---

## [2.1] — 1995-06-12

- Added the 'Generic Actions' Built-in Class.
- Added SURE[] exec command.
- Added ASCII[] Offset Command to recognize ASCII files.
- Added Arexx commands: AppIconify, Show, Status, GetClass.
- Changed running tasks system, now I use Systemtags(). We do not need tmp files anymore.
- you can now define a delay for CLI run mode.
- Added 'string' and "string" for offsets definition.
- ToolType OUTPUT is obsolete now and not used anymore, we use the new ToolType CLISIZE in replacement.
- Actions requester is now well sized, appear below the mouse pointer and uses to frontmost public screen.
- Swapped the buttons 'Use' and 'Save" and added 'Cancel' in ClassActionPrefs to follow the Amiga prefs look, moved the button 'about' in top right corner as '?'.
- ClassActionPrefs Cycle gadgets routines weren't 100 % system friendly and some patches like Cycle2Menu makes bugs with ClassActionPrefs; It's fixed now.
- Improved the recognizer code and the Info routine: they are up to 400% faster.
- Listview hilight color error removed.
- ClassAction does not anymore lock the Workbench screen when AppIconified.
- A nasty bug found and removed in ClassActionPrefs.
- Right Mouse button shows Assigns only if mouse is in the requester and Right Mouse again brings back to the Directory.
- ClassAction remember now the window position.

---

## [2.00 (Major Update)] — 1995-05-23

- ClassAction has now an AppIcon.
- ClassAction is now a commodity.
- ClassAction has now an Arexx port.
- ClassAction use now the default WB Font.
- Exec mode 'Arexx' added.
- Different color for Directories/Files.
- Up/Down gadgets added to ClassActionPrefs.
- 'Use' gadget added to ClassActionPrefs.
- Classes are now sorted into ClassActionPrefs Listview.
- APPSTART, ICONNAME, ICONX, ICONY, CX_PRIORITY, WBFONT, OUTPUT, ICONFILE ToolTypes added.
- When the window is iconnified it have now the right height regarding the screen default font.
- New Save Format (CASF20).
- Suffix/Prefix Button removed. Replaced by MatchName Gadget who accept any Wildcard.
- Config file moved into ENVARC:
- an installer is now provided with the archive.
- some Code optimization done.

---

## [1.43] — 1995-05-05

- Cleaned up the requester code, now 5% faster.
- ClassAction now look for his name using WBstartup structure and then can be renamed .

---

## [1.42] — 1995-05-02

- 'No Cli' Exec mode added to ClassActionPrefs.
- "" are always added to filenames even if not needed it's easier for AREXX scripts.

---

## [1.4] — 1995-04-06

- ToolType HEIGHT added.
- Some code optimization added.

---

## [1.31] — 1995-02-22

- If you click twice on the same file the first action will be lanched (it's faster than selected the first action by hand).
- this version is now ShareWare and you must register to get the registered version.

---

## [1.3] — 1995-01-15

- Added REQD[] and REQF[] interactive commands.
- Copy gadget added to ClassActionPrefs.
- minor improvments made.
- Beta testers reported this version is really stable.

---

## [1.22] — 1994-11-25

- First Public Release.
- Button 'Info' Added.
- ClassAction was Locking() the directories it read without UnLocking() them .... *FIXED*
- Code Optimization.
- Minor other Bugs removed.

---

## [1.21] — 1994-11-08

- Program was crashing with empty floppy units .. *FIXED*
- Volume/Name Bug Fixed
- <..> Item removed when root of a volume.

---

## [1.2] — 1994-11-07

- New interface, I have included my own fast file requester.
- STARTDIR & DRIVE1 to DRIVE11 ToolTypes added.

---

## [1.1] — 1994-11-01

- Now using xfdmaster library to recognize and decrunch files.
- Configuration with ToolTypes added (DECRUNCH,AUTOSELECT).
- 'Unknown Class' is now a built in class with unlimited actions.
- New save format (CASF11).

---

## [1.0] — 1994-10-16

- New Save Format (CASF10).
- Window has now a zoom gadget.
- a lot of classes definitions added.

---

## [Beta Version] — 1994-10-10

- tmp file bug removed.
- Offset increment error removed.
- using asl library for the file requester.

---

## [Alpha Version] — 1994-10-01

- (no notes in the History node)
