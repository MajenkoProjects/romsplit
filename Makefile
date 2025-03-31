
all: romsplit romjoin

romsplit: romsplit.o
	$(CC) -o romsplit romsplit.o

romjoin: romsplit
	ln -s romsplit romjoin
