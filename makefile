all:
	mkdir -p build && cd build && cmake .. && make -j

run:
	./build/job

format:
	astyle --style=kr --indent=force-tab=4 *.cc *.hh

clean:
	rm -rf build