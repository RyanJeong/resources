# CUDA
A repository for practice CUDA programming

## How to compile?
```Bash
$ nvcc -o <BIN_FILE> <SOURCE.cu>
```

## Basic
* 프로젝트를 수행하기 전에, CPU로 처리했을 때 소모되는 시간과 'PC에서 Device로 메모리 복사(입력) + GPU로 처리했을 때 소모되는 시간 + Device에서 PC로 메모리 복사(결과)'를 사전에 파악하는 것이 좋음

* SP(Streaming Processor)
    * GPU에서 주요한 연산을 수행하는 코어 유닛
    * 데이터 연산에 필요한 레지스터와 FPU(Floating point), ALU(Integer), LSU(Compare, Move)를 갖추고 있음
    * SP는 CUDA 프로그램에서 4 개의 스레드 사용

* SM(Streaming Multiprocessor)
    * 8 개의 SP가 모이면 SM을 구성
    * SP를 제어하고 명령어 캐시와 데이터 캐시를 가짐
    * I(Instruction) 캐시는 명령어를, C(Constant) 캐시는 명령을 수행한 결과를 보관
    >   I-cache holds the instructions of a block that an SM is executing. Every time the GTE(Giga Thread Engine) assigns a block to an SM, it also fills its cache with the instructions that make up this block.
    >   C-cache is another very important type of cache, as it caches values that are immutable. This includes constants, which are used in programs to feed constant values to the threads, such as filter coefficients that do not change throughout the entire program. The operation of this cache differs significantly in that this cache is responsible for repeating and supplying the same value to multiple (e.g., 16) cores, as opposed to 16 different values to 16 cores.
    * SP가 서로 공유하며 사용할 수 있는 Shared memory가 있으며, L1 cache 성능을 나타냄
    * 하나의 SM 당 두 개의 SFU(Special Function Unit)가 있으며, 사인, 코사인, 제곱근, 역수 등 특수한 연산을 수행하고, 클록 또는 스레드 당 하나의 연산을 처리함
    * MT issue는 SM 내에 있는 SP와 SFU에 명령을 전달
    * SM은 CUDA 프로그램에서 블록과 워프를 실행함
    


## Features
* [Use of cudamalloc(). Why the double pointer?](https://stackoverflow.com/questions/7989039/use-of-cudamalloc-why-the-double-pointer)
>   All CUDA API functions return an error code (or cudaSuccess if no error occured). All other parameters are passed by reference. However, in plain C you cannot have references, that's why you have to pass an address of the variable that you want the return information to be stored. Since you are returning a pointer, you need to pass a double-pointer.<br><br>
