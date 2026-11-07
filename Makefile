OBJS = brckbrk.o model.o ball.o paddle.o brick.o score.o lives.o render.o raster.o input.o clock.o asynch.o synch.o cond.o

a.out: $(OBJS)
	cc68x $(OBJS)
