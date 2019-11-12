cc:= gcc
prom:= server
source:= main.c game.c threadpool.c
cflags:= -lpthread
 
$(prom): $(source)
	$(cc) -o $(prom) $(source) $(cflags)

.PHONY:clean
clean:
	rm -rf $(prom) Leaderboard.txt temp.txt