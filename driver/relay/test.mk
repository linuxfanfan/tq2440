CROSS:=arm-linux-
OBJ=relay
$(OBJ):main.c 
	$(CROSS)gcc $^ -o $@
	$(CROSS)strip $@
	cp $@ /work/nfs_tq/sbin
clean:
	rm -rf leds *.o
.PHONY:$(OBJ) clean
