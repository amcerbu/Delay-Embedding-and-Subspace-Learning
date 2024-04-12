# Audio Visualization via Delay Embedding & Subspace Learning

This is the code repository to accompany a submission to DAFx 2024. Included are three Jupyter notebooks and a C++ application.

- `Figures.ipynb`: The notebook used to generate the figures in the Methods section of our paper, and to generate all videos in this repository.
- `Frequency.ipynb`: A self-contained notebook demonstrating the frequency-detection algorithm described in Appendix B.
- `Scope`: Source code for a realtime C++ visualization application. A demonstration video is available [here](https://youtu.be/Sl570aeDftM). The project depends on SDL, PortAudio, Eigen3, and RtMidi. 

--- 

## Interpreting the videos

The videos embedded in this document each have an attached description. Each video clip this section is the visualizations of a single sound,
rendered simultaneously with the parameters of figures 1, 3, 4, and 5 from the manuscript. The shading of the curve indicates the distance to the
plane of projection (lighter is closer). The colored dots -- or, when $N$ is large, what may appear to be a multicolored curve -- are the rows of the
matrix $A$ learned whose columns are orthonormal and approximate the trajectory in $\mathbf R^n$. 

Notice how the positions of the colored dots -- the subspace learned -- tend to depend only on the pitch. In the highest-dimensional case, the learned
matrix is often a bandpass filterbank which selects for the high-amplitude frequencies present. In that case the multicolored curve will be displayed as an
arc of a circle; the angle through which this arc turns corresponds to the frequency detected. This notion is only a heuristic; for exact reconstruction 
of frequency information using our methods see `Frequency.ipynb`.

## Synthetic sounds

Below is a synthesized tone created by passing white noise through a harmonically tuned collection of two-pole bandpass filters. The resonance of those filters
is adjusted over the course of the video. Notice how the projection information stabilizes when the sound is pitched. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/dbb8dd2b-fd4f-4930-8d06-41cbed02f3ab

Below is a synthesized glissando. Notice the changes in shape -- due to aliasing -- in the projections of low-dimensional embeddings (the first row). Notice also
how the shapes of the colorful curves change as pitch changes.

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/25db16c7-1057-4aaf-8852-8bedfa14ff72

Below is a synthesized sum of sinusoids whose harmonicity is modulated. 

[video]

Below is a synthesized sum of sinusoids whose brightness is modulated. 

[video]


## Visualizations of OrchideaSOL

We have visualized a number of orchestral samples from the OrchideaSOL database: for a number of instruments, each playing a number of pitches at three different dynamics.
Those can be found in the `Vid` directory; a few are selected as examples below. These visualizations have been produced with the same four choices of parameters as above. As with the synthetic visualizations, the
shading of the gray curve records distance to projecting plane, and the colorful dots record the choice of projection. 



https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/5816ea1c-149b-4a44-9487-655d9a16584a


https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/c7f33a45-a16f-401c-bbdf-832ac051f56f


https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/7f6ebc24-d231-49f0-85f0-38ce7cd65256


https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/b5d85add-ad73-49dd-b97b-0bdd7720e3ea


https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/7df11750-394c-46ba-a2f4-0ebb86ddab00




### Miscellaneous

We include below a handful of miscellaneous visualizations with other parameter choices. 


A clarinet playing $B3$, with embedding dimension $N = 48$ and analysis dimension $k = 4$. In order to visualize the learned four-dimensional curve, we 
draw its projections to the six two-dimensional coordinate planes in $\mathbf R^4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/d14c0acf-ab31-4aac-885e-e190953530b5

<!---
https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/c0b11fe9-2ae5-4808-946f-d3528e631e76

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/bb948c32-0fda-47d5-b76e-9cd5a9722ce5

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/08e00f3c-cf22-4bf5-8cb1-84c9c89582f8

A cello playing $B3$, mezzo-forte. The embedding dimension is $N = 23$ and projection dimension $k = 2$. Note the tracking of the colored curve with the vibrato pitch (the colorful curve records the rows of $A$, 
the matrix whose columns span the $2$-dimensional subspace of best fit in $\mathbf R^{23}$). 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/8a219a0b-33cb-4cfd-ad05-410b2bc5ca5c


Visualizing a synthetic waveform that becomes brighter (approaching a sawtooth wave), then darker: embedding dimension $N = 48$ and projection dimension $k = 4$ (as with the clarinet example
above, in order to display the four-dimensional curve we display its projection to the six coordinate planes in $\mathbf R^4$). 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/8583e43c-3b56-4e06-94e1-7a4f336de471

Visualizing a synthetic waveform made by stacking prime harmonics, each with an independent decay parameter. Here again $N = 48$ and $k = 4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/44ef5f3e-2582-4f12-9eac-07fa0e071f9a
--->

Visualizing a short bass clarinet melody, $N = 15$ and again $k = 4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/14c80198-687f-487f-b2a6-dde5d30c8a9a

