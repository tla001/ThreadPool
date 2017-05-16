#所有的路径变量都不允许重名
AUX_SOURCE_DIRECTORY(. T1_SRC_LIST)
#设置成静态库
ADD_LIBRARY(threadpool ${T1_SRC_LIST})


