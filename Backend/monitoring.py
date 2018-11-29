import numpy as np
# import math
from numpy import sqrt
# import scipy.stats as st
import sys
import json
import os
import matplotlib.pyplot as plt

##########################################################################################
## Monitoring accel ########
##########################################################################################
def get_vm_data(json_file,testID):
    with open(json_file) as jsonfile:
        data=json.load(jsonfile)
        testID_count=0

        for testdata in data['testset']:
            if(testID == int(testdata['TestID'])):
                testID_count=testID_count+1
                vm=testdata['vm']
        if(testID_count>1):
            print("multiple testID found  !!!!!")
            return
        elif(testID_count==0):
            print("testID not found !!!!!!!")
            return
        else:
            print("testID found")
            max_vm_index,max_vm = find_max_vm(vm)
            return vm, max_vm_index,max_vm


def monitor_vm(json_file,testID):
    with open(json_file) as jsonfile:
        data=json.load(jsonfile)
        testID_count=0

        for testdata in data['testset']:
            if(testID == int(testdata['TestID'])):
                testID_count=testID_count+1
                vm=testdata['vm']
        if(testID_count>1):
            print("multiple testID found  !!!!!")
        elif(testID_count==0):
            print("testID not found !!!!!!!")
        else:
            print("testID found")
            max_vm_index,max_vm = find_max_vm(vm)

            plt.plot(vm)
            annotation_string = 'peak at '+str(max_vm_index) + "("+str(max_vm)+")"
            plt.annotate(annotation_string, xy=(max_vm_index, max_vm), xytext=(max_vm_index+5, max_vm),
            arrowprops=dict(facecolor='black', shrink=0.05),
            )
            plt.ylabel('vm')
            plt.title("VectorMg of Test ID = "+str(testID))
            plt.show()

def find_max_vm(vm_array):
    max=-9999
    max_index=0
    for i in range(0,len(vm_array)):
        if(vm_array[i]>max):
            max=vm_array[i]
            max_index=i
    return max_index,max
##########################################################################################
def monitor_all_testid(json_file):

    with open(json_file) as jsonfile:
        data=json.load(jsonfile)
        maxid= data['testset'][0]['TestID']
        max_index=0
        for i in range(0,len(data['testset'])):
            if (data['testset'][i]['TestID'] >maxid):
                maxid=data['testset'][i]['TestID']
                max_index=i
        last_TestID=int(data['testset'][max_index]['TestID'])
        for i in range(0,last_TestID+1):
            monitor_vm("data.json",i)
##########################################################################################
def monitor_some_testids_in_one_screen(json_file,id_array):
    f, axarr = plt.subplots(len(id_array), sharex=True)
    i=0

    for idnumber in id_array:
        vm, max_vm_index,max_vm=get_vm_data(json_file,idnumber)
        axarr[i].plot(vm)
        # annotation_string = 'peak at '+str(max_vm_index) + "("+str(max_vm)+")"
        # plt.annotate(annotation_string, xy=(max_vm_index, max_vm), xytext=(max_vm_index+5, max_vm),
        # arrowprops=dict(facecolor='black', shrink=0.05),
        # )

        i=i+1
    plt.show()

testId_array=[2,3]
monitor_some_testids_in_one_screen("data.json",testId_array)
