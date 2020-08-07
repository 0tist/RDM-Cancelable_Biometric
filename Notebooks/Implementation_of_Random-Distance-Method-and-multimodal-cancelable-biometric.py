#!/usr/bin/env python
# coding: utf-8

# In[2]:


import numpy as np
from LogGabor import LogGabor
import cv2
import matplotlib.pyplot as plt


# In[2]:

#for each user separately
np.random.seed(4)


# For now I'll be working on `cv2.getGaborKernel`, just to experiment on things but in future, maybe I'll shift to `LogGabor` as it offers more flexibility

# In the paper, authors have used 24 different filters(4 scales $\times$ 6 orientations), by scale it meant the sixe of the fiter for example 3$\times$3, 4$\times$4, 28$\times$28, etc. and orientations mean the angle = [pi/4, pi/3, pi/2, pi]. I'll fill in on this as I understand more about the concept.

# The filters are contsructed in terms of two components:
# 1. The radial component, which controls the frequency band to which the filter responds
# 2. The angular component, which controls the orientation of the filter

# In[6]:


ksize = 28
sigma = 8
theta = np.pi / 10
lambd = np.pi / 10
gamma = 0.5
phi = 0


# In[7]:


log_gabor = cv2.getGaborKernel((ksize, ksize), sigma, theta, lambd, gamma, phi, ktype=cv2.CV_32F)


# In[8]:


plt.imshow(log_gabor)


# $ False\ acceptance\ ratio\ = \frac{impostor\ scores\ exceeding\ threshold}{all\ impostor\ scores} = \frac{falsly\ accepted}{all the accepted cases}$  <br>
# $ False\ Rejection\ ratio = \frac{genuine\ scores\ falling\ below\ threshold}{all\ genuine\ scores} = \frac{flasly\ rejected}{all\ rejected\ cases}$

# In[9]:


from pathlib import Path
from fastcore.imports import *
from fastai2.vision.all import *
import os
cwd = os.getcwd()
path = Path(cwd + '/data/DB1_B/')


#image_sample = path.ls()[0]
from PIL import Image
#sample_img = Image.open(image_sample)

all_images = path.ls()[:10]
pairs = list()

for image_i in all_images:
    for image_j in all_images:
        if(image_i != image_j):
            pairs.append([image_i, image_j])
            
    all_images.remove(image_i)


df = pd.DataFrame()

df['img_1'] = [pair[0] for pair in pairs]

df['img_2'] = [pair[1] for pair in pairs]

df['related'] = [1 if pair[0].stem[:3] == pair[1].stem[:3] else 0 for pair in pairs]


#sample_img = sample_img.resize((128,128))
#
#fft = np.fft.fft2(sample_img)
#fft_shift = np.fft.fftshift(fft)
#iff_shift = np.fft.ifftshift(fft_shift)
#ifft = np.fft.ifft2(iff_shift)

#plt.subplot(1,2,1), plt.imshow(sample_img, cmap='gray')
## plt.subplot(1,5,2), plt.imshow(np.log(1 + np.abs(fft)), cmap='gray')
## plt.subplot(1,5,3), plt.imshow(np.log(1 + np.abs(fft_shift)), cmap='gray')
## plt.subplot(1,5,4), plt.imshow(np.log(1 + np.abs.ifft_shift), cmap='gray')
#plt.subplot(1,2,2), plt.imshow(np.abs(ifft), cmap='gray')
#
#plt.show()


# not much difference, eh ?


scales = [3, 5, 7, 9]
orientations = [np.pi, 5*np.pi/6, 4*np.pi/6, np.pi/2, np.pi/3, np.pi/6]
lambd = 3.
gamma = 0.55
sigma = 1.5

filters = list()

# σθ = 1.5, mult = 3
for ksize in scales:
    for theta in orientations:
        filters.append(cv2.getGaborKernel((ksize, ksize), sigma, theta, lambd, gamma))

