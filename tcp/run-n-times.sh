#!/bin/bash

SECONDS=0  # Reset timer

for i in {1..10000}
do
    ./cmake-build-release/1_client_n-sends > "./cmake-build-release/output_$i.log" 2>&1 &
done

wait  # Wait for all background processes to finish

echo "All processes have finished."
echo "Total execution time: ${SECONDS} seconds."