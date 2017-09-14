 #!/bin/bash
i=0
while [ $i -lt 1 ]; do
    seed=$(date +%s+%N)
    echo $seed
    ./my_fm /home/lab/testfile/bible cx 123
    ./my_fm /home/lab/testfile/bible bx 123
    ./my_fm /home/lab/testfile/bible px 123
    echo "------------------------"
    let i=i+1
done
wait 
exit 0
