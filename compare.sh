master=$($2 $1)
master_norm=$(echo -e "$master" | sed 's/\r$//')

echo -e "\e[4;36m$2\e[0m : \e[94mMaster\e[0m"
echo "$master"
echo "$master" | xxd

count=0
equal_count=0
approx_count=0
diff_count=0

for i in "${@:3}"; do
  out=$($i $1)
  header="\e[4;36m$i\e[0m : "
  echo ""

  if [ "$master" == "$out" ]; then
    echo -e "$header\e[92mEqual\e[0m"
    ((equal_count++))
  else
    norm=$(echo -e "$out" | sed 's/\r$//')
    if [ "$master_norm" != "$norm" ]; then
      echo -e "$header\e[91mNot equal\e[0m"
      echo "$out"
      ((diff_count++))
    else
      echo -e "$header\e[93mEqual with normalized line endings\e[0m"
      ((approx_count++))
    fi
    echo "$out" | xxd
  fi
done

echo ""
echo "Equal:  $equal_count"
echo "Approx: $approx_count"
echo "Diff:   $diff_count"
