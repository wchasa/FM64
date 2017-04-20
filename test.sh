DATAPATH=/home/lab/testfile/
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

for data in $data6 $data7 $data8 $data9 $data10 $data11;do
	 seed=$(date +%s+%N)
    echo $seed
    ./my_fm $data cx $seed
    ./my_fm $data bx $seed
	echo "----------------------------------------------------------------------------------"
done
