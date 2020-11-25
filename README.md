# seans_asr_review
review mozilla deep speech, seq2seq and tensorflow simple audio recognition

# github configuration

## model files need to be installed manually
cd into src/deepspeech_models and run the download_models.sh.  They are 1 gb

# deep speech documentation root:
https://deepspeech.readthedocs.io/en/v0.9.1/?badge=latest

## install notes
* sudo apt install virtualenv
* sudo apt install curl

### Don't do this (deep speech doesn't work with cuda 11):
* install cuda 11.0 from https://developer.nvidia.com/cuda-11.0-download-archive?target_os=Linux&target_arch=x86_64&target_distro=Ubuntu&target_version=2004&target_type=deblocal
* download cudnn tarfile v8.0.5 from https://developer.nvidia.com/rdp/cudnn-download
** Unzip and copy files install using instructions at https://docs.nvidia.com/deeplearning/cudnn/install-guide/index.html

### Do this: (install cuda 10.1)
* follow https://medium.com/@stephengregory_69986/installing-cuda-10-1-on-ubuntu-20-04-e562a5e724a0


#### fix missing cublas lib
There seems to be a known issue with 10.1 with libcublas not being included. (This described herehttps://forums.developer.nvidia.com/t/cublas-for-10-1-is-missing/71015/18)
What worked for me was to download the cublas.dpkg and manually unzip and then copy the required 10.1 libcublas files into /usr/local/cuda:
download https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/libcublas10_10.1.0.105-1_amd64.deb
then manually unzip the 10.1 libs:
ar x libcublas10_10.1.0.105-1_amd64.deb
tar xvf data.tar.xz
sudo cp usr/lib/x86_64-linux-gnu/libcublasLt.so.10.1.0.105 /usr/local/cuda-10.1/lib64/libcublasLt.so
sudo cp usr/lib/x86_64-linux-gnu/libcublas.so.10.1.0.105 /usr/local/cuda-10.1/lib64/libcublas.so
sudo cp usr/lib/x86_64-linux-gnu/libnvblas.so.10.1.0.105 /usr/local/cuda-10.1/lib64/libnvblas.so

### Test an inference using CPU
Result using cpu:
experience proves this 
Inference took 0.733s for 1.975s audio file.

### Test GPU
Follow instructions to setup gpu.
Result using gpu:
experience proves this
Inference took 0.413s for 1.975s audio file.

### Using a pretrained model notes
* Using desktop gpu so stick to pbmm and not the 'tensorflowlite runtime'

## build and run deepspeech_hello 
cd src/deepspeech_hello
make
source ./sourceme.sh
./hello
->expect to see output like "one two three four five"

## todo
1. should I update scorer?
2. am I using the streaming setup properly? I don't really want to open it
   up this way/keep an infinite stream going
3. review the docs now I understand it better.
