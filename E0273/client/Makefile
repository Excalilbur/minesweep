cc:= gcc
prom:= client
source:= main.c game.c
 
$(prom): $(source)
	$(cc) -o $(prom) $(source)

.PHONY:clean
clean:
	rm -rf $(prom) 