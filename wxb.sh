#!/bin/bash

wxb=$(grep -b -o -a "@ @ @ " D1.GRP)
echo "get wxb:"
echo ${wxb}

pos=${wxb%:*}
echo "get pos:"
echo ${pos}

echo -n "Wei Xiaobao is at: "
if [ ${pos} == "268608" ]; then
  echo "Gao Sheng"
elif [ ${pos} == "4762" ]; then
  echo "He Luo"
elif [ ${pos} == "13518" ]; then
  echo "You Jian"
elif [ ${pos} == "176450" ]; then
  echo "Yue Lai"
elif [ ${pos} == "264362" ]; then
  echo "Long Men"
else
  echo "Unknow hotel."
fi


