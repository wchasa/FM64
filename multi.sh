 #!/bin/bash
i=0
while [ $i -lt 1 ]; do
    seed=$(date +%s+%N)
    starttime=`date +'%Y-%m-%d %H:%M:%S'`
<<<<<<< HEAD
./my_fm /home/wch/testfile/bible cx 23 128
   # ./my_fm_m /home/wch/testfile/influenza bx 123 4
=======
./my_fm /home/wch/testfile/bible cx 121223 
>>>>>>> tempnewsample
#timer./my_fm /home/wch/testfile/influenza cx 123 8
endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_seconds=$(date --date="$starttime" +%s);
end_seconds=$(date --date="$endtime" +%s);
echo "本次运行时间： "$((end_seconds-start_seconds))"s"
echo "----------------------------"
starttime=`date +'%Y-%m-%d %H:%M:%S'`
#执行程序\
<<<<<<< HEAD
#../my_fm_newsample /home/wch/testfile/bible cx 23 
=======
#../my_fm_oldsample /home/wch/testfile/proteins.100MB cx 11223 
>>>>>>> tempnewsample
endtime=`date +'%Y-%m-%d %H:%M:%S'`
start_seconds=$(date --date="$starttime" +%s);
end_seconds=$(date --date="$endtime" +%s)
echo "本次运行时间： "$((end_seconds-start_seconds))"s"
    let i=i+1
done
wait 
<<<<<<< HEAD
exit 0256
=======
exit 0
>>>>>>> tempnewsample
