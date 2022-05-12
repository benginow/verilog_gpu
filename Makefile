
clean:
	rm -rf obj_dir
	rm -f sim

sim:
	verilator -Isrc -cc src/vgademo.v --exe src/sim.cpp
	LIBS=-lSDL2 make -C obj_dir -j 8 -f Vvgademo.mk Vvgademo
	cp obj_dir/Vvgademo sim
