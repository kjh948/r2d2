# r2d2
r2d2 with ros


1. install

rosdep install --from-paths src --ignore-src -r -y

sudo apt-get install ros-noetic-web-video-server
rosrun web_video_server web_video_server


roslaunch pi_face_tracker face_tracker_usb_cam.launch

https://stackoverflow.com/questions/19579528/pygame-installation-sdl-config-command-not-found

sudo apt-get install libsdl2-dev
pip install pygame

https://learnopencv.com/install-dlib-on-ubuntu/
https://github.com/RobotWebTools/mjpeg_server

2. how to run

2.1 run rosserial
rosrun rosserial_python serial_node.py _port:=/dev/ttyACM0

2.2 run keyop
rosrun teleop_twist_keyboard teleop_twist_keyboard.py

