# Audio Visualization via Delay Embedding & Subspace Learning

This is the code repository to accompany a submission to DAFx 2024. Included are three Jupyter notebooks and a C++ application.

- `Visualization.ipynb`: The main notebook, implementing the video-generation algorithms. 
- `Figures.ipynb`: A smaller notebook used to generate the figures in the Methods section of our paper.
- `Frequency.ipynb`: A self-contained notebook demonstrating the frequency-detection algorithm described in Appendix B.
- `Scope`: Source code for a realtime C++ visualization application. A demonstration video is available [here](https://youtu.be/Sl570aeDftM). The project depends on SDL, PortAudio, Eigen3, and RtMidi. 

Visualizing a synthetic waveform that becomes brighter (approaching a sawtooth wave), then darker: embedding dimension $N = 48$ and projection dimension $k = 4$ (note that in order to 
display the four-dimensional curve we dislay its projection to the six coordinate planes in $\mathbf R^4$). 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/8583e43c-3b56-4e06-94e1-7a4f336de471

Visualizing a synthetic waveform made by stacking prime harmonics, each with an independent decay parameter. Here again $N = 48$ and $k = 4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/44ef5f3e-2582-4f12-9eac-07fa0e071f9a

Visualizing a short bass clarinet melody, $N = 15$ and $k = 4$. 

https://github.com/amcerbu/Delay-Embedding-and-Subspace-Learning/assets/2309180/14c80198-687f-487f-b2a6-dde5d30c8a9a

