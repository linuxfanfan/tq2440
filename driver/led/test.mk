CROSS:=arm-linux-
OBJ=leds
$(OBJ):main.c 
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@			#去除编译程序中的调试信息,使可执行文件减小
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf $(OBJ) *.o
.PHONY:$(OBJ) clean
