# Fish-Tracker

## Building

Open a terminal inside Scripts folder and than run your systems respective script, simple as that.

For now, we only suport [Windows](Scripts/Win-GenProjects.bat) and [Linux](Scripts/Lin-GenProjects) systems.

For Windows users, it is set de generate a solution for VS 2019, if you have another version of Visual Studio or want to build
for another compiler, just change the 3rd line of [Win-GenProjects.bat](Scripts/Win-GenProjects.bat).

Ex:

from:
```bat
$ call cmake -G "Visual Studio 16 2019" .
```
to:
```bat
$ call cmake -G "Visual Studio 15 2017" .
```

## Observations

Make sure you have set the correct path to both OpenCV and wxWidgets directories (check line 13 and 14 of the [root CMakeLists.txt](CMakeLists.txt)).
