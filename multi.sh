 #!/bin/bash
i=0
while [ $i -lt 1 ]; do
    seed=$(date +%s+%N)
    starttime=`date +'%Y-%m-%d %H:%M:%S'`
../my_fm_newsample /home/wch/testfile/world_leaders cx 11223 
#timer./my_fm /home/wch/testfile/influenza cx 123 8
endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_seconds=$(date --date="$starttime" +%s);
end_seconds=$(date --date="$endtime" +%s);
echo "本次运行时间： "$((end_seconds-start_seconds))"s"
echo "----------------------------"
starttime=`date +'%Y-%m-%d %H:%M:%S'`
#执行程序\
../my_fm_oldsample /home/wch/testfile/world_leaders cx 11223 
endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_seconds=$(date --date="$starttime" +%s);
end_seconds=$(date --date="$endtime" +%s)
echo "本次运行时间： "$((end_seconds-start_seconds))"s"
    let i=i+1
done
wait 
exit 0