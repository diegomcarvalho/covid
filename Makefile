all: socnet
	python3 setup.py build
	cd socnet; make

clean:
	rm -fr build dist socnet.egg-info
	cd socnet; make clean