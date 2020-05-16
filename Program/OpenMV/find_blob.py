# Untitled - By: Pulag - Fri Apr 17 2020

import sensor, image, time , math

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_whitebal(False,(-0.5,-1.2,-0.1))

sensor.skip_frames(time = 2000)

red = (60, 84, 33, 62, 7, 53) #(36, 77, 30, 55, 5, 54)
blue = (100, 55, -39, 20, -48, -6)
green = (85, 95, -94, 5, 38, 0)

light_filter = (80, 100, -120, 127, -120, 127)


clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()#.binary([light_filter], invert=False, zero=True)


    red_blobs = img.find_blobs([red],merge=True)
    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)
    all_line = img.find_lines(threshold = 2100, theta_margin = 10, rho_margin = 30)


    for a in red_blobs:
        if(a.rect()[2] * a.rect()[3] < 100):
            continue
        img.draw_rectangle(a.rect(),color = (255,0,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))
            line_y_mid = (l.line()[3] - l.line()[1])
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))

            if(apple203 < 15):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())
            else:
                print("e: ")
                print(A * x_mid)
                print(y_mid)
                print(B)
                print(apple203)
                print(l.line())
                print(a.rect())

    for a in blue_blobs:
        img.draw_rectangle(a.rect(),color = (0,0,255))


    for a in green_blobs:
        img.draw_rectangle(a.rect(),color = (0,255,0))
        #print(a.rotation())

