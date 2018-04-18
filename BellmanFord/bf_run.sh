function bf(){
	make clean
	make
	for g in rmat22
	do
		amplxe-cl -collect hotspots -analyze-system -start-paused ./serialBF inputs/${g}.dimacs | tee pr_serial_${g}.txt;
		for t in 1 2 4 8
		do
			#./parallelBF inputs/${g}.dimacs ${t} > bf_parallel_${g}_${t}.txt
			amplxe-cl -collect hotspots -analyze-system -start-paused ./parallelBF inputs/${g}.dimacs ${t} | tee pr_${g}_${t}.txt;
		done
	done
}

bf
