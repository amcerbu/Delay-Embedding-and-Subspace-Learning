import scipy
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm

import wavio
import os
import shutil
import IPython.display as ipd

import matplotlib
matplotlib.rcParams['figure.dpi'] = 200
matplotlib.use('Agg')

from matplotlib.collections import LineCollection
from mpl_toolkits.mplot3d.art3d import Line3DCollection
from matplotlib.collections import PolyCollection
from matplotlib.colors import LinearSegmentedColormap
import matplotlib.patheffects as path_effects

import time


def analyze(x, delays, k, oversample = 1, alpha = 0.999, epsilon = 0.0001, delta = 0.01, gamma = 0.01, normalize = False, dtype = 'float', randomize = True, mode = 'qr', corrected = True, pad = False):
    N = len(delays)
    x = x.astype(dtype)
    # ds = np.cumsum(delays)
    ds = np.array(delays)
    
    if pad:
        X = np.append(np.zeros(ds[-1], dtype = dtype), x)
    else:
        X = x
         
    A = np.eye(N,k, dtype = dtype)
    Y = np.random.random((N,N)) + (1j if dtype == 'complex' else 0) * np.random.random((N, N))
    Y = Y - np.conj(Y.T)
    if randomize:
        A = scipy.linalg.expm(Y) @ A
    
    B = np.zeros((N,N), dtype = dtype)
    
    if pad:    
        trajectory = np.zeros((len(x), k), dtype = dtype)
        bases = np.zeros((len(x), N, k), dtype = dtype)
        distances = np.zeros(len(x), dtype = float)
        sep = np.zeros(len(x), dtype = dtype)
    else:
        trajectory = np.zeros((len(x) - ds[-1], k), dtype = dtype)
        bases = np.zeros((len(x) - ds[-1], N, k), dtype = dtype)
        distances = np.zeros(len(x) - ds[-1], dtype = float)
        sep = np.zeros(len(x) - ds[-1], dtype = dtype)

    
    try:
        for i in tqdm(range(ds[-1], len(X))):
            y = X[i - ds]
            if normalize:
                B = alpha * B + (1 - alpha) * np.outer(y, np.conj(y)) / (gamma + np.conj(y) @ y)
            else:
                B = alpha * B + (1 - alpha) * np.outer(y, np.conj(y))
                
            for j in range(oversample):
                
                Z = B @ A
                if mode == 'gradient':
                    A += epsilon * np.sqrt(N * k) * (Z - A @ (np.conj(A.T) @ Z))

                    A = A / np.linalg.norm(A, axis = 0) # orthogonalization
                    corr = np.conj(A.T) @ A
                    E = A @ (np.eye(k) - np.conj(A.T) @ A)
                    A += (1 / k) / (1 / k + np.max(np.abs(E))) * E
                    
                elif mode == 'qr':
                    C, D = scipy.linalg.qr(Z, mode = 'economic')
                    new = C

                    if corrected:
                        # if new and A have similar images, this will be close to a rotation matrix
                        Y = np.conj(new.T) @ A

                        # find the closest k x k unitary matrix
                        u, d, v = scipy.linalg.svd(Y, full_matrices = False)
                        Y = u @ v
                    else:
                        Y = np.eye(k)

                    # spin new so that it is close to the old A
                    A = new @ Y

                elif mode == 'svd':
                    # correct Z to an orthonormal basis, C
                    u, d, v = scipy.linalg.svd(Z, full_matrices = False)
                    C = u @ v
                    
                    if corrected:
                        Y = np.conj(C.T) @ A
                        
                        # find the closest k x k unitary matrix
                        u, d, v = scipy.linalg.svd(Y, full_matrices = False)
                        Y = u @ v
                    else:
                        Y = np.eye(k)

                    # spin C so that it is close to the old A
                    A = C @ Y
                    
            trajectory[i - ds[-1]] = np.conj(A.T) @ y
            bases[i - ds[-1]] = A
            if normalize:
                distances[i - ds[-1]] = np.real((np.conj(y) @ y - np.conj(trajectory[i - ds[-1]]) @ trajectory[i - ds[-1]]) / (gamma + np.conj(y) @ y))
            else:
                distances[i - ds[-1]] = np.real((np.conj(y) @ y - np.conj(trajectory[i - ds[-1]]) @ trajectory[i - ds[-1]]))
                
            sep[i - ds[-1]] = (y - A @ trajectory[i - ds[-1]])[0]
    except KeyboardInterrupt:
        return trajectory[:i-ds[-1]], bases[:i-ds[-1]], distances[:i-ds[-1]], sep[:i-ds[-1]]
    
    return trajectory, bases, distances, sep


# def analyze(x, delays, k, oversample = 1, alpha = 0.9, epsilon = 0.001, delta = 0.1, gamma = 0.01, dtype = 'float', randomize = True, seed = False, track = False, pad = False):
#     N = len(delays)
#     x = x.astype(dtype)
#     # ds = np.cumsum(delays)
#     ds = np.array(delays)
    
#     if pad:
#         X = np.append(np.zeros(ds[-1], dtype = dtype), x)
#     else:
#         X = x
    
#     A = np.eye(N,k, dtype = dtype)
#     Y = np.random.random((N,N)) + (1j if dtype == 'complex' else 0) * np.random.random((N, N))
#     Y = Y - np.conj(Y.T)
#     if randomize:
#         A = scipy.linalg.expm(Y) @ A
#     elif seed:
#         for i in range(k):
#             A[:,i] = X[i * (len(X) - k - ds[-1]) // k + ds[-1] - ds]
#             A[:,i] /= np.linalg.norm(A[:,i])
            
#         for i in (tqdm(range(N)) if track else range(N)):
#             A += 1 / N * A @ (np.eye(k) - np.conj(A.T) @ A)
        
#         for i in range(k):
#             A[:,i] /= np.linalg.norm(A[:,i])
    
#     B = np.zeros((N,N), dtype = dtype)
    
#     if pad:    
#         trajectory = np.zeros((len(x), k), dtype = dtype)
#         bases = np.zeros((len(x), N, k), dtype = dtype)
#         distances = np.zeros(len(x), dtype = float)
#     else:
#         trajectory = np.zeros((len(x) - ds[-1], k), dtype = dtype)
#         bases = np.zeros((len(x) - ds[-1], N, k), dtype = dtype)
#         distances = np.zeros(len(x) - ds[-1], dtype = float)
    
#     try:
#         for i in (tqdm(range(ds[-1], len(X))) if track else range(ds[-1], len(X))):
#             y = X[i - ds]
#             B = alpha * B + np.outer(y,y) / (gamma + np.conj(y) @ y)
#             for j in range(oversample):
#                 C = B @ A
#                 A += epsilon / N * np.sqrt(k) * (C - A @ (np.conj(A.T) @ C)) # subspace learning
#                 A += delta / N * np.sqrt(k) * A @ (np.eye(k) - np.conj(A.T) @ A) # orthogonalization
                
#             trajectory[i - ds[-1]] = np.conj(A.T) @ y
#             bases[i - ds[-1]] = A
#             distances[i - ds[-1]] = (np.abs(np.conj(y) @ y) - np.abs(np.conj(trajectory[i - ds[-1]]) @ trajectory[i - ds[-1]])) / (gamma + np.abs(np.conj(y) @ y))
#     except KeyboardInterrupt:
#         return trajectory[:i-ds[-1]], bases[:i-ds[-1]], distances[:i-ds[-1]]
    
#     return trajectory, bases, distances