#fimg = cv2.filter2D(np.array(sample_img), cv2.CV_8UC1, filters[0])
#
#
#fimg_ifft_shift = np.fft.ifftshift(fimg)
#fimg_ifft = np.fft.ifft2(fimg_ifft_shift)
#
#plt.imshow(np.abs(fimg_ifft), cmap='gray')
#
#image_vectors = list()
#
#for fil in filters:
#    img = cv2.filter2D(np.array(sample_img), cv2.CV_8UC1, fil)
#    img = img.reshape(-1)
#    image_vectors.append(img)
#fv = np.array(image_vectors).reshape(-1)
#fv = fv * 100
## I dont understand the user-specificty about the RG, but for now I think I'll use a random seed to specify each
## user, afterall each seed signifies a diff set of random numbers generated.
#RG = np.random.randint(1, 255, fv.shape[0])
#fs = fv + RG
#fX = fs[:fs.shape[0]//2]
#fY = fs[fs.shape[0]//2:]
#K = np.random.uniform(-100, 100, fs.shape[0])
#K0 = K[:K.shape[0]//2]
#K1 = K[K.shape[0]//2:]
#D = np.sqrt((fX - K0)**2 + (fY - K1)**2)
#fft.shape

def median_filter(array, fsize):
    a = np.zeros(array.shape[0] + 2 * (fsize//2))
    a[fsize//2: array.shape[0] + fsize//2] = array
    for i in range(fsize//2, array.shape[0] + fsize//2):
        fg = a[i-fsize//2: i+fsize//2+1]
        fg = sorted(fg)
        a[i] = fg[fsize//2]
        
    return a[fsize//2:  array.shape[0] + fsize//2 + 1]

#Tf = median_filter(D, 5)
#Tf

scales = [3, 5, 7, 9]
orientations = [np.pi, 5*np.pi/6, 4*np.pi/6, np.pi/2, np.pi/3, np.pi/6]
lambd = 3.
gamma = 0.55
sigma = 1.5

def img_to_TF(src, filters, seed = 7, fsize=5):
    
    img = Image.open(src)
    img = img.resize((128, 128))
    img = np.array(img)
            
    filtered_images = list()
    for f in filters:
        filtered_images.append(cv2.filter2D(img, cv2.CV_8UC1, f))
        
    image_vectors = list()
    for img in filtered_images:
        fft = np.fft.fft2(img)
        fft_shift = np.fft.fftshift(fft)
        iff_shift = np.fft.ifftshift(fft_shift)
        ifft = np.fft.ifft2(iff_shift)
        image_vectors.append(ifft.reshape(-1))
        
    fv = np.array(image_vectors).reshape(-1) * 100
    np.random.seed(seed)
    RG = np.random.randint(1, 255, fv.shape[0])
    fs = fv + RG
    
    fX = fs[:fs.shape[0]//2]
    fY = fs[fs.shape[0]//2:]
    
    K = np.random.uniform(-100, 100, fs.shape[0])
    K0 = K[:K.shape[0]//2]
    K1 = K[K.shape[0]//2:]
    
    D = np.sqrt((fX - K0)**2 + (fY - K1)**2)
    Tf = median_filter(D, fsize)
    Tf = Tf.reshape(-1, 1)
    return Tf, RG, K

from sklearn.metrics.pairwise import cosine_similarity

# !python3 -m pip install pyDML

from dml.kda import KDA


def get_tf(src):
    return img_to_TF(src, filters)
#cosine_similarity(get_tf(df.img_1[0])[0], get_tf(df.img_2[0])[0])

import pandas as pd
data = pd.DataFrame()

data['scores'] = [cosine_similarity(get_tf(df.img_1[i])[0], get_tf(df.img_2[i])[0]) for i in range(len(df))]
data['labels'] = [df.related[i] for i in range(len(df))]

from sklearn.model_selection import train_test_split
x_train, x_test, y_train, y_test = train_test_split(data_cropped.scores, data_cropped.labels, test_size=0.2, random_state=7)

data.to_csv('calculated.csv')
