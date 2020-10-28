import sensor, image, time

from pid import PID
from pyb import Servo
from pyb import UART



red_threshold  = [(14,26,6,24,3,20)]

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
def ball_track():
    num=0
    F=0
    while(True):

        key=uart.readchar()
        if key==2:
           break
        clock.tick() # Track elapsed milliseconds between snapshots().
        img = sensor.snapshot() # Take a picture and return the image.

        blobs = img.find_blobs(red_threshold,merge=True,margin=1,x_stride=2,y_stride=1)
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
            if(max_blob.cx()>=78 and max_blob.cx()<=82 and  max_blob.cy()<=62 and max_blob.cy()>=58):
                num= num+1
            if (num>=2000):
                G=0

                break;
