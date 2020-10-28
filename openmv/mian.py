import sensor, image, time
from image import SEARCH_EX, SEARCH_DS
from pyb import UART
from color_model import cal
from error_color import color_track
from balls import ball_track
#import model_
# 颜色跟踪阈值(L Min, L Max, A Min, A Max, B Min, B Max)
# 下面的阈值跟踪一般红色/绿色的东西。你不妨调整他们...
#blue=[(34, 40, 10, 18, -60, -40)]
#green=[(36, 58, -39, -24, -3, 19)]#green
#red=[(41, 54, 67, 80, 30, 63)] # generic_blue_thresholds,green(36, 58, -39, -24, -3, 19),
                #blue (41, 54, 57, 80, 16, 63)

#sensor.reset()
#sensor.set_pixformat(sensor.RGB565)
#sensor.set_framesize(sensor.QQVGA)
#sensor.skip_frames(time = 2000)
#sensor.set_auto_gain(False) # must be turned off for color tracking
#sensor.set_auto_whitebal(False) # must be turned off for color tracking
#sensor.set_vflip(False)
#sensor.set_hmirror(False)
#clock = time.clock()

uart=UART(3,19200)

#track_color()
color_track()
while True:
    key=uart.readchar()
    if key==1:
        cal()
    elif key==2:
        color_track()
    elif key==3:
        print(3)
    elif key==4:
        print(4)


