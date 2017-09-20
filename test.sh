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
#$data1 $data2 $data3 $data4 $data5 $data6 $data7 $data8 $data9 $data10 $data11 $data12 $data13
for data in $data1 $data2 $data3 $data4 $data5 $data6 $data7 $data8 $data9 $data10 $data11 $data12 $data13 ;do
	# seed=$(date +%s+%N)
    #echo $seed
    ../my_fm $data cx 123 2  
    ../my_fm $data bx 123 2  
    ../my_fm_m     $data cx 123 2
    ../my_fm_m     $data bx 123 2
    ../my_fm_m     $data px 123 2
    #./my_fm $data 1
    #./my_fm $data 0   
	echo "----------------------------------------------------------------------------------"
done
