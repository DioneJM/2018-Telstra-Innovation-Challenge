import numpy as np
# import math
from numpy import sqrt
# import scipy.stats as st
import sys
import os
import json

import matplotlib.pyplot as plt

# "accel_x":[0.763824897154395,0.5866469526798834,0.8286135232151721],
# "accel_y":[0.615104364012065,0.284677924127019,0.631172176317478],
# "accel_z":[0.28731752062261395,0.3932860143470731,0.9103526978528023]}

class testdata:
    def __init__(self):
        # self.vm=[np.random.random() for i in range(n) ]
        self.x_axis = [0.763824897154395,0.5866469526798834,0.8286135232151721]
        self.y_axis = [0.615104364012065,0.284677924127019,0.631172176317478]
        self.z_axis = [0.28731752062261395,0.3932860143470731,0.9103526978528023]

        self.n=len(self.x_axis)

class sampledata:
    def __init__(self,n):
        # self.vm=[np.random.random() for i in range(n) ]
        self.x_axis = [np.random.random() for i in range(n)]
        self.y_axis = [np.random.random() for i in range(n)]
        self.z_axis = [np.random.random() for i in range(n)]

        self.n=n


class features:

    def __init__(self, arr_x, arr_y, arr_z,freq):


        self.x_axis = arr_x #acceleration on x-axis
        self.y_axis = arr_y #acceleration on y-axis
        self.z_axis = arr_z #acceleration on z-axis
        self.vm = self.vm_calculation(self.x_axis,self.y_axis,self.z_axis)  # acceleration vector magnitude
        self.ths = 1.8 #threshold for detecting movement (unit is g)
        self.imp_dex = 2 #impact window index
        self.post_dex = 3 #post-imp window index
        self.freq_rate = freq #sampling frequency, please change based on the device's sampling rate
        self.features_array=[]
    #
    # def record_data_to_json(self):
    #     self.x_axis
    #     self.y_axis
    #     self.z_axis
    #     self.vm_axis
    #     self.features_array


    def vm_calculation(self,x_axis, y_axis, z_axis):
        x2 = [x ** 2 for x in x_axis]
        y2 = [x ** 2 for x in y_axis]
        z2 = [x ** 2 for x in z_axis]
        sum = [x + y + z for x, y, z in zip(x2, y2, z2)]
        vm = np.sqrt(sum)
        return vm

    def features_calculation(self):
        
        pre_win_index = self.freq_rate
        imp_index = self.imp_dex * self.freq_rate
        post_win_index = self.post_dex * self.freq_rate

        #calculating pre-impact features
        pre_mean,_,_,pre_velo, pre_sma, pre_ema = self.features_list(self.vm[:pre_win_index],self.x_axis[:pre_win_index],self.y_axis[:pre_win_index], self.z_axis[:pre_win_index], self.freq_rate)

        #calculating impact features
        imp_mean, imp_min_val, imp_rms, imp_velo, imp_sma, imp_ema = self.features_list(self.vm[pre_win_index:imp_index],
        self.x_axis[pre_win_index:imp_index],self.y_axis[pre_win_index:imp_index], self.z_axis[pre_win_index:imp_index], self.freq_rate)

        #calculating post-impact features
        _, post_min_val, _, _, _, post_ema = self.features_list(self.vm[imp_index:post_win_index],
        self.x_axis[imp_index:post_win_index],self.y_axis[imp_index:post_win_index], self.z_axis[imp_index:post_win_index], self.freq_rate)

        instance = [pre_mean, pre_sma, pre_velo, pre_ema,
        imp_mean, imp_min_val, imp_rms, imp_velo, imp_sma, imp_ema,
        post_min_val, post_ema]


        update_json_log("data.json","data.json",self.x_axis,self.y_axis,self.z_axis,self.vm.tolist(),instance)
        plt.plot(self.vm.tolist())
        plt.show()

        return instance


    def features_list(self,vm, x_ax, y_ax, z_ax, freq_rate):
        new_array = np.array(vm)
        mean = round(new_array.mean(),2)
        power_by_two = np.array(vm)**2
        means_array = power_by_two.mean()
        min_val = round(min(vm),2)
        rms = round(sqrt(means_array),2)
        raw_velo = self.integrate(vm, freq_rate)
        velo = round(raw_velo,2)
        raw_sma = self.smafeat(x_ax,y_ax,z_ax,freq_rate)
        sma = round(raw_sma,2)
        raw_ema = self.ema_calc(vm)
        ema = round(raw_ema,2)

        return mean, min_val, rms, velo, sma, ema

    def ema_calc(self,arrdat):
        CONST_ALPHA =  0.01 #float(2)/(len(arrdat)+1) # this is calculated by : 2/N+1 where N is total number of the data
        sem = []
        for i in range(0,len(arrdat)):

            if i == 0:
                sval = 0

            else:
                sval = (CONST_ALPHA * arrdat[i]) + (1-CONST_ALPHA) * sem[i-1]

            sem.append(sval)


        emval = sem[len(sem)-1]

        return emval

    def integrate(self,arrdat, freq_rate):

        Tperiod = 1/float(freq_rate) #calculate period

        velocity = 0 #initial value of velocity

        for n in range(0,len(arrdat)):

            velocity = velocity + arrdat[n] * Tperiod

        return velocity

    def smafeat(self,datXa,datYa,datZa, freq_rate):

        dataXsq1 = sqrt(np.array(datXa) ** 2)
        dataYsq1 = sqrt(np.array(datYa) ** 2)
        dataZsq1 = sqrt(np.array(datZa) ** 2)


        dataXc1 = self.integrate(dataXsq1, freq_rate)
        dataYc1 = self.integrate(dataYsq1, freq_rate)
        dataZc1 = self.integrate(dataZsq1, freq_rate)
        sma = (dataXc1 + dataYc1 + dataZc1) / float(len(dataXsq1))

        return sma


