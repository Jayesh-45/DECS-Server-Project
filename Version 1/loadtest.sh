#! /usr/bin/bash 

#if no of arguments not equal to 3
if [ $# -ne 3 ]; then
	echo "Usage $0 <numClients> <loopNum> <sleepTimeSeconds>"
	exit 1
fi

# M no of clients
M=$1
x=1
total_throughput=0
# sumN no of successful responces
sum_N=0

#sum of successful responses multiplied by average response time
sum_NR=0

for (( i=1 ; i<=$M ; i++ )); 
do
	filename="output$i.txt"
	./submit 127.0.0.1:8080 code.cpp $2 $3 1>$filename &
	pid=$!
		wait $pid
	avg_res=$(cat $filename | grep "Average response time:" | cut -d ' ' -f 4)

	N_i=$(cat $filename | grep "Successful responses:" | cut -d ' ' -f 3)
	
	#Calculate the throughput and total throughput
	throughput=$(echo $x*1000 / $avg_res | bc -l)
	total_throughput=$(echo $total_throughput + $throughput | bc -l)
	
	#Calculate the sum of N_i*R_i i.e. number_of_successful*average_response_time 
	sum_NR=$(echo $sum_NR+$avg_res*$N_i | bc -l)
	sum_N=$(echo $N_i + $sum_N | bc -l)	
done

total_avg_response_time=$(echo $sum_NR / $sum_N | bc -l)
echo "Overall throughput: $total_throughput requests/sec" 
echo "Average response time: $total_avg_response_time"
