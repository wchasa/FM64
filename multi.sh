 #!/bin/bash
i=0
while [ $i -lt 1 ]; do
    seed=$(date +%s+%N)
    echo $seed
    ./my_fm /home/wch/testfile/influenza cx 123 8
    ./my_fm /home/wch/testfile/influenza bx 123 8
    ./my_fm /home/wch/testfile/influenza px 123 8
    echo "------------------------"
    let i=i+1
done
wait 
exit 0
