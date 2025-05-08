#!/bin/bash

SECONDS=0  # Reset timer

for i in {1..100}
do
    ./your_program > "output_$i.log" 2>&1 &
done

wait  # Wait for all background processes to finish

echo "All processes have finished."
echo "Total execution time: ${SECONDS} seconds."