import sensor, image, time
from image import SEARCH_EX, SEARCH_DS
from pyb import UART
def distinguish_obj()
	while(True):
		sum_zfx=0
		sum_yx=0
		sum_sjx=0
		clock.tick()
		img = sensor.snapshot(1.8)
		for x in templates :
			img = sensor.snapshot(1.8)
			img = img.to_grayscale()
			for t in x:
				flag=0
				clock.tick()
				img = sensor.snapshot(1.8)
				img = img.to_grayscale()
				template = image.Image(t)
				if x == zfx_tempaltes:
					r = img.find_template(template, 0.80, step=4, search=SEARCH_EX)
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
					r = img.find_template(template, 0.80, step=4, search=SEARCH_EX)
					if r:
						print(t)
						sjx = r
						sum_sjx=sum_sjx+1
		if (sum_zfx>sum_yx and sum_zfx>sum_sjx) :
			r=zfx
			t="zfx"
		elif (sum_yx>sum_zfx and sum_yx>sum_sjx) :
			r=yx
			t="yx"
		else:
			r=sjx
			t="sjx"
		if (sum_zfx!=0 or sum_yx!=0 or sum_sjx!=0):
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
			sensor.set_auto_gain(False)
			sensor.set_auto_whitebal(False)
			sensor.set_vflip(False)
			sensor.set_hmirror(False)
			img = sensor.snapshot(1.8)
			i=3
			while(i>0):
				blobs = img.find_blobs(blue,roi=r,pixel_threshold=200,area_threshold=200)
				if blobs:
					max_blob = find_max(blobs)
					img.draw_rectangle(r)
					img.draw_cross(max_blob.cx(), max_blob.cy())
					img.draw_circle(x+int(w/2),y+int(h/2),int(w/2))
					sum_blue=sum_blue+1
				blobs = img.find_blobs(red,roi=r,pixel_threshold=200,area_threshold=200)
				if blobs:
					max_blob = find_max(blobs)
					img.draw_rectangle(r)
					img.draw_cross(center_x, center_y)
					img.draw_circle(x+int(w/2),y+int(h/2),int(h/2))
					sum_red=sum_red+1
				blobs = img.find_blobs(green,roi=r,pixel_threshold=200,area_threshold=200)
				if blobs:
					max_blob = find_max(blobs)
					img.draw_rectangle(r)
					img.draw_cross(center_x, center_y)
					sum_green=sum_green+1
				i=i-1
			if (sum_red>sum_green and sum_red>sum_blue) :
				flag="red"
			elif (sum_green>sum_red and sum_green>sum_blue) :
				flag="green"
			else:
				flag="blue"
		if(r==0 or flag == 0):
			print("没找到")
		else:
			Lm = int(r[2]/2)
			K = 25
			length = K/Lm
			print("length:",length)
			print("color:",flag,"object:",t,"range:",r,"red:",sum_red,
					"green:",sum_green,"blue:",sum_blue,"zfx_model:",sum_zfx,"yx_model:",
					sum_yx,"sjx_model:",sum_sjx)
			if (r[2]%2)==1:
				c_x=int(r[0]+(r[2]+1)/2)
			else:
				c_x=int(r[0]+(r[2]/2))
			if (r[3]%2)==1:
				c_y=int(r[1]+(r[3]+1)/2)
			else:
				c_y=int(r[1]+(r[3]/2))
			uart.writechar(c_x)
			uart.writechar(c_y)