# Audio Visualization via Delay Embedding & Subspace Learning

This is the code repository to accompany a submission to DAFx 2024. Included are two Jupyter notebooks and a C++ application.

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

## Visualizations of OrchideaSOL

We have visualized a number of orchestral samples from the OrchideaSOL database: for a number of instruments, each playing a number of pitches at three different dynamics.
Those can be found in the `Orch` directory; a few are selected as examples below. These visualizations have been produced with the same four choices of parameters as above. As with the synthetic visualizations, the
shading of the gray curve records distance to projecting plane, and the colorful dots record the choice of projection. 


An accordion playing $C3$. 


https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/824a05bd-d10f-4bbe-ac51-80356c40af2c

A bassoon playing $C3$. 


https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/03d187e5-7635-4d6c-ab80-4a7baba4637f

An alto saxophone playing $D4$. Notice how the high partials appear only after the fundamental is at resonance. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/6418726b-f265-40da-9ba8-3d2ef07c2b1e

A flute playing $C4$. Note the overall consistency of the shape despite the presence of air noise. Note also that
the visualizations created with larger $N$ have filtered out the noise (the curves are smoother). 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/668e4073-610c-48fc-a1c5-dfc78c893cee

A viola playing $D4$. Note the sensitivity of the image to the slight fluctuations in bow pressure and speed. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/797e9ad7-f91c-419b-9dd3-cfa5bcf20608

A contrabass playing $G2$. Note, toward the end, that the string vibrates inharmonically without the phase-locking from the bow. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/e74a815b-12f0-48b7-ad3b-774f930883f8


## Synthetic sounds

Below is a synthesized tone created by passing white noise through a harmonically tuned collection of two-pole bandpass filters. The resonance of those filters
is adjusted over the course of the video. Notice how the projection information stabilizes when the sound is pitched. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/b6ae26a1-2d43-4c79-b680-601ac9105f24



Below is a synthesized glissando. Notice the changes in shape -- due to aliasing -- in the projections of low-dimensional embeddings (the first row). Notice also
how the shapes of the colorful curves change as pitch changes.

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/a3c8e77b-4d08-4e1a-8aa0-f5b24bf09882




Below is a synthesized sum of sinusoids whose harmonicity is modulated. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/4c103475-01a1-4334-a117-09fc02e3fd1f




Below is a synthesized sum of sinusoids whose brightness is modulated. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/426e71fc-6813-4fbf-a682-f166fcf04ecb






### Miscellaneous

We include below a handful of miscellaneous visualizations with other parameter choices. 


A clarinet playing $B3$, with embedding dimension $N = 48$ and analysis dimension $k = 4$. In order to visualize the learned four-dimensional curve, we 
draw its projections to the six two-dimensional coordinate planes in $\mathbf R^4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/d14c0acf-ab31-4aac-885e-e190953530b5

Visualizing a short bass clarinet melody, $N = 15$ and again $k = 4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/14c80198-687f-487f-b2a6-dde5d30c8a9a

