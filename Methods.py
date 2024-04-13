import scipy
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
import itertools

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


def analyze(x, delays, k, oversample = 1, alpha = 0.99, epsilon = 0.0001, delta = 0.01, gamma = 0.01, normalize = False, dtype = 'float', randomize = True, mode = 'qr', corrected = True, pad = False, track = True):
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
        for i in (tqdm(range(ds[-1], len(X))) if track else range(ds[-1], len(X))):
            y = X[i - ds]
            if normalize:
                B = alpha * B + (1 - alpha) * np.outer(y, np.conj(y)) / (gamma + np.conj(y) @ y)
            else:
                B = alpha * B + (1 - alpha) * np.outer(y, np.conj(y))
                
            for j in range(oversample):
                
                Z = (1 - delta) * A + delta * B @ A
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


def sinusoid(frequency, T, p = 0, SR = 48000, dtype = 'float'):
    phase = 2 * np.pi * (frequency * np.arange(0, T, 1 / SR) - p)
    if dtype == 'complex':
        return np.exp(1j * phase)
    return np.sin(phase)

# bandlimited
def square(frequency, T, p = 0, SR = 48000, cutoff = 2):
    return 4 / np.pi * sum(sinusoid(frequency * n, T, p, SR) / n for n in range(1, int(SR / (cutoff * frequency)), 2))

# bandlimited
def saw(frequency, T, p = 0.5, SR = 48000, cutoff = 2):
    return 2 / np.pi * sum(sinusoid(frequency * n, T, p, SR) / n for n in range(1, int(SR / (cutoff * frequency))))

# bandlimited
def triangle(frequency, T, p = 0, SR = 48000, cutoff = 2):
    return 8 / np.pi ** 2 * sum((-1) ** ((n - 1) / 2) * sinusoid(frequency * n, T, p, SR) / (n ** 2) for n in range(1, int(SR / (cutoff * frequency)), 2))

def lfosquare(frequency, T, SR = 48000):
    return (((frequency * np.arange(0, T, 1 / SR) % 1) < 0.5) - 0.5) + 0.5

def lfotri(frequency, T, SR = 48000):
    return 4 * np.cumsum(((frequency * np.arange(0, T, 1 / SR) % 1) < 0.5) - 0.5) * (frequency / SR)

def timbre_vs_frequency(collections, d, name, SR = 48000, fm = 60, basename = 'Fig'):
    color = plt.cm.rainbow(np.linspace(0, 1, len(d)))
    fig, ax = plt.subplots(nrows = len(collections[0]), ncols = len(collections))
    for i, collection in enumerate(tqdm(collections)):
        for j, s in enumerate(collection):
            a = ax[j,i]
            a.set_xlim([-np.sqrt(len(d)),np.sqrt(len(d))])
            a.set_ylim([-np.sqrt(len(d)),np.sqrt(len(d))])
            a.set_aspect('equal')
            a.set_axis_off()

            trajectory, bases, distances, sep = analyze(s, d, 2, alpha = 0.999, randomize = False, delta = 0.05, pad = False, mode = 'svd', normalize = True, track = False)

            K = SR // fm
            segments = np.zeros((K - 1, 2, 2))
            segments[:,0] = trajectory[-K:-1]
            segments[:,1] = trajectory[-K+1:]

            distances = distances[-K:]
            darknesses = 0.5 * (1 - distances)
            colors = np.zeros((K - 1, 4))
            for l, _ in enumerate(colors):
                colors[l] = (darknesses[l], darknesses[l], darknesses[l], 1)

            lc = LineCollection(segments, color = colors, linewidth = 1, path_effects=[path_effects.Stroke(capstyle="round")])
            a.add_collection(lc)

            # a.plot(*trajectory[-K:].T, linewidth = 1)
            if (len(d) > 2):
                for n, c in zip(range(len(d)), color):
                    a.plot(*(0.75 * np.sqrt(len(d)) * bases[-K:, n].T), c = c, linewidth = 2, alpha = 0.75, solid_capstyle='round')
                
    plt.savefig(f'{basename}/{name}.pdf', bbox_inches='tight')  
    
def get_file_dict(SOLPath):
    file_dict = {}

    # Traverse the directory structure
    for root, dirs, files in os.walk(SOLPath):
        for file in files:
            # Ignore .DS_Store files
            if file == ".DS_Store":
                continue

            # Split the filename to extract parameters
            parts = file.split('-')

            # Extract parameters
            instrument = parts[0]
            playing_style = parts[1]
            note = parts[2]
            dynamic = parts[3]

            # Create dictionary keys if they don't exist
            if instrument not in file_dict:
                file_dict[instrument] = {}
            if playing_style not in file_dict[instrument]:
                file_dict[instrument][playing_style] = {}
            if note not in file_dict[instrument][playing_style]:
                file_dict[instrument][playing_style][note] = {}
            if dynamic not in file_dict[instrument][playing_style][note]:
                file_dict[instrument][playing_style][note][dynamic] = []

            # Append file path to the dictionary
            file_dict[instrument][playing_style][note][dynamic].append(os.path.join(root, file))

    return file_dict

