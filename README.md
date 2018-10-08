# Maze_Solver_bot
This is the code for an Arduino UNO and LDR sensor based **Line follower/ Maze solver bot**.

Please note that this is a hardware dependent code and that you may need to modify this code according to the hardware you are using.

The hardware used for this project is as follows:
* **Arduino UNO:** Microcontroller acting as the brain of the bot.
* **LDR sensors:** To sense the line.
* **IC, L293D:** Used as the 'Motor driver board'.

All the turns in the maze are assumed to be at 90 degrees to each other.

# Files Description

* **README.md:** is this README file.

* **LICENSE:** contains the terms of the MIT license under which this code is being published.

* **LineFollower.c:** is a well tested code for a line follower bot. Note that a line follower bot does not have a memory action, i.e. a line follower bot does not store the optimal paths at a *T like point of the maze* in its memory during the trial run. Please note that some heavy bobbling issues may be encountered while taking a right turn if the gap between the sensors is incorrectly set. This is because **default-left algorithm** is being used by the bot.

* **MazeSolver.c:** is the code for a maze-solver bot. These bots have a memory action. However, we were unable to thoroughly test the memory action because of the lack of appropriate hardware. Basically, for an effective memory action it is necessary to nullify the bobbling of the bot. This requires some additional hardware such as **servo motors** and **PID (Proportional-integral-derivative) controllers** that was not available to us. Issues with the right turn (as described above), will occur if the sensor gap is not proper.
