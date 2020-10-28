import sensor, image, time
from image import SEARCH_EX, SEARCH_DS
from pyb import UART
from color_model import cal
from error_color import track_color


uart=UART(3,19200)


while True:
    key=uart.readchar()
    if key==1:
        print(1)
    elif key==2:
        print(2)
    elif key==3:
        print(3)
    elif key==4:
        print(4)
