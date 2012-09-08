Artibot - a portrait painting robot
Geekcon 2012 project

Artibot is a simple robot that paints a portrait.
The full system consists of an iPhone app that takes an image, scales it down to 100x100 pixels,
and convert the image to a grayscale one.
It then uploads the image to a Google App Engine server component that can queue multiple images and serve them to the robot.

The robot arm is made of two servo motors - a shoulder and elbow motors that spins the arm parts over the srawing surface and a third servo that is used to move a pen up and down.

The robot arm is controlled by an Arduino with an Ethernet shield. The Arduino talks to the server to get the next image to draw.

The actual drawing is done by rotating the shoulder servo by one degree step, and for each such step rotating the elbow servo to reach different parts of the page. The third servo can be moved up and down to control wheter or not a dot, or small line is drawn at a certain position on the page.

The current code actually can place up the pen on the paper up to 3 times at the same spot to affect the color intensity.

