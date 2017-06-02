all: v1 v2 v3 v4 v5 v6 v7 run

v1: v1.c
	gcc -fopenmp -O2 -ov1 v1.c

v2: v2.c
	gcc -fopenmp -O2 -ov2 v2.c

v3: v3.c
	gcc -fopenmp -O2 -ov3 v3.c

v4: v4.c
	gcc -fopenmp -O2 -ov4 v4.c

v5: v5.c
	gcc -fopenmp -O2 -ov5 v5.c

v6: v6.c
	gcc -fopenmp -O2 -ov6 v6.c

v7: v7.c
	gcc -fopenmp -O3 -ov7 v7.c

run: v1 v2 v3 v4 v5 v6 v7
	./v1
	./v2
	./v3
	./v4
	./v5
	./v6
	./v7

clean:
	rm -f v1 v2 v3 v4 v5 v6 v7
