function run_pi {
	for v in pi_v1 pi_v2 pi_v3 pi_v4 pi_v5;
	do
		echo "Version " $v >> "pi.out"
		echo "Runtime, PI" >> "pi.out"
		for n in 1 2 4 8;
		do
			$v $n >> "pi.out"
		done
		echo "" >> "pi.out"

	done
}
if [ -f /pi.out ]; then
    rm -f pi.out
fi

echo "Pi Runtimes" > "pi.out"
run_pi