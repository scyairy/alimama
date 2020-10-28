import sensor, image, time
from image import SEARCH_EX, SEARCH_DS
from pyb import UART
import hanshu

class color_track:
    red  = [(41, 54, 67, 80, 30, 63),(35,55,38,65,10,45),(40,53,62,72,36,47),(38,45,60,70,35,44)]
    blue=[(30,40,-10,20,-45,-30),(35,40,13,20,-60,-43),(40,47,-3,6,-36,-26),(30 ,34,12,21,-58,49),
             (38,58,-6,23,-63,-27)]
    green = [(30,40,-30,-20,5,15),(25,33,-27,-23,2,13),(20,26,-25,-20,3,12),(20,39,-30,-20,0,17),
         (23,33,-27,-24,5,15),(31,38,-35,-28,8,17),(17,26,-30,-20,7,17)]
    yellow = [(63,70,-3,5,59,68),(71,77,-6,9,35,56)]
    black = [(1,8,-10,10,-10,10),(3,8,-4,6,8,12)]
    white = [72,79,-1,3,-8,1]
    usart=UART(3,115200)
    def _init_(self)


        self.flag=0
        self.zfx=0
        self.yx=0
        self.sjx=0
        self.r=[0,0,0,0]
    def color()

        self.sum_zfx=0
        self.sum_yx=0
        self.sum_sjx=0

        clock.tick()
        img = sensor.snapshot(1.8)
        #img1 = img.binary(blue)

        for x in templates :
            img = sensor.snapshot(1.8)
            img = img.to_grayscale()
            for t in x:
                self.flag=0
                clock.tick()
                img = sensor.snapshot(1.8)
                img = img.to_grayscale()

                template = image.Image(t)
                #ball = image.Image(t)
                if x == zfx_tempaltes:
                    r = img.find_template(template, 0.80, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
                    if r:
                        print(t)
                        self.zfx = r
                        self.sum_zfx=sum_zfx+1
                elif x == yx_tempaltes:
                    for c in img.find_circles(threshold = 3500, x_margin = 10, y_margin = 10, r_margin = 10,r_min = 2, r_max = 100, r_step = 2):
                        img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                        if c.r()>1:
                            self.x=c.x()-c.r()
                            self.y=c.y()-c.r()
                            self.w=c.r()*2
                            self.h=c.r()*2
                            self.r=[x,y,w,h]
                            self.yx = r
                            self.sum_yx=20
                elif x == sjx_tempaltes:
                    r = img.find_template(template, 0.80, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
                    if r:
                        print(t)
                        self.sjx = r
                        self.sum_sjx=sum_sjx+1
        if (self.sum_zfx>self.sum_yx and self.sum_zfx>self.sum_sjx) :
            self.r=zfx
            self.t="zfx"
        elif (self.sum_yx>self.sum_zfx and self.sum_yx>self.sum_sjx) :
            self.r=yx
            self.t="yx"
        else:
            self.r=sjx
            self.t="sjx"
        if (self.sum_zfx!=0 or self.sum_yx!=0 or self.sum_sjx!=0):

            #change[0]=r[0]+0
            #change[1]=r[1]+0
            #change[2]=r[2]-0
            #change[3]=r[3]-0
            self.sum_red=0
            self.sum_green=0
            self.sum_blue=0
            self.x=r[0]
            self.y=r[1]
            self.w=r[2]
            self.h=r[3]
            self.center_x=r[0]+int(r[2]/2)
            self.center_y=r[1]+int(r[3]/2)
            sensor.reset()
            sensor.set_pixformat(sensor.RGB565)
            sensor.set_framesize(sensor.QQVGA)
            sensor.skip_frames(time = 300)
            sensor.set_auto_gain(False) # must be turned off for color tracking
            sensor.set_auto_whitebal(False) # must be turned off for color tracking
            sensor.set_vflip(False)
            sensor.set_hmirror(False)
            img = sensor.snapshot(1.8)
            #r=list(r)

            self.i=3
            while(self.i>0):
                blobs = img.find_blobs(blue,roi=r,pixel_threshold=200,area_threshold=200)
                if blobs:

                    max_blob = hanshu.find_max(blobs)
                    img.draw_rectangle(r) # rect
                    #img.draw_cross(center_x, center_y) # cx, cy
                    img.draw_cross(max_blob.cx(), max_blob.cy())
                    #img.draw_line(x+int(w/2),y,x,y+h)
                    #img.draw_line(x,y+h,x+w,y+h)
                    #img.draw_line(x+w,y+h,x+int(w/2),y)#三角形

                    img.draw_circle(x+int(w/2),y+int(h/2),int(w/2))
                    self.sum_blue=sum_blue+1

                blobs = img.find_blobs(red,roi=r,pixel_threshold=200,area_threshold=200)
                if blobs:

                    max_blob = hanshu.find_max(blobs)
                    img.draw_rectangle(r) # rect
                    img.draw_cross(center_x, center_y) # cx, cy
                    img.draw_circle(x+int(w/2),y+int(h/2),int(h/2))
                    self.sum_red=sum_red+1



                blobs = img.find_blobs(green,roi=r,pixel_threshold=200,area_threshold=200)
                if blobs:

                    max_blob = hanshu.find_max(blobs)
                    img.draw_rectangle(r) # rect
                    img.draw_cross(center_x, center_y) # cx, cy
                    self.sum_green=sum_green+1
                self.i=self.i-1

            if (self.sum_red>self.sum_green and self.sum_red>self.sum_blue) :
                self.flag="red"
            elif (self.sum_green>self.sum_red and self.sum_green>self.sum_blue) :
                self.flag="green"
            else:
                self.flag="blue"

        if(r==0 or self.flag == 0):
            print("没找到")
        else:
            self.Lm = int(r[2]/2)
            self.K = 25
            self.length = K/Lm
            #edge =
            print("length:",length)
            print("color:",flag,"object:",t,"range:",r,"red:",sum_red,
                    "green:",sum_green,"blue:",sum_blue,"zfx_model:",sum_zfx,"yx_model:",
                    sum_yx,"sjx_model:",sum_sjx)
            if (r[2]%2)==1:
                self.c_x=int(r[0]+(r[2]+1)/2)
            else:
                self.c_x=int(r[0]+(r[2]/2))

            if (r[3]%2)==1:
                self.c_y=int(r[1]+(r[3]+1)/2)
            else:
                self.c_y=int(r[1]+(r[3]/2))
            uart.writechar(self.c_x)
            uart.writechar(self.c_y)


                        #如果为红色, blob.code()==1; 如果为绿色, blob.code==2.
                        #如果为数字0, t=="0.pgm"; 如果为数字1, t=="1.pgm".


            #print(clock.fps())
