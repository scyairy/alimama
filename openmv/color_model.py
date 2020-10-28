# 多颜色多模板匹配示例
#
# 这个例子显示了使用OpenMV的多色跟踪。

import sensor, image, time
from image import SEARCH_EX, SEARCH_DS
from pyb import UART
from error_color import color_track

# 颜色跟踪阈值(L Min, L Max, A Min, A Max, B Min, B Max)
# 下面的阈值跟踪一般红色/绿色的东西。你不妨调整他们...
#blue=[(34, 40, 10, 18, -60, -40)]
#green=[(36, 58, -39, -24, -3, 19)]#green
#red=[(41, 54, 67, 80, 30, 63)] # generic_blue_thresholds,green(36, 58, -39, -24, -3, 19),
                #blue (41, 54, 57, 80, 16, 63)
red  = [(41, 54, 67, 80, 30, 63),(35,55,38,65,10,45),(40,53,62,72,36,47),(38,45,60,70,35,44)]
blue=[(30,40,-10,20,-45,-30),(35,40,13,20,-60,-43),(40,47,-3,6,-36,-26),(30 ,34,12,21,-58,49),
      (38,58,-6,23,-63,-27)]
green = [(30,40,-30,-20,5,15),(25,33,-27,-23,2,13),(20,26,-25,-20,3,12),(20,39,-30,-20,0,17),
         (23,33,-27,-24,5,15),(31,38,-35,-28,8,17),(17,26,-30,-20,7,17),(22,29,-28,-25,6,12),
         (65,70,-24,-16,11,20),(47,50,-50,-45,35,40)]
yellow = [(63,70,-3,5,59,68),(71,77,-6,9,35,56)]
black = [(1,8,-10,10,-10,10),(3,8,-4,6,8,12)]
basketball = [(14,26,6,24,3,20)]
volleyball = [(66,70,-5,3,56,70),(30,36,0,9,28,45),(75,87,0,17,30,50)]
soccerball = [(37,45,-23,-1,-33,-28),(27,33,-9,1,-28,20),(42,48,-11,-8,-26,-25)]
white = [72,79,-1,3,-8,1]
#不要超过16个颜色阈值
uart = UART(3,19200)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.set_vflip(False)
sensor.set_hmirror(False)
clock = time.clock()

# 只有比“pixel_threshold”多的像素和多于“area_threshold”的区域才被
# 下面的“find_blobs”返回。 如果更改相机分辨率，
# 请更改“pixels_threshold”和“area_threshold”。 “merge = True”合并图像中所有重叠的色块。
change=[0,0,0,0]


def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob
#balls = ["basketball3.pgm","football1.pgm","volleyball2.pgm"]

zfx_tempaltes = ["zfx1.pgm","zfx2.pgm","zfx3.pgm","zfx4.pgm","zfx5.pgm",
                 "zfx6.pgm","zfx7.pgm","zfx8.pgm","zfx9.com"]
yx_tempaltes = ["yx1.pgm","yx2.pgm","yx3.pgm","yx4.pgm","yx5.pgm"]
sjx_tempaltes = ["sjx1.pgm","sjx2.pgm","sjx3.pgm"]
templates = [zfx_tempaltes,yx_tempaltes,sjx_tempaltes] #保存多个模板

