CFLAGS = -g

all:  driver

driver:	driver.o scanner.o reduce.o amap.o aux.o
	gcc $(CFLAGS) -o $@ $^

