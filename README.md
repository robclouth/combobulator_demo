## Setup

git clone recursive
git submodule sync
git submodule update --init --recursive --jobs 0
restore third_party/psimd deleted files

install https://s3.amazonaws.com/ossci-windows/mkl_2020.2.254.7z (not necessary)

cd /Libraries/PyTorch
mkdir build
cd build

$env:CMAKE_INCLUDE_PATH="C:\Users\Rob\Documents\Projects\Music\Tools\Combobulator\Libraries\mkl\include;C:\Users\Rob\Documents\Projects\Music\Tools\Combobulator\Libraries\PyTorch\third_party\onnx"; $env:LIB="C:\Users\Rob\Documents\Projects\Music\Tools\Combobulator\Libraries\mkl\lib"; cmake -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_BUILD_TYPE:STRING=Release -DUSE_CUDA:BOOL=OFF -DBUILD_CAFFE2:BOOL=OFF -DBUILD_CAFFE2_OPS:BOOL=OFF -DUSE_XNNPACK:BOOL=OFF -DUSE_CUDNN:BOOL=OFF -DUSE_QNNPACK:BOOL=OFF -DUSE_NUMPY:BOOL=OFF -DUSE_FBGEMM:BOOL=OFF -DUSE_TENSORPIPE:BOOL=OFF -DBUILD_CUSTOM_PROTOBUF:BOOL=ON -DBUILD_PYTHON:BOOL=OFF -DBUILD_TEST:BOOL=OFF -DUSE_DISTRIBUTED:BOOL=OFF -DUSE_KINETO:BOOL=OFF -DONNX_ML:BOOL=ON ..

cmake --build . -j 8
