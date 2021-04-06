all: src
	cd src; make

clean:
	rm -fr build dist socnet.egg-info
	find . -type f -name *.pyc -delete
	find . -type d -name __pycache__ -delete
	cd src; make clean
