import cv2
import numpy as np
import random 
# import matplotlib.pyplot as plt
_LEN = 1600
_WIDTH = 300
_MAX_V = 10
CA_dt = np.dtype([('status',np.bool),('v',np.int8),('change',np.int8),
                  ('acc',np.int8), ('is_auto', np.bool), ('time',np.int16)])
class CA_street :

    _p = 0.5
    _min_delta_time = 3 * np.ones(100)
    # 构造函数，交通元胞自动机类
    def __init__(self, B, L, length, toll_booth_len, model_type = True, ) :
        self.B = B
        self.L = L
        self.length = length
        self.tb_length = toll_booth_len
        self.type = model_type
        self.init_map()
        self.toll_booth = np.zeros([B, length], CA_dt)
        self.toll_booth[0,0]['status'] = True
        self.flux = 0
        self.boom = 0
        self.input = 0
        self.out = 0
        self.total_time = 0
        self.change_times = 0
        self.delay_time_car = np.zeros(1000)
        self.delay_time = np.zeros(100)

    # 初始化toll_booth的图像，用于后续画图
    def init_map(self) :
        self.init = 255 * np.ones([_WIDTH, _LEN, 3], np.uint8)
        delta_length = int(_LEN / self.length + 0.5)
        delta_width = int(_WIDTH / self.B + 0.5)
        for i in range(self.length + 1) :
            if i <= min(self.tb_length) :
                cv2.line(self.init, (i * delta_length, 0 ),
                    (i * delta_length, _WIDTH), (0,0,0))
            else :
                cv2.line(self.init, 
                    (i * delta_length, delta_width*int((self.B-self.L)/2)),
                    (i * delta_length, delta_width*int((self.B+self.L)/2)), 
                    (0,0,0))
        for j in range(self.B) :
            cv2.line(self.init, (0, j*delta_width), 
                (self.tb_length[j]*delta_length, j*delta_width), (0,0,0))
        cv2.line(self.init, (0, int((self.B+self.L)/2)*delta_width), 
            (max(self.tb_length)*delta_length, int((self.B+self.L)/2)*delta_width), 
            (0,0,0))

    # 展示toll_booth当前状态
    def display(self) :
        delta_length = int(_LEN / self.length + 0.5)
        delta_width = int(_WIDTH / self.B + 0.5)
        dis_pic = self.init.copy()
        tmp = 0
        for i in range(self.B) :
            for j in range(self.length) :
                if self.toll_booth[i,j]['status'] :
                    cv2.circle(dis_pic, (int((j+0.5)*delta_length), int((i+0.5)*delta_width)), 
                        int(min(delta_length/2-2, delta_width/2-2)), (0,0,255), 3)
                    # tmp += 1
        cv2.imshow('init_pic', dis_pic)

    # 等概率在初始位置产生新车
    def get_in_equal(self) :
        tmp_status = np.zeros(self.B, np.bool)
        n = 0
        for i in range(self.B) :
            # if self.delay_time[i] >= _min_delta_time[i] :j
            if self.delay_time[i] >= 3 :
                tmp_status[i] = True
                n += 1
            else :
                tmp_status[i] = False
            self.delay_time[i] += 1
        while n > 0 :
            tmp = int(random.random()*self.B)
            if tmp_status[tmp] == False :
                continue
            tmp_status[tmp] = False
            n -= 1
            tmp2 = random.random()
            if tmp2 > self._p and self.toll_booth[tmp, 0]['status'] == False :
                self.input += 1
                self.toll_booth[tmp,0]['status'] = 1
                self.toll_booth[tmp,0]['v'] = 1
                self.toll_booth[tmp,0]['acc'] = 0
                self.toll_booth[tmp,0]['change'] = 0
                self.toll_booth[tmp,0]['time'] = 0
                self.toll_booth[tmp,0]['is_auto'] = False

    def check_out(self, x, y) :
        # 在规定范围内
        if (x >= 0 and x < self.B) and y < self.tb_length[x] :
            return 1
        # 驶出规定区域
        if (y >= self.length and self.tb_length[x] == self.length) :
            return -1
        # 规定区域外
        return 0

    def sync(self) :
        for j in range(self.length-1,-1,-1) : # 倒序检索
            for i in range(int(self.B/2+1), self.B) :
                if self.toll_booth[i,j]['status'] == False : continue

                self.toll_booth[i,j]['v'] += self.toll_booth[i,j]['acc']
                tmp_y = j + self.toll_booth[i,j]['v']
                tmp_x = i + self.toll_booth[i,j]['change']
                if self.toll_booth[i,j]['change'] != 0 :
                    self.change_times += 1
                self.toll_booth[i,j]['time'] += 1
                if self.check_out(tmp_x, tmp_y) == 1 :
                    if self.toll_booth[tmp_x,tmp_y]['status'] == True and (self.toll_booth[i,j]['v'] != 0 or self.toll_booth[i,j]['v'] == 0 and self.toll_booth[i,j]['change'] != 0) :
                        self.boom += 1
                    self.toll_booth[tmp_x,tmp_y] = self.toll_booth[i,j]
                    self.toll_booth[tmp_x,tmp_y]['change'] = 0
                    self.toll_booth[tmp_x,tmp_y]['acc'] = 0
                    self.toll_booth[i,j]['status'] = False
                elif self.check_out(tmp_x, tmp_y) == -1 :
                    self.flux += 1
                    self.toll_booth[i,j]['status'] = False
                    self.total_time += self.toll_booth[i,j]['time']
                    self.delay_time_car[self.toll_booth[i,j]['time']] += 1
                else :
                    print('fuck!')
                    self.out += 1
            for i in range(0, int(self.B/2+1)) :
                if self.toll_booth[i,j]['status'] == False : continue

                self.toll_booth[i,j]['v'] += self.toll_booth[i,j]['acc']
                tmp_y = j + self.toll_booth[i,j]['v']
                tmp_x = i + self.toll_booth[i,j]['change']
                if self.toll_booth[i,j]['change'] != 0 :
                    self.change_times += 1
                self.toll_booth[i,j]['time'] += 1
                if self.check_out(tmp_x, tmp_y) == 1 :
                    if self.toll_booth[tmp_x,tmp_y]['status'] == True and (self.toll_booth[i,j]['v'] != 0 or self.toll_booth[i,j]['v'] == 0 and self.toll_booth[i,j]['change'] != 0) :
                        self.boom += 1
                    self.toll_booth[tmp_x,tmp_y] = self.toll_booth[i,j]
                    self.toll_booth[tmp_x,tmp_y]['change'] = 0
                    self.toll_booth[tmp_x,tmp_y]['acc'] = 0
                    self.toll_booth[i,j]['status'] = False
                elif self.check_out(tmp_x, tmp_y) == -1 :
                    self.flux += 1
                    self.toll_booth[i,j]['status'] = False
                    self.total_time += self.toll_booth[i,j]['time']
                    self.delay_time_car[self.toll_booth[i,j]['time']] += 1
                else :
                    print('fuck!')
                    self.out += 1
    
    def get_v_and_pos(self, i, j, dir) :
        if dir == 1 : # 正向查找
            pos = j+1
            v = 100 * np.ones(10)
            while pos < self.tb_length[i] :
                if self.toll_booth[i,pos]['status'] and self.toll_booth[i,pos]['change'] == 0 :
                    v[1] = self.toll_booth[i,pos]['v'] + self.toll_booth[i,pos]['acc']

                if self.check_out(i-1,pos) and self.toll_booth[i-1,pos]['status'] and self.toll_booth[i-1,pos]['change'] == 0 :
                    v[2] = self.toll_booth[i-1,pos]['v'] + self.toll_booth[i-1,pos]['acc']
                if self.check_out(i+1,pos) and self.toll_booth[i+1,pos]['status'] and self.toll_booth[i+1,pos]['change'] == 0 :
                    v[3] = self.toll_booth[i+1,pos]['v'] + self.toll_booth[i+1,pos]['acc']

                v[0] = min(v)
                if v[0] != 100 :
                    return [pos, v[0]]
                pos += 1
            return [pos, v[0]]
        elif dir == 0 :
            pos = j-1
            v = 0
            while pos >= 0 :
                if self.toll_booth[i,pos]['status'] and self.toll_booth[i,pos]['change'] == 0 :
                    v = self.toll_booth[i,pos]['v']
                    return [pos, v]
                pos -= 1
            return [pos, v]

    def get_gap(self, v1, v2 ,is_auto=False) :
        gama = 0.4
        da = 2
        gap = 1 + (v1**2 - v2**2)/(2*da) + 2*gama*v1 + 0.5
        if is_auto :
            gap -= 2 * gama *v1
        return int(gap) + v1 - v2
