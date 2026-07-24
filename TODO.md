# TODO

**Note:** This file is for **short-term** TODOs only — a scratch list to hand context between work
sessions/machines and to brief agents (human or AI) picking up a task. It is not a roadmap, issue
tracker, or design doc. Prune entries as they're fixed; don't let this grow into a graveyard.

Generally will track stuff here between releases

## Open
- Fix Skel to generate VS2k26 project files too
- rename Build/ToolSrc.... stuff to match top level Tools and clearly document diff - why some stuff goes where

- Library/Projects/QtCreator/StroikaDevRoot.files is rotted - 31 of its 35 Build/Tests script entries
  name files that don't exist (stale .sh/.pl suffixes, plus scripts deleted years ago). Prune/regenerate
  it - or just drop the QtCreator project if nobody uses it anymore
- rename ScriptsLibPath var in ThirdPartyComponents/boost/Makefile (value is Build/Scripts now)
- Documentation/Building-Stroika.md - describe the Build/{Scripts,Lib,Shared} split for users
  (rationale is written up in AGENTS.md now)

- Move makedefaultconfigs from makefile into script in that folder
- Finish CONTAINER SEQUNNCE BIDI ITERATOR STUFF
- add more g++16 c++23 configs
- test WTF 
- test IPAM - etc...
- test HearHE
