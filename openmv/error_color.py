import sensor, image, time

from pid import PID
from pyb import Servo
from pyb import UART



all_threshold  = [(30,40,-30,-20,5,15),(25,33,-27,-23,2,13),(20,26,-25,-20,3,12),
(20,39,-30,-20,0,17),(23,33,-27,-24,5,15),(31,38,-35,-28,8,17),(17,26,-30,-20,7,17),
(22,29,-28,-25,6,12),(41, 54, 67, 80, 30, 63),(35,55,38,65,10,45),(40,53,62,72,36,47),
(38,45,60,70,35,44), (30,40,-10,20,-45,-30),(35,40,13,20,-60,-43),(40,47,-3,6,-36,-26),
(30 ,34,12,21,-58,49),(38,58,-6,23,-63,-27),(65,70,-24,-16,11,20),(47,50,-50,-45,35,40),
]

sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.RGB565) # use RGB565.
sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
sensor.skip_frames(10) # Let new settings take affect.
sensor.set_auto_whitebal(False) # turn this off.
clock = time.clock() # Tracks FPS.

def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob

uart=UART(3,19200)
def color_track():
    num=0
    F=0
    while(True):

        key=uart.readchar()
        if key==2:
           break
        clock.tick() # Track elapsed milliseconds between snapshots().
        img = sensor.snapshot() # Take a picture and return the image.

        blobs = img.find_blobs(all_threshold,merge=True,margin=1,x_stride=2,y_stride=1)
        if blobs:
            max_blob = find_max(blobs)


            img.draw_rectangle(max_blob.rect()) # rect
            img.draw_cross(max_blob.cx(), max_blob.cy()) # cx, cy

            F=1
            G=1
            uart.writechar(0x55)
            uart.writechar(0x52)
            uart.writechar(max_blob.cx())
            uart.writechar(max_blob.cy())
            uart.writechar(F)
            uart.writechar(G)
            uart.writechar(2)
            uart.writechar(0)
            print(max_blob.cx(),max_blob.cy())
            if(max_blob.cx()>=68 and max_blob.cx()<=70 and  max_blob.cy()<=69 and max_blob.cy()>=67):
                num= num+1
            if (num>=2000):
                G=0
                break