# gets flattened dict of instrument, pitch, dynamic where
# instrument ranges over given list, pitch and dynamic over
# all values common to the dataset.
def get_flattened(file_dict, instruments):
    A = [set([*file_dict[a]['ord']]) for a in instruments]
    pitches = A[0]
    for a in A:
        pitches = pitches & a

    B = [set([*file_dict[a]['ord'][b]]) for a in instruments for b in pitches]
    dynamics = B[0]
    for b in B:
        dynamics = dynamics & b

    flattened = {}
    for i, p, d in itertools.product(instruments, pitches, dynamics):
        flattened[(i, p, d)] = sorted(file_dict[i]['ord'][p][d])[0]

    return flattened

def get_keys(flattened):
    inst = set()
    pitch = set()
    dynam = set()
    
    for i, p, d in flattened:
        inst.add(i)
        pitch.add(p)
        dynam.add(d)
    
    return inst, pitch, dynam

def ntom(note):
    lookup = {'C': 0, 'D': 2, 'E': 4, 'F': 5, 'G': 7, 'A': 9, 'B': 11}
    accident = {'#': 1, 'b': -1}
    
    midi = lookup[note[0]] + 12 * int(note[-1]) + 12
    midi += accident[note[1]] if len(note) == 3 else 0
    
    return midi

# harmonic sum of sinusoids with varying coeffs
def varying_brightness(f0, lfo, T, overtones, darkness, brightness, SR = 48000):
    oscillators = np.array([sinusoid(i * f0, T) for i in range(1, overtones + 1)])
    decay = darkness + (brightness - darkness) * lfo
    amplitudes = np.array([decay ** i / np.sqrt(i) for i in range(1, overtones + 1)])
    correction = np.sum(amplitudes, axis = 0)
    
    signal = np.sum(oscillators * amplitudes, axis = 0) / correction
    x = signal / np.max(np.abs(signal))
    
    params = {
        'synth' : True,
        'SR' : SR,
        'name' : 'bright'
    }

    return x, params

# glissando
def glissando(f0, lfo, T, overtones, darkness, spread, SR = 48000):
    frequency = (1 - np.pad(lfo, SR * T // 8)) * f0 + np.pad(lfo, SR * T // 8) * (spread * f0)

    ticks = 2 * np.pi * frequency / SR
    oscillators = np.exp(1j * np.array([np.cumsum(ticks * i) for i in range(1, overtones + 1)]))
    amps = np.array([darkness ** i for i in range(overtones)])

    waveform = amps @ oscillators / np.sum(amps)
    x = np.real(waveform)
    
    params = {
        'synth' : True,
        'SR' : SR,
        'name' : 'gliss'
    }

    return x, params

# varying harmonicity
def metallic(f0, lfo, T, overtones, darkness, start, end, SR = 48000):
    inharmonicity = (1 - np.pad(lfo, SR * T // 8)) * start + np.pad(lfo, SR * T // 8) * end
    
    ratios = np.repeat([np.arange(1, overtones + 1)], len(inharmonicity), axis = 0).T ** inharmonicity
    
    ticks = 2 * np.pi * f0 * ratios / SR
    oscillators = np.exp(1j * np.cumsum(ticks, axis = 1))
    amps = np.array([darkness ** i for i in range(overtones)])

    waveform = amps @ oscillators / np.sum(amps)
    x = np.real(waveform)
    
    params = {
        'synth' : True,
        'SR' : SR,
        'name' : 'inharmonic'
    }
    
    return x, params

# varying noise: harmonic stack of two-pole filters (real parts of one-pole compelex filters)
def noisy(f0, lfo, T, overtones, darkness, start, end, SR = 48000):
    modulator = lfo
    # excitation = 2 * np.random.rand(*modulator.shape) - 1
    excitation = np.random.randn(*modulator.shape, 2).view(np.complex128)[:,0]
    
    
    
    decays = start * (1 - modulator) + end * modulator
    r = np.exp(-1 / (SR * decays))
    alpha = np.exp([2j * np.pi * i * f0 / SR for i in range(1, overtones + 1)])
    
    y = np.zeros((overtones, len(modulator)), dtype = 'complex')
    for i in range(1, len(modulator)):
        y[:,i] = excitation[i] + r[i] * alpha * y[:,i - 1]
 
    amps = np.array([darkness ** i for i in range(overtones)])
        
    params = {
        'synth' : True,
        'SR' : SR,
        'name' : 'noisy'
    }
    
    x = amps @ y
    x /= np.max(np.abs(x))
        
    return np.real(x), params

