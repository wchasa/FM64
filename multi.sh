 #!/bin/bash
i=0
while [ $i -lt 1 ]; do
    seed=$(date +%s+%N)
    echo $seed
    ./my_fm /home/wch/testfile/kernel cx 123 2
    ./my_fm_old /home/wch/testfile/kernel bx 123 2
  #  ./my_fm /home/wch/testfile/kernel px 123 2
    echo "------------------------"
    let i=i+1
done
wait 
exit 0
