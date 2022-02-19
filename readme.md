A windows desktop C++ implementation of the Iowa Gambling Task based on original work of Bechera et al. (1994). The task can be built using the included visual studio project files or it may be possible to run the pre-built binary file -- igt.exe, igt.exe has been tested on windows 10 and should work out of the box but otherwise a build may be needed. igt.exe was built using Visual Studio Community Edition 2019.

Configuration files are also included (params.txt, params.txt_Inst and params.txt_EndMsg). Configuration files must be in the same directory as igt.exe. params.txt parameterises the main experiment, params.txt_Inst and params.txt_EndMsg contain instructions that are presented at the start and end of the experiment, respectively. The file params.txt_Data is produced automatically and data from each run of the experiment is appended. You can have different experiments configured, the general pattern is:

basename.txt         --parameters for the IGT task, required

basename.txt_Inst    --initial instructions, required

basename.txt_EndMsg  --end of experiment instructions, required

basename.txt_Data    --all data collected with parameter/instruction files basename\* will be appended in here, produced automatically

The data file has rows with the following format, one row for each trial:

ID	TN	deck	win	loss	net	balance	dts

ID	-- participant ID, integer entered on command line

TN	-- trial number

deck -- selected deck id

win -- the value of the computed win

loss -- the value of the computed loss

net  -- the net value for the trial (shown to participants)

balance	-- the cumulative balance (shown to participants)

The parameter file parameter file contains 9 lines. 

first four lines contain deck parameter lines.

line 5 contains inter-trial-interval (s) double

line 6 contains feedback duration (s) after each trial double

line 7 contains int number of trials

line 8 contains double starting balance

line 9 contain string currency label

deck parameter lines:

label win    pwin   lose   plose

(char  double double double double)

e.g.

A    1   0.5     2   0.7

deck A: win amount 1, p=0.5; average win will be 0.5; lose amount 2, p=0.7; average loss will be 1.4; giving an average net loss for deck A of 0.9

Note that the trial outcomes are computed randomly on each trial using a Mersenne Twister random number generator so with small runs the actual values experienced by participants may depart from the programmed values. The data file contains the actual values so a check can be made.

basename.txt_Inst and basename.txt_EndMsg should be short text files. Enter paragraphs on separate lines.

ctrl-Q quits

start the program from a console in the same directory as igt.exe and its three required files with:

igt basename.txt 99

where 99 is the participant ID

Bechara A, Damasio AR, Damasio H, Anderson SW. Insensitivity to future consequences following damage to human prefrontal cortex. Cognition 1994;50:7–15.