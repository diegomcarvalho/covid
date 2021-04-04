all: src
	cd src; make

clean:
	rm -fr build dist socnet.egg-info
	cd src; make clean