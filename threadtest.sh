DATAPATH=/home/wch/testfile/
data1=${DATAPATH}"book1"
data2=${DATAPATH}"bible"
data3=${DATAPATH}"E.coli"
data4=${DATAPATH}"world192"
data5=${DATAPATH}"dna"
data6=${DATAPATH}"sources"
data7=${DATAPATH}"dblp.xml"
data8=${DATAPATH}"para"
data9=${DATAPATH}"kernel"
data10=${DATAPATH}"influenza"
data11=${DATAPATH}"world_leaders"
data12=${DATAPATH}"english" 
data13=${DATAPATH}"dblp.xml" 
for data in $data1 $data2 $data3 $data4  $data6 $data7 $data9 $data10 $data11 $data13 ;do
for k in $( seq 1 10 )
   do
   echo $k
   echo "."
    ./my_fm $data bx 123 500 $k
   done
   done