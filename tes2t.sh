 #!/bin/bash
i=0
while [ $i -lt 100 ]; do
    ./my_fm_bx & ./my_fm_cx
    let i=i+1
done
wait // 等待执行完成 即可
exit 0
