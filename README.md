# r2d2
r2d2 with ros


1. install
https://stackoverflow.com/questions/19579528/pygame-installation-sdl-config-command-not-found

sudo apt-get install libsdl2-dev
pip install pygame

2. how to run

2.1 run rosserial
rosrun rosserial_python serial_node.py _port:=/dev/ttyACM0

2.2 run keyop
rosrun teleop_twist_keyboard teleop_twist_keyboard.py

