# my Galaga

A simple galaga clone done entirely by me as an exercise for game deveopment

Done in a single file on purpose, minimal ADT, (except a single struct for easily managing runtime variables)

Depends on two library:

* [raylib](https://github.com/raysan5/raylib) -> for the graphics API
* [jsoncpp](https://github.com/nlohmann/json) -> for saving highScores in a json file

## Features
* The spaceship can move 4-way and shoot at the same time, although slowed down,
* Enemies spawn at the top of the screen and shoot at the player at 360°
* Three pickable spaceship upgrades (only once at the time on the screen):
	* Pacman -> you can move beyond the walls and come out from the other side
	* speed -> your speed increase by one, start at 4 maximum is 15
	* bullets -> increase the number of bullets fired at one, start at 1 maximum is 10
* Game over screen with retry and exit
* High scores saved on a file
* Spaceship stats displayed at the top of the screen
* Different upgrade spawn chache based on type
* Increasing enemies spawn rate based on upgrades collected 