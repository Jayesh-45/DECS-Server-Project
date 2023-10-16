#!/bin/bash
echo Analysing and Generating Plots!!

# Different number of clients
SIZE='
2
4
6
8
10
'	
touch throughput_results.txt
touch avg_res_results.txt
for i in ${SIZE}; do
    echo -n "${i} " >> throughput_results.txt
    echo -n "${i} " >> avg_res_results.txt
    bash loadtest.sh ${i} 5 2 > tempoutput.txt 
    cat tempoutput.txt | grep "Overall throughput:" | cut -d ' ' -f 3 >> throughput_results.txt
    cat tempoutput.txt | grep "Average response time:" | cut -d ' ' -f 4 >> avg_res_results.txt
done
rm tempoutput.txt
# Plot Number of clients vs Overall throughput 
cat throughput_results.txt | graph -T png --bitmap-size "1400x1400" -g 3 -L "Clients vs Throughput" -X "Number of Clients" -Y "Overall throughput (in request/s)" -r 0.25> ./throughput_plot.png

# Plot Number of clients vs Average response time 
cat avg_res_results.txt | graph -T png --bitmap-size "1400x1400" -g 3 -L "Clients vs Response Time" -X "Number of Clients" -Y "Average response time (in ms)" -r 0.25> ./avg_res_plot.png

