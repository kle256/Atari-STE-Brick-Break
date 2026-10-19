
# Renderer test build
CC = cc68x
INC = -I..\model -I..\raster

tst: trender.c render.c tmodel.c trasrc.c
	$(CC) $(INC) trender.c render.c tmodel.c trasrc.c -o tst
