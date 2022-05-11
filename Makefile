
clean:
	rm -rf obj_dir
	rm -f demo

demo:
	verilator -cc vgademo.v --exe demo.cpp
	make -C obj_dir -j 8 -f Vvgademo.mk Vvgademo
	cp obj_dir/Vvgademo demo
