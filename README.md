# A-Simple-and-Small-Deep-Learning-framework-built-from-only-C-stl-
This is a repository of a simple yet very helpful for learning deep learning framework's source code , documentations and examples. This repository contains the source code with descriptions so that anyone who is currently diving deep into deep learning would understand fundamentals for it .


## Before we start our journey to dive deep into....:
Me as the author , wish to tell you that this framework is not meant to be for poduction grade or for commercial use. This library uses handcoded C++ codes to implement the tools such as functions , classes etc. This raw C++ unoptimized code is , atleast from my prespective , the perfect resource for understanding "what's under the hood?" . Thus , please treat this project as that way.

## [1] The Reason and the startings:
Some poeple will say that "why reinvent the wheal?". To them , I kindly want to remind you that "To understand how a wheal really works before you say you know how it works, you have to own the knowledge of how it works". And what is the best way for sort of "owning" that knowledge ? Practise + Implementation + Improving the existing knowledge . There are many young people in nowadays who loves deep diving into technical fields such as Deep Learning. To them, I want to tell you that , this repo can be treated as thier source of the hidden things happen in modern frameworks. 

## [2] What is this framework actually?:

This is a simple hand - crafted from Linalg to Neural Networks deep learning framework built in C++14. Many great frameworks like PyTorch, TensorFlow etc. exists . But to understand what happens , you need to get to source codes, and realize that they use something called "OpenBlas" . Ok , then you go to open blas ... wait are this handcoded assembly ? Yes ! It exactly is .. and it's why you don't have to wait 1 hours instead of 100 hours training a model. And that level of boilerplate hides the underlying logic. This "mini framework" is exactly to understand "that" underlying logic.


# Main Stuff:
## Tabel Of Contents
## [1] Linalg Documentation Link : 
## [2] NeuralNet Documentation Link :
## [3] How to download and run by myself?: (see below)


# Download and setup: 
To Whom who wants to experiment or many wants to improve the code, you have to manually download the code.
Then compile locally . Don't worry , there is 0 CMAKE hassle . This is a header only library , so you have to just include the NeuralNet/*****.hpp.
Requirements: C++ STL for 14 or over.
Building tips(kind of must actually): IF you are using Clang, use this flag for compiling:
```
 clang++ -O3 -ffast-math -march=native -fopenmp [your_file_name].cpp -o [your_file_name].exe
```
