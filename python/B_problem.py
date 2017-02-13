from CA import *
# import cv2
lenth = [50,50,100,100,100,100,50,50]
toll_booth = CA_street(8, 4, 100, lenth)
toll_booth.get_in_equal()
toll_booth.display()
while True :
    if cv2.waitKey() == 27 :
        break
    toll_booth.sync()
    toll_booth.get_in_equal()
    toll_booth.display()
    