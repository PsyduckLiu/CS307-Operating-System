#/******************************************************
#> Author       : HaolinLiu 
#> Last modified: 2020-03-12 16:16
#> Email        : 25915043@qq.com
#> Filename     : Makefile
#> Description  : makefile 
#*******************************************************/

obj-m += simple.o jiffies.o seconds.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

