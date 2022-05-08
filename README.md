**Game Overview & Fretures**

Broadly the game is a tile based game where the player moves in a grid like structure and the entire design of the map of the campus has been designed in a grid based fashion. The tiles in the grid are enumerated with different colors and the corresponding map with numbers is loaded into the lazy.map file. While running the game the file is read, map is designed and the game gets ready to be played. Data is taken from the keyboard by polling for an event and corresponding changes are made in the game. The game includes various locations that are present in the IITD campus, like hostels, resedential areas, west campus, east campus, Food outlets, Central library, Lecture Hall Complex, Student Activity Centre, IIT Hospital, Yulu stands. Every institute level building and hostel contains a maze. The player goes opens the randomly generated maze and wins the treasure after completing the maze. The corresponding indicators (hunger, fatigue and money) are accordingly changed. The player can eat food inside Food outlets. Three of them are present inside the game. Rajdhani, Amul and Nescafe. Besides this the player can take a yulu ride and the speed of the player can be increased by using the ride. The player can rest in the residential area as well to regain health. The game gets over when the fatigue gets to full or heath becomes 0. Money will help buy the player food and gain health. The player can sleep in residential areas to reduce fatigue. Solving the entire maze gives the player treasure points. 

**Libraries Used and their Installation Process**

1) SDL2 -> The SDL2 library was installed using homebrew on the OSX operating system
2)SDL2_image ->The SDL2_image library was installed using homebrew on the OSX operating system
3)SDL2_mixer ->The SDL2_image library was installed using homebrew on the OSX operating system
4)SDL2_ttf ->The SDL2_image library was installed using homebrew on the OSX operating system

**Other Online Resources Used**

Lazyfoo for learning SDL https://lazyfoo.net/tutorials/SDL/index.php
Maze Generation Wikipaedia https://en.wikipedia.org/wiki/User:Dllu/Maze
IIT Delhi map https://home.iitd.ac.in/for-faculty-around-campus-campus-map.php
IIT Delhi main building image https://home.iitd.ac.in
Game music
1) https://youtu.be/Q7HjxOAU5Kc
2) https://youtu.be/BxYzjjs6d1s
3) https://youtu.be/PcFIeLVmGz4
4) https://youtu.be/Eh9b7zRUsvE