def feature_calculation_test():
    sample_data=testdata()
    test_features = features(sample_data.x_axis, sample_data.y_axis, sample_data.z_axis, freq=int(sample_data.n / 3))
    return test_features.features_calculation()

def feature_calculation_sample_request():
    sample_data = sampledata(9)

    test_features = features(sample_data.x_axis, sample_data.y_axis, sample_data.z_axis,freq=int(sample_data.n / 3))

    return test_features.features_calculation()

def feature_calculation_request(accx,accy,accz):
    test_features = features(accx, accy, accz,freq=len(accx)/ 3)
    return test_features.features_calculation()


def sample_call_from_js():
    array = feature_calculation_sample_request()
    for element in array:
        print(element)

def call_from_js():

    print("start")
    accx=np.fromstring(sys.argv[1],dtype=float,sep=',').tolist()
    accy=np.fromstring(sys.argv[2],dtype=float,sep=',').tolist()
    accz=np.fromstring(sys.argv[3],dtype=float,sep=',').tolist()
    features = feature_calculation_request(accx, accy, accz)
    for feature in features:
        print(feature)
    print("end")
##########################
def initialize_json_log(pre_file_name,accx,accy,accz,vm,features):
    with open(pre_file_name, 'w') as outfile:
        data={}
        data['testset']=[]
        data['testset'].append({'TestID':0,'accel_x':accx,'accel_y':accy,'accel_z':accz,'vm':vm,'features':features})
        json.dump(data,outfile, indent=2, sort_keys=True)

def update_json_log(pre_file_name,updated_file_name,accx,accy,accz,vm,features):
    if(os.path.isfile(pre_file_name)):
        # print("pre_file is here~!!")
        if(os.stat(pre_file_name).st_size == 0):
            initialize_json_log(pre_file_name,accx,accy,accz,vm,features)
        else:
            with open(pre_file_name) as pre_file:
                data=json.load(pre_file)
                with open(updated_file_name, 'w') as updated_file:
                    maxid= data['testset'][0]['TestID']
                    max_index=0
                    for i in range(0,len(data['testset'])):
                        if (data['testset'][i]['TestID'] >maxid):
                            maxid=data['testset'][i]['TestID']
                            max_index=i
                    last_TestID=int(data['testset'][max_index]['TestID'])
                    # print(last_TestID)
                    data['testset'].append({'TestID':str(last_TestID+1),'accel_x':accx,'accel_y':accy,'accel_z':accz,'vm':vm,'features':features})
                    updated_file.seek(0,2)
                    json.dump(data,updated_file, indent=2, sort_keys=True)
    # else:
        # print("error~pre_file is not here")



###################
##output for node.js file -> iot-hub.js
call_from_js()
# print(feature_calculation_test())
