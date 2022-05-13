
clean:
	rm -rf obj_dir
	rm -f sim

sim:
	verilator -Isrc --trace -cc src/gpu.v --exe src/sim.cpp
	make -C obj_dir -j 8 -f Vgpu.mk Vgpu
	cp obj_dir/Vgpu sim
