
yum install gtk+-devel gtk2-devel
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bazel-out/k8-opt/bin/src/:/opt/intel/openvino/opencv/lib
g++ src/example/CustomNodeResize/test_app.cpp -o test -lnode_resize_opencv -Lbazel-bin/src/ -L/opt/intel/openvino/opencv/lib