 #!/bin/bash
i=0
while [ $i -lt 10 ]; do
    seed=$(date +%s+%N)
    echo $seed
    ./my_fm /home/wch/testfile/dblp.xml cx $seed
    ./my_fm /home/wch/testfile/dblp.xml bx $seed
    echo "------------------------"
    let i=i+1
done
wait 
exit 0