def cal():
    flag=0
    zfx=0
    yx=0
    sjx=0
    r=[0,0,0,0]
    key = 0
    G=0
    while(True):
        key=uart.readchar()
        if key==1:
            break
        sum_zfx=0
        sum_yx=0
        sum_sjx=0
        dis=0
        clock.tick()
        img = sensor.snapshot(1.8)
        #img1 = img.binary(blue)

        for x in templates :
            img = sensor.snapshot(1.8)
            img = img.to_grayscale()
            flag = 0
            for t in x:
                clock.tick()
                img = sensor.snapshot(1.8)
                img = img.to_grayscale()

                template = image.Image(t)
                #ball = image.Image(t)
                if x == zfx_tempaltes:
                    r = img.find_template(template, 0.80, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
                    if r:
                        print(t)
                        zfx = r
                        sum_zfx=sum_zfx+1
                elif x == yx_tempaltes:
                    for c in img.find_circles(threshold = 3500, x_margin = 10, y_margin = 10, r_margin = 10,r_min = 2, r_max = 100, r_step = 2):
                        img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                        if c.r()>1:
                            x=c.x()-c.r()
                            y=c.y()-c.r()
                            w=c.r()*2
                            h=c.r()*2
                            r=[x,y,w,h]
                            yx = r
                            sum_yx=20
                elif x == sjx_tempaltes:
                    r = img.find_template(template, 0.80, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
                    if r:
                        print(t)
                        sjx = r
                        sum_sjx=sum_sjx+1
        if (sum_zfx>sum_yx and sum_zfx>sum_sjx) :
            r=zfx
            t=8#"zfx"
        elif (sum_yx>sum_zfx and sum_yx>sum_sjx) :
            r=yx
            t=9#"yx"
        else:
            r=sjx
            t=10#"sjx"
        if (sum_zfx!=0 or sum_yx!=0 or sum_sjx!=0):

            #change[0]=r[0]+0
            #change[1]=r[1]+0
            #change[2]=r[2]-0
            #change[3]=r[3]-0
            sum_red=0
            sum_green=0
            sum_blue=0
            x=r[0]
            y=r[1]
            w=r[2]
            h=r[3]
            center_x=r[0]+int(r[2]/2)
            center_y=r[1]+int(r[3]/2)
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

            i=3
            while(i>0):
                blobs = img.find_blobs(blue,roi=r,pixel_threshold=200,area_threshold=200)
                if blobs:

                    max_blob = find_max(blobs)
                    img.draw_rectangle(r) # rect
                    #img.draw_cross(center_x, center_y) # cx, cy
                    img.draw_cross(max_blob.cx(), max_blob.cy())
                    #img.draw_line(x+int(w/2),y,x,y+h)
                    #img.draw_line(x,y+h,x+w,y+h)
                    #img.draw_line(x+w,y+h,x+int(w/2),y)#三角形

                    img.draw_circle(x+int(w/2),y+int(h/2),int(w/2))
                    sum_blue=sum_blue+1

                blobs = img.find_blobs(red,roi=r,pixel_threshold=200,area_threshold=200)
                if blobs:

                    max_blob = find_max(blobs)
                    img.draw_rectangle(r) # rect
                    img.draw_cross(center_x, center_y) # cx, cy
                    img.draw_circle(x+int(w/2),y+int(h/2),int(h/2))
                    sum_red=sum_red+1



                blobs = img.find_blobs(green,roi=r,pixel_threshold=200,area_threshold=200)
                if blobs:

                    max_blob = find_max(blobs)
                    img.draw_rectangle(r) # rect
                    img.draw_cross(center_x, center_y) # cx, cy
                    sum_green=sum_green+1
                i=i-1

            if (sum_red>sum_green and sum_red>sum_blue) :
                flag=5#"red"
            elif (sum_green>sum_red and sum_green>sum_blue) :
                flag=6#"green"
            elif (sum_blue>sum_red and sum_blue>sum_green):
                flag=7#"blue"
            else :
                flag = 0

        if(r==0 or flag == 0):
            print("没找到")
        else:
            Lm = int(r[2]/2)
            K = 25
            G=1
            length = K/Lm
            #edge =
            print("length:",length)
            print("color:",flag,"object:",t,"range:",r,"red:",sum_red,
                    "green:",sum_green,"blue:",sum_blue,"zfx_model:",sum_zfx,"yx_model:",
                    sum_yx,"sjx_model:",sum_sjx)
            uart.writechar(0x55)
            uart.writechar(0x53)
            uart.writechar(flag)
            uart.writechar(t)
            uart.writechar(Lm)
            uart.writechar(K)
            uart.writechar(G)
            uart.writechar(1)
            G=0
            break
                        #如果为红色, blob.code()==1; 如果为绿色, blob.code==2.
                        #如果为数字0, t=="0.pgm"; 如果为数字1, t=="1.pgm".


            #print(clock.fps())


