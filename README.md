
Welcome to Octopuzzler!

— PREMISE —
Your goal is to solve all of the puzzles and move your files to the floppy disk
before the octopus gets ink all over your computer. It will take guts. It will
take tenacity. But most of all, it will take a hunger for the waffles you DESERVE.

From start to finish, the game should take around half an hour to complete.

— CONTROLS —
This game can be played entirely with the mouse! As the controls have proved
difficult to explain through text in our playtests, we have elected to present
them as animated guides throughout certain levels. If you are confused by the
controls in later levels, refer back to the GIFs located in the "guides" folder.

— BORDERLESS FULLSCREEN —
Borderless fullscreen can be toggled with the F11 key!

— SPEEDRUN MODE —
To run the game in Speedrun Mode, you will need to open your game folder in the
Command Prompt (Windows) or Terminal (MacOS or Linux) and type:
	Octopuzzler.exe --speedrun
Doing this will launch your game in speedrun mode, unlocking all controls always
and causing the game to reset completely on death.

— CUSTOM CONTENT —
Custom levels should follow the following format:
{
	"level" : [
		"WWWWWW",
		"WS  GW",
		"WWWWWW"],
	"controls" : ")", 
	"guideLeft" : "bind",
	"guideRight" : "moving"
}

The grid can be any width or height but should have rows of equal length.
- W's represent Walls, spaces that cannot be moved into.
- Spaces represent paths, spaces that can be moved into.
- H's represent Holes, spaces that cannot be directly moved into but can be dashed
  over.
- Each level should have one S, representing the Start, and one G, representing
  the Goal.

Where the "R" is present in the example are the control options for this level.
Each desired control should be present within the quotation marks. To have every
control unlocked, "*,()^_<>" should be in place of the example's ")".
- * is for Up.
- , is for Down.
- ( is for Left.
- ) is for Right.
- ^ is for Dash Up.
- _ is for Dash Down.
- < is for Dash Left.
- > is for Dash Right.

The game has two spots available for Guides: one on the left ("guideLeft") and
one on the right ("guideRight"). Within res/ObjectData/UI of your game folder,
are the png files that compose the animations for the game's three Guides,
"bind", "moving", and "multibind". When a guide is displayed in-game, it will
start on 0th image of that animation and run to the final image of the animation
before restarting. In order to add a custom Guide, name each frame of your Guide
with your Guide's name followed by the number for that frame from zero. The frames
will be played in sequential order. You can add a Guide to a level by putting the
name of the Guide in quotations where "bind" or "moving" is present in the example.
To omit guides from your level, simply remove the field(s) "guideLeft" and/or
"guideRight".

The order that levels are given in follows similar naming conventions to that of
animations. Levels are named "level" followed directly by the number in the
sequence that they appear in. The example level shown above is called "level00" in
the levels folder, res/StageData of the game folder. The base game has 15 levels,
numbered from 00 to 14. Take note that these levels do not have a suffix such as
"txt", denoting a text document. Instead of "level00.txt", the example level is
simply "level00".



Created by 2FoamBoards and TheSunCat. Music by Orchid.
