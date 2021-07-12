# Microcosm
A weekend project, was seeing how feasible it would be to create a Civilization clone that runs in the command line.  Currently, this is little more than a map generator.  I would like to flesh this out into a full game, but I'm not sure if I'll have the time.

The idea, above all, is to keep the code simple and lightweight.  I would love to port this to the Sega Genesis or GBA at some point as both those consoles are missing a strategy game like Civ and have C compilers.  The map generator is less than 300 lines of C.  I also think simplified/streamlined Civ rules would make the game more fun to play and accessible, but those rules have yet to be coded.

I used the CodeBlocks IDE again.  This depends on PDCurses but no other libraries, you'll have to supply the library yourself.  There's also a Python file I used to test the generator before porting it to C, it requires Pygame to run